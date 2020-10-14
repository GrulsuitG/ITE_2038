#ifndef __FILE_H__
#define __FILE_H__

#define PAGE_SIZE 4096
#define DEFAULT_FREE_PAGE 10
#define VALUE_SIZE 120
#define LEAF_ORDER 4
#define INTERNAL_ORDER 5



#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
#ifndef O_DIRECT
#define O_DIRECT 00040000
#endif

extern char* filename;

typedef uint64_t pagenum_t;

typedef struct record {
    int64_t key;
    char* value;
}record;


typedef struct inter_record{
    int64_t key;
    pagenum_t pagenum;
}inter_record;

typedef struct page_t{
	pagenum_t freePageNum;
	pagenum_t rootPageNum;
	pagenum_t numOfPage;

	pagenum_t nextFreePageNum;

	pagenum_t parentPageNum;
	int isLeaf;
	int numOfKey;
	pagenum_t pointer;
	
	record *info[LEAF_ORDER];
	inter_record *inter_info[INTERNAL_ORDER];
}page_t;

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
