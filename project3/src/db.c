#include "db.h"

int open_table(char* pathname){
	int i;
	char* name;
	
	if(table_name[0] == NULL){
        unique_id =0;
    }
    for(i=0; i<unique_id; i++){    	
        if( strcmp(pathname, table_name[i]) == 0){
            filename = table_name[i];
            return i;
        }
     }
     
    name = (char*)malloc(sizeof(pathname));
    strncpy(name, pathname, sizeof(pathname));
	filename = name;
    make_file();
    
    table_name[unique_id] = (char*)malloc(sizeof(pathname));
    strncpy(table_name[unique_id] , pathname, sizeof(pathname));
    

    return unique_id++;
}

int db_insert(int64_t key, char* value){
  	pagenum_t *root = malloc(sizeof(pagenum_t));
  	//pagenum_t root= 0;
    if(filename == NULL)
        return -1;
    	
    if(find(root, key) != NULL){        
        
        return -1;
    }
	//printf("db : %d\n", *root);
    insert(root, key, value);
	
    return 0;
}
/*
int db_delete(int64_t key){
    node *n;
    
    page_t *page = init_page();
    page_t *header = init_page();
	pagenum_t pagenum;
    if(filename == NULL){
        return -1;
    }

  	if(find(root, key, false) == NULL){ 
        return -1;
    }
   	root = delete(root,key);
   	while( queue!= NULL){
        n = dequeue();
        if(n->num_keys == 0){
        	file_free_page(n->pagenum);
        }
		else{    
	        page = node_to_page(n);
	       	file_write_page(n->pagenum, page);
		}
	}
    file_read_page(0,header);
	if(root == NULL || root->pagenum != header->rootPageNum ){		
		header->rootPageNum = root ? root->pagenum : 0;
		file_write_page(0, header);	
	}
    return 0;
}
*/
int db_find(int64_t key, char *ret_val){
    record *r;
    pagenum_t *root = malloc(sizeof(pagenum_t));
    
    if(filename == NULL){
        return -1;
    }

    r = find(root, key);
    if(r == NULL){
        return -1;
    }

    else
        strncpy(ret_val, r->value, VALUE_SIZE);
    return 0;

}
