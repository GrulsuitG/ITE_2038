#include "buffer.h"

int make_buf(int size){
	int i;
	buf_size = size;
	buf_pool = (page_t*)malloc(sizeof(page_t*)*size);
	if(buf_pool ==NULL){
		perror("make buf error");
		exit(EXIT_FAILURE);
	}
	block = (buf_ctrl_block**)malloc(sizeof(buf_ctrl_block*)*size);
	if(block ==NULL){
		perror("make buf_ctrl_block error");
		exit(EXIT_FAILURE);
	}
	memset(block, 0, sizeof(block));
	for(i=0; i<size; i++){
		buf_pool[i] = init_page();
		block[i]->frame = buf_pool[i];
	
	}
	return 0;
}

page_t* buf_read_page(int table_id, pagenum_t pagenum){
	int index = find_empty(table_id, pagenum);

	file_read_page(table_id, pagenum, buf_pool[index]);

	block[index]->table_id = table_id;
	block[index]->pagenum = pagenum;
	block[index]->pin_count++;
	block[index]->ref_bit = true;

	if(head == NULL){
		head = block[index];
		tail = block[index];
		tail->next = head;
		head->next = tail;
		head->prev = tail;
		tail->prev = head;
	}
	else{
		head->next = block[index];
		block[index]->prev = head;
		head = block[index];
		head->next = tail;
	}

	return buf_pool[index];
}

void buf_return_page(int table_id, pagenum_t pagenum, bool is_dirty){
	int index = find_place(table_id, pagenum);

	block[index] -> pin_count--;
	blocl[index] -> is_dirty = is_dirty;
	block[index] -> ref_bit = true;
}

void buf_write_page(int table_id, pagenum_t pagenum, page_t *src){
	int index = find_place(table_id, pagenum);

	file_write_page(table_id, pagenum, block[index]);
}

page_t* buf_alloc_page(int table_id){

}

void buf_free_page(int table_id, pagenum_t pagenum){

}
