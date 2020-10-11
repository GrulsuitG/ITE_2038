#include"type.h"
#include"bpt.h"
#include"file.h"
#include<string.h>

#define MAX_TABLE_NUM 100

int unique_id = 0;
char* table_name[MAX_TABLE_NUM];
node* root;

node* Q;


int open_table(char *pathname);
int db_insert(int64_t key, char *value);
int db_find(int64_t key, char* ret_val);
int db_delete(int64_t key);
pagenum_t node_to_page(node* node, page_t* page);

void enQ(node* n);
node* deQ();

