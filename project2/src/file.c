#include "file.h"

//char* filename = (char*)malloc (sizeof(char)* 100);
char* filename;

pagenum_t file_alloc_page(){
	pagenum_t pagenum;
	page_t* header=init_page_t();
	int fd;
	void* store_info =  malloc(sizeof(pagenum_t));
	file_read_page(0,header);
	
	
	if((fd=open(filename, O_RDWR|O_EXCL|O_SYNC|O_DIRECT)) <0){
	    perror("file open error for alloc");
		exit(EXIT_FAILURE);
	}
	lseek(fd,PAGE_SIZE*header->freePageNum,SEEK_SET);
	if(read(fd, store_info, sizeof(pagenum_t)) < 0){
        perror("file read error for alloc");
        exit(EXIT_FAILURE);
    }
	if(store_info == 0){
	
		globalpagenum = header->numOfPage+1;
		lseek(fd,0,SEEK_END);
		
		if(truncate(filename,PAGE_SIZE*(header->numOfPage+11))<0){
            perror("file truncate error for alloc");
            exit(EXIT_FAILURE);
        }
		
		for(int i =0 ; i<DEFAULT_FREE_PAGE; i++){
			globalpagenum++;
            if(write(fd,+globalpagenum, sizeof(pagenum_t))< 0){
                perror("file write error for alloc");
                exit(EXIT_FAILURE);
            }
			lseek(fd, PAGE_SIZE-sizeof(pagenum_t), SEEK_CUR);
		}
		if(write(fd,0, sizeof(pagenum_t)) < 0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
		
		
		lseek(fd, 0, SEEK_SET);
		if(write(fd,(void*)header->numOfPage+1, sizeof(pagenum_t))<0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
		lseek(fd, sizeof(pagenum_t), SEEK_CUR);
		if(write(fd, (void*)globalpagenum,sizeof(pagenum_t)) <0){
            perror("file write error for alloc");
            exit(EXIT_FAILURE);
        }
        
        close(fd);
        return header->freePageNum;
	}
    
    lseek(fd, 0, SEEK_SET);
    write(fd, store_info, sizeof(pagenum_t));
	close(fd);

	return header->freePageNum;
}

void file_free_page(pagenum_t pagenum){
	int fd;
	void *read_info = malloc(sizeof(pagenum_t));
	
	if((fd = open(filename, O_RDWR|O_SYNC|O_DIRECT)) < 0){
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
	if(read(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for free");
        exit(EXIT_FAILURE);
    }
	
	//move the reseted page and write the frist free page
	lseek(fd, pagenum*PAGE_SIZE, SEEK_SET);
	if(write(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file write error for free");
        exit(EXIT_FAILURE);
    }
	
	//write the reseted page number at first free page 
	lseek(fd, 0, SEEK_SET);
	if(write(fd, (void*)pagenum, sizeof(pagenum_t)) < 0){
        perror("file write error for free");
        exit(EXIT_FAILURE);
    }
	
}

void file_read_page(pagenum_t pagenum, page_t* dest){
	int fd,isExist;
	void *read_info = malloc(sizeof(pagenum_t));
	
	
	if((fd = open(filename, O_RDONLY)) < 0){
		perror("file open error for read");
		exit(EXIT_FAILURE);
	}
	// default read the head page
	lseek(fd, 0, SEEK_SET);
	if(read(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
	dest->freePageNum = atoi(read_info);
	if(read(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
	dest->rootPageNum = atoi(read_info);
	if(read(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
	dest->numOfPage = atoi(read_info);
	

	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		free(read_info);
		close(fd);
		return;
	}
	//move the pagenum and read page header
	else{
		lseek(fd, pagenum*PAGE_SIZE,SEEK_SET);
		if(read(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
		dest->parentPageNum = atoi(read_info);
		if(read(fd, read_info, sizeof(int)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    };
		dest->isLeaf =atoi(read_info);
		if(read(fd, read_info, sizeof(int)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
		dest->numOfKey = atoi(read_info);
		lseek(fd, PAGE_SIZE*pagenum + 120, SEEK_SET);
		if(read(fd, read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for read");
        exit(EXIT_FAILURE);
    }
		dest->rightSibling = atoi(read_info);

		if(dest->isLeaf == 1){
			for(int i=0; i<dest->numOfKey; i++){
				if(read(fd, read_info, sizeof(int64_t)) < 0){
                    perror("file read error for read");
                    exit(EXIT_FAILURE);
                 }
				dest->info[i]->key= atoi(read_info);
				
				if(read(fd, dest->info[i]->value, sizeof(char)*VALUE_SIZE) < 0){
					perror("file read error for read");
					exit(EXIT_FAILURE);
				}
			}
		}

		else{
			for(int i=0; i<dest->numOfKey; i++){
				if(read(fd, read_info, sizeof(int64_t)) < 0){
                    perror("file read error for read");
                    exit(EXIT_FAILURE);
                }
				dest->inter_info[i]->key = atoi(read_info);
				
				if(read(fd, read_info, sizeof(pagenum_t)) < 0){
                    perror("file read error for read");
                    exit(EXIT_FAILURE);
                }
				dest->inter_info[i]->pagenum = atoi(read_info);
			}
		}
	}
	free(read_info);
	close(fd);
}



void file_write_page(pagenum_t pagenum, const page_t* src){
	int fd;
	void *store_info = malloc(sizeof(pagenum_t));
	
	page_t* header = init_page_t();

	file_read_page(0, header);

	if((fd=open(filename, O_WRONLY|O_APPEND|O_SYNC|O_DIRECT)) < 0){
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
		
		if(write(fd, (void*)src->freePageNum, sizeof(pagenum_t)) < 0){
		    perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		if(write(fd, (void*)src->rootPageNum, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
		if(write(fd, (void*) src->numOfPage, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
		free(store_info);
		close(fd);
	}
	//move the pagenum and write page header
	else{
		lseek(fd,PAGE_SIZE*pagenum,SEEK_SET);
		//sprintf(store_info, "%lu", src->parentPageNum);
		if(write(fd, (void*)src->parentPageNum, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
        if(write(fd, (void*)src->isLeaf, sizeof(int)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }

		if(write(fd,(void*)src->numOfKey, sizeof(int)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
        lseek(fd, PAGE_SIZE*pagenum + 120, SEEK_SET);
		if(write(fd, (void*)src->rightSibling, sizeof(pagenum_t)) < 0){
            perror("file write error for write");
            exit(EXIT_FAILURE);
        }
		
        if(src->isLeaf == 1){	
			for(int i=0; i<src->numOfKey; i++){
			    if(write(fd, (void*)src->info[i]->key,sizeof(int64_t)) < 0){
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
				if(write(fd, (void*)src->inter_info[i]->key, sizeof(int64_t)) <0){
			        perror("file write error for write");
                    exit(EXIT_FAILURE);
                }
				if(write(fd, (void*)src->inter_info[i]->pagenum, sizeof(pagenum_t)) <0){
                    perror("file write error for write");
                    exit(EXIT_FAILURE);
                }

		    }
	    }
    }
	free(store_info);
	close(fd);
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
        fsync(fd);
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
}	
