#include "db.h"

char* filename;

int open_table(char *pathname){ 
    int i;
    char* name;
    close_table();
    if(table_name[0] == NULL){
        unique_id =0;
    }
     for(i=0; i<unique_id; i++){
     	
        if( strcmp(pathname, table_name[i]) == 0){
            filename = table_name[i];
            root = syncFileAndTree();
            return i;
        }
     }
    name = (char*)malloc(sizeof(pathname));
    strncpy(name, pathname, sizeof(pathname));
	filename = name;
    make_file();
   root = syncFileAndTree();
    
   
	table_name[unique_id] = (char*)malloc(sizeof(pathname));
    strncpy(table_name[unique_id] , pathname, sizeof(pathname));
    

    return unique_id++;
  

}

int db_insert(int64_t key, char* value){
    node *n;
	
    page_t *page= init_page();
	page_t *header = init_page();
	
    pagenum_t pagenum;
	
    if(filename == NULL)
        return -1;
    

	
    if(find(root, key, false) != NULL){
            
        return -1;
    }
	
    root = insert(root, key, value);
	
	
    while(queue != NULL){
		n = dequeue();
		page = node_to_page(n);
		file_write_page(n->pagenum, page); 
		

    }
	file_read_page(0 , header);
	if(root->pagenum != header->rootPageNum ){

		header->rootPageNum = n->pagenum;
		file_write_page(0, header);	
	}	
	free_page(header);
	free_page(page);
    return 0;
}

int db_delete(int64_t key){
    node *n;
    
    page_t *page = init_page();
    page_t *header = init_page();
	pagenum_t pagenum;
    if(filename == NULL){
        return -1;
    }

  if(find_leaf(root, key, false) == NULL){ 
        return -1;
    }
   root = delete(root,key);
   while( queue!= NULL){
        n = dequeue();
        if(n->num_keys == 0){
        	file_free_page(n->pagenum);
        	continue;
        }    
        page = node_to_page(n);
       file_write_page(n->pagenum, page);
    }
    
	if(root->pagenum != header->rootPageNum ){
		header->rootPageNum = n->pagenum;
		file_write_page(0, header);	
	}
    return 0;
}

void make_free(){
	page_t *header = init_page();
	node* n;
	int i;
	int *list = get_freelist();
	file_read_page(0, header);
	enqueue(root);
	while(queue){
		n = dequeue();
		list[n->pagenum] = 1;
		if(!n->is_leaf)
			for( i = 0; i<=n->num_keys; i++)
				enqueue(n->pointers[i]);	
	}
	for( i= 1 ; i<header->numOfPage; i++){
		if(!list[i])
			file_free_page(i);
			
	}
}

page_t* node_to_page(node *n){
    page_t *page = init_page();
    node *tmp;
    record *tmp_record;  
    page->is_leaf = n->is_leaf;
	page->num_keys= n->num_keys;	
    if(n->parent == NULL)
    	page->parentPageNum = 0;
	else
		page->parentPageNum = n->parent->pagenum;
	tmp_record = n->pointers[0];
    
    if(n-> is_leaf){ 
        tmp =(node*) n -> pointers[LEAF_ORDER-1];
        if(tmp == NULL)
            page->pointer = 0;
        else
            page->pointer = tmp->pagenum;

        for(int i =0; i< n->num_keys; i++){
            page->key[i] = n->keys[i];
			tmp_record = n->pointers[i];
            strncpy(page->record[i], tmp_record->value, VALUE_SIZE);
        }
 
    }

    else{
    	tmp = n->pointers[0];
    	page->pointer = tmp->pagenum;
        for(int i =0; i < n->num_keys; i++){
            page->key[i] = n->keys[i];
            tmp = n->pointers[i+1];
            if(tmp)
            	page->pagenum[i] = tmp->pagenum;
        }
    }

    return page;

}

int db_find(int64_t key, char *ret_val){
    record *r;
    
    if(filename == NULL){
        return -1;
    }

    r = find(root, key, false);
    if(r == NULL){
        return -1;
    }

    else
        strncpy(ret_val, r->value, VALUE_SIZE);
    return 0;

}

node* page_to_node(page_t *page, pagenum_t pagenum){
    node *n;
    record *temp;
    if(page->is_leaf)
    	n = make_leaf();
    else
        n = make_node();  
	
    n->pagenum = pagenum;
    n->is_leaf = page->is_leaf;
    n->num_keys = page->num_keys;
    if(n->is_leaf){
        for(int i=0; i<n->num_keys; i++){
            n->keys[i] = page->key[i];
            temp = make_record(page->record[i]);
            n->pointers[i] = temp;
        }
        n->pointers[LEAF_ORDER-1] = NULL;
    }
    else{
        for(int i =0; i<n->num_keys; i++){
            n->keys[i] = page->key[i];
        }
    }
    return n;
}

node* syncFileAndTree(){
    node *parent, *child, *tmp;
    page_t *parentpage, *childpage, *header;
    if(root != NULL){
    	root = destroy_tree(root);
    }
   	header = init_page();
   	
    file_read_page(0, header);
    if(header->rootPageNum == 0)
        return root;
   	parentpage = init_page();
    file_read_page(header->rootPageNum, parentpage);
	
    root = page_to_node(parentpage, header->rootPageNum);
    
	enqueue(root);
    
    while ( queue ){
    
        parent= dequeue();
        if(!parent->is_leaf){
        	file_read_page(parent->pagenum, parentpage);
      		childpage = init_page();
        	file_read_page(parentpage->pointer, childpage);

            
			child = page_to_node(childpage, parentpage->pointer);
			parent->pointers[0] = child;
			child->parent = parent;
			enqueue(child);
            for(int i =0; i<parent->num_keys; i++){
            	
                file_read_page(parentpage->pagenum[i], childpage);
                child = page_to_node(childpage, parentpage->pagenum[i]);
                parent->pointers[i+1] = child;
                child->parent = parent;
                enqueue(child);
				if(child->is_leaf){
					tmp = parent->pointers[i];
					tmp->pointers[LEAF_ORDER-1] = child;
				}
            } 
        }
    }
  	free_page(header);
  	free_page(parentpage);
  	free_page(childpage);
    return root;
}

void close_table(){
	if(root)
		make_free();
}
