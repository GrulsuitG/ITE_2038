#include "file.h"

char* filename;

pagenum_t file_alloc_page(){
	pagenum_t pagenum, num;
	page_t* header=init_page_t();
	int fd;
	pagenum_t read_info;
	file_read_page(0,header);
	
	if((fd=open(filename, O_RDWR)) <0){
	    perror("file open error for alloc");
		exit(EXIT_FAILURE);
	}
	lseek(fd,PAGE_SIZE*header->freePageNum,SEEK_SET);
	if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for alloc");
        exit(EXIT_FAILURE);
    }
	if(read_info == 0){
	
		pagenum = header->numOfPage+1;
		lseek(fd,0,SEEK_END);
		
        // file size up
		if(truncate(filename,PAGE_SIZE*(header->numOfPage+DEFAULT_FREE_PAGE))<0){
            perror("file truncate error for alloc");
            exit(EXIT_FAILURE);
        }
		//write next free page num at new pages
		for(int i =1 ; i<DEFAULT_FREE_PAGE; i++){
			pagenum++;
            if(write(fd,&pagenum, sizeof(pagenum_t))< 0){
                perror("file write error for alloc");
                exit(EXIT_FAILURE);
            }
			lseek(fd, PAGE_SIZE-sizeof(pagenum_t), SEEK_CUR);
		}
        num = 0;
		if(write(fd, &num, sizeof(pagenum_t)) < 0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
		
		//write free pagen num
		lseek(fd, 0, SEEK_SET);
        num = header->numOfPage+1;
		if(write(fd ,&num, sizeof(pagenum_t))<0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
        //write total page num
		lseek(fd, sizeof(pagenum_t), SEEK_CUR);
		if(write(fd, &pagenum,sizeof(pagenum_t)) <0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
	}
    //write next free page num
    else{
        lseek(fd, 0, SEEK_SET);
        if(write(fd, &read_info, sizeof(pagenum_t)) < 0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
    }
    if( fsync(fd) < 0){
        perror("file sync error for alloc");
        exit(EXIT_FAILURE);
    }
	close(fd);
	return header->freePageNum;
}

void file_free_page(pagenum_t pagenum){
	int fd;
    pagenum_t read_info;
	
	if((fd = open(filename, O_RDWR)) < 0){
        perror("file open error for free");
        exit(EXIT_FAILURE);
    }
	
    /*
	//move the pagenum and reset
	lseek(fd, pagenum*PAGE_SIZE, SEEK_SET);
	write(fd, NULL, PAGE_SIZE);
	*/
	//find first free page
	lseek(fd, 0, SEEK_SET);
	if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for free");
        exit(EXIT_FAILURE);
    }
	
	//move the reseted page and write the frist free page
	lseek(fd, pagenum*PAGE_SIZE, SEEK_SET);
	if(write(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file write error for free");
        exit(EXIT_FAILURE);
    }
	
	//write the reseted page number at first free page 
	lseek(fd, 0, SEEK_SET);
	if(write(fd, &pagenum, sizeof(pagenum_t)) < 0){
        perror("file write error for free");
        exit(EXIT_FAILURE);
    }

    if( fsync(fd) < 0){
        perror("filw sync error for free");
        exit(EXIT_FAILURE);
    }
    close(fd);
	return ;
}

void file_read_page(pagenum_t pagenum, page_t* dest){
	int fd;
	pagenum_t read_info;
    int read_info2;
	
	
	if((fd = open(filename, O_RDONLY)) < 0){
		perror("file open error for read");
		exit(EXIT_FAILURE);
	}
	// default read the head page
	lseek(fd, 0, SEEK_SET);
	if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
	dest->freePageNum = read_info;
	if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
	dest->rootPageNum = read_info;
	if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
	dest->numOfPage = read_info;
	

	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		close(fd);
		return;
	}
	//move the pagenum and read page header
	else{
		lseek(fd, pagenum*PAGE_SIZE,SEEK_SET);
		if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
            perror("file read error for read");
            exit(EXIT_FAILURE);
        }
		dest->parentPageNum = read_info;

		if(read(fd, &read_info2, sizeof(int)) < 0){
            perror("file read error for read");
            exit(EXIT_FAILURE);
        }
		dest->isLeaf =read_info2;
		
        if(read(fd, &read_info2, sizeof(int)) < 0){
            perror("file read error for read");
            exit(EXIT_FAILURE);
        }
		dest->numOfKey = read_info2;
		
        lseek(fd, PAGE_SIZE*pagenum + 120, SEEK_SET);
		if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
            perror("file read error for read");
            exit(EXIT_FAILURE);
        }
		dest->rightSibling = read_info;

		if(dest->isLeaf == 1){
			init_info(dest);
			for(int i=0; i<dest->numOfKey; i++){
				if(read(fd, &read_info, sizeof(int64_t)) < 0){
                    perror("file read error for read");
                    exit(EXIT_FAILURE);
                 }
				dest->info[i]->key= read_info;
				
				if(read(fd, dest->info[i]->value, sizeof(char)*VALUE_SIZE) < 0){
					perror("file read error for read");
					exit(EXIT_FAILURE);
				}
			}
		}

		else{
			init_inter_info(dest);
			for(int i=0; i<dest->numOfKey; i++){
				if(read(fd, &read_info, sizeof(int64_t)) < 0){
                    perror("file read error for read");
                    exit(EXIT_FAILURE);
                }
				dest->inter_info[i]->key = read_info;
				
				if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
                    perror("file read error for read");
                    exit(EXIT_FAILURE);
                }
				dest->inter_info[i]->pagenum = read_info;
			}
		}
	}
	
	close(fd);
}



void file_write_page(pagenum_t pagenum, const page_t* src){
	int fd;
	
	if((fd=open(filename, O_WRONLY)) < 0){
		perror("file open error for write");
		exit(EXIT_FAILURE);
	}
    /*
	if(pagenum == header->freePageNum){
		pagenum = file_alloc_page();
		return file_write_page(pagenum, src);
	}
	*/
	
	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		
		if(write(fd, &src->freePageNum, sizeof(pagenum_t)) < 0){
		    perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		if(write(fd, &src->rootPageNum, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
		if(write(fd, &src->numOfPage, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
	}
	//move the pagenum and write page header
	else{
		lseek(fd,PAGE_SIZE*pagenum,SEEK_SET);
		if(write(fd, &src->parentPageNum, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
        if(write(fd, &src->isLeaf, sizeof(int)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }

		if(write(fd,&src->numOfKey, sizeof(int)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
        lseek(fd, PAGE_SIZE*pagenum + 120, SEEK_SET);
		if(write(fd, &src->rightSibling, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
        if(src->isLeaf == 1){	
			for(int i=0; i<src->numOfKey; i++){
			    if(write(fd, &src->info[i]->key,sizeof(int64_t)) < 0){
                   perror("file write error for write");
                   exit(EXIT_FAILURE);
                }
				if(write(fd, src->info[i]->value,sizeof(char)*VALUE_SIZE) < 0){ 
                    perror("file write error for write");
                    exit(EXIT_FAILURE);
                }
		    }
        }
		else{
			for(int i=0; i<INTERNAL_ORDER; i++){
				if(write(fd, &src->inter_info[i]->key, sizeof(int64_t)) <0){
			        perror("file write error for write");
                    exit(EXIT_FAILURE);
                }
				if(write(fd, &src->inter_info[i]->pagenum, sizeof(pagenum_t)) <0){
                    perror("file write error for write");
                    exit(EXIT_FAILURE);
                }

		    }
	    }
    }
    if(fsync(fd) <0){
        perror("file sync error for wrte");
        exit(EXIT_FAILURE);
    }
	close(fd);
    return;
}

void make_file(){
	int fd, isExist;
    pagenum_t pagenum, num;
	isExist = access(filename, 00);

	/*if there is not a file
	  make a file & initailize the file
	  make a head page and default free page*/
	if (isExist != -1){
		return;
	}


	if((fd = open(filename,O_RDWR|O_CREAT,0644)) < 0){
		perror("make file error");
		exit(EXIT_FAILURE);
	}
	if(truncate(filename, 12*PAGE_SIZE) < 0){
        perror("file truncate error for make");
    }
	pagenum = 1;

	lseek(fd, PAGE_SIZE, SEEK_SET);
	for(int i=0; i<DEFAULT_FREE_PAGE; i++){
		pagenum++;		
		if(write(fd, &pagenum, sizeof(pagenum_t)) < 0 ){
            perror("file write error1 for make");
            exit(EXIT_FAILURE);
        }
		lseek(fd, PAGE_SIZE-sizeof(pagenum_t), SEEK_CUR);
	}
	num =0;
	if(write(fd, &num, sizeof(pagenum_t)) < 0){
        perror("file write error2 for make");
        exit(EXIT_FAILURE);
    }

	lseek(fd, 0 , SEEK_SET);
    num = 1;
	if(write(fd, &num, sizeof(pagenum_t)) < 0){
        perror("file write error3 for make");
        exit(EXIT_FAILURE);
    }
    num = 0;
	if(write(fd, &num, sizeof(pagenum_t)) < 0){
        perror("file write error4 for make");
        exit(EXIT_FAILURE);
    }
    pagenum++;
	if(write(fd, &pagenum, sizeof(pagenum_t)) < 0){
        perror("file write error5 for make");
        exit(EXIT_FAILURE);
    }
    if(fsync(fd) < 0){
        perror("file sync error for make");
        exit(EXIT_FAILURE);
    }
	close(fd);

}
page_t* init_page_t(){
	page_t *page;
	
	page = (page_t*) malloc(sizeof(page_t));
	if( page == NULL){
        perror("page creation for init");
        exit(EXIT_FAILURE);
    }
    
    return page;
}

void init_info(page_t *page){
	for(int i =0; i<LEAF_ORDER; i++){
		page->info[i] = (record*)malloc(sizeof(record));
        if(page->info[i] == NULL){
            perror("page info creation for init.");
            exit(EXIT_FAILURE);
        }
	}
	
	for(int i= 0 ; i<LEAF_ORDER; i++){
		page->info[i]->value = (char*)malloc(sizeof(char)*VALUE_SIZE);
	    if(page->info[i]->value == NULL){
            perror("page info value creation for init.");
            exit(EXIT_FAILURE);
        }
    }
}

void init_inter_info(page_t *page){
	for(int i =0; i<INTERNAL_ORDER; i++){
		page->inter_info[i] = (inter_record*)malloc(sizeof(inter_record));
        if(page->inter_info[i] == NULL){
            perror("page inter info creation for init.");
            exit(EXIT_FAILURE);
        }
    }
    return;
}

void free_page_t(page_t *page){
	
	for(int i=0; i<LEAF_ORDER; i++){
		if(page->info[i] != NULL)
			free(page->info[i]);
	}
	for(int i =0; i<INTERNAL_ORDER; i++){
		if(page->inter_info[i] != NULL)
			free(page->inter_info[i]);
	}
	
	if(page !=NULL)
		free(page);

    return ;
}	
