#ifndef __FILE_H__
#define __FILE_H__

#define PAGE_SIZE 4096
#define DEFAULT_FREE_PAGE 10

#ifndef SIZE_ORDER
#define SIZE_ORDER
#define VALUE_SIZE 120
#define LEAF_ORDER 32
#define INTERNAL_ORDER 249
#endif


#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#ifndef O_DIRECT
#define O_DIRECT 00040000
#endif

extern char* filename;

typedef uint64_t pagenum_t;

#ifndef __RECORD__
#define __RECORD__
typedef struct record {
    char* value;
} record;
#endif
typedef struct page_t{
	pagenum_t freePageNum;
	pagenum_t rootPageNum;
	pagenum_t numOfPage;

	pagenum_t nextFreePageNum;

	pagenum_t parentPageNum;
	int is_leaf;
	int num_keys;
	pagenum_t pointer;
	
	record *record;
	int64_t *key;
	pagenum_t *pagenum;
}page_t;

pagenum_t file_alloc_page();
void file_free_page(pagenum_t pagenum);
void file_read_page(pagenum_t pagenum, page_t* dest);
void file_write_page(pagenum_t pagenum, const page_t* src);

void make_file();
int* get_freelist();
page_t* init_page();

void free_page(page_t *page);

#endif /* __FILE_H__*/
