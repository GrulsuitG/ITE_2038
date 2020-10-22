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
    if(filename == NULL)
        return -1;
    	
    if(find(root, key) != NULL){        
        
        return -1;
    }
    insert(root, key, value);
	
    return 0;
}

int db_delete(int64_t key){
   pagenum_t *root = malloc(sizeof(pagenum_t));
    if(filename == NULL){
        return -1;
    }

  	if(find(root, key) == NULL){ 
        return -1;
    }
   	delete(root,key);
   	
    return 0;
}

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
