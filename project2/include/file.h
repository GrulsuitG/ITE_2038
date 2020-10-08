#ifndef __FILE_H__
#define __FILE_H__

#define PAGESIZE 4096
#define DEFAULT_FREE_PAGE 10
#define LEAF_RECORD 31
#define INTER_RECORD 248
#define VALUESIZE 120

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
#include"bpt.h"


typedef uint64_t pagenum_t;

typedef struct inter_record{
	long key;
	pagenum_t pagenum;
}inter_record;

typedef struct page_t{
	pagenum_t freePageNum;
	pagenum_t rootPageNum;
	long numOfPage;

	pagenum_t nextFreePageNum;

	pagenum_t parentPageNum;
	int isLeaf;
	int numOfKey;
	pagenum_t rightSibling;
	
	record *info[LEAF_RECORD];
	inter_record *inter_info[INTER_RECORD];
}page_t;

char *filename;
pagenum_t globalpagenum;


pagenum_t file_alloc_page();
void file_free_page(pagenum_t pagenum);
void file_read_page(pagenum_t pagenum, page_t* dest);
void file_write_page(pagenum_t pagenum, const page_t* src);
void make_file();
page_t* init_page_t();
void init_info(page_t *page);
void init_inter_info(page_t *page);
void free_page_t(page_t *page);

#endif /* __FILE_H__*/
