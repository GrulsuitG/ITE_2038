/*
 *  bpt.c  
 */
#define Version "1.14"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010-2016  Amittai Aviram  http://www.amittai.com
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, 
 *  this list of conditions and the following disclaimer in the documentation 
 *  and/or other materials provided with the distribution.
 
 *  3. Neither the name of the copyright holder nor the names of its 
 *  contributors may be used to endorse or promote products derived from this 
 *  software without specific prior written permission.
 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE.
 
 *  Author:  Amittai Aviram 
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *  Original Date:  26 June 2010
 *  Last modified: 17 June 2016
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *  
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

#include "index.h"

// GLOBALS.

/* The order determines the maximum and minimum
 * number of entries (keys and pointers) in any
 * node.  Every node has at most order - 1 keys and
 * at least (roughly speaking) half that number.
 * Every leaf has as many pointers to data as keys,
 * and every internal node has one more pointer
 * to a subtree than the number of keys.
 * This global variable is initialized to the
 * default value.
 */

node* root;

/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
node * queue = NULL;

/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
bool verbose_output = false;


// FUNCTION DEFINITIONS.

// OUTPUT AND UTILITIES

/* Copyright and license notice to user at startup. 
 */
/* Routine to print portion of GPL license to stdout.
 */

/* Helper function for printing the
 * tree out.  See print_tree.
 */
void enqueue( node * new_node ) {
    node * c;
    if (queue == NULL) {
        queue = new_node;
        queue->next = NULL;
    }
    else {
        c = queue;
        while(c->next != NULL) {
            c = c->next;
        }
        c->next = new_node;
        new_node->next = NULL;
    }
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
node * dequeue( void ) {
    node * n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}
/*
void enQ(page_t *page ) {
    page_t *c;
    if (Q == NULL) {
        Q = page;
        Q->next = NULL;
    }
    else {
        c = Q;
        while(c->next != NULL) {
            c = c->next;
        }
        c->next = page;
        page->next = NULL;
    }
}
page_t* deQ( void ) {
    page_t *p = Q;
    Q = Q->next;
    p->next = NULL;
    return p;
}*/
/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
 /*
void print_leaves( node * root ) {
    int i, order;
	order = LEAF_ORDER;
    node * c = root;
    if (root == NULL) {
        printf("Empty tree.\n");
        return;
    }
    while (!c->is_leaf)
        c = c->pointers[0];
    while (true) {
        for (i = 0; i < c->num_keys; i++) {
            if (verbose_output)
                printf("%lx ", (unsigned long)c->pointers[i]);
            printf("%d ", c->keys[i]);
        }
        if (verbose_output)
            printf("%lx ", (unsigned long)c->pointers[order - 1]);
        if (c->pointers[order - 1] != NULL) {
            printf(" | ");
            c = c->pointers[order - 1];
        }
        else
            break;
    }
    printf("\n");
}


/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
 /*
int height( node * root ) {
    int h = 0;
    node * c = root;
    while (!c->is_leaf) {
        c = c->pointers[0];
        h++;
    }
    return h;
}


/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
 
int path_to_root( node * root, node * child ) {
    int length = 0;
    node * c = child;
    while (c != root) {
        c = c->parent;
        length++;
    }
    return length;
}


/* Prints the B+ tree in the command
 * line in level (rank) order, with the 
 * keys in each node and the '|' symbol
 * to separate nodes.
 * With the verbose_output flag set.
 * the values of the pointers corresponding
 * to the keys also appear next to their respective
 * keys, in hexadecimal notation.
 */
 
 page_t* node_to_page(node *n){
    page_t *page;
    node *tmp;
    record *tmp_record;
	if(!n){
		return page;
	}  
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
            page->keys[i] = n->keys[i];
			tmp_record = n->pointers[i];
            strncpy(page->record[i]->value, tmp_record->value, VALUE_SIZE);
        }
 
    }

    else{
    	tmp = n->pointers[0];
    	page->pointer = tmp->pagenum;
        for(int i =0; i < n->num_keys; i++){
            page->keys[i] = n->keys[i];
            tmp = n->pointers[i+1];
            if(tmp)
            	page->pagenum[i] = tmp->pagenum;
        }
    }

    return page;

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
            n->keys[i] = page->keys[i];
            temp = make_record(page->record[i]->value);
            n->pointers[i] = temp;
        }
        n->pointers[LEAF_ORDER-1] = NULL;
    }
    else{
        for(int i =0; i<n->num_keys; i++){
            n->keys[i] = page->keys[i];
        }
    }
    return n;
}

node* syncFileAndTree(int table_id){
    node *parent, *child, *tmp;
    page_t *parentpage, *childpage, *header;
    if(root != NULL){
    	root = destroy_tree(root);
    }
   	header = init_page();
   	parentpage = init_page();
   	childpage = init_page();
    file_read_page(table_id, 0, header);
    if(header->rootPageNum == 0)
        return root;
   	
    file_read_page(table_id, header->rootPageNum, parentpage);
	
    root = page_to_node(parentpage, header->rootPageNum);
    if(parentpage->pointer == 0)
    	return root;
	enqueue(root);
    
    while ( queue ){
    
        parent= dequeue();
        if(!parent->is_leaf){
        	file_read_page(table_id, parent->pagenum, parentpage);
      		
        	file_read_page(table_id, parentpage->pointer, childpage);

            
			child = page_to_node(childpage, parentpage->pointer);
			parent->pointers[0] = child;
			child->parent = parent;
			enqueue(child);
            for(int i =0; i<parent->num_keys; i++){
            	
                file_read_page(table_id, parentpage->pagenum[i], childpage);
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
    return root;
}

 
void print_tree(int table_id) {
	
	node* root =syncFileAndTree(table_id); 
    node * n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL) {
        printf("Empty tree.\n");
        return;
    }
    queue = NULL;
    enqueue(root);
    while( queue != NULL ) {
        n = dequeue();
        if (n->parent != NULL && n == n->parent->pointers[0]) {
		   	new_rank = path_to_root( root, n );
            if (new_rank != rank) {
                rank = new_rank;
                printf("\n");
            }
        }
        if (verbose_output) 
            printf("(%lx)", (unsigned long)n);
        if(n->parent)
			printf("(%lu,%lu) " , n->pagenum,n->parent->pagenum);
		else
			printf("(%lu) ", n->pagenum);
		for (i = 0; i < n->num_keys; i++) {
            if (verbose_output)
                printf("%lx ", (unsigned long)n->pointers[i]);
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->num_keys; i++)
                enqueue(n->pointers[i]);
        if (verbose_output) {
            if (n->is_leaf) 
                printf("%lx ", (unsigned long)n->pointers[LEAF_ORDER - 1]);
            else
                printf("%lx ", (unsigned long)n->pointers[n->num_keys]);
        }
        printf("| ");
    }
    printf("\n");
}


/* Finds the record under a given key and prints an
 * appropriate message to stdout.
 *//*
void find_and_print(node * root, int64_t key, bool verbose) {
    record * r = find(root, key, verbose);
    if (r == NULL)
        printf("Record not found under key %ld.\n", key);
    else 
        printf("Record at %lx -- key %ld, value %s.\n",
                (unsigned long)r, key, r->value);
}


/* Finds and prints the keys, pointers, and values within a range
 * of keys between key_start and key_end, including both bounds.
 *//*
void find_and_print_range( node * root, int64_t key_start, int64_t key_end,
        bool verbose ) {
    int i;
    int array_size = key_end - key_start + 1;
    int64_t returned_keys[array_size];
    void * returned_pointers[array_size];
    int num_found = find_range( root, key_start, key_end, verbose,
            returned_keys, returned_pointers );
    if (!num_found)
        printf("None found.\n");
    else {
        for (i = 0; i < num_found; i++)
            printf("Key: %ld   Location: %lx  Value: %s\n",
                    returned_keys[i],
                    (unsigned long)returned_pointers[i],
                    ((record *)
                     returned_pointers[i])->value);
    }
}


/* Finds keys and their pointers, if present, in the range specified
 * by key_start and key_end, inclusive.  Places these in the arrays
 * returned_keys and returned_pointers, and returns the number of
 * entries found.
 *//*
int find_range( node * root, int64_t key_start, int64_t key_end, bool verbose,
        int64_t returned_keys[], void * returned_pointers[]) {
    int i, num_found;
    num_found = 0;
    node * n = find_leaf( root, key_start, verbose );
    if (n == NULL) return 0;
    for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++) ;
    if (i == n->num_keys) return 0;
    while (n != NULL) {
        for ( ; i < n->num_keys && n->keys[i] <= key_end; i++) {
            returned_keys[num_found] = n->keys[i];
            returned_pointers[num_found] = n->pointers[i];
            num_found++;
        }
        n = n->pointers[LEAF_ORDER - 1];
        i = 0;
    }
    return num_found;
}
*/

/* Traces the path from the root to a leaf, searching
 * by key.  Displays information about the path
 * if the verbose flag is set.
 * Returns the leaf containing the given key.
 */
page_t* find_leaf(int table_id, pagenum_t root, int64_t key) {
	int i,num;
   	page_t *page = buf_read_page(table_id, root);
	while(!page->is_leaf){
		buf_return_page(table_id, page->mypage, false, page->index);
		num = page->num_keys;
		if(key<page->keys[0]){
			page =buf_read_page(table_id, page->pointer);
			continue;
		}
    	for(i=1; i<num; i++){
			if(page->keys[i-1] <= key && key < page->keys[i]){
				page = buf_read_page(table_id, page->pagenum[i-1]);
				break;
			}
		}
		if(i == num && page->keys[i-1] <= key){
			page = buf_read_page(table_id, page->pagenum[i-1]);
		}
		
    }
	return page;
    
}


/* Finds and returns the record to which
 * a key refers.
 */
record* find(int table_id, pagenum_t *root, int64_t key) {
    int num, i = 0;
	page_t *header = buf_read_page(table_id, 0);
	page_t *page;
	*root= header->rootPageNum;
	buf_return_page(table_id, 0, false, header->index);
	if(*root == 0){
		return NULL;
		}
    page = find_leaf(table_id, *root, key);
	buf_return_page(table_id, page->mypage, false, page->index);
    for (i = 0; i < page->num_keys; i++)
        if (page->keys[i] == key) break;
    if (i == page->num_keys){
        return NULL;
    }
    else{
        return page->record[i];
    }
	return NULL;
}

record* find_record(int table_id, int64_t key, int trx_id, int lock_mode ,trxList *t, char* str){
	int ret, i = 0;
	page_t *header = buf_read_page(table_id, 0);
	page_t *page;
	pagenum_t pagenum;
	lock_t * lock_obj = lock_make_node();
	buf_return_page(table_id, 0, false, header->index);
	if(header->rootPageNum == 0){
		return NULL;
	}
    page = find_leaf(table_id, header->rootPageNum, key);
	pagenum =page->mypage;
    for (i = 0; i < page->num_keys; i++)
        if (page->keys[i] == key) break;
        
    if (i == page->num_keys){
    	buf_return_page(table_id, page->mypage, false, page->index);
        return NULL;
    }
    else{
    	ret = lock_acquire(table_id, key, trx_id, lock_mode, lock_obj, t);
		if(ret == ACQUIRED){
			if(lock_mode == EXCLUSIVE){
				strncpy(lock_obj->stored, page->record[i]->value, VALUE_SIZE);
				strncpy(page->record[i]->value, str, VALUE_SIZE);
				buf_return_page(table_id, page->mypage, true, page->index);
			}
			else if(lock_mode == SHARED){
				strncpy(str, page->record[i]->value, VALUE_SIZE);
				buf_return_page(table_id, page->mypage, false, page->index);
			}
			
			return page->record[i];
		}
		else if(ret == NEED_TO_WAIT){
			buf_return_page(table_id, page->mypage, false, page->index);
			lock_wait(lock_obj);
			return NULL;
		}
		else if(ret == DEADLOCK){
			buf_return_page(table_id, page->mypage, false, page->index);
			return NULL;
		}
		else{
			perror("error");
			exit(EXIT_FAILURE);
		}
    }

}

/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}

int index_init(int num_buf){
	return buf_init(num_buf);
}

int index_open(int table_id, char* filename){
	return buf_open(table_id, filename);
}

int index_close(int table_id){
	return buf_close(table_id);
}
// INSERTION

/* Creates a new record to hold the value
 * to which a key refers.
 */
record * make_record(char *value) {
    record * new_record = (record *)malloc(sizeof(record));
    if (new_record == NULL) {
        perror("Record creation.");
        exit(EXIT_FAILURE);
    }
	new_record -> value = (char*)malloc(120);
    if (new_record == NULL) {
        perror("Record creation.");
        exit(EXIT_FAILURE);
    }
    else {
        strncpy(new_record->value, value, VALUE_SIZE);
    }
    return new_record;
}


/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
node * make_node( void ) {
    node * new_node;
    new_node = malloc(sizeof(node));
    if (new_node == NULL) {
        perror("Node creation.");
        exit(EXIT_FAILURE);
    }
    new_node->keys = malloc( (INTERNAL_ORDER-1 ) * sizeof(int) );
    if (new_node->keys == NULL) {
        perror("New node keys array.");
        exit(EXIT_FAILURE);
    }
    new_node->pointers = malloc( (INTERNAL_ORDER) * sizeof(void *) );
    if (new_node->pointers == NULL) {
        perror("New node pointers array.");
        exit(EXIT_FAILURE);
    }
	new_node->pagenum = 0;
    new_node->is_leaf = false;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}
/*
void free_node(node* n){
	int i;
	record* r;
	if(n->is_leaf){
		for(i=0; i<LEAF_ORDER-1; i++){
			r= n->poitners[i];
			free(r->value);
		}
	}
	
		
	
	
}
*/
/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
node * make_leaf( void ) {
    node * leaf;
	leaf = malloc (sizeof(node));
    if (leaf == NULL) {
        perror("leaf creation.");
        exit(EXIT_FAILURE);
    }
    leaf->keys = malloc( (LEAF_ORDER - 1) * sizeof(int) );
    if (leaf->keys == NULL) {
        perror("New leaf keys array.");
        exit(EXIT_FAILURE);
    }
    leaf->pointers = malloc( LEAF_ORDER * sizeof(void *) );
    if (leaf->pointers == NULL) {
        perror("New leaf pointers array.");
        exit(EXIT_FAILURE);
    }
	//leaf->pagenum = file_alloc_page();
    leaf->num_keys = 0;
    leaf->parent = NULL;
	leaf->next = NULL;
	leaf->is_leaf = true;
    return leaf;
}



/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to 
 * the node to the left of the key to be inserted.
 */
int get_left_index(page_t *parent, pagenum_t left) {

    int left_index = 0;
	if(parent->pointer == left)
		return -1;
    while (left_index <= parent->num_keys && 
            parent->pagenum[left_index] != left)
        left_index++;
    return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
void insert_into_leaf(int table_id,  page_t * leaf, int64_t key, record * pointer ) {

    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->record[i] = leaf->record[i - 1];
    }
    leaf->keys[insertion_point] = key;
    leaf->record[insertion_point] = pointer;
    leaf->num_keys++;
	buf_return_page(table_id, leaf->mypage,true, leaf->index);
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
pagenum_t insert_into_leaf_after_splitting(int table_id, pagenum_t root, page_t * leaf, int64_t key, record * pointer) {
	
    page_t * new_leaf = buf_alloc_page(table_id);
  
    int * temp_keys;
	record ** temp_pointers;
    int insertion_index, split, i, j, order;
	int64_t new_key;
	order = LEAF_ORDER;
	new_leaf->num_keys =0;
	new_leaf->is_leaf =1;

    temp_keys = malloc( order * sizeof(int) );
    if (temp_keys == NULL) {
        perror("Temporary keys array.");
        exit(EXIT_FAILURE);
    }

    temp_pointers = malloc( order * sizeof(record) );
    if (temp_pointers == NULL) {
        perror("Temporary pointers array.");
        exit(EXIT_FAILURE);
    }

    insertion_index = 0;
    while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->keys[i];
        temp_pointers[j] = leaf->record[i];
    }

    temp_keys[insertion_index] = key;
    temp_pointers[insertion_index] = pointer;

    leaf->num_keys = 0;

    split = cut(order-1);

    for (i = 0; i < split; i++) {
        leaf->record[i] = temp_pointers[i];
        leaf->keys[i] = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < order; i++, j++) {
        new_leaf->record[j] = temp_pointers[i];
        new_leaf->keys[j] = temp_keys[i];
        new_leaf->num_keys++;
    }

    free(temp_pointers);
    free(temp_keys);

    new_leaf->pointer = leaf->pointer;
    leaf->pointer = new_leaf->mypage;

    for (i = leaf->num_keys; i < order - 1; i++)
        leaf->record[i] = NULL;
    for (i = new_leaf->num_keys; i < order - 1; i++)
        new_leaf->record[i] = NULL;

    new_leaf->parentPageNum = leaf->parentPageNum;
    new_key = new_leaf->keys[0];
	
	return insert_into_parent(table_id, root, leaf, new_key, new_leaf);



}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
void insert_into_node(int table_id, page_t *p, int left_index, 
			int64_t key, page_t *right) {
    int i;
    for (i = p->num_keys-1; i > left_index; i--) {
		p->pagenum[i+1 ] = p->pagenum[i];
        p->keys[i+1] = p->keys[i];
    }
    p->pagenum[left_index + 1] = right->mypage;
    p->keys[left_index+1] = key;
    p->num_keys++;

	buf_return_page(table_id, p->mypage, true, p->index);
	buf_return_page(table_id, right->mypage, true, right->index);
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
pagenum_t insert_into_node_after_splitting(int table_id, pagenum_t root, page_t *old_page, int left_index, 

    int64_t key, page_t *right) {

    int i, j, split, order;
    int64_t k_prime;
	page_t *new_page = buf_alloc_page(table_id);
	page_t *child;
    int * temp_keys;
	node *temp;
    pagenum_t *temp_pagenum;
	new_page ->is_leaf = 0;

	order = INTERNAL_ORDER;
    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places. 
     * Then create a new node and copy half of the 
     * keys and pointers to the old node and
     * the other half to the new.
 */

    temp_pagenum = malloc( (order + 1) * sizeof(pagenum_t) );
    if (temp_pagenum == NULL) {
        perror("Temporary pointers array for splitting nodes.");
        exit(EXIT_FAILURE);
    }
    temp_keys = malloc( order * sizeof(int) );
    if (temp_keys == NULL) {
        perror("Temporary keys array for splitting nodes.");
        exit(EXIT_FAILURE);
    }
	
	temp_pagenum[0] = old_page->pointer;
    for (i = 0, j = 0; i < old_page->num_keys + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pagenum[j+1] = old_page->pagenum[i];
    }

    for (i = 0, j = 0; i < old_page->num_keys; i++, j++) {
        if (j == left_index+1) j++;
        temp_keys[j] = old_page->keys[i];
    }

    temp_pagenum[left_index + 2] = right->mypage;
    temp_keys[left_index+1] = key;

	/* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
    split = cut(order);
    old_page->num_keys = 0;
	new_page->num_keys = 0;
	old_page->pointer = temp_pagenum[0];
    for (i = 0; i < split - 1; i++) {
        old_page->pagenum[i] = temp_pagenum[i+1];
        old_page->keys[i] = temp_keys[i];
        old_page->num_keys++;
    }

    k_prime = temp_keys[split-1];
	new_page->pointer = temp_pagenum[++i];
    for ( j = 0; i < order; i++, j++) {
        new_page->pagenum[j] = temp_pagenum[i+1];
        new_page->keys[j] = temp_keys[i];
        new_page->num_keys++;
    }
    free(temp_pagenum);
    free(temp_keys);
    new_page->parentPageNum = old_page->parentPageNum;
	child = buf_read_page(table_id, new_page->pointer);
	child->parentPageNum = new_page->mypage;
	buf_return_page(table_id, child->mypage, true, child->index);
	for (i = 0; i < new_page->num_keys; i++) {
        child = buf_read_page(table_id, new_page->pagenum[i]);
        child->parentPageNum = new_page->mypage;
		buf_return_page(table_id, child->mypage, true, child->index);
		
    }
    /* Insert a new key into the parent of the two
     * pages resulting from thesplit, with
     * the old page to the left and the new to the right.
     */
    return insert_into_parent(table_id, root, old_page, k_prime, new_page);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
pagenum_t insert_into_parent(int table_id, pagenum_t root,page_t *left, int64_t key, page_t *right) {

    int left_index;
    page_t *parent;

	/* Case: new root. */
    if (left->parentPageNum == 0){
        return insert_into_new_root(table_id, left, key, right);
	}
    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */
	parent = buf_read_page(table_id, left->parentPageNum);
    left_index = get_left_index(parent, left->mypage);


    /* Simple case: the new key fits into the node. 
     */

    if (parent->num_keys < (INTERNAL_ORDER-1)){
    	insert_into_node(table_id, parent, left_index, key, right);
		buf_return_page(table_id, left->mypage, true, left->index);
		return root;
	}
    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */
	buf_return_page(table_id, left->mypage, true, left->index);
	buf_return_page(table_id, right->mypage, true, left->index);
    return insert_into_node_after_splitting(table_id, root, parent, left_index, key, right);
	
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
pagenum_t insert_into_new_root(int table_id, page_t *left, int64_t key, page_t *right) {

    page_t * root =	buf_alloc_page(table_id);
    root->is_leaf = 0;
	root->keys[0] = key;
    root->pointer = left->mypage;
    root->pagenum[0] = right->mypage;
    root->num_keys = 1;
    root->parentPageNum = 0;
    left->parentPageNum = root->mypage;
    right->parentPageNum = root->mypage;

	buf_return_page(table_id, left->mypage, true, left->index);
	buf_return_page(table_id, right->mypage, true, right->index);
	buf_return_page(table_id, root->mypage, true, root->index);
    return root->mypage;
}



/* First insertion:
 * start a new tree.
 */
pagenum_t start_new_tree(int table_id, int64_t key, record * pointer) {

    page_t *page = buf_alloc_page(table_id);
	page->keys[0] = key;
    page->record[0] = pointer;
    page->pointer= 0;
    page->parentPageNum = 0;
	page->is_leaf = 1;
    page->num_keys =1;
	
	buf_return_page(table_id, page->mypage, true, page->index);	
	return page->mypage;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
 void insert(int table_id, pagenum_t root, int64_t key, char *value ) {

	page_t *header;
	pagenum_t root_page = 0;
	page_t *page; 
   	record * pointer;

    /* Create a new record for the
     * value.
     */
    pointer = make_record(value);

    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root == 0) {
     	root_page=start_new_tree(table_id, key, pointer);
	}


    /* Case: the tree already exists.
     * (Rest of function body.)
     */
    
	
	/* Case: leaf has room for key and pointer.
     */
	else{
		page = find_leaf(table_id, root,key);
		if (page->num_keys < LEAF_ORDER - 1) {
        	insert_into_leaf(table_id, page, key, pointer);
				
		}
        


    /* Case:  leaf must be split.
     */	
	 	else
    		root_page =insert_into_leaf_after_splitting(table_id, root, page, key, pointer);
	}
	
	if(root_page != 0 && root != root_page){
		header = buf_read_page(table_id, 0);
		header->rootPageNum = root_page;
		buf_return_page(table_id, 0,true, header->index);
	}

}



// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_neighbor_index(int table_id, page_t *p ) {

    int i;
	page_t *parent;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
	parent = buf_read_page(table_id, p->parentPageNum);
	if(parent->pointer == p->mypage){
		buf_return_page(table_id, parent->mypage, false, parent->index);
		return -1;
	}
    for (i = 0; i < parent->num_keys; i++){
        if (parent->pagenum[i] == p->mypage){
			buf_return_page(table_id, parent->mypage, false, parent->index);
        	return i;
		}
	}
}


page_t* remove_entry_from_page(page_t *p, int64_t key, pagenum_t pagenum) {

    int i, index, order, neighbor_index;
	record *r;
	node *tmp;
    // Remove the key and shift other keys accordingly.
    i = 0;
    while (p->keys[i] != key)
        i++;
	index = i;
    for (++i; i < p->num_keys; i++)
        p->keys[i - 1] = p->keys[i];
		

    // Remove the pointer and shift other pointers accordingly.
    // First determine number of pointers.
	//page is leaf
	if(p->is_leaf){
		i=0;
		while(index != i)
			i++;
		for(++i; i< p->num_keys; i++)
			p->record[i-1] = p->record[i];
	}


	//page is internal
	else{
		if(p->pointer==pagenum){
			p->pointer = p->pagenum[0];
    		for(i = 0; i< p->num_keys; i++)
				p->pagenum[i] = p->pagenum[i+1];
		}
		else{
			i =0;
			if(p->num_keys ==1){
				if(p->pointer == pagenum)
					p->pointer =p->pagenum[0];
			}
			else{
    			while (p->pagenum[i] != pagenum)
        			i++;
   	 			for (++i; i < p->num_keys; i++)
        			p->pagenum[i - 1] = p->pagenum[i];
			}
		}
	}
    // One key fewer.
    p->num_keys--;
    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
	return p;
}


pagenum_t adjust_root(int table_id, page_t *root) {

    int i;
	page_t *new_root;
//	page = buf_read_page(table_id, root);
    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */
    if (root->num_keys > 0){
		buf_return_page(table_id, root->mypage, true, root->index);

		return root->mypage;
	}
    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.
    if (!root->is_leaf) {
		new_root = buf_read_page(table_id, root->pointer);
        new_root->parentPageNum = 0;

		buf_free_page(table_id, root->mypage, root->index);
		buf_return_page(table_id, new_root->mypage, true, new_root->index);
		return new_root->mypage;
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else{
     	buf_free_page(table_id, root->mypage, root->index);
	 	return 0;
	}
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
pagenum_t coalesce_nodes(int table_id, pagenum_t root, page_t *p, page_t* parent, page_t *neighbor, int neighbor_index, int64_t k_prime) {

    int i, j, neighbor_insertion_index, n_end;
	pagenum_t pagenum;
	page_t *child;
    
	/* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */
	if(neighbor_index ==-1){
		for(i =neighbor->num_keys; i>0; i--){
			neighbor->keys[i] = neighbor->keys[i-1];
			neighbor->pagenum[i]= neighbor->pagenum[i-1];
		}
		neighbor->pagenum[0] = neighbor->pointer;
		
		neighbor->keys[0] = k_prime;
		neighbor->pointer = p->pointer;

	}
	else{
		neighbor_insertion_index = neighbor->num_keys;
		neighbor->keys[neighbor_insertion_index] = k_prime;
		neighbor->pagenum[neighbor_insertion_index] = p->pointer;
	}
	neighbor->num_keys++;
	child = buf_read_page(table_id, p->pointer);
	child->parentPageNum = neighbor->mypage;
	buf_return_page(table_id, child->mypage, true, child->index);
	
	buf_return_page(table_id, neighbor->mypage, true, neighbor->index);
	pagenum = p->mypage;
	buf_free_page(table_id, p->mypage, p->index);
	
	return delete_entry(table_id, root, parent, k_prime, pagenum);
}
	

/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
pagenum_t redistribute_nodes(int table_id, pagenum_t root, page_t *p, page_t* parent, page_t *neighbor, int neighbor_index, 
        int k_prime_index, int64_t k_prime) {  

    int i;
    page_t* tmp;

    /* Case: n has a neighbor to the left. 
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

    if (neighbor_index != -1) {
        for (i = p->num_keys; i > 0; i--) {
            p->keys[i] = p->keys[i - 1];
            p->pagenum[i] = p->pagenum[i - 1];
        }
		p->pagenum[0] = p->pointer;

		p->pointer = neighbor->pagenum[neighbor->num_keys-1];
		tmp = buf_read_page(table_id, p->pointer);
		tmp->parentPageNum= p->mypage;

		p->keys[0] = k_prime;
		parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys-1];
    }
	
    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {  
        p->keys[p->num_keys] = k_prime;
        p->pagenum[p->num_keys] = neighbor->pointer;
        tmp = buf_read_page(table_id, neighbor->pointer);
        tmp->parentPageNum = p->mypage;
        parent->keys[k_prime_index] = neighbor->keys[0];
        
		neighbor->pointer = neighbor->pagenum[0];
        for (i = 0; i < neighbor->num_keys - 1; i++) {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->pagenum[i] = neighbor->pagenum[i + 1];
        }
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */
    p->num_keys++;
    neighbor->num_keys--;

	buf_return_page(table_id, p->mypage,true, p->index);
	buf_return_page(table_id, neighbor->mypage, true, neighbor->index);
	buf_return_page(table_id, tmp->mypage, true, tmp->index);
	buf_return_page(table_id, parent->mypage, true, parent->index);

    return root;
}


/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
pagenum_t delete_entry(int table_id,  pagenum_t root, page_t *p, int64_t key, pagenum_t pagenum) {

    page_t *neighbor, *parent;
	pagenum_t num;
    int neighbor_index;
    int k_prime_index;
	int64_t k_prime;
//	printf("cur page :%ld, %d\n", p->mypage, p->num_keys);
//	print_buf();
//	printf("root : %ld\n", root);
    p = remove_entry_from_page(p, key, pagenum);
    /* Case:  deletion from the root. 
     */
	if(p->mypage ==root){
	//	buf_return_page(table_id, p->mypage,true);
//		printf("a");
		return adjust_root(table_id, p);
	}
    /* Case:  deletion from a node below the root.
     * (Rest of function body.)
     */

    /* Determine minimum allowable size of node,
     * to be preserved after deletion.
     */


    /* Case:  node stays at or above minimum.
     * (The simple case.)
     */

    if (p->num_keys > 0){
//		printf("b");
    	buf_return_page(table_id, p->mypage, true, p->index);
		return root;
	}

    /* Case:  node falls below minimum.
     * Either coalescence or redistribution
     * is needed.
     */

    /* Find the appropriate neighbor node with which
     * to coalesce.
     * Also find the key (k_prime) in the parent
     * between the pointer to node n and the pointer
     * to the neighbor.
     */
//	printf("c");
    neighbor_index = get_neighbor_index(table_id, p );
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
	parent = buf_read_page(table_id, p->parentPageNum);
	k_prime = parent->keys[k_prime_index];
    if(neighbor_index == -1)
		neighbor = buf_read_page(table_id, parent->pagenum[0]);
	else if(neighbor_index== 0)
		neighbor = buf_read_page(table_id, parent->pointer);
	else
    	neighbor = buf_read_page(table_id, parent->pagenum[neighbor_index]);

	if(p->is_leaf){
		if(neighbor_index != -1){
			neighbor->pointer = p->pointer;
			buf_return_page(table_id, neighbor->mypage, true, neighbor->index);
		}
		else
			buf_return_page(table_id, neighbor->mypage, false, neighbor->index);
			
		num = p->mypage;
		buf_free_page(table_id, p->mypage, p->index);
//		printf("d");
		return delete_entry(table_id, root, parent,k_prime, num);
	}
	/* Coalescence. */
	if(neighbor->num_keys != INTERNAL_ORDER-1){
//		printf("d");
    	return coalesce_nodes(table_id, root, p, parent,neighbor, neighbor_index, k_prime);
	}
    /* Redistribution. */
	else{
  //  	printf("e");
		return redistribute_nodes(table_id, root, p, parent, neighbor, neighbor_index, k_prime_index, k_prime);
	}
}



/* Master deletion function.
 */
void delete(int table_id, pagenum_t root, int64_t key) {

    pagenum_t root_page;
	page_t *key_leaf;
	page_t *header;

    key_leaf = find_leaf(table_id, root, key);

    if (key_leaf != NULL) {
        root_page = delete_entry(table_id, root, key_leaf, key, 0);
		if(root_page != root){
			//printf("root_page : %ld\n", root_page);
			header =buf_read_page(table_id, 0);
			header->rootPageNum = root_page;
			buf_return_page(table_id, 0, true, header->index);
		}
    }
}


void destroy_tree_nodes(node * root) {
    int i;
    if (root->is_leaf)
        for (i = 0; i < root->num_keys; i++)
            free(root->pointers[i]);
    else
        for (i = 0; i < root->num_keys + 1; i++)
            destroy_tree_nodes(root->pointers[i]);
    free(root->pointers);
    free(root->keys);
    free(root);
}


node * destroy_tree(node * root) {
    destroy_tree_nodes(root);
    return NULL;
}

