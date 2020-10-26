#include "buffer.h"

int make_buf(int size){
	int i;
	buf_size = size;
	buf_pool = (page_t**)malloc(sizeof(page_t*)*size);
	if(buf_pool ==NULL){
		perror("make buf error");
		exit(EXIT_FAILURE);
	}
	block = (buf_ctrl_block**)malloc(sizeof(buf_ctrl_block*)*size);
	if(block ==NULL){
		perror("make buf_ctrl_block error");
		exit(EXIT_FAILURE);
	}
	for(i=0; i<size; i++){
		block[i] = (buf_ctrl_block*)malloc(sizeof(buf_ctrl_block));
		if(block[i] == NULL){
			perror("make buf_ctrl_block error");
			exit(EXIT_FAILURE);
		}
		memset(block[i], 0, sizeof(block[i]));
	}
	for(i=0; i<size; i++){
		buf_pool[i] = init_page();
		block[i]->frame = buf_pool[i];
		block[i]->id = i;
		block[i]->next = block[(i+1)%size];
		block[(i+1)%size]->prev = block[i];
	}
	return 0;
}

page_t* buf_read_page(int table_id, pagenum_t pagenum){
	int index = find_place(table_id, pagenum);

	if(index == -1)
		index = find_empty(table_id, pagenum);               

	file_read_page(table_id, pagenum, buf_pool[index]);

	block[index]->table_id = table_id;
	block[index]->pagenum = pagenum;
	block[index]->pin_count++;
	block[index]->ref_bit = true;
	head = block[index];
	//enList(index);

	return buf_pool[index];
}

void buf_return_page(int table_id, pagenum_t pagenum, bool is_dirty){
	int index = find_place(table_id, pagenum);

	if(index == -1)
		index = find_empty(table_id, pagenum);

	block[index] -> pin_count--;
	block[index] -> is_dirty = is_dirty;
	block[index] -> ref_bit = true;
	head = block[index];
//	enList(index);
}

page_t* buf_alloc_page(int table_id){
	pagenum_t pagenum = file_alloc_page(table_id);
	int index = find_empty(table_id, pagenum);
	
	buf_pool[index]->mypage =pagenum;

	block[index]->pagenum = pagenum;
	block[index]->table_id = table_id;
	block[index]->pin_count++;
	block[index]->ref_bit = true;
	head = block[index];
//	enList(index);
	
	return buf_pool[index];
}
void buf_free_page(int table_id, pagenum_t pagenum){
	int index = find_place(table_id, pagenum);

	file_free_page(table_id, pagenum);

	block[index]->table_id = 0;
//	block[index]->next->prev = block[index]->prev;
//	block[index]->prev->next = block[index]->next;
}

void enList(int index){
	if(!head){
		head = block[index];
		tail = block[index];
		head->next = tail;
		head->prev = tail;
		tail->next = head;
		tail->prev = head;
	}
	else {
		if(block[index]->next){
			block[index]->next->prev = block[index]->prev;
			block[index]->prev->next = block[index]->next;
		}
		head->next = block[index];
		block[index]->next =tail;
		block[index]->prev = head;
		head = block[index];
	}
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
	int num = (table_id+pagenum)% buf_size;

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
	pagenum_t num;
	page_t *page;
	tail = head->next;
	print_buf();
	while(tail != head){
		printf("prev: %d, cur: %d, next: %d\n", tail->prev->id, tail->id, tail->next->id);
		if(!tail->ref_bit && tail->pin_count == 0){
			if(tail->is_dirty){
				page = (page_t*)tail->frame;
				if(tail->pagenum ==0){
					file_write_root(tail->table_id, page->rootPageNum);
				}
				else
					file_write_page(tail->table_id, tail->pagenum, page);
			}
			head = tail->prev;
			return find_place(tail->table_id, tail->pagenum);
		}
		else if(tail->pin_count == 0 && tail->ref_bit){
			tail->ref_bit = false;
		}
		tail = tail->next;
	}
	if(tail == head){
		perror("why");
		exit(EXIT_FAILURE);
	}
}

int buf_close_table(int table_id){
	int i ;
	page_t *page;
	for(i = 0; i<buf_size; i++){
		if(block[i]->table_id == table_id){
			if(block[i]->is_dirty && block[i]->pin_count ==0){
				page = (page_t*)block[i]->frame;
				if(block[i]->pagenum == 0){
					file_write_root(block[i]->table_id,page->rootPageNum);
				}
				file_write_page(block[i]->table_id,block[i]->pagenum, page);
			}
			if(block[i]->is_dirty && block[i]->pin_count!=0){
				while(true){
					if(block[i]->pin_count ==0)
						break;
				}
				page = (page_t*)block[i]->frame;
				if(block[i]->pagenum == 0){
					file_write_root(block[i]->table_id,page->rootPageNum);
				}
				file_write_page(block[i]->table_id,block[i]->pagenum, page);
			}
			block[i]->table_id = 0;
		}
	}
	return 0;
}

void buf_destroy(){
	int i ;
	for(i =0; i< buf_size; i++){
		free(buf_pool[i]);
	}
	free(buf_pool);
	free(block);
}

void print_buf(){
	int i ;
	for(i=0; i<buf_size; i++){
		printf("[%d] : %d, %ld, %d, %d, %d\n", i, block[i]->table_id, block[i]->pagenum, block[i]->is_dirty, block[i]->ref_bit, block[i]->pin_count);
	}
}
