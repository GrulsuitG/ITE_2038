#ifndef __DB_H__
#define __DB_H__
#include"bpt.h"
#include"file.h"



#define MAX_TABLE_NUM 100

int unique_id;
extern char* filename;
char* table_name[MAX_TABLE_NUM];
node* root;

int open_table(char *pathname);
int db_insert(int64_t key, char *value);
int db_find(int64_t key, char *ret_val);
int db_delete(int64_t key);

void make_free();
page_t* node_to_page(node *node);
node* page_to_node(page_t *page, pagenum_t pagenum);

node* syncFileAndTree();

#endif
