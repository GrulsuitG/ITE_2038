#ifndef __BUF_H__
#define __BUF_H__
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"file.h"


typedef struct buf_ctrl_block{
	void* frame;
	int table_id;
	pagenum_t pagenum;
	bool is_dirty;
	bool ref_bit;
	int pin_count;
	struct buf_ctrl_block *next;
	struct buf_ctrl_block *prev;
}buf_ctrl_block;

page_t **buf_pool;
buf_ctrl_block **block;
int buf_size;

void make_buf(int size);



page_t* buf_read_page(int table_id, pagenum_t pagenum);
void buf_return_page(int table_id, pagenum_t pagenum, bool is_dirty);
void buf_write_page(int table_id, pagenum_t pagenum, page_t* src);
page_t* buf_alloc_page(int table_id);
void buf_free_page(int table_id, pagenum_t pagenum);

int get_page(int table_id, pagenum_t pagenum);

#endif
