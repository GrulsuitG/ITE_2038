#include "file.h"

pagenum_t file_alloc_page(){
	pagenum_t pagenum;
	page_t* header=init_page_t();
	header->freePageNum = 3;
	int fd;
	char* store_info = (char*) malloc(sizeof(pagenum_t));
	file_read_page(0,header);
	
	
	if((fd=open(filename, O_RDWR|O_EXCL)) ==-1){
		fprintf(stderr, "file open error for alloc\n");
		exit(1);
	}
	lseek(fd,PAGESIZE*header->freePageNum,SEEK_SET);
	read(fd, store_info, sizeof(pagenum_t));
	
	pagenum = atoi(store_info);
	if(pagenum == 0){
	
		globalpagenum = header->numOfPage+1;
		lseek(fd,0,SEEK_END);
		
		truncate(filename,PAGESIZE*(header->numOfPage+11));
		
		for(int i =0 ; i<DEFAULT_FREE_PAGE; i++){
			sprintf(store_info,"%lu", ++globalpagenum);
			write(fd,store_info, sizeof(pagenum_t));
			lseek(fd, PAGESIZE-sizeof(pagenum_t), SEEK_CUR);
		}
		sprintf(store_info,"%d", 0);
		write(fd,store_info, sizeof(pagenum_t));
		
		
		lseek(fd, 0, SEEK_SET);
		sprintf(store_info,"%lu", 1+header->numOfPage);
		write(fd,store_info, sizeof(pagenum_t));
		lseek(fd, sizeof(pagenum_t), SEEK_CUR);
		sprintf(store_info, "%lu",globalpagenum);
		write(fd, store_info,sizeof(pagenum_t));

	}

	close(fd);
	

	return header->freePageNum;
}

void file_free_page(pagenum_t pagenum){
	int fd;
	char *read_info = (char*)malloc(sizeof(pagenum_t));
	
	fd = open(filename, O_RDWR);
	
	//move the pagenum and reset
	lseek(fd, pagenum*PAGESIZE, SEEK_SET);
	write(fd, NULL, PAGESIZE);
	
	//find first free page
	lseek(fd, 0, SEEK_SET);
	read(fd, read_info, sizeof(pagenum_t));
	
	//move the reseted page and write the frist free page
	lseek(fd, pagenum*PAGESIZE, SEEK_SET);
	write(fd, read_info, sizeof(pagenum_t));
	
	//write the reseted page number at first free page 
	lseek(fd, 0, SEEK_SET);
	sprintf(read_info, "%lu", pagenum);
	write(fd, read_info, sizeof(pagenum_t));
	
}

void file_read_page(pagenum_t pagenum, page_t* dest){
	int fd,isExist;
	char *read_info = (char*)malloc(sizeof(pagenum_t));
	
	isExist = access(filename, 00);

	/*if there is not a file
	  make a file & initailize the file
	  make a head page and default free page*/
	if (isExist == -1){
		make_file();
	}
	fd = open(filename, O_RDONLY);
	if(fd<0){
		fprintf(stderr, "file open error for read\n");
		exit(1);
	}
	// default read the head page
	lseek(fd, 0, SEEK_SET);
	read(fd, read_info, sizeof(pagenum_t));
	dest->freePageNum = atoi(read_info);
	read(fd, read_info, sizeof(pagenum_t));
	dest->rootPageNum = atoi(read_info);
	read(fd, read_info, sizeof(pagenum_t));
	dest->numOfPage = atoi(read_info);
	

	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		free(read_info);
		close(fd);
		return;
	}
	//move the pagenum and read page header
	else{
		lseek(fd,PAGESIZE*pagenum,SEEK_SET);
		read(fd, read_info, sizeof(pagenum_t));
		dest->parentPageNum = atoi(read_info);
		read(fd, read_info, sizeof(int));
		dest->isLeaf =atoi(read_info);
		read(fd,read_info, sizeof(int));
		dest->numOfKey = atoi(read_info);
		lseek(fd, PAGESIZE*pagenum + 120, SEEK_SET);
		read(fd, read_info, sizeof(pagenum_t));
		dest->rightSibling = atoi(read_info);

		if(dest->isLeaf == 1){
			for(int i=0; i<dest->numOfKey; i++){
				read(fd, read_info,sizeof(uint64_t));
				dest->info[i]->key= atoi(read_info);
				
				read(fd, dest->info[i]->value, sizeof(char)*VALUESIZE);
			}
		}

		else{
			for(int i=0; i<dest->numOfKey; i++){
				read(fd, read_info, sizeof(uint64_t));
				dest->inter_info[i]->key = atoi(read_info);
				
				read(fd, read_info, sizeof(pagenum_t));
				dest->inter_info[i]->pagenum = atoi(read_info);
			}
		}
	}
	free(read_info);
	close(fd);
}



void file_write_page(pagenum_t pagenum, const page_t* src){
	int fd;
	char* store_info = (char*)malloc(sizeof(pagenum_t));
	
	page_t* header = init_page_t();

	file_read_page(0, header);

	if((fd=open(filename, O_WRONLY|O_APPEND))== -1){
		fprintf(stderr, "file open error for write\n");
		exit(1);
	}

	if(pagenum == header->freePageNum){
		pagenum = file_alloc_page();
		return file_write_page(pagenum, src);
	}
	
	
	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		sprintf(store_info, "%lu", src->freePageNum);
		write(fd, store_info, sizeof(pagenum_t));
		sprintf(store_info, "%lu", src->rootPageNum);
		write(fd, store_info, sizeof(pagenum_t));
		sprintf(store_info, "%lu", src->numOfPage);
		write(fd, store_info, sizeof(uint64_t));
		
		free(store_info);
		close(fd);
	}
	//move the pagenum and write page header
	else{
		lseek(fd,PAGESIZE*pagenum,SEEK_SET);
		sprintf(store_info, "%lu", src->parentPageNum);
		write(fd, store_info, sizeof(pagenum_t));
		
		sprintf(store_info, "%d", src->isLeaf);
		write(fd, store_info, sizeof(int));
		
		sprintf(store_info, "%d", src->numOfKey);
		write(fd,store_info, sizeof(int));
		
		lseek(fd, PAGESIZE*pagenum + 120, SEEK_SET);
		sprintf(store_info, "%lu", sizeof(pagenum_t));
		write(fd, store_info, sizeof(pagenum_t));

		if(src->isLeaf == 1){
			
			for(int i=0; i<src->numOfKey; i++){
				sprintf(store_info, "%lu", src->info[i]->key);
				write(fd, store_info,sizeof(uint64_t));
			
				write(fd, src->info[i]->value,sizeof(char)*VALUESIZE);
			}
		}

		else{
			for(int i=0; i<INTER_RECORD; i++){
				sprintf(store_info, "%lu", src->inter_info[i]->key);
				write(fd, store_info,sizeof(uint64_t));
				
				sprintf(store_info, "%lu", src->inter_info[i]->pagenum);
				write(fd, store_info,sizeof(pagenum_t));
			}
		}
	}
	free(store_info);
	close(fd);
}

void make_file(){
	int fd;
	char* store_info =(char*) malloc(sizeof(pagenum_t));

	if((fd = open(filename,O_RDWR|O_CREAT,0644)) ==-1){
		fprintf(stderr, "make file error\n");
		exit(1);
	}
	truncate(filename, 12*PAGESIZE);
	globalpagenum = 1;

	lseek(fd, PAGESIZE, SEEK_SET);
	for(int i=0; i<DEFAULT_FREE_PAGE; i++){
		sprintf(store_info,"%lu",++globalpagenum);
		write(fd, store_info, sizeof(pagenum_t));
		lseek(fd, PAGESIZE-sizeof(pagenum_t), SEEK_CUR);

	}
	sprintf(store_info,"%d", 0);
	write(fd, store_info, sizeof(pagenum_t));


	lseek(fd, 0 , SEEK_SET);
	sprintf(store_info,"1" );
	write(fd, store_info, sizeof(pagenum_t));
	sprintf(store_info,"0");
	write(fd, store_info, sizeof(pagenum_t));
	sprintf(store_info,"%lu", globalpagenum+1);
	write(fd, store_info, sizeof(pagenum_t));

	close(fd);

}
page_t* init_page_t(){
	page_t *page;
	
	page = (page_t*) malloc(sizeof(page_t));
	return page;
}

void init_info(page_t *page){
	for(int i =0; i<LEAF_RECORD; i++){
		page->info[i] = (record*)malloc(sizeof(record));
	}
	
	for(int i= 0 ; i<LEAF_RECORD; i++){
		page->info[i]->value = (char*)malloc(sizeof(char)*VALUESIZE);
	}
}

void init_inter_info(page_t *page){
	for(int i =0; i<INTER_RECORD; i++)
		page->inter_info[i] = (inter_record*)malloc(sizeof(inter_record));
}

void free_page_t(page_t *page){
	
	for(int i=0; i<LEAF_RECORD; i++){
		if(page->info[i] != NULL)
			free(page->info[i]);
	}
	for(int i =0; i<INTER_RECORD; i++){
		if(page->inter_info[i] != NULL)
			free(page->inter_info[i]);
	}
	
	
	if(page !=NULL)
		free(page);
}

int main(){
	filename = "datafile";
	page_t *dst = init_page_t();
}
	
