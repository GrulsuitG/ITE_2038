#ifndef __BUF_H__
#define __BUF_H__
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include"file.h"


typedef struct buf_ctrl_block{
	page_t *frame;
	
	int id;
	int table_id;
	pagenum_t pagenum;
	bool is_dirty;
	bool ref_bit;
	pthread_mutex_t *page_latch;

	struct buf_ctrl_block *next;
	struct buf_ctrl_block *prev;
}buf_ctrl_block;

pthread_mutex_t buffer_manager_latch;
buf_ctrl_block **block;
int buf_size;

buf_ctrl_block* head;
buf_ctrl_block* tail;

int buf_init(int num_buf);

int buf_open(int table_id, char* filename);

page_t* buf_read_page(int table_id, pagenum_t pagenum);
void buf_return_page(int table_id, pagenum_t pagenum,bool is_dirty);
page_t* buf_alloc_page(int table_id);
void buf_free_page(int table_id, pagenum_t pagenum);

int find_empty(int table_id, pagenum_t pagenum);
int find_place(int iable_id, pagenum_t pagenum);

int eviction();

void buf_clear(int index);

int buf_close(int table_id);
void buf_destroy();

void print_buf();

#endif
