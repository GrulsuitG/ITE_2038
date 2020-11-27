
int trx_begin();
int trx_commit(int trx_id);

int init_db(int num_buf);
int open_table(char *pathname);
int db_insert(int table_id, int64_t key, char *value);
int db_delete(int table_id, int64_t key, char *ret_val);
int close_table(int table_id);
int shutdown_db();
