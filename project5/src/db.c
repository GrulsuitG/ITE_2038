#include "db.h"


int init_db(int num_buf){
	int i;
	if(num_buf <1)
		return -1;
	if(init)
		return -1;
	if(index_init(num_buf))
		return -1;
	for(i =0; i< MAX_TABLE_NUM; i++)
		tableList[i].is_open = false;
	init = true;
	unique_id = 1;
	init_trx();
	return 0;
}

int open_table(char* pathname){
	int i;
	char* name;
	if(!init)
		return -1;
		
	if(strlen(pathname) > MAX_PATH_LENGTH)
		return -1;

	if(unique_id> MAX_TABLE_NUM){
		return -1;
	}

    for(i=0; i<(unique_id-1); i++){    	
        if( strcmp(pathname, tableList[i].name) == 0){
		if(!tableList[i].is_open){
			tableList[i].is_open = true;
			tableList[i].fd =index_open(i+1, tableList[i].name);
		}
            return i+1;
        }
     }
     
   tableList[unique_id-1].fd =index_open(unique_id, pathname);
    
    tableList[unique_id-1].name = (char*)malloc(strlen(pathname));
    strncpy(tableList[unique_id-1].name , pathname, strlen(pathname));
    
	tableList[unique_id-1].is_open = true;
    return unique_id++;
}

int db_insert(int table_id, int64_t key, char* value){
  	if(!init){
		return -1;
  	}
  	if(!tableList[table_id-1].is_open){
		return -1;
	}
  	pagenum_t root;

    if(find(table_id, &root, key) != NULL){        
        return -1;
    }
	insert(table_id, root, key, value);
    return 0;
}

int db_delete(int table_id, int64_t key){
	if(!init)
		return -1;
  	
  	if(!tableList[table_id-1].is_open){
		return -1;
	}
   	
	pagenum_t root;

  	if(find(table_id, &root, key) == NULL){ 
        return -1;
    }
   	delete(table_id, root,key);
   	
    return 0;
}

int db_find(int table_id, int64_t key, char *ret_val, int trx_id){
  	if(!init){
		trx_abort(trx_id);
		return -1;
	}
  	
  	if(!tableList[table_id-1].is_open){
		trx_abort(trx_id);
		return -1;
	}
    trxList* t;
	t = trx_hash_find(trx_id, trx_table);
	if(t == NULL){
		trx_abort(trx_id);
		return -1;
	}
	if(t->init == false){
		trx_abort(trx_id);
		return -1;
		}	
	page_t *p;
	record *r;
    lock_t *l;
	int index;
    p = find_page(table_id, key);
    if(p == NULL){
		trx_abort(trx_id);
        return -1;
    }
	index = find_place(table_id, p->mypage);
	for(int i=0; i<p->num_keys; i++){
		if(p->keys[i] == key){
			r = p->record[i];
			l = lock_acquire(table_id, key, trx_id, 0);
			if(l == NULL){
				pthread_mutex_unlock(block[index]->page_latch);
				trx_abort(trx_id);
				return -1;
			}
				l->trx_next = t->lock;
				t->lock = l;
				l->pagenum = p->mypage;
			pthread_mutex_unlock(block[index]->page_latch);
		}
	}
	if(r ==NULL){
		pthread_mutex_unlock(block[index]->page_latch);
		trx_abort(trx_id);
		return -1;
	}
    else{
		pthread_mutex_lock(block[index]->page_latch);
        strncpy(ret_val, r->value, VALUE_SIZE);
		pthread_mutex_unlock(block[index]->page_latch);
    	return 0;
	}

}
int db_update(int table_id, int64_t key, char *values, int trx_id){
  	if(!init){
		trx_abort(trx_id);
		return -1;
	}
  	
  	if(!tableList[table_id-1].is_open){
		trx_abort(trx_id);
		return -1;
	}
    trxList* t;
	t = trx_hash_find(trx_id, trx_table);
	if(t == NULL){
		trx_abort(trx_id);
		return -1;
	}
	if(t->init == false){
		trx_abort(trx_id);
		return -1;
	}
	page_t *p;
	record *r;
    lock_t *l;
	int index;
    p = find_page(table_id, key);
    if(p == NULL){
		trx_abort(trx_id);
        return -1;
    }
	index = find_place(table_id, p->mypage);
	for(int i=0; i<p->num_keys; i++){
		if(p->keys[i] == key){
			r = p->record[i];
			l = lock_acquire(table_id, key, trx_id, 1);
			if(l == NULL ){
				pthread_mutex_unlock(block[index]->page_latch);
				trx_abort(trx_id);
				return -1;
			}
				l->trx_next = t->lock;
				t->lock = l;
				l->pagenum = p->mypage;
			pthread_mutex_unlock(block[index]->page_latch);
		}
	}
	if(r == NULL){
		trx_abort(trx_id);
		return -1;
	}
	else{
		p = buf_read_page(table_id,l->pagenum);
		strncpy(l->stored, r->value, VALUE_SIZE);
		strncpy(r->value, values, VALUE_SIZE);
		buf_return_page(table_id, l->pagenum, 1);
		return 0;
	}
	
}


int close_table(int table_id){
	if(table_id < 1 || table_id >10){
		return -1;
	}
	if(tableList[table_id-1].is_open){
		if(index_close(table_id)){
			return -1;
			}
		else{
			tableList[table_id-1].is_open = false;
			return 0;
		}
	}
	return -1;
}

int shutdown_db(){
	int i ;
	for(i = 0; i< MAX_TABLE_NUM; i++){
		if(tableList[i].is_open){
			if(close_table(i+1)){
				return -1;
			}
		}
	}
	buf_destroy();
	init = false;
	return 0;
}
