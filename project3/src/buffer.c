#include "buffer.h"

int make_buf(int size){
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
	}
	return 0;
}

page_t* buf_read_page(int table_id, pagenum_t pagenum){
	int index = find_place(table_id, pagenum);

	if(index == -1){
		index = find_empty(table_id, pagenum);
	//	buf_clear(index);
		file_read_page(table_id, pagenum, block[index]->frame);
		}

	
	block[index]->table_id = table_id;
	block[index]->pagenum = pagenum;
	block[index]->pin_count++;
	block[index]->ref_bit = true;
	head = block[index];

	return block[index]->frame;
}

void buf_return_page(int table_id, pagenum_t pagenum,  bool is_dirty){
	int index = find_place(table_id, pagenum);
	page_t* page;
	if(index == -1){
		index = find_empty(table_id, pagenum);
		block[index] ->pagenum = pagenum;
		block[index] ->table_id = table_id;
	}
	block[index] -> pin_count--;
	if(is_dirty)
		block[index] -> is_dirty = is_dirty;
	block[index] -> ref_bit = true;
	head = block[index];

}

page_t* buf_alloc_page(int table_id){
	pagenum_t pagenum = file_alloc_page(table_id);
	int index = find_empty(table_id, pagenum);
	buf_clear(index);
	block[index]->frame->mypage =pagenum;

	block[index]->pagenum = pagenum;
	block[index]->table_id = table_id;
	block[index]->pin_count++;
	block[index]->ref_bit = true;
	head = block[index];
	
	
	return block[index]->frame;
}
void buf_free_page(int table_id, pagenum_t pagenum){
	int index = find_place(table_id, pagenum);
	
	block[index]->pin_count--;
	file_free_page(table_id, pagenum);
	block[index] -> table_id = 0;

	
}


int find_empty(int table_id, pagenum_t pagenum){
	int num = (table_id+pagenum)%buf_size;

	if(block[num]->table_id == 0)
		return num;
	num = (num+1) % buf_size;
	
	while(num != (table_id+pagenum)%buf_size){
		if(block[num]->table_id ==0)
			return num;
		num = (num+1) % buf_size;
	}
	
	return eviction();
	
}

int find_place(int table_id, pagenum_t pagenum){
	int num;
	num = (table_id+pagenum)% buf_size;

	if(block[num]->table_id == table_id && block[num]->pagenum == pagenum)
		return num;
	
	num = (num+1) % buf_size;
	while(num != (table_id+pagenum) % buf_size){
		if(block[num]->table_id == table_id && block[num]-> pagenum == pagenum)
			return num;
		num = (num+1) % buf_size;
	}

	return -1;
}

int eviction(){
	int num = 0;
	page_t *page;
	tail = head->next;

	while(true){
		if(tail->ref_bit == 0 && tail->pin_count == 0){
			if(tail->is_dirty){
				page = (page_t*)tail->frame;
				if(tail->pagenum ==0){
					file_write_root(tail->table_id, page->rootPageNum);
				}
				else{
					file_write_page(tail->table_id, tail->pagenum, page);
				}
			}
			buf_clear(tail->id);
			head = tail;
			return find_place(tail->table_id, tail->pagenum);
		}
		else if(tail->pin_count == 0 && tail->ref_bit == 1){
			tail->ref_bit = false;
		}
		tail = tail->next;
		if(head == tail){
			num++;
		}
	}
	//perror("all page can't eviction");
	//exit(EXIT_FAILURE);
	//return 0;
	
}

int buf_close_table(int table_id){
	int i ;
	page_t *page;
	for(i = 0; i<buf_size; i++){
		if(block[i]->table_id == table_id){
			while(true){
				if(block[i]->pin_count ==0){
					break;
				}
			}
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
		}
	}
	return 0;
}

void buf_clear(int index){
	block[index]->table_id =0;
	block[index]->pagenum =0;
	block[index]->ref_bit = false;
	block[index]->is_dirty = false;
	block[index]->pin_count = 0;
	free_page(block[index]->frame);
	block[index]->frame = NULL;
	block[index]->frame = init_page();
}

void buf_destroy(){
	int i ;
	for(i =0; i< buf_size; i++){
		if(block[i]->frame)
			free_page(block[i]->frame);
		free(block[i]);
	}
	free(block);
}

void print_buf(){
	int i,j;
	
	for(i=0; i<buf_size; i++){
	printf("[%d] : %d, %ld, %d, %d, %d\n", i, block[i]->table_id, block[i]->pagenum, block[i]->is_dirty, block[i]->ref_bit, block[i]->pin_count);
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
