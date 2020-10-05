#include "file.h"

pagenum_t file_alloc_page(){
	
	
	

	return globalpagenum;
}

void file_free_page(pagenum_t pagenum){

}

void file_read_page(pagenum_t pagenum, page_t* dest){
	int fd,isExist;
	page_t* header = (page_t*)malloc(sizeof(page_t));
	
	isExist = access(filename, 00);


	/*if there is not a file
	  make a file & initailize the file
	  make a head page and 10 free page*/
	if (isExist == -1){
		if((fd=creat(filename, ,0744)) == -1){
			fprintf(stderr, "file make error");
		}
		globalpagenum = 0;
		//make 10 free page
		for(int i = 0; i<9; i++){
			lseek(fd,PageSize, SEEK_CUR);
			write(fd, ++globalpagenum, sizeof(pagenum_t));	
		}
		lseek(fd,PageSize, SEEK_CUR);
		write(fd, 0, sizeof(pagenum_t));
		globalpagenum++;

		//initailize head page
		lseek(fd,0, SEEK_SET);
		write(fd,2, sizeof(pagenum_t));
		write(fd,0, sizeof(pagenum_t));
		write(fd,globalpagenum+1, sizeof(pagenum_t));
	}
}

void file_write_page(pagenum_t pagenum, const page_t* src){

}


