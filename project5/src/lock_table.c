#include "lock_table.h"

int init_lock_table() {
	pthread_mutex_init(&lock_table_latch,0);

	return 0;
}

lock_t* lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode) {
	lock_t *obj = lock_make_node();
	
	lock_t *tmp;
	list *l;
	int flag;
	pthread_mutex_lock(&lock_table_latch);
	l = hash_find(table_id, key, lock_table);
	if (l == NULL){
		l = hash_add(table_id, key, lock_table);
	}
	obj->pointer = l;
	obj->lock_mode = lock_mode;
	obj->trx_id = trx_id;
	tmp = l->tail;
	if(l->head ==NULL){
		l->head = obj;
		l->tail = obj;
		obj->get = true;
		pthread_mutex_lock(l->mutex);
	}
	else if(tmp != NULL && tmp->trx_id == trx_id){
		if(tmp->lock_mode ==0){
			if(lock_mode ==1){
				if(tmp->prev){
					pthread_cond_wait(tmp->prev->cond, &lock_table_latch);
					pthread_mutex_lock(l->mutex);
				}
				obj->get = true;
			}
		}
		tmp->next = obj;
		obj->prev = tmp;
		l->tail = obj;
	}
	else if(l->head){
		
		if(detection(trx_id, tmp->trx_id, &flag)){
			pthread_mutex_unlock(&lock_table_latch);
			if(flag == 1)
				return lock_acquire(table_id, key, trx_id, lock_mode);
			else
				return NULL;
		}
		tmp->next = obj;
		
		obj->prev = tmp;
		l->tail = obj;
		if(lock_mode == 0){
			if(tmp->lock_mode == 0){
				if(tmp->get){
					obj->get = true;
				}
				else if (tmp->get == false){
					pthread_cond_wait(tmp->cond, &lock_table_latch);
					obj->get = true;
					tmp = obj->next;
					if(tmp && tmp->lock_mode == 0)
						pthread_cond_signal(obj->cond);
				}
			}
			else if(tmp->lock_mode == 1){
				pthread_cond_wait(tmp->cond, &lock_table_latch);
				pthread_mutex_lock(l->mutex);
				obj->get = true;
				tmp = obj->next;
				if( tmp && tmp->lock_mode == 0)
					pthread_cond_signal(obj->cond);
			}
		}
		else if(lock_mode == 1){
			pthread_cond_wait(tmp->cond, &lock_table_latch);
			pthread_mutex_lock(l->mutex);
			obj->get = true;
		}
	}
	pthread_mutex_unlock(&lock_table_latch);
	return obj;
}

int lock_release(lock_t* lock_obj) {
	list *l = lock_obj->pointer;
	pthread_mutex_lock(&lock_table_latch);
	if(l->head == l->tail){
		l->head = NULL;
		l->tail = NULL;
	}
	else if(l->head != l->tail ){
		l->head= lock_obj->next;
		if(l->head)
			l->head->prev = NULL;
		pthread_cond_signal(lock_obj->cond);
			
	}
	pthread_mutex_unlock(l->mutex);
 	pthread_mutex_unlock(&lock_table_latch);
	free(lock_obj->cond);
	free(lock_obj->stored);
	free(lock_obj);
	return 0;
}

lock_t* lock_make_node(){
	lock_t *node = (lock_t*) malloc(sizeof(lock_t));
	if(node == NULL){
		perror("make lock_t error");
		exit(EXIT_FAILURE);
	}
	node->stored = (char*) malloc(VALUE_SIZE);
	node->cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	pthread_cond_init(node->cond,0);
	node->pointer = NULL;
	node->next = NULL;
	node->prev = NULL;

	return node;
}
list* make_list(int table_id, int64_t key){
	list *l = (list*)malloc(sizeof(list));
	if(l == NULL){
		perror("make list error");
		exit(EXIT_FAILURE);
	}
	l->table_id = table_id;
	l->key = key;
	l->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l->mutex, 0);
	l->link = NULL;
	return l;
}
int hash_function(int table_id, int64_t key){
	return (table_id+(5*key)) % TABLE_SIZE;
}

list* hash_add(const int table_id, const int64_t key, list *ht[]){
	int hash_value = hash_function(table_id,key);
	list * ptr = make_list(table_id, key);
	if(ht[hash_value] == NULL)
		ht[hash_value] = ptr;
	else{
		ptr->link = ht[hash_value];
		ht[hash_value] = ptr;
	}
	
	return ptr;
}
list* hash_find(int table_id, int64_t key, list *ht[]){
	list *node;
	int hash_value = hash_function(table_id, key);
	for(node = ht[hash_value]; node; node = node->link){
		if( node->table_id == table_id && node->key == key){
			return node;
		}
	}
	return NULL;
}
void hash_chain_print(struct list *ht[]){
	struct list *node;
	for(int i = 0; i < TABLE_SIZE; i++){
		printf("[%d]-> ",i);
		for(node = ht[i]; node; node = node->link){
			printf("(%d, %ld)-> ",node->table_id, node->key);
		}
		printf("\n");
	}
}
