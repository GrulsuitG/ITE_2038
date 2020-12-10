#include "lock_table.h"

int init_lock_table() {
	lock_table_latch = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(lock_table_latch,0);
	return 0;
}

int lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode, lock_t* ret_lock, trxList* t) {
	
	lock_t *tmp;
	list *l = hash_find(table_id, key, lock_table);
	if (l == NULL){
		l = hash_add(table_id, key, lock_table);
	}
	ret_lock->pointer = l;
	ret_lock->lock_mode = lock_mode;
	ret_lock->trx_id = trx_id;
	l->lock_num++;
	ret_lock->trx_next = t->lock;
	t->lock = ret_lock;
	
	//lock 오브젝트를 처음 다는 경우
	if(l->lock_num == 1){
		l->head = ret_lock;
		l->tail = ret_lock;
		pthread_mutex_lock(l->mutex);
		ret_lock->get_mutex = true;
		ret_lock->get = true;
		return ACQUIRED;
	}
	//앞에 달린 lock오브젝트가 있는 경우
	else{
		tmp = l->tail;
		l->tail = ret_lock;
		tmp->next = ret_lock;
		ret_lock->prev = tmp;
		//앞 lock 오브젝트의 trx_id 가 자기와 같은 경우
		if(tmp->trx_id == trx_id){
			if(tmp->lock_mode == SHARED && lock_mode == EXCLUSIVE){
				//othertrx_lock(0) -> my_prev_lock(0) -> my_cur_lock(1)
				if(tmp->prev){
					return NEED_TO_WAIT;
				}
				
			}
			// lock_mode 를 exclusive로 업그레이드 해서 List에 달아줌.
			else if(tmp->lock_mode == EXCLUSIVE && lock_mode == SHARED){
				ret_lock->lock_mode = EXCLUSIVE;
				
			}
			pthread_mutex_unlock(l->mutex);
			pthread_mutex_lock(l->mutex);
			ret_lock->get = true;
			return ACQUIRED;
		}
		//앞 lock 오브젝트랑 trx_id가 다른 경우
		else {
			//if(detection(trx_id, tmp->trx_id)){
				//return DEADLOCK;
			//}
			
			//ret_lock의 앞에 달린 lock오브젝트가 shared 나도 shared
			if(tmp->lock_mode == SHARED && lock_mode == SHARED){
					if(tmp->get == false){
						return NEED_TO_WAIT;
					}
					if(tmp->get == true){
						pthread_mutex_unlock(l->mutex);
						pthread_mutex_lock(l->mutex);
						ret_lock->get = true;
						return ACQUIRED;
					}
			}
			//앞에 달린 lock이 shared 내가 exclusive
 			else if(tmp->lock_mode == SHARED && lock_mode == EXCLUSIVE){
  				return NEED_TO_WAIT;
 			}
			//앞에 달린 Lock이 exclusive 인 경우
			else if(tmp->lock_mode == EXCLUSIVE){
  				return NEED_TO_WAIT;
			}
		}
	}
	printf("error\n");
	return -1;
}

int lock_release(lock_t* lock_obj, int aborted) {
	list *l = lock_obj->pointer;
	lock_t *tmp;
	
	
	// 1개 인경우
	if(l->lock_num == 1){
		l->head = NULL;
		l->tail = NULL;
		
	}
	// 마지막인경우
	else if(l->tail == lock_obj ){
		l->tail = lock_obj->prev;
		lock_obj->prev->next = NULL;
	}
	// 여러개 중 list의 가장 첫번째 인 경우
	else if(l->head == lock_obj){
		tmp = lock_obj->next;
		l->head =tmp;
		tmp->prev = NULL;
		
		if(aborted ==1){
			tmp->prev_lock_aborted = true;
		}
		pthread_cond_signal(lock_obj->cond);
	}
	// list 중간에 있는 경우
	else{
		lock_obj->next->prev = lock_obj->prev;
		lock_obj->prev->next = lock_obj->next;
		if(aborted ==1){
			lock_obj->next->prev_lock_aborted = true;
		}
		pthread_cond_signal(lock_obj->cond);
	}
	
	if(lock_obj->get_mutex)
		pthread_mutex_unlock(l->mutex);
	
 	l->lock_num--;
	pthread_cond_destroy(lock_obj->cond);
	free(lock_obj->cond);
	free(lock_obj->stored);
	free(lock_obj);
	return 0;
}

void lock_wait(lock_t *lock_obj){
	return ;
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
	node->get = false;
	node->prev_lock_aborted = false;
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
	l->lock_num =0;
	l->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l->mutex, 0);
	l->link = NULL;
	return l;
}
int hash_function(int table_id, int64_t key){
	return (table_id+(9*key)) % TABLE_SIZE;
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
