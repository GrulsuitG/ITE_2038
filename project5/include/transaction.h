#ifndef __TRX_H__
#define __TRX_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lock_table.h"
#include "buffer.h"
#define TRX_TABLE_SIZE 431


unsigned global_trx_id;
int active_num;
int **graph;
pthread_mutex_t trx_manager_latch;

typedef struct trxList{
	unsigned id;
	lock_t * lock;
	
	struct trxList *link;
	bool init;
}trxList;


trxList *trx_table[TRX_TABLE_SIZE];

void init_trx();
int trx_begin();
int trx_commit(int trx_id);

bool dfs(int v, int visit[]);
bool detection(int trx_id, int wait, int *flag);
void trx_abort(int trx_id);

trxList* trx_make_list();

int trx_hash(int trx_id);
trxList* trx_hash_add(int trx_id, trxList *ht[]);
trxList* trx_hash_find(int trx_id, trxList *ht[]);
#endif
