#ifndef __TRX_H__
#define __TRX_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct trxList trxList;
#include "lock_table.h"
#include "buffer.h"
#define TRX_TABLE_SIZE 4321


int global_trx_id;
int **graph;
bool cycle;
pthread_mutex_t trx_manager_latch;

struct trxList{
	unsigned id;
	lock_t *lock;
	pthread_mutex_t *mutex;	

	struct trxList *link;
	bool init;
};


trxList *trx_table[TRX_TABLE_SIZE];

void init_trx();
int trx_begin();
int trx_commit(int trx_id);

void dfs(int v, int visit[], int n);
bool detection(int trx_id, int wait);
void trx_abort(int trx_id);

trxList* trx_make_list();

int trx_hash(int trx_id);
trxList* trx_hash_add(int trx_id, trxList *ht[]);
trxList* trx_hash_find(int trx_id, trxList *ht[]);
#endif
