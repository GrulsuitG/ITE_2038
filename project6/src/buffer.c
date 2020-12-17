#include "buffer.h"

int buf_init(int size){
	int i;
	buf_size = size;
	/*buf_pool = (page_t**)malloc(sizeof(page_t*)*size);
	if(buf_pool ==NULL){
		perror("make buf error");
		exit(EXIT_FAILURE);
	}*/
	block = (buf_ctrl_block**)malloc(sizeof(buf_ctrl_block*)*size);
	if(block ==NULL){
		perror("make buf_ctrl_block error");
		exit(EXIT_FAILURE);
	}
	memset(block, 0, sizeof(block));
	for(i=0; i<size; i++){
		block[i] = (buf_ctrl_block*)malloc(sizeof(buf_ctrl_block));
		if(block[i] == NULL){
			perror("make buf_ctrl_block error");
			exit(EXIT_FAILURE);
		}
		memset(block[i], 0, sizeof(buf_ctrl_block*));
	}
	for(i=0; i<size; i++){
		block[i]->frame = init_page();
		block[i]->id = i;
		block[i]->next = block[(i+1)%size];
		block[(i+1)%size]->prev = block[i];
		block[i]->page_latch = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(block[i]->page_latch,0);
	}
	buffer_manager_latch = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(buffer_manager_latch,0);
	return 0;
}

int buf_open(int table_id, char* filename){
	return file_open(table_id, filename);
}

page_t* buf_read_page(int table_id, pagenum_t pagenum){
	pthread_mutex_lock(buffer_manager_latch);
	int index = find_place(table_id, pagenum);

	if(index == -1){
		index = find_empty(table_id, pagenum);
		file_read_page(table_id, pagenum, block[index]->frame);
	}
	pthread_mutex_unlock(buffer_manager_latch);
	pthread_mutex_lock(block[index]->page_latch);
	
	block[index]->frame->index = index;
	block[index]->table_id = table_id;
	block[index]->pagenum = pagenum;
	block[index]->ref_bit = true;
	head = block[index];
	
	
	
	return block[index]->frame;
}

void buf_return_page(int table_id, pagenum_t pagenum,  bool is_dirty, int index){
	pthread_mutex_lock(buffer_manager_latch);
	if(is_dirty)
		block[index] -> is_dirty = is_dirty;
	block[index] -> ref_bit = true;
	head = block[index];
	pthread_mutex_unlock(block[index]->page_latch);
	pthread_mutex_unlock(buffer_manager_latch);
	return;
}

page_t* buf_alloc_page(int table_id){
	pthread_mutex_lock(buffer_manager_latch);
	pagenum_t pagenum = file_alloc_page(table_id);
	int index = find_place(table_id, pagenum);
	if(index == -1){
		index = find_empty(table_id, pagenum);
	}
	//fprintf(fp,"mutex lock ");
	pthread_mutex_unlock(buffer_manager_latch);
	pthread_mutex_lock(block[index]->page_latch);
	//fprintf(fp, "%d\n", index);
	
	buf_clear(index);
	block[index]->frame->mypage =pagenum;
	block[index]->frame->index = index;
	
	
	block[index]->pagenum = pagenum;
	block[index]->table_id = table_id;
	block[index]->ref_bit = true;
	head = block[index];
	
	return block[index]->frame;
}
void buf_free_page(int table_id, pagenum_t pagenum, int index){
	pthread_mutex_lock(buffer_manager_latch);
	file_free_page(table_id, pagenum);
	block[index] -> table_id = 0;
	pthread_mutex_unlock(buffer_manager_latch);
	pthread_mutex_unlock(block[index]->page_latch);	
	
	return;
}


int find_empty(int table_id, pagenum_t pagenum){
	int num = (table_id+11*pagenum)%buf_size;
	int flag =num;
	if(block[num]->table_id == 0){
		return num;
	}
	num = (num+1) % buf_size;
	
	while(num != flag){
		if(block[num]->table_id ==0){
			return num;
		}
		num = (num+1) % buf_size;
	}
	return eviction();
	
}

int find_place(int table_id, pagenum_t pagenum){
	int num = (table_id+11*pagenum)% buf_size;
	int flag =num;
	if(block[num]->table_id == table_id && block[num]->pagenum == pagenum){
		return num;
	}
	
	num = (num+1) % buf_size;
	while(num != flag){
		if(block[num]->table_id == table_id && block[num]-> pagenum == pagenum){
			return num;
		}
		num = (num+1) % buf_size;
	}
	return -1;
}

int eviction(){
	int num = 0;
	int table_id;
	pagenum_t pagenum;
	page_t *page;
	tail = head->next;
	pthread_mutex_unlock(buffer_manager_latch);
	while(true){
		if(tail->ref_bit == false ){
			if(pthread_mutex_trylock(tail->page_latch) == 0){
				table_id = tail->table_id;
				pagenum = tail->pagenum;
				pthread_mutex_lock(log_buffer_latch);
				logbuf_flush();
				pthread_mutex_unlock(log_buffer_latch);
				if(tail->is_dirty){
					page = tail->frame;
					if(tail->pagenum == 0){
						file_write_root(tail->table_id, page->rootPageNum);
					}
					else{
						file_write_page(tail->table_id, tail->pagenum, page);
					}
				}
				pthread_mutex_unlock(tail->page_latch);
				head = tail;
				pthread_mutex_lock(buffer_manager_latch);
				return tail->id;
			}
		}
		
		else if(tail->ref_bit == true){
			tail->ref_bit = false;
		}
		tail = tail->next;
		if(head == tail){
			num++;
		}
	}
	
}

int buf_close(int table_id){
	int i ;
	page_t *page;
	pthread_mutex_lock(buffer_manager_latch);
	for(i = 0; i<buf_size; i++){
		if(block[i]->table_id == table_id){
			pthread_mutex_lock(block[i]->page_latch);
			if(block[i]->is_dirty){
				page = block[i]->frame;
				if(block[i]->pagenum == 0){
					file_write_root(block[i]->table_id,page->rootPageNum);
				}
				else{
					file_write_page(block[i]->table_id,block[i]->pagenum, page);
				}
			}
			block[i]-> table_id = 0;
			pthread_mutex_unlock(block[i]->page_latch);
		}
	}
	pthread_mutex_unlock(buffer_manager_latch);
	return file_close(table_id);
}

void buf_clear(int index){
	block[index]->table_id =0;
	block[index]->pagenum =0;
	block[index]->ref_bit = false;
	block[index]->is_dirty = false;
	free_page(block[index]->frame);
	block[index]->frame = NULL;
	block[index]->frame = init_page();
}

void buf_destroy(){
	int i ;
	for(i =0; i< buf_size; i++){
		if(block[i]->frame)
			free_page(block[i]->frame);
		pthread_mutex_destroy(block[i]->page_latch);
		free(block[i]->page_latch);
		free(block[i]);
	}
	pthread_mutex_destroy(buffer_manager_latch);
	free(buffer_manager_latch);
	free(block);
}

void print_buf(){
	int i,j;
	
	for(i=0; i<buf_size; i++){
	printf("[%d] : %d, %ld, %d, %d\n", i, block[i]->table_id, block[i]->pagenum, block[i]->is_dirty, block[i]->ref_bit);
	//	if(block[i]->pagenum ==3){
	//		printf("key : %ld", block[i]->frame->rootPageNum);
//			for(j=0; j<buf_pool[i]->num_keys; j++){
//				printf("%ld ", buf_pool[i]->keys[j]);
//			}
//			printf("\n");
	//	}
	}
	printf("========================\n");
}
