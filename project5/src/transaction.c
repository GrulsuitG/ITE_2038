#include "transaction.h"
void init_trx(){
	global_trx_id = 1;
	active_num =0;
	pthread_mutex_init(&trx_manager_latch,0);
}

int trx_begin(){
	pthread_mutex_lock(&trx_manager_latch);
	trxList* t = trx_hash_add(global_trx_id, trx_table);
	t->id = global_trx_id;
	t->init = true;
	active_num++;
	pthread_mutex_unlock(&trx_manager_latch);
	return global_trx_id++;
}

int trx_commit(int trx_id){
	pthread_mutex_lock(&trx_manager_latch);
	int index = trx_hash(trx_id);
	trxList* t = trx_hash_find(trx_id, trx_table);
	trxList* tempt;
	if(t == NULL){
		pthread_mutex_unlock(&trx_manager_latch);
		return 0;
	}
	if(t->init == false){
		pthread_mutex_unlock(&trx_manager_latch);
		return 0;
	}
	
	lock_t *l, *temp;
	l = t->lock;
	while(l != NULL){
		temp = l;
		l = l->trx_next;
		lock_release(temp);
	}
	t->init = false;
	tempt= trx_table[index];
	if(tempt != t){
		while(tempt->link != t)
			tempt = tempt->link;
	}
	tempt->link = t->link;
	active_num--;
	free(t);
	pthread_mutex_unlock(&trx_manager_latch);
	return trx_id;

}

trxList* trx_make_list(){
	trxList *t = (trxList*)malloc(sizeof(trxList));
	if( t== NULL){
		perror("make trxList error");
		exit(EXIT_FAILURE);
	}
	return t;
}

int trx_hash(int trx_id){
	return trx_id % TRX_TABLE_SIZE;
}

trxList* trx_hash_add(int trx_id, trxList *ht[]){
	int index = trx_hash(trx_id);
	trxList* l = trx_make_list();
	if(ht[index] ==NULL)
		ht[index] = l;
	else{
		l->link = ht[index];
		ht[index] = l;
	}
	
	return l;
}

trxList* trx_hash_find(int trx_id, trxList *ht[]){
	trxList *node;
	int index = trx_hash(trx_id);
	for(node = ht[index]; node; node = node->link){
		if(node->id == trx_id)
			return node;
	}
	return NULL;
}

bool detection(int trx_id, int wait, int *flag){
	pthread_mutex_lock(&trx_manager_latch);
	trxList* t;
	lock_t *cur, *prev;
	int i, id = 0;
	int active[global_trx_id];
	int visit[active_num];
	if(active_num <2){
		*flag = 1;
		pthread_mutex_unlock(&trx_manager_latch);
		return true;
	}
	bool result = false;
	for(i =0; i<global_trx_id; i++){
		t = trx_hash_find(i, trx_table);
		if(t && t->init)
			active[i] = id++;
	}
	graph = (int**)malloc(sizeof(int*)*active_num);
	for(i = 0; i<active_num; i++){
		graph[i] = (int*)malloc(sizeof(int)*active_num);
	}
	for(i =1; i<=active_num; i++){
		t = trx_hash_find(i, trx_table);
		if(t){
			cur = t->lock;
			while(cur){
					prev = cur->prev;
					if(prev){
						graph[active[cur->trx_id]][active[prev->trx_id]] = 1;	
					}
				cur = cur->trx_next;
			}
		}
	}
	graph[active[trx_id]][active[wait]] =1;
/*	for(i=0; i<active_num; i++){
		for(int j=0; j<active_num; j++)
			printf("%d ", graph[i][j]);
		printf("\n");
	}*/
//	printf("%d\n", active_num);
//	printf("%d %d\n", trx_id, wait);
	for(i = 0; i<active_num; i++){
		if(dfs(i,visit)){
		//	printf("%d->%d cycle\n", trx_id, wait);
			result = true;
			break;	
		}
		for(int j=0; j<active_num; j++){
			visit[j] =0;
		}
	}
	for(i =0; i<active_num; i++){
		free(graph[i]);
	}
	free(graph);
	pthread_mutex_unlock(&trx_manager_latch);
	return result;
}

bool dfs(int v, int visit[]){
	if(visit[v] ==1)
		return true;
	visit[v] = 1;
	for(int i =0; i<active_num; i++){
		if(graph[v][i] ==1)
			if(dfs(i,visit))
				return true;
	}
	return false;
}

void trx_abort(int trx_id){
//	printf("abort\n");
	pthread_mutex_lock(&trx_manager_latch);
	trxList *t = trx_hash_find(trx_id, trx_table);
	int index;
	trxList *tmp;
	if(t == NULL){
		pthread_mutex_unlock(&trx_manager_latch);
		return;
	}
	if(t->init == false){
		pthread_mutex_unlock(&trx_manager_latch);
		return;
		}
	lock_t *cur = t->lock;
	lock_t *temp;
	list *l;
	record *r;
	page_t* page;
	int i;
	while(cur){
		if(cur->lock_mode ==0){
			if(cur->prev){
				cur->prev->next = cur->next;
	
				if(cur->next){
					cur->next->prev = cur->prev;
				}
			}
			else{
				temp =cur;
				lock_release(temp);
			}
			cur = cur->trx_next;
		}
		else if(cur->lock_mode ==1){
			l = cur->pointer;
			page = buf_read_page(l->table_id, cur->pagenum);
			for(i = 0; i<page->num_keys; i++){
				if(page->keys[i] == l->key){
					r= page->record[i];
					break;
				}
			}
			if(r != NULL){
				strncpy(r->value, cur->stored, VALUE_SIZE);
			}
			buf_return_page(l->table_id, cur->pagenum, 1);
			temp = cur;
			cur = cur->trx_next;
			lock_release(temp);
		}
		
	}
	t->init = false;
	index= trx_hash(trx_id);
	tmp = trx_table[index];
	if(tmp !=t)
		while(tmp->link != t)
			tmp = tmp->link;
	tmp->link = t->link;

	free(t);
	active_num--;
	pthread_mutex_unlock(&trx_manager_latch);
}

