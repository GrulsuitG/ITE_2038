#include "db.h"


int init_db(int num_buf, int flag, int log_num, char* log_path, char* logmsg_path){
	int i;
	if(num_buf <1)
		return -1;
	if(init)
		return -1;
	if(index_init(num_buf))
		return -1;
	for(i =1; i<= MAX_TABLE_NUM; i++)
		tableList[i].is_open = false;
	init = true;
	init_trx();
	init_lock_table();
	init_log();
	
	recovery(flag, log_num, log_path, logmsg_path);
	
	return 0;
}

int open_table(char* pathname){
	int i, id;
	char *name;
	if(!init)
		return -1;
	if(strncmp(pathname, "DATA", 4) != 0){
		return -1;
	}
	name = pathname+4;
	id = atoi(name);
	if(id >10 || id<1){
		return -1;
	}
	if(tableList[id].is_open)
		return id;
	else{
		tableList[id].fd = index_open(id, pathname);
		tableList[id].is_open = true;
		return id;
	}

    return 0;
}

int db_insert(int table_id, int64_t key, char* value){
  	if(!init){
		return -1;
  	}
  	if(!tableList[table_id].is_open){
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
  	
  	if(!tableList[table_id].is_open){
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
  	trxList* t;
	int result;
	

  	if(!init || !tableList[table_id].is_open){
		trx_abort(trx_id);
		return -1;
	}
  	
	t = trx_hash_find(trx_id, trx_table);
	if(t == NULL || t->init == false){
		trx_abort(trx_id);
		return -1;
	}
	
	result = find_record(table_id, key, trx_id, SHARED ,t, ret_val);
	if(result == FAIL){
		return -1;
	}
	return 0;
}
int db_update(int table_id, int64_t key, char *values, int trx_id){
	trxList* t;
	int result;
	
  	if(!init || !tableList[table_id].is_open){
		trx_abort(trx_id);
		return -1;
	}
  	
	t = trx_hash_find(trx_id, trx_table);
	if(t == NULL || t->init == false){
		trx_abort(trx_id);
		return -1;
	}
	result = find_record(table_id, key, trx_id, EXCLUSIVE, t, values);
	if(result == FAIL){
		return -1;
	}

	return 0;
	
}


int close_table(int table_id){
	if(table_id < 1 || table_id >10){
		return -1;
	}
	if(tableList[table_id].is_open){
		if(index_close(table_id)){
			return -1;
		}
		else{
			tableList[table_id].is_open = false;
			return 0;
		}
	}
	return -1;
}

int shutdown_db(){
	int i ;
	for(i = 1; i<= MAX_TABLE_NUM; i++){
		if(tableList[i].is_open){
			if(close_table(i)){
				return -1;
			}
		}
	}
	buf_destroy();
	init = false;
	return 0;
}
