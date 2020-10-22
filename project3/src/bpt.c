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

#include "bpt.h"

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
            page->key[i] = n->keys[i];
			tmp_record = n->pointers[i];
            strncpy(page->record[i]->value, tmp_record->value, VALUE_SIZE);
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
            temp = make_record(page->record[i]->value);
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
   	parentpage = init_page();
   	childpage = init_page();
    file_read_page(0, header);
    if(header->rootPageNum == 0)
        return root;
   	
    file_read_page(header->rootPageNum, parentpage);
	
    root = page_to_node(parentpage, header->rootPageNum);
    if(parentpage->pointer == 0)
    	return root;
	enqueue(root);
    
    while ( queue ){
    
        parent= dequeue();
        if(!parent->is_leaf){
        	file_read_page(parent->pagenum, parentpage);
      		
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

 
void print_tree() {
	
	node* root =syncFileAndTree(); 
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
page_t* find_leaf( pagenum_t *root, int64_t key) {
    int i;
    page_t *page = init_page();
    //num = *root;
	file_read_page(*root, page);
    while(!page->is_leaf){
	//	printf("a");
		if(key<page->key[0]){
	//		printf("b");
			file_read_page(page->pointer, page);
			continue;
		}
    	for(i=1; i<page->num_keys; i++){
			if(page->key[i-1] <= key && key < page->key[i]){
			//	printf("%d",i);
				file_read_page(page->pagenum[i-1], page);
				break;
			}
		}
		if(i == page->num_keys && page->key[i-1] <= key){
			//printf("d");
			file_read_page(page->pagenum[i-1], page);
			}
		
    }
	//printf("\n");*/
	return page;
    
}


/* Finds and returns the record to which
 * a key refers.
 */
record* find(pagenum_t *root, int64_t key) {
    int num, i = 0;
	page_t *header = init_page();
	page_t *page = init_page();
	file_read_page(0, header);
	*root= header->rootPageNum;
	if(*root == 0){
		return NULL;
		}
    page = find_leaf(root, key);
    for (i = 0; i < page->num_keys; i++)
        if (page->key[i] == key) break;
    if (i == page->num_keys) 
        return NULL;
    else{
        return page->record[i];
    }
	return NULL;
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
void insert_into_leaf( page_t * leaf, int64_t key, record * pointer ) {

    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->key[insertion_point] < key)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->key[i] = leaf->key[i - 1];
        leaf->record[i] = leaf->record[i - 1];
    }
    leaf->key[insertion_point] = key;
    leaf->record[insertion_point] = pointer;
    leaf->num_keys++;
	file_write_page(leaf->mypage,leaf);
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
pagenum_t insert_into_leaf_after_splitting(pagenum_t *root, page_t * leaf, int64_t key, record * pointer) {

    page_t * new_leaf = init_page();
    int * temp_keys;
	record ** temp_pointers;
    int insertion_index, split, i, j, order;
	int64_t new_key;
	order = LEAF_ORDER;
	new_leaf->is_leaf =1;
    new_leaf->mypage = file_alloc_page();

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
    while (insertion_index < order - 1 && leaf->key[insertion_index] < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->key[i];
        temp_pointers[j] = leaf->record[i];
    }

    temp_keys[insertion_index] = key;
    temp_pointers[insertion_index] = pointer;

    leaf->num_keys = 0;

    split = cut(order-1);

    for (i = 0; i < split; i++) {
        leaf->record[i] = temp_pointers[i];
        leaf->key[i] = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < order; i++, j++) {
        new_leaf->record[j] = temp_pointers[i];
        new_leaf->key[j] = temp_keys[i];
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
    new_key = new_leaf->key[0];
	
	return insert_into_parent(root, leaf, new_key, new_leaf);



}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
void insert_into_node(page_t *p, int left_index, 
			int64_t key, page_t *right) {
    int i;
    for (i = p->num_keys-1; i > left_index; i--) {
		p->pagenum[i+1 ] = p->pagenum[i];
        p->key[i+1] = p->key[i];
    }
    p->pagenum[left_index + 1] = right->mypage;
    p->key[left_index+1] = key;
    p->num_keys++;

	file_write_page(p->mypage, p);
	file_write_page(right->mypage, right);
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
pagenum_t insert_into_node_after_splitting(pagenum_t *root, page_t *old_page, int left_index, 

    int64_t key, page_t *right) {

    int i, j, split, order;
    int64_t k_prime;
	page_t *new_page;
	page_t *child = init_page();
    int * temp_keys;
	node *temp;
    pagenum_t *temp_pagenum;

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
        temp_keys[j] = old_page->key[i];
    }

    temp_pagenum[left_index + 2] = right->mypage;
    temp_keys[left_index+1] = key;

	/* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
    split = cut(order);
    new_page = init_page();
	new_page->mypage = file_alloc_page();
    old_page->num_keys = 0;
	old_page->pointer = temp_pagenum[0];
    for (i = 0; i < split - 1; i++) {
        old_page->pagenum[i] = temp_pagenum[i+1];
        old_page->key[i] = temp_keys[i];
        old_page->num_keys++;
    }

    k_prime = temp_keys[split-1];
	new_page->pointer = temp_pagenum[++i];
    for ( j = 0; i < order; i++, j++) {
        new_page->pagenum[j] = temp_pagenum[i+1];
        new_page->key[j] = temp_keys[i];
        new_page->num_keys++;
    }
    free(temp_pagenum);
    free(temp_keys);
    new_page->parentPageNum = old_page->parentPageNum;
	file_read_page(new_page->pointer, child);
	child->parentPageNum = new_page->mypage;
	file_write_page(child->mypage, child);
	for (i = 0; i < new_page->num_keys; i++) {
        file_read_page(new_page->pagenum[i], child);
        child->parentPageNum = new_page->mypage;
		file_write_page(child->mypage, child);
		
    }
    /* Insert a new key into the parent of the two
     * pages resulting from the split, with
     * the old page to the left and the new to the right.
     */
    return insert_into_parent(root, old_page, k_prime, new_page);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
pagenum_t insert_into_parent(pagenum_t *root,page_t *left, int64_t key, page_t *right) {

    int left_index;
	page_t *header = init_page();
    page_t *parent;

	/* Case: new root. */
    if (left->parentPageNum == 0){
        return insert_into_new_root(left, key, right);
	}
    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */
	parent = init_page();
	file_read_page(left->parentPageNum,parent);
    left_index = get_left_index(parent, left->mypage);


    /* Simple case: the new key fits into the node. 
     */

    if (parent->num_keys < (INTERNAL_ORDER-1)){
    	insert_into_node(parent, left_index, key, right);
		file_write_page(left->mypage, left);
		return *root;
	}
    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */
	file_write_page(left->mypage, left);
	file_write_page(right->mypage, right);
    return insert_into_node_after_splitting(root, parent, left_index, key, right);
	
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
pagenum_t insert_into_new_root(page_t *left, int64_t key, page_t *right) {

    page_t * root = init_page();
	root->mypage = file_alloc_page();
    root->is_leaf = 0;
	root->key[0] = key;
    root->pointer = left->mypage;
    root->pagenum[0] = right->mypage;
    root->num_keys++;
    root->parentPageNum = 0;
    left->parentPageNum = root->mypage;
    right->parentPageNum = root->mypage;

	file_write_page(left->mypage, left);
	file_write_page(right->mypage, right);
	file_write_page(root->mypage, root);
    return root->mypage;
}



/* First insertion:
 * start a new tree.
 */
pagenum_t start_new_tree(int64_t key, record * pointer) {

    page_t *page = init_page();
	page->mypage= file_alloc_page();
	page->key[0] = key;
    page->record[0] = pointer;
    page->pointer= 0;
    page->parentPageNum = 0;
	page->is_leaf = 1;
    page->num_keys++;
	
	file_write_page(page->mypage, page);
	free_page(page);	
	return page->mypage;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
 void insert(pagenum_t *root, int64_t key, char *value ) {

	page_t *header = init_page();
	pagenum_t root_page = 0;
	page_t *page, *q; 
   	record * pointer;
    node * leaf;

    /* Create a new record for the
     * value.
     */
    pointer = make_record(value);

    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (*root == 0) {
       	root_page=start_new_tree(key, pointer);
	//	file_read_page(0, header);
	//	header->rootPageNum = root;
	//	file_write_page(0, header);
	//	free_page(header);
	}


    /* Case: the tree already exists.
     * (Rest of function body.)
     */
    
	
	/* Case: leaf has room for key and pointer.
     */
	else{
		page = find_leaf(root,key);
		if (page->num_keys < LEAF_ORDER - 1) {
        	insert_into_leaf(page, key, pointer);
			
		}
        


    /* Case:  leaf must be split.
     */	
	 	else
    		root_page =insert_into_leaf_after_splitting(root, page, key, pointer);
	}
	
	file_read_page(0,header);
	if(root_page != 0 && *root != root_page){
		header->rootPageNum = root_page;
		file_write_page(0, header);
	}

	free_page(header);
}



// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_neighbor_index( node * n ) {

    int i;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
    for (i = 0; i <= n->parent->num_keys; i++)
        if (n->parent->pointers[i] == n)
            return i - 1;

    // Error state.
    printf("Search for nonexistent pointer to node in parent.\n");
    printf("Node:  %#lx\n", (unsigned long)n);
    exit(EXIT_FAILURE);
}


node * remove_entry_from_node(node * n, int64_t key, node * pointer) {

    int i, num_pointers, order, neighbor_index;
	node *tmp;

    // Remove the key and shift other keys accordingly.
    i = 0;
    while (n->keys[i] != key)
        i++;
    for (++i; i < n->num_keys; i++)
        n->keys[i - 1] = n->keys[i];

    // Remove the pointer and shift other pointers accordingly.
    // First determine number of pointers.
    num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
    i = 0;
    while (n->pointers[i] != pointer)
        i++;
    for (++i; i < num_pointers; i++)
        n->pointers[i - 1] = n->pointers[i];

	
    // One key fewer.
    n->num_keys--;
	order = n->is_leaf ? LEAF_ORDER : INTERNAL_ORDER;
    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
    if (n->is_leaf)
        for (i = n->num_keys; i < order - 1; i++)
            n->pointers[i] = NULL;
    else
        for (i = n->num_keys + 1; i < order; i++)
            n->pointers[i] = NULL;
/*	if(n->is_leaf){
		enqueue(n);
	}*/
	if(!n->is_leaf){
		if(pointer->next ==NULL)
			enqueue(pointer);
		if(n->next ==NULL)
			enqueue(n);
	}
	
	return n;
}


node * adjust_root(node * root) {

    int i;
	node *new_root, *temp;
    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */
    if (root->num_keys > 0){
    	if(root->next == NULL)
			enqueue(root);
		return root;
	}
    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.
    if (!root->is_leaf) {
		new_root = root->pointers[0];
        new_root->parent = NULL;
		if(new_root->next== NULL)
			enqueue(new_root);
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else{
        new_root = NULL;
	}
	if(root->next ==NULL)
		enqueue(root);
	return new_root;
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int64_t k_prime) {

    int i, j, neighbor_insertion_index, n_end;
    pagenum_t pagenum;
	node *tmp;
    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */
    if (neighbor_index == -1) {
		pagenum = neighbor->pagenum;
		neighbor->pagenum = n->pagenum;
		n->pagenum = pagenum;
        tmp = n;
        n = neighbor;
        neighbor = tmp;
		//pagenum = neighbor->pagenum;
		//neighbor->pagenum = n->pagenum;
		//n->pagenum = pagenum;
		
    }

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */
	
    neighbor_insertion_index = neighbor->num_keys;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!n->is_leaf) {

        /* Append k_prime.
         */
		
        neighbor->keys[neighbor_insertion_index] = k_prime;
        neighbor->num_keys++;


        n_end = n->num_keys;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->pointers[i] = n->pointers[j];
            neighbor->num_keys++;
            n->num_keys--;
        }

        /* The number of pointers is always
         * one more than the number of keys.
         */

        neighbor->pointers[i] = n->pointers[j];

        /* All children must now point up to the same parent.
         */

        for (i = 0; i < neighbor->num_keys + 1; i++) {
            tmp = (node *)neighbor->pointers[i];
            tmp->parent = neighbor;
			//enqueue(tmp->parent);
        }
		if(neighbor->next == NULL)
			enqueue(neighbor);
    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
		for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->pointers[i] = n->pointers[j];
            neighbor->num_keys++;
        }
		n->num_keys =0;
        neighbor->pointers[LEAF_ORDER - 1] = n->pointers[LEAF_ORDER - 1];
    	}
	
	if(neighbor_index == -1){
		return delete_entry(root, neighbor->parent,k_prime, n); 
	}
	else{
		return delete_entry(root, n->parent, k_prime ,n);
	}
}


/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, 
        int k_prime_index, int64_t k_prime) {  

    int i;
    node * tmp;

    /* Case: n has a neighbor to the left. 
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

    if (neighbor_index != -1) {
        if (!n->is_leaf)
            n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
        for (i = n->num_keys; i > 0; i--) {
            n->keys[i] = n->keys[i - 1];
            n->pointers[i] = n->pointers[i - 1];
        }
        if (!n->is_leaf) {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys];
            tmp = (node *)n->pointers[0];
            tmp->parent = n;
            neighbor->pointers[neighbor->num_keys] = NULL;
            n->keys[0] = k_prime;
            n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
        }
        else {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
            neighbor->pointers[neighbor->num_keys - 1] = NULL;
            n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
            n->parent->keys[k_prime_index] = n->keys[0];
        }
    }
	
    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {  
        if (n->is_leaf) {
            n->keys[n->num_keys] = neighbor->keys[0];
            n->pointers[n->num_keys] = neighbor->pointers[0];
            n->parent->keys[k_prime_index] = neighbor->keys[1];
        }
        else {
            n->keys[n->num_keys] = k_prime;
            n->pointers[n->num_keys + 1] = neighbor->pointers[0];
            tmp = (node *)n->pointers[n->num_keys + 1];
            tmp->parent = n;
            n->parent->keys[k_prime_index] = neighbor->keys[0];
        }
        for (i = 0; i < neighbor->num_keys - 1; i++) {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->pointers[i] = neighbor->pointers[i + 1];
        }
        if (!n->is_leaf)
            neighbor->pointers[i] = neighbor->pointers[i + 1];
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */
	if(n->next == NULL)
		enqueue(n);
	if(neighbor->next == NULL)
		enqueue(neighbor);
	if(n->parent->next == NULL)
		enqueue(n->parent);
    n->num_keys++;
    neighbor->num_keys--;

    return root;
}


/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
node * delete_entry( node * root, node * n, int64_t key, void * pointer ) {

    node * neighbor;
    int neighbor_index;
    int k_prime_index;
	int64_t k_prime;
    // Remove key and pointer from node.
    n = remove_entry_from_node(n, key, pointer);
    /* Case:  deletion from the root. 
     */
	if(n ==root){
		return adjust_root(root);
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

    if (n->num_keys > 0)
        return root;

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

    neighbor_index = get_neighbor_index( n );
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = n->parent->keys[k_prime_index];
    neighbor = neighbor_index == -1 ? n->parent->pointers[1] : 
    n->parent->pointers[neighbor_index];
	/*if(n->is_leaf){
		if(neighbor_index != -1)
			neighbor->pointers[LEAF_ORDER-1] = n->pointers[LEAF_ORDER-1];
		return delete_entry(root, n->parent,key, n);
	}*/
	/* Coalescence. */
	if(neighbor->num_keys != INTERNAL_ORDER-1)
    	return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);

    /* Redistribution. */
	else
    	return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}



/* Master deletion function.
 */
node * delete(node * root, int64_t key) {

    node  *key_leaf, *temp;
    record * key_record;
/*
    key_record = find(root, key, false);
    key_leaf = find_leaf(root, key, false);*/
    if (key_record != NULL && key_leaf != NULL) {
        root = delete_entry(root, key_leaf, key, key_record);
		if(!queue)
			enqueue(key_leaf);
		free(key_record->value);

		free(key_record);
    }
    return root;
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

