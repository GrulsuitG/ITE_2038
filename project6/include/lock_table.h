#ifndef __LOCK_TABLE_H__
#define __LOCK_TABLE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct lock_t lock_t;
typedef struct list list;

#include "transaction.h"
#include "file.h"

#define SHARED 0
#define EXCLUSIVE 1


#define TABLE_SIZE 431

#define ACQUIRED 0
#define NEED_TO_WAIT 1
#define DEADLOCK 2



struct lock_t {
	pthread_cond_t *cond;
	list *pointer;

	lock_t *next;
	lock_t *prev;

	char *stored;
	bool get;

	int lock_mode;
	int trx_id;
	uint64_t prev_LSN;
	lock_t *trx_next;
};

struct list{

	int table_id;
	int64_t key;
	int lock_num;
	pagenum_t pagenum;
	pthread_mutex_t *mutex;
	
	list *link;

	lock_t *head;
	lock_t *tail;
};

list *lock_table[TABLE_SIZE];

pthread_mutex_t *lock_table_latch;

/* APIs for lock table */
int init_lock_table();
int lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode, lock_t* ret_lock, trxList* t);
int lock_release(lock_t* lock_obj);
void lock_wait(lock_t* locK_obj, trxList* t);

lock_t* lock_make_node();
list* make_list(int table_id, int64_t key);

/* APIs for hash */
int hash_function(int table_id, int64_t key);
list* hash_add(int table_id, int64_t key, list *ht[]);
list* hash_find(int table_id, int64_t key, list *ht[]);
void hash_chain_print(list *ht[]);

#endif /* __LOCK_TABLE_H__ */
