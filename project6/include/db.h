#ifndef __DB_H__
#define __DB_H__
#include "transaction.h"
#include "lock_table.h"
#include "index.h"
#include "log.h"




bool init;
int unique_id;

typedef struct table{
	bool is_open;
	int fd;
}table;

table tableList[MAX_TABLE_NUM];

int init_db(int num_buf, int flag, int log_num, char* log_path, char* logmsg_path);
int open_table(char *pathname);
int db_insert(int table_id, int64_t key, char *value);
int db_find(int table_id, int64_t key, char *ret_val, int trx_id);
int db_update(int table_id, int64_t key, char* values, int trx_id);
int db_delete(int table_id, int64_t key);
int close_table(int table_id);
int shutdown_db();

void make_free();

#endif
