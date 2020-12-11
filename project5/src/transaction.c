#include "transaction.h"
void init_trx(){
	global_trx_id = 1;
	trx_manager_latch = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(trx_manager_latch,0);
	cycle = false;
}

int trx_begin(){
	pthread_mutex_lock(trx_manager_latch);
	trxList* t = trx_hash_add(global_trx_id, trx_table);
	t->id = global_trx_id++;
	t->init = true;
	fprintf(fp, "begin %d\n", t->id);
	pthread_mutex_unlock(trx_manager_latch);
	return t->id;
}

int trx_commit(int trx_id){
	pthread_mutex_lock(trx_manager_latch);
	
	lock_t *l, *temp;
	trxList * templist;
	int index = trx_hash(trx_id);
	trxList* t = trx_hash_find(trx_id, trx_table);
	pthread_mutex_unlock(trx_manager_latch);
	if(t == NULL){
		fprintf(fp, "%d list NULL\n", trx_id);
		return 0;
	}
	if(t->init == false){
		fprintf(fp, "%d list init x\n", trx_id);
		return 0;
	}
	l = t->lock;
	while(l != NULL){
		
		temp = l;
		l = l->trx_next;
		pthread_mutex_lock(lock_table_latch);
		lock_release(temp);
		pthread_mutex_unlock(lock_table_latch);
	}

	t->init = false;
	if(trx_table[index] == t){
		trx_table[index] = t->link;
	}
	else{
		templist = trx_table[index];
		while(templist->link == t){
			templist = templist->link;
		}
		templist->link = t->link;
	}
	//pthread_mutex_unlock(t->mutex);
	pthread_mutex_destroy(t->mutex);
	free(t->mutex);
	free(t);
	fprintf(fp, "%d commit\n", trx_id);
	
	return trx_id;

}

trxList* trx_make_list(){
	trxList *t = (trxList*)malloc(sizeof(trxList));
	if( t== NULL){
		perror("make trxList error");
		exit(EXIT_FAILURE);
	}
	t->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(t->mutex,0);
	t->init = false;
	t->lock = NULL;
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

bool detection(int trx_id, int wait){
	printf("Detection!\n");
	pthread_mutex_lock(trx_manager_latch);
	trxList* t;
	lock_t *cur, *next;
	int i, j, id = 1;
	int active[global_trx_id+1];
	int *visit;
	
	
	//find active trx
	for(i =1; i<global_trx_id; i++){
		t = trx_hash_find(i, trx_table);
		if(t && t->init){
			active[i] = id++;
		}
	}
	//init graph
	graph = (int**)malloc(sizeof(int*)*(id));
	visit = (int*)malloc(sizeof(int)*(id));
	for(i = 1; i<id; i++){
		graph[i] = (int*)malloc(sizeof(int)*(id));
		visit[i] =0;
		for(j= 1; j<id; j++){
			graph[i][j] = 0;
		}
		
	}
	
	//draw graph
	for(i =1; i<global_trx_id; i++){
		t = trx_hash_find(i, trx_table);
		if(t && t->init){
			cur = t->lock;
			while(cur){
		//	printf("detect %d\n", i);
					next = cur->next;
					if(next != NULL && next->get == false){
						graph[active[next->trx_id]][active[cur->trx_id]] = 1;	
					}
				cur = cur->trx_next;
			}
		}
	}
	graph[active[trx_id]][active[wait]] =1;
	/*for(i=1; i<=active_num; i++){
		for(int j=1; j<=active_num; j++){
			printf("%d ", graph[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/
	
	cycle= false;
	//find cycle

	for(i = 1; i<id; i++){
		if(visit[i] == 0){
			dfs(i,visit, id);
		}
		if(cycle == true){
			pthread_mutex_unlock(trx_manager_latch);
			return true;
		}
	}
	
	for(i =1; i<id; i++){
		free(graph[i]);
	}
	free(graph);
	
	pthread_mutex_unlock(trx_manager_latch);
	return false;
}

void dfs(int v, int visit[], int n){
	int i;
	visit[v] = 1;
	for(i =1; i<n; i++){
		if(graph[v][i] == 1){
			switch(visit[i]){
				case 0:
					dfs(i, visit, n);
					break;
				case 1:
					cycle = true;
					break;
				case 2:
					break;
				default:
					break;
			}
		}
	}
	visit[v] = 2;
}

void trx_abort(int trx_id){
	printf("abort %d\n", trx_id);
	pthread_mutex_lock(trx_manager_latch);
	trxList *t = trx_hash_find(trx_id, trx_table);
	int index;
	trxList *tmp;
	if(t == NULL){
		pthread_mutex_unlock(trx_manager_latch);
		return;
	}
	if(t->init == false){
		pthread_mutex_unlock(trx_manager_latch);
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
		}
		/*else if(cur->lock_mode ==1){
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
			buf_return_page(l->table_id, cur->pagenum, true);
		}
		temp = cur;
		cur = cur->trx_next;
		lock_release(temp, 1);
	*/
	}
	t->init = false;
//	index= trx_hash(trx_id);

/*	tmp = trx_table[index];
	if(tmp !=t)
		while(tmp->link != t)
			tmp = tmp->link;
	tmp->link = t->link;
*/
	//free(t);
	pthread_mutex_unlock(trx_manager_latch);
}

