#ifndef __TRX_H__
#define __TRX_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct trxList trxList;
#include "lock_table.h"
#include "buffer.h"
#define TRX_TABLE_SIZE 431

#define SUCCESS 0
#define FAIL 1

int global_trx_id;
bool cycle;
pthread_mutex_t *trx_manager_latch;
FILE *fp;


typedef struct adj_node{
	struct adj_node *next;
	struct adj_node *prev;
	trxList *pointer;
	int trx_id;
	int id;
	struct adj_node *adj;
}adj_node;

typedef struct adj_list{
	adj_node *head;
	adj_node *tail;
	int node_num;
}adj_list;

struct trxList{
	int id;
	lock_t *lock;
	pthread_mutex_t *mutex;	

	struct trxList *link;
	adj_node *node;
	bool init;
};



adj_list *adj;
int **graph;
trxList *trx_table[TRX_TABLE_SIZE];

void init_trx();
int trx_begin();
int trx_commit(int trx_id);

void dfs(int v, int visit[], int n);
bool detection(int trx_id, int wait);
int trx_abort(int trx_id);

trxList* trx_make_list();

int trx_hash(int trx_id);
trxList* trx_hash_add(int trx_id, trxList *ht[]);
trxList* trx_hash_find(int trx_id, trxList *ht[]);
void trx_hash_delete(int index, trxList *t);
#endif
