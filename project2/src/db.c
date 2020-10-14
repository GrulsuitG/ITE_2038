#include "db.h"

char* filename;

int open_table(char *pathname){ 
    int i;
    filename = (char*) malloc(strlen(pathname));
    strncpy(filename, pathname, sizeof(pathname));
    make_file();
    root = syncFileAndTree();
	
    if(table_name[0] == NULL){
        unique_id =0;
    }
		
    for(i=0; i<unique_id; i++){
        if( strcmp(pathname, table_name[i]) == 0)
            return i;
        }
  
    table_name[unique_id] = (char*)malloc(sizeof(char)*strlen(filename));
    table_name[unique_id] = filename;

    return unique_id++;
  

}

int db_insert(int64_t key, char* value){
    node *n;
	//printf("a");
    page_t *page = init_page_t();
	//printf("b");
    pagenum_t pagenum;
	//printf("c");
    if(filename == NULL){
        printf("first open table\n");
        return -1;
    }

	//printf("a");
    if(find(root, key, false) != NULL){
        printf("the key %ld is already exist\n", key);    
        return -1;
    }
	//printf("b");
    root = insert(root, key, value);
	
    
	
    while(Q != NULL){
        n = deQ();
        if(n->is_leaf)
            init_info(page);
        else
            init_inter_info(page);

        pagenum = node_to_page(n, page);
		file_write_page(pagenum, page);
    }

    free_page_t(page);
    
    return 0;
}

int db_find(int64_t key, char *ret_val){
    node *n;
    
    if(filename == NULL){
        printf("first open table\n");
        return -1;
    }

    n = find_leaf(root, key, false);
    if(n == NULL){
        printf("the key: %ld is not exist\n", key);
        return -1;
    }

    else{
        for(int i =0; i<n->num_keys; i++){
            if(key == n->keys[i]){
                ret_val = (char*)n->pointers[i];
                break;
            }
        }
        return 0;
    }
}

int db_delete(int64_t key){
    node *n;
    page_t *page= init_page_t();
    pagenum_t pagenum;
    if(filename == NULL){
        printf("first open table\n");
        return -1;
    }

    n= find_leaf(root, key, false);
    if( n == NULL){
        printf("the key : %ld is not exist\n", key);
        return -1;
    }
    delete(root,key);
    while( Q!= NULL){
        n = dequeue();
        if(n->is_leaf)
            init_info(page);
        else
            init_inter_info(page);
        pagenum = node_to_page(n, page);
        file_write_page(pagenum, page);
    }
    free_page_t(page);
    return 0;
}

pagenum_t node_to_page(node* n, page_t* page){
    node* tmp;
    record* tmprec;
    if(n->parent == NULL)
    	page->parentPageNum = 0;
	else
		page->parentPageNum = n->parent->pagenum;

	page->isLeaf = n->is_leaf;
    page->numOfKey= n->num_keys;
    
    if(n-> is_leaf){
        init_info(page);
        
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

void page_to_node(page_t* page,node* n, pagenum_t pagenum){
    if(page->isLeaf)
        n = make_leaf();
    else
        n = make_node();

    n->pagenum = pagenum;
    n->is_leaf = page->isLeaf;
    n->num_keys = page->numOfKey;
    if(n->is_leaf){
        for(int i=0; i<n->num_keys; i++){
            n->keys[i] = page->info[i]->key;
            n->pointers[i] = page->info[i]->value;
        }
        n->pointers[LEAF_ORDER] = NULL;
    }
    else{
        for(int i =0; i<n->num_keys; i++){
            n->keys[i] = page->inter_info[i]->key;
        }
    }
    
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

node* syncFileAndTree(){
    node *parent, *child, *tmp;
    page_t *parentpage, *childpage;
    if(root != NULL){
    	root = destroy_tree(root);
    }
    parentpage = init_page_t();
    file_read_page(0, parentpage);
    if(parentpage->rootPageNum == 0)
        return root;
    
    file_read_page(parentpage->rootPageNum, parentpage);
    page_to_node(parentpage, root, parentpage->rootPageNum);
    enQ(root);
    
    while ( Q != NULL){
        parent= dequeue();
        if(!parent->is_leaf){
			file_read_page(parentpage->rightSibling, childpage);
			page_to_node(childpage, child, parentpage->rightSibling);
			parent->pointers[0] = child;
			enQ(child);
            for(int i =1; i<parent->num_keys; i++){
                file_read_page(parentpage->inter_info[i]->pagenum, childpage);
                page_to_node(childpage, child, parentpage->inter_info[i]->pagenum);
                parent->pointers[i] = child;
                enQ(child);
				if(child->is_leaf){
					tmp = parent->pointers[i-1];
					tmp->pointers[LEAF_ORDER] = child;
				}
            } 
        }
		
			
    }

    return root;
}
