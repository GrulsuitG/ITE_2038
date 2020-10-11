#include "index.h"


int open_table(char *pathname){
  
  filename = pathname;
  
  for(int i=0; i<unique_id; i++){
    if( strcmp(pathname, table_name[i]) == 0)
      return i;
    }
  
  table_name[unique_id] = (char*)malloc(sizeof(char)*strlen(pathname));
  table_name[unique_id] = pathname;

  return unique_id++;
  

}

int db_insert(int64_t key, char* value){
   // Queue queue;
   insert(root, key, value);
   // if(find)
     //   return -1;
    
/*
    while(!queue_is_empty(queue)){
        node *node = dequeue();
        page_t *page;
        pagenum_t pagenum = node_to_page(node, page);

        file_write_page(pagenum, page);
    }*/

    return 0;
}

int db_find(int64_t key, char *ret_val){

}

int db_delete(int64_t key){

}

pagenum_t node_to_page(node* n, page_t* page){
    page = init_page_t();
    node* tmp;
    record* tmprec;
    
    if(n-> is_leaf){
        init_info(page);
        
        page->parentPageNum = n->parent->pagenum;
        page->isLeaf = n->is_leaf;
        page->numOfKey= n->num_keys;
        
        tmp =(node*) n -> pointers[LEAF_ORDER];
        if(tmp == NULL)
            page->rightSibling = 0;
        else
            page->rightSibling = tmp->pagenum;

        for(int i =0; i<page->numOfKey; i++){
            page->info[i]->key = n->keys[i];
            tmprec = (record*) n->pointers[i];
            page->info[i]->value = tmprec -> value;
        }

    }

    else{
        init_inter_info(page);

        page->parentPageNum = n->parent->pagenum;
        page->isLeaf = n->is_leaf;
        page->numOfKey = n->num_keys;
        
        tmp = (node*) n->pointers[0];
        page->rightSibling = tmp->pagenum;

        for(int i =0; i < page->numOfKey; i++){
            page->inter_info[i]->key = n->keys[i];
            tmp = (node*) n->pointers[i+1];
            page->inter_info[i]->pagenum = tmp->pagenum;
        }
    
    
    }

    return n->pagenum;

}

void enQ(node* n){
    node* c=Q;
    if(Q== NULL){
        Q = n;
        Q->next = NULL;
    }
    else{
        while( c->next != NULL){
            c = c->next;
        }
        c->next = n;
        n->next = NULL;;
    }
}

node* deQ(){
    
    node* n= Q;
    Q = Q->next;
    n->next = NULL;
    return n;
}
