#include "file.h"

table tableList[MAX_TABLE_NUM];

pagenum_t file_alloc_page(int table_id){
	pagenum_t pagenum, num;
	page_t* header=init_page();
	char* filename = tableList[table_id-1].name;
	int fd;
	pagenum_t read_info;
	file_read_page(table_id, 0,header);
	if((fd=open(filename, O_RDWR|O_SYNC)) <0){
	    perror("file open error for alloc");
		exit(EXIT_FAILURE);
	}
	if(header->freePageNum == 0){
	
		pagenum = header->numOfPage;
		
        // file size up
		if(truncate(filename,PAGE_SIZE*(header->numOfPage+DEFAULT_FREE_PAGE))<0){
            perror("file truncate error for alloc");
            exit(EXIT_FAILURE);
        }
		//write next free page num at new pages
		for(int i =1 ; i<DEFAULT_FREE_PAGE; i++){
			lseek(fd, PAGE_SIZE*pagenum, SEEK_SET);
			pagenum++;
            if(write(fd,&pagenum, sizeof(pagenum_t))< 0){
                perror("file write error 1 for alloc");
                exit(EXIT_FAILURE);
            }
			
		}
		lseek(fd, PAGE_SIZE*pagenum, SEEK_SET);
		pagenum++;
        num = 0;
		if(write(fd, &num, sizeof(pagenum_t)) < 0){
            perror("file write error 2 for alloc");
            exit(EXIT_FAILURE);
        }
		
		//write free pagen num
		lseek(fd, 0, SEEK_SET);
        num = header->numOfPage;
		if(write(fd ,&num, sizeof(pagenum_t))<0){
            perror("file write error 3 for alloc");
            exit(EXIT_FAILURE);
        }
        //write total page num
		lseek(fd, sizeof(pagenum_t), SEEK_CUR);
		if(write(fd, &pagenum,sizeof(pagenum_t)) <0){
            perror("file write error 4 for alloc");
            exit(EXIT_FAILURE);
        }
        
        return file_alloc_page(table_id);
        
	}
    //write next free page num
    else{
        lseek(fd, header->freePageNum*PAGE_SIZE, SEEK_SET);
        if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
            perror("file write error 4 for alloc");
            exit(EXIT_FAILURE);
        }
        lseek(fd,0, SEEK_SET);
        if(write(fd, &read_info,sizeof(pagenum_t)) <0){
            perror("file write error 5 for alloc");
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

void file_free_page(int table_id, pagenum_t pagenum){
	int fd;
    pagenum_t read_info;
	char* filename = tableList[table_id-1].name;
	if((fd = open(filename, O_RDWR)) < 0){
        perror("file open error for free");
        exit(EXIT_FAILURE);
    }
    
	//find first free page
	lseek(fd, 0, SEEK_SET);
	if(read(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file read error for free");
        exit(EXIT_FAILURE);
    }
	
	//move the reseted page and write the frist free page
	lseek(fd, pagenum*PAGE_SIZE, SEEK_SET);
	if(write(fd, &read_info, sizeof(pagenum_t)) < 0){
        perror("file write error 2 for free");
        exit(EXIT_FAILURE);
    }
	
	//write the reseted page number at first free page 
	lseek(fd, 0, SEEK_SET);
	if(write(fd, &pagenum, sizeof(pagenum_t)) < 0){
        perror("file write error 3 for free");
        exit(EXIT_FAILURE);
    }

    if( fsync(fd) < 0){
        perror("filw sync error for free");
        exit(EXIT_FAILURE);
    }
    close(fd);
	return ;
}

void file_read_page(int table_id, pagenum_t pagenum, page_t* dest){
	int fd;
	char* filename = tableList[table_id-1].name;
	
	if((fd = open(filename, O_RDONLY)) < 0){
		perror("file open error for read");
		exit(EXIT_FAILURE);
	}
	dest->mypage = pagenum;
// default read the headpage
	lseek(fd, 0, SEEK_SET);
	if(read(fd, &dest->freePageNum, sizeof(pagenum_t)) < 0){
        perror("file read error 1 for read");
        exit(EXIT_FAILURE);
    }
	
	if(read(fd, &dest->rootPageNum, sizeof(pagenum_t)) < 0){
        perror("file read error 2 for read");
        exit(EXIT_FAILURE);
    }
	
	if(read(fd, &dest->numOfPage, sizeof(pagenum_t)) < 0){
        perror("file read error 3 for read");
        exit(EXIT_FAILURE);
    }
	
	

	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		close(fd);
		return;
	}
	//move the pagenum and read page header
	else{
		lseek(fd, pagenum*PAGE_SIZE,SEEK_SET);
		if(read(fd, &dest->parentPageNum, sizeof(pagenum_t)) < 0){
            perror("file read error 4 for read");
            exit(EXIT_FAILURE);
        }
		

		if(read(fd, &dest->is_leaf, sizeof(int)) < 0){
            perror("file read error 5 for read");
            exit(EXIT_FAILURE);
        }
		
		
        if(read(fd, &dest->num_keys, sizeof(int)) < 0){
            perror("file read error 6 for read");
            exit(EXIT_FAILURE);
        }
		
		
        lseek(fd, PAGE_SIZE*pagenum + 120, SEEK_SET);
		if(read(fd, &dest->pointer, sizeof(pagenum_t)) < 0){
            perror("file read error 7 for read");
            exit(EXIT_FAILURE);
        }

		if(dest->is_leaf == 1){
			
			for(int i=0; i<dest->num_keys; i++){
				if(read(fd, &dest->keys[i], sizeof(int64_t)) < 0){
                    perror("file read error 8 for read");
                    exit(EXIT_FAILURE);
                 }
				
				
				if(read(fd, dest->record[i]->value, sizeof(char)*VALUE_SIZE) < 0){
					perror("file read error 9 for read");
					exit(EXIT_FAILURE);
				}
				//strncpy(dest->record[i], read_info,VALUE_SIZE);
			}
		}

		else{
			for(int i=0; i<dest->num_keys; i++){
				if(read(fd, &dest->keys[i], sizeof(int64_t)) < 0){
                    perror("file read error 10 for read");
                    exit(EXIT_FAILURE);
                }
				
				if(read(fd, &dest->pagenum[i] , sizeof(pagenum_t)) < 0){
                    perror("file read error 11 for read");
                    exit(EXIT_FAILURE);
                }
			}
		}
	}
	
	close(fd);
	return;
}



void file_write_page(int table_id, pagenum_t pagenum, const page_t* src){
	int fd;
	char* filename = tableList[table_id-1].name;

	if((fd=open(filename, O_WRONLY)) < 0){
		perror("file open error for write");
		exit(EXIT_FAILURE);
	}
	//if the pagenum is 0, that is headpage
	if(pagenum ==0){
		
		if(write(fd, &src->freePageNum, sizeof(pagenum_t)) < 0){
		    perror("file write error 1 for write");
            exit(EXIT_FAILURE);
        }
		if(write(fd, &src->rootPageNum, sizeof(pagenum_t)) < 0){
            perror("file write error 2 for write");
            exit(EXIT_FAILURE);
        }
		if(write(fd, &src->numOfPage, sizeof(pagenum_t)) < 0){
            perror("file write error 3 for write");
            exit(EXIT_FAILURE);
        }
	}
	//move the pagenum and write page header
	else{
		lseek(fd,PAGE_SIZE*pagenum,SEEK_SET);
		if(write(fd, &src->parentPageNum, sizeof(pagenum_t)) < 0){
            perror("file write error 4 for write");
            exit(EXIT_FAILURE);
        }
        if(write(fd, &src->is_leaf, sizeof(int)) < 0){
            perror("file write error 5 for write");
            exit(EXIT_FAILURE);
        }
		if(write(fd,&src->num_keys, sizeof(int)) < 0){
            perror("file write error 6 for write");
            exit(EXIT_FAILURE);
        }
        lseek(fd, PAGE_SIZE*pagenum + 120, SEEK_SET);
		if(write(fd, &src->pointer, sizeof(pagenum_t)) < 0){
            perror("file write error 7 for write");
            exit(EXIT_FAILURE);
        }
        if(src->is_leaf == 1){	
			for(int i=0; i<src->num_keys; i++){
			    if(write(fd, &src->keys[i],sizeof(int64_t)) < 0){
                   perror("file write error 8 for write");
                   exit(EXIT_FAILURE);
                }
				if(write(fd, src->record[i]->value,VALUE_SIZE) < 0){
                    perror("file write error 9 for write");
                    exit(EXIT_FAILURE);
                }
		    }
        }
		else{
			for(int i=0; i<src->num_keys; i++){
				if(write(fd, &src->keys[i], sizeof(int64_t)) <0){
			        perror("file write error 10 for write");
                    exit(EXIT_FAILURE);
                }
                fsync(fd);
				if(write(fd, &src->pagenum[i], sizeof(pagenum_t)) <0){
                    perror("file write error 11 for write");
                    exit(EXIT_FAILURE);
                }
                fsync(fd);
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

void file_write_root(int table_id, pagenum_t pagenum){
	char* filename = tableList[table_id-1].name;
	
	int fd;
	
	if((fd=open(filename, O_WRONLY)) < 0){
		perror("file open error for write root");
		exit(EXIT_FAILURE);
	}
	lseek(fd, sizeof(pagenum_t), SEEK_SET);
	if(write(fd, &pagenum, sizeof(pagenum_t)) < 0){
		perror("file write error for write root");
		exit(EXIT_FAILURE);
	}
	if(fsync(fd) <0){
        perror("file sync error for wrte root");
        exit(EXIT_FAILURE);
    }
	close(fd);
    return;
}

void make_file(char* filename){
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
            perror("file write error 1 for make");
            exit(EXIT_FAILURE);
        }
		lseek(fd, PAGE_SIZE-sizeof(pagenum_t), SEEK_CUR);
	}
	num =0;
	if(write(fd, &num, sizeof(pagenum_t)) < 0){
        perror("file write error 2 for make");
        exit(EXIT_FAILURE);
    }

	lseek(fd, 0 , SEEK_SET);
    num = 1;
	if(write(fd, &num, sizeof(pagenum_t)) < 0){
        perror("file write error 3 for make");
        exit(EXIT_FAILURE);
    }
    num = 0;
	if(write(fd, &num, sizeof(pagenum_t)) < 0){
        perror("file write error 4 for make");
        exit(EXIT_FAILURE);
    }
    pagenum++;
	if(write(fd, &pagenum, sizeof(pagenum_t)) < 0){
        perror("file write error 5 for make");
        exit(EXIT_FAILURE);
    }
    if(fsync(fd) < 0){
        perror("file sync error for make");
        exit(EXIT_FAILURE);
    }
	close(fd);

}

int* get_freelist(int table_id){
	page_t *header=init_page();
	pagenum_t pagenum;
	char* filename = tableList[table_id-1].name;
	int fd;
	int *list;
	
	file_read_page(table_id, 0, header);
	list = malloc(sizeof(int) * header->numOfPage);
	
	if((fd = open(filename, O_RDONLY)) <0){
		perror("file open error for get freelist");
		exit(EXIT_FAILURE);
	}
	pagenum = header->freePageNum;
	while(pagenum != 0){
		list[pagenum] = 1;
		lseek(fd, PAGE_SIZE * pagenum, SEEK_SET);
		if(read(fd,&pagenum, sizeof(pagenum_t)) < 0){
			perror("file read error for get freelist");
			exit(EXIT_FAILURE);
		}
	}
	
	return list;
}

page_t* init_page(){
	page_t *page;
	int i;
	page = malloc(sizeof(page_t));
	if( page == NULL){
        perror("page creation for init");
        exit(EXIT_FAILURE);
    }
    
	page->keys = malloc((INTERNAL_ORDER-1) * sizeof(int64_t));
    if(page->keys == NULL){
    	perror("page keys creation for init.");
		exit(EXIT_FAILURE);
	}
	memset(page->keys, 0, sizeof(page->keys));
	
	page->record = (record**)malloc(sizeof(record*) * (LEAF_ORDER-1));
	if(page->record == NULL){
		perror("page record creation for init");
		exit(EXIT_FAILURE);
	}
	for(i=0; i<LEAF_ORDER-1; i++){
		page->record[i] = (record*)malloc(sizeof(record));
	    if(page->record[i] == NULL){
            perror("page record creation for init.");
            exit(EXIT_FAILURE);
        }
        memset(page->record[i], 0, sizeof(page->record[i]));
	}

	for(i= 0 ; i<LEAF_ORDER-1; i++){
		page->record[i]->value = (char*)malloc(VALUE_SIZE);
	    if(page->record[i]->value == NULL){
            perror("page record creation for init.");
            exit(EXIT_FAILURE);
        }
        memset(page->record[i]->value, 0, sizeof(page->record[i]->value));
    }	
	
	page->pagenum = malloc(sizeof(pagenum_t) * (INTERNAL_ORDER -1));
    if(page->pagenum == NULL){
    	perror("page info creation for init.");
		exit(EXIT_FAILURE);
	}
	memset(page->pagenum, 0, sizeof(page->pagenum));
	return page;
}

void free_page(page_t *page){
	int i;
	for(i=0; i<LEAF_ORDER-1; i++){
			//free(page->record[i]->value);
			//free(page->record[i]);
	}
	
	free(page->record);
	
	free(page->pagenum);
	free(page->keys);
	free(page);

    return ;
}	

