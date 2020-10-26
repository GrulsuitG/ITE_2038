#include "db.h"

int init_db(int num_buf){
	if(!make_buf(num_buf))
		return -1;
	
	return 0;
}

int open_table(char* pathname){
	int i;
	char* name;
	
	if(strlen(pathname) > MAX_PATH_LENGTH)
		return -1;

	if(table_name[0] == NULL){
        unique_id =1;
    }

	if(unique_id> MAX_TABLE_NUM){
		return -1;
	}

    for(i=0; i<(unique_id-1); i++){    	
        if( strcmp(pathname, table_name[i]) == 0){
            return i;
        }
     }
     
    make_file(pathname);
    
    table_name[unique_id-1] = (char*)malloc(sizeof(pathname));
    strncpy(table_name[unique_id-1] , pathname, sizeof(pathname));
    

    return unique_id++;
}

int db_insert(int table_id, int64_t key, char* value){
  	if(table_id >= unique_id){
		return -1;
	}
	pagenum_t *root = malloc(sizeof(pagenum_t));
    	
    if(find(table_id, root, key) != NULL){        
        
        return -1;
    }
    insert(table_id, root, key, value);
	
    return 0;
}

int db_delete(int table_id, int64_t key){
	if(table_id >= unique_id){
		return -1;
	}
   	
	pagenum_t *root = malloc(sizeof(pagenum_t));

  	if(find(table_id, root, key) == NULL){ 
        return -1;
    }
   	delete(table_id, root,key);
   	
    return 0;
}

int db_find(int table_id, int64_t key, char *ret_val){
  	if(table_id >= unique_id){
		return -1;
	}
    
	record *r;
    pagenum_t *root = malloc(sizeof(pagenum_t));
    

    r = find(table_id, root, key);
    if(r == NULL){
        return -1;
    }

    else
        strncpy(ret_val, r->value, VALUE_SIZE);
    return 0;

}
