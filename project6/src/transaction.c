#include "transaction.h"
void init_trx(){
	global_trx_id = 1;
	trx_manager_latch = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(trx_manager_latch,0);
	cycle = false;
	adj = (adj_list*) malloc(sizeof(adj_list));
	adj->node_num = 0;
}

int trx_begin(){
	pthread_mutex_lock(trx_manager_latch);
	trxList* t = trx_hash_add(global_trx_id, trx_table);
	
	adj_node* n = (adj_node*)malloc(sizeof(adj_node));
	n->next = n->prev = NULL;
	if(adj->node_num == 0){
		adj->head = n;
		adj->tail = n;
	}
	else{
		adj->tail->next = n;
		n->prev = adj->tail;
		adj->tail = n;
	}
	adj->node_num++;
	n->trx_id = global_trx_id;
	t->id = global_trx_id++;
	t->init = true;
	t->node = n;
	n->pointer = t;
	pthread_mutex_lock(log_buffer_latch);
	t->LSN = log_write(BEGIN, t->id, 0, 0, NULL, 0, NULL, 0);
	pthread_mutex_unlock(log_buffer_latch);
	pthread_mutex_lock(t->mutex);
	
	pthread_mutex_unlock(trx_manager_latch);
	//printf("%d begin\n", t->id);
	return t->id;
}

int trx_commit(int trx_id){
	lock_t *lock_obj, *temp;
	trxList * templist;
	
	int index = trx_hash(trx_id);
	
	trxList* t = trx_hash_find(trx_id, trx_table);
	
	if(t == NULL || t->init == false){
		//fprintf(fp, "%d list NULL\n", trx_id);
		return 0;
	}
	
	
	
	pthread_mutex_lock(lock_table_latch);
	lock_obj = t->lock;
	while(lock_obj != NULL){
		
		temp = lock_obj;
		lock_obj = lock_obj->trx_next;
		
		lock_release(temp);
		
	}
	pthread_mutex_unlock(lock_table_latch);
	
	pthread_mutex_lock(log_buffer_latch);
	log_write(COMMIT, t->id, 0, 0, NULL, 0, NULL, 0);
	logbuf_flush();
	pthread_mutex_unlock(log_buffer_latch);
	trx_hash_delete(index,t);
	//fprintf(fp, "%d commit\n", trx_id);
	//printf("%d commit\n", trx_id);
	
	return trx_id;

}

int trx_abort(int trx_id){
	int index, i;
	uint64_t LSN;
	trxList *t;
	list *l;
	lock_t *temp, *cur;	
	page_t* page;
	t = trx_hash_find(trx_id, trx_table);
	if(t == NULL || t->init == false){
		return 0;
	}
	
	cur = t->lock;
	
	while(cur){
		if(cur->lock_mode == EXCLUSIVE){
			l = cur->pointer;
			page = buf_read_page(l->table_id, l->pagenum);
			for(i = 0; i<page->num_keys; i++){
				if(page->keys[i] == l->key) {
					strncpy(page->record[i]->value, cur->stored, VALUE_SIZE);
					break;
				}
			}
			pthread_mutex_lock(log_buffer_latch);
			LSN = log_write(COMPENSATE, trx_id, t->LSN, l->table_id, page, i ,cur->stored, cur->prev_LSN); 
			pthread_mutex_unlock(log_buffer_latch);
			page->LSN = LSN;
			buf_return_page(l->table_id, l->pagenum, true, page->index);
		}
		
		temp = cur;
		cur = cur->trx_next;
		
		lock_release(temp);
		
	}
	
	pthread_mutex_unlock(t->mutex);
	pthread_mutex_lock(log_buffer_latch);
	log_write(ROLLBACK, trx_id, t->LSN, 0, NULL, 0 ,NULL, 0); 
	pthread_mutex_unlock(log_buffer_latch);
	index = trx_hash(trx_id);
	trx_hash_delete(index, t);
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
	t->init = true;
	t->lock = NULL;
	t->LSN =0;
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
	pthread_mutex_lock(trx_manager_latch);
	trxList *node;
	int index = trx_hash(trx_id);
	for(node = ht[index]; node; node = node->link){
		if(node->id == trx_id){
			pthread_mutex_unlock(trx_manager_latch);
			return node;
		}
	}
	pthread_mutex_unlock(trx_manager_latch);
	return NULL;
}


void trx_hash_delete(int index, trxList *t){
	pthread_mutex_lock(trx_manager_latch);
	trxList *templist;
	t->init = false;
	
	adj_node *n = t->node;
	if(adj->node_num == 1){
		adj->head = NULL;
		adj->head = NULL;
	}
	else if(adj->tail == n){
		adj->tail = n->prev;
		n->prev->next = NULL;
	}
	else if(adj->head == n){
		adj->head = n->next;
		n->next->prev = NULL;
	}
	else{
		n->next->prev = n->prev;
		n->prev->next = n->next;
	}
	
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
	adj->node_num--;
	pthread_mutex_unlock(t->mutex);
	pthread_mutex_destroy(t->mutex);
	free(n);
	free(t->mutex);
	free(t);
	pthread_mutex_unlock(trx_manager_latch);
}

bool detection(int trx_id, int wait){
	//printf("Detection!\n");
	trxList* t;
	adj_node *n;
	lock_t *cur, *next;
	int i, j, num;
	int *visit, *active;
	
	num = adj->node_num;
	active = (int*)malloc(sizeof(int) *(global_trx_id+1));
	visit = (int*) malloc(sizeof(int) * num);
	graph = (int**)malloc(sizeof(int*) * num);
	for(i=0; i<num; i++){
		graph[i] = malloc(sizeof(int) *num);
	}
	//init adj
	
	i =0;
	n = adj->head;
	while(n){
		//fprintf(fp, "%d\n", n->trx_id);
		active[n->trx_id] = i++;
		n = n->next;
	}
	//fprintf(fp, "\n");
	
	
	
	for(i=0; i<num; i++){
		for(j=0; j<num; j++){
			graph[i][j] = 0;
		}
	}
	//draw graph
	fprintf(fp, "%d\n", num);
	for(n = adj->head; n; n = n->next){
		t = n->pointer;
		if(t && t->init){
			cur = t->lock;
				
			while(cur){
				next = cur->next;
				if(next && (next->get == false)){
					if(cur->trx_id == next->trx_id){
						cur = cur->trx_next;
						continue;	
					}

					//graph[active[next->trx_id]][active[n->trx_id]] = 1;
					
				}
				cur = cur->trx_next;
			}
		}
	}
	graph[active[trx_id]][active[wait]] = 1;
	/*for(i=0; i<num; i++){
		for(j=0; j<num; j++){
			printf("%d ", graph[i][j]);
			fprintf(fp, "%d ", graph[i][j]);
		}
		fprintf(fp, "\n");
		printf("\n");
	}
	fprintf(fp, "\n");
	printf("\n");
	*/
	cycle= false;
	//find cycle
	for(i= 0; i<num; i++){
		visit[i] = 0;
	}
	for(i = 0; i<num; i++){
		if(visit[i] == 0){
			dfs(i,visit, num);
		}
		if(cycle){
			return true;
		}
	}
	
	for(i=0; i<num; i++){
		free(graph[i]);
	}
	free(graph);
	free(visit);
	free(active);
	return false;
}

void dfs(int v, int visit[], int n){
	int i;
	visit[v] = 1;
	for(i =0; i<n; i++){
		if(graph[v][i] == 1){
			switch(visit[i]){
				case 0:
					dfs(i, visit, n);
					break;
				case 1:
					cycle = true;
					return;
				case 2:
					break;
				default:
					break;
			}
		}
		
	}
	visit[v] = 2;
}



