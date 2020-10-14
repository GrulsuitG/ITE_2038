#ifndef __INDEX_H__
#define __INDEX_H__

#include"bpt.h"
#include"file.h"
#include<string.h>

#define MAX_TABLE_NUM 100

int unique_id;
extern char* filename;
char* table_name[MAX_TABLE_NUM];
node* root;

node *Q;


int open_table(char *pathname);
int db_insert(int64_t key, char *value);
int db_find(int64_t key, char *ret_val);
int db_delete(int64_t key);
pagenum_t node_to_page(node *node, page_t *page);

void page_to_node(page_t *page, node **n, pagenum_t pagenum);
void enQ(node *n);
node* deQ();

node* syncFileAndTree();

#endif
