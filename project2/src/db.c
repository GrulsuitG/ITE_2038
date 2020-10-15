#include "db.h"

char* filename;
//node* queue = NULL;

int open_table(char *pathname){ 
    //printf("0");
    int i;
    char* name;
    
    //printf("a");
    if(table_name[0] == NULL){
        unique_id =0;
    }
   //printf("b");
     for(i=0; i<unique_id; i++){
     	
        if( strcmp(pathname, table_name[i]) == 0){
            filename = table_name[i];
            //root = syncFileAndTree();
            return i;
        }
     }
    //printf("c");
    name = (char*)malloc(sizeof(pathname));
    strncpy(name, pathname, sizeof(pathname));
   //printf("d");
	filename = name;
   //printf("e");
    make_file();
	//printf("f");
   //root = syncFileAndTree();
	//printf("g");
    
   
	table_name[unique_id] = (char*)malloc(sizeof(pathname));
    strncpy(table_name[unique_id] , pathname, sizeof(pathname));
    

    return unique_id++;
  

}

int db_insert(int64_t key, char* value){
    node *n;
	
    page_t *page;
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
		
    	if(n->is_leaf)
            page = init_leaf();
        else
            page =init_inter();
            
        if(n->pagenum == 0){
        	n->pagenum = file_alloc_page();
        }
		
		node_to_page(n, page);
		file_write_page(n->pagenum, page); 
		
		
    }
	file_read_page(0 , header);
	if(root->pagenum != header->rootPageNum ){
		header->rootPageNum = n->pagenum;
		file_write_page(0, header);	
	}	

    return 0;
}
int db_find(int64_t key, char *ret_val){ return 0;}
int db_delete(int64_t key){
    node *n;
    
    page_t *page;
    page_t *header = init_page();
	pagenum_t pagenum;
    if(filename == NULL){
        return -1;
    }

  if(find_leaf(root, key, false) == NULL){
        //printf("b");
        return -1;
    }
    root = delete(root,key);
   while( queue!= NULL){
        //printf("a");
        n = dequeue();
        if(n->is_leaf)
            page = init_leaf();
        else
            page = init_inter();
        if(n->num_keys == 0){
        	printf("pagenum : %lu , %d\n", n->pagenum, n->num_keys);
        	//file_free_page(n->pagenum);
        	continue;
        }    
        printf("pagenum : %lu , %d\n", n->pagenum, n->num_keys);
        node_to_page(n, page);
       file_write_page(n->pagenum, page);
    }
    /*
	if(root->pagenum != header->rootPageNum ){
		header->rootPageNum = n->pagenum;
		file_write_page(0, header);	
	}
	
	free_page(header);*/
    return 0;
}

void node_to_page(node *n, page_t *page){
    
    node *tmp;
    //memcpy(tmp, n, sizeof(n));
    record *tmp_record;  
    page->is_leaf = n->is_leaf;
	page->num_keys= n->num_keys;	
    if(n->parent == NULL)
    	page->parentPageNum = 0;
	else
		page->parentPageNum = n->parent->pagenum;

	
    
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

    return;

}
/*
int db_find(int64_t key, char *ret_val){
    node *n;
    //record *tmp;
    
    if(filename == NULL){
        //printf("first open table\n");
        return -1;
    }

    n = find_leaf(root, key, false);
    if(n == NULL){
        //printf("the key: %ld is not exist\n", key);
        return -1;
    }

    else{
        for(int i =0; i<n->num_keys; i++){
            if(key == n->keys[i]){
                //tmp = n->pointers[i];
               // ret_val = tmp->value;
                break;
            }
        }
        return 0;
    }
}




*/
/*
void page_to_node(page_t *page,node *n, pagenum_t pagenum){
    if(page->is_leaf)
       n = make_leaf();
    else
        *n = make_node();

    n->pagenum = pagenum;
    n->is_leaf = page->is_leaf;
    n->num_keys = page->num_keys;
    if(n->is_leaf){
        for(int i=0; i<LEAF_ORDER-1; i++){
            n->keys[i] = page->info[i]->key;
            n->pointers[i] = page->info[i]->value;
        }
        n->pointers[LEAF_ORDER] = NULL;
    }
    else{
        for(int i =0; i<INTERNAL_ORDER-1; i++){
            n->keys[i] = page->inter_info[i]->key;
        }
    }
    
}
/*
node* syncFileAndTree(){
    node *parent, *child, *tmp;
    page_t *parentpage, *childpage;
    if(root != NULL){
    	root = destroy_tree(root);
    }
   
    parentpage = init_page_t();
    childpage = init_page_t();
    file_read_page(0, parentpage);
    if(parentpage->rootPageNum == 0)
        return root;
    
    file_read_page(parentpage->rootPageNum, parentpage);
	
    page_to_node(parentpage, &root, parentpage->rootPageNum);
    
	enqueue(root);
    
    while ( queue != NULL){
    
        parent= dequeue();
        file_read_page(parent->pagenum, parentpage);
        if(!parent->is_leaf){
        	
			file_read_page(parentpage->pointer, childpage);
			//printf("%d\n" ,childpage->inter_info[0]->key);
			//for(int i =0; i<INTERNAL_ORDER-1; i++){
        	//printf("%d\n" ,childpage->inter_info[i]->key);
            
        //}
			page_to_node(childpage, &child, parentpage->pointer);
			parent->pointers[0] = child;
			child->parent = parent;
			enqueue(child);
            for(int i =1; i<=parent->num_keys; i++){
                file_read_page(parentpage->inter_info[i-1]->pagenum, childpage);
                page_to_node(childpage, &child, parentpage->inter_info[i-1]->pagenum);
                parent->pointers[i] = child;
                child->parent = parent;
                enqueue(child);
				if(child->is_leaf){
					tmp = parent->pointers[i-1];
					tmp->pointers[LEAF_ORDER] = child;
				}
            } 
        }
    }
    free_page_t(childpage);
    free_page_t(parentpage);
   
    return root;
}*/
