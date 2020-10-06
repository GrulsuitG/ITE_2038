#include "file.h"

pagenum_t file_alloc_page(){
	pagenum_t pagenum;
	page_t *header =init_page();
	
	int fd;
	char* store_info = (char*) malloc(sizeof(pagenum_t));
	file_read_page(0,header);

	if((fd=open(filename, O_RDWR|O_EXCL)) ==-1){
		fprintf(stderr, "file open error for alloc\n");
		exit(1);
	}
	truncate(filename,PageSize*(header->numOfPage+11));

	lseek(fd,PageSize *header->freePageNum,SEEK_SET);
	read(fd, store_info, sizeof(pagenum_t));

	pagenum = atoi(store_info);
	//if(pagenum == 0){
		globalpagenum = header->numOfPage;
		lseek(fd,0,SEEK_END);
		for(int i =0 ; i<DEFAULT_FREE_PAGE; i++){
			sprintf(store_info,"%lu", ++globalpagenum);
			write(fd,store_info, sizeof(pagenum_t));
			lseek(fd, PageSize-sizeof(pagenum_t), SEEK_CUR);
		}
		sprintf(store_info,"%d", 0);
		write(fd,store_info, sizeof(pagenum_t));
		globalpagenum++;
		
		lseek(fd, 0, SEEK_SET);
		sprintf(store_info,"%lu", 1+header->numOfPage);
		write(fd,store_info, sizeof(pagenum_t));

	//}

	close(fd);
	

	return header->freePageNum;
}

void file_free_page(pagenum_t pagenum){

}

void file_read_page(pagenum_t pagenum, page_t* dest){
	int fd,isExist, success =1;
	char *read_info, *read_info2;
	page_t* header = init_page();
	
	isExist = access(filename, 00);


	/*if there is not a file
	  make a file & initailize the file
	  make a head page and default free page*/
	if (isExist == -1){
		make_file();
	}
	if((fd=open(filename, O_RDONLY)) == -1){
		fprintf(stderr, "file open error for read\n");
		exit(1);
	}
	// default read the head page
	read(fd, read_info, sizeof(pagenum_t));
	header->freePageNum = atoi(read_info);
	read(fd, read_info, sizeof(pagenum_t));
	header->rootPageNum = atoi(read_info);
	read(fd, read_info, sizeof(pagenum_t));
	header->numOfPage = atoi(read_info);

	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		dest->freePageNum = header->freePageNum;
		dest->rootPageNum = header->rootPageNum;
		dest->numOfPage = header->numOfPage;
		return;
	}

	else{
		lseek(fd,PageSize*pagenum,SEEK_SET);
		read(fd, read_info, sizeof(pagenum_t));
		dest->parentPageNum = atoi(read_info);
		read(fd, read_info, sizeof(int));
		dest->isLeaf =atoi(read_info);
		read(fd,read_info, sizeof(int));
		dest->numofKey = atoi(read_info);
		lseek(fd, PageSize*pagenum + 120, SEEK_SET);
		read(fd, read_info, sizeof(pagenum_t));
		dest->rightSibling = atoi(read_info);

		if(dest->isLeaf == 1){
			for(int i=0; i<31; i++){
				read(fd, read_info,sizeof(long));
				read(fd, read_info2, sizeof(char)*120);
				if(read_info == NULL && read_info2 == NULL)
					break;

				dest->recordinfo[i]->key= atoi(read_info);
				dest->recordinfo[i]->value = read_info2;
			}
		}

		else{
			for(int i=0; i<248; i++){
				read(fd, read_info, sizeof(long));
				read(fd, read_info2, sizeof(pagenum_t));

				if(read_info == NULL && read_info2 == NULL)
					break;
				
				dest->internalrecordinfo[i]->key = atoi(read_info);
				dest->internalrecordinfo[i]->pagenum = atoi(read_info2);
			}
		}
	}
	close(fd);
}



void file_write_page(pagenum_t pagenum, const page_t* src){
	int fd;
	page_t* header = init_page();

	file_read_page(0, header);

	if((fd=open(filename, O_WRONLY|O_APPEND))== -1){
		fprintf(stderr, "file open error for write\n");
		exit(1);
	}

	if(pagenum == header->freePageNum){
		pagenum = file_alloc_page();
		return file_write_page(pagenum, src);
	}



}




void make_file(){
	int fd;
	char* store_info =(char*) malloc(sizeof(pagenum_t));

	if((fd = creat(filename,0644)) ==-1){
		fprintf(stderr, "make file error\n");
		exit(1);
	}
	truncate(filename, 12*PageSize);
	globalpagenum = 0;

	lseek(fd, PageSize, SEEK_SET);
	for(int i=0; i<DEFAULT_FREE_PAGE; i++){
		sprintf(store_info,"%lu",++globalpagenum);
		write(fd, store_info, sizeof(pagenum_t));
		lseek(fd, PageSize-sizeof(pagenum_t), SEEK_CUR);

	}
	sprintf(store_info,"%d", 0);
	write(fd, store_info, sizeof(pagenum_t));
	globalpagenum++;

	lseek(fd, 0 , SEEK_SET);
	sprintf(store_info,"1" );
	write(fd, store_info, sizeof(pagenum_t));
	sprintf(store_info,"0");
	write(fd, store_info, sizeof(pagenum_t));
	sprintf(store_info,"%lu", globalpagenum+1);
	write(fd, store_info, sizeof(pagenum_t));

	close(fd);

}
page_t* init_page(){
	page_t *page;
	
	page = (page_t*) malloc(sizeof(page_t));
	
	page->recordinfo = (record**) malloc(sizeof(record)*31);
	
	for(int i= 0 ; i<31; i++){
		page->recordinfo[i]->value = (char*)malloc(sizeof(char)*120);
	}
	
	page->internalrecordinfo = (internalrecord**) malloc(sizeof(internalrecord)*248);
	
	return page;
}



int main(){
	filename = "datafile";
	page_t *dst = init_page();
	file_read_page(0,dst);
	file_alloc_page();
	
}
	
