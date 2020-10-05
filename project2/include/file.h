#ifndef __FILE_H__
#define __FILE_H__

#define PageSize 409P6

#include<fcntl.h>
#include<unistd.h>
#include<io.h>


typedef uint64_t pagenum_t;

typedef struct page_t{
	pagenum_t freePageNum;
	pagenum_t rootPageNum;
	long numOfPage;

	pagenum_t nextFreePageNum;

	pagenum_t parentPageNum;
	int isLeaf;
	int numofKey;
	pagenum_t rightSibling;
	
}page_t;

extern char* filename;
extern pagenum_t globalpagenum;


pagenum_t file_alloc_page();
void file_free_page(pagenum_t pagenum);
void file_read_page(pagenum_t pagenum, page_t* dest);
void file_write_page(pagenum_t pagenum, const page_t* src);


#endif /* __FILE_H__*/
