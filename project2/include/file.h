#ifndef __FILE_H__
#define __FILE_H__

#define PageSize 4096
#define DEFAULT_FREE_PAGE 10

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
//include<io.h>


typedef uint64_t pagenum_t;

typedef struct record {
    long key;
    char *value;
} record;

typedef struct internalrecord{
	long key;
	pagenum_t pagenum;
}internalrecord;

typedef struct page_t{
	pagenum_t freePageNum;
	pagenum_t rootPageNum;
	long numOfPage;

	pagenum_t nextFreePageNum;

	pagenum_t parentPageNum;
	int isLeaf;
	int numofKey;
	pagenum_t rightSibling;
	
	record **recordinfo;
	internalrecord **internalrecordinfo;
}page_t;

char *filename;
pagenum_t globalpagenum;


pagenum_t file_alloc_page();
void file_free_page(pagenum_t pagenum);
void file_read_page(pagenum_t pagenum, page_t* dest);
void file_write_page(pagenum_t pagenum, const page_t* src);
void make_file();
page_t* init_page();


#endif /* __FILE_H__*/
