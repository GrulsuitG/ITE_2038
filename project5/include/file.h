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

#define MAX_TABLE_NUM 10


#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#ifndef O_DIRECT
#define O_DIRECT 00040000
#endif

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

	pagenum_t parentPageNum;
	int is_leaf;
	int num_keys;
	pagenum_t pointer;
	
	int64_t *keys;
	record **record;
	
	pagenum_t *pagenum;

	pagenum_t mypage;

	int index;
}page_t;



int fdList[MAX_TABLE_NUM];

pagenum_t file_alloc_page(int table_id );
void file_free_page(int table_id, pagenum_t pagenum);
void file_read_page(int table_id, pagenum_t pagenum, page_t* dest);
void file_write_page(int table_id, pagenum_t pagenum, const page_t* src);
void file_write_root(int table_id, pagenum_t pagenum);

int file_open(int table_id, char* filename);
int file_close(int table_id);
int make_file(char* filname);
int* get_freelist(int table_id);
page_t* init_page();

void free_page(page_t *page);

#endif /* __FILE_H__*/
