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

/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
node * queue = NULL;
node * Q = NULL;

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
/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
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
void print_tree( node * root ) {

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
 */
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
 */
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
 */
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


/* Traces the path from the root to a leaf, searching
 * by key.  Displays information about the path
 * if the verbose flag is set.
 * Returns the leaf containing the given key.
 */
node * find_leaf( node * root, int64_t key, bool verbose ) {
    int i = 0;
    node * c = root;
    if (c == NULL) {
        if (verbose) 
            printf("Empty tree.\n");
        return c;
    }
    while (!c->is_leaf) {
        if (verbose) {
            printf("[");
            for (i = 0; i < c->num_keys - 1; i++)
                printf("%d ", c->keys[i]);
            printf("%d] ", c->keys[i]);
        }
        i = 0;
        while (i < c->num_keys) {
            if (key >= c->keys[i]) i++;
            else break;
        }
        if (verbose)
            printf("%d ->\n", i);
        c = (node *)c->pointers[i];
    }
    if (verbose) {
        printf("Leaf [");
        for (i = 0; i < c->num_keys - 1; i++)
            printf("%d ", c->keys[i]);
        printf("%d] ->\n", c->keys[i]);
    }
    return c;
}


/* Finds and returns the record to which
 * a key refers.
 */
record * find( node * root, int64_t key, bool verbose ) {
    int i = 0;
    node * c = find_leaf( root, key, verbose );
    if (c == NULL) return NULL;
    for (i = 0; i < c->num_keys; i++)
        if (c->keys[i] == key) break;
    if (i == c->num_keys) 
        return NULL;
    else
        return (record *)c->pointers[i];
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
	leaf->pagenum = 0;
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
int get_left_index(node * parent, node * left) {

    int left_index = 0;
    while (left_index <= parent->num_keys && 
            parent->pointers[left_index] != left)
        left_index++;
    return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
node * insert_into_leaf( node * leaf, int64_t key, record * pointer ) {

    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
        insertion_point++;

    for (i = leaf->num_keys; i > insertion_point; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }
    leaf->keys[insertion_point] = key;
    leaf->pointers[insertion_point] = pointer;
    leaf->num_keys++;
	return leaf;
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
node * insert_into_leaf_after_splitting(node * root, node * leaf, int64_t key, record * pointer) {

    node * new_leaf;
    int * temp_keys;
    void ** temp_pointers;
    int insertion_index, split, i, j, order;
	int64_t new_key;
	order = LEAF_ORDER;
    new_leaf = make_leaf();

    temp_keys = malloc( order * sizeof(int) );
    if (temp_keys == NULL) {
        perror("Temporary keys array.");
        exit(EXIT_FAILURE);
    }

    temp_pointers = malloc( order * sizeof(void *) );
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
        temp_pointers[j] = leaf->pointers[i];
    }

    temp_keys[insertion_index] = key;
    temp_pointers[insertion_index] = pointer;

    leaf->num_keys = 0;

    split = cut(order-1);

    for (i = 0; i < split; i++) {
        leaf->pointers[i] = temp_pointers[i];
        leaf->keys[i] = temp_keys[i];
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < order; i++, j++) {
        new_leaf->pointers[j] = temp_pointers[i];
        new_leaf->keys[j] = temp_keys[i];
        new_leaf->num_keys++;
    }

    free(temp_pointers);
    free(temp_keys);

    new_leaf->pointers[LEAF_ORDER-1] = leaf->pointers[LEAF_ORDER-1];
    leaf->pointers[LEAF_ORDER-1] = new_leaf;

    for (i = leaf->num_keys; i < order - 1; i++)
        leaf->pointers[i] = NULL;
    for (i = new_leaf->num_keys; i < order - 1; i++)
        new_leaf->pointers[i] = NULL;

    new_leaf->parent = leaf->parent;
    new_key = new_leaf->keys[0];
	
	//enqueue(new_leaf);
	//enqueue(leaf);

    return insert_into_parent(root, leaf, new_key, new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
node * insert_into_node(node * root, node * n, 
        int left_index, int64_t key, node * right) {
    int i;
    for (i = n->num_keys; i > left_index; i--) {
        n->pointers[i + 1] = n->pointers[i];
        n->keys[i] = n->keys[i - 1];
    }
    n->pointers[left_index + 1] = right;
    n->keys[left_index] = key;
    n->num_keys++;
	enqueue(n);
	return root;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
node * insert_into_node_after_splitting(node * root, node * old_node, int left_index, 
        int64_t key, node * right) {

    int i, j, split, order;
    int64_t k_prime;
	node * new_node, * child;
    int * temp_keys;
	node *temp;
    node ** temp_pointers;

	order = INTERNAL_ORDER;
    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places. 
     * Then create a new node and copy half of the 
     * keys and pointers to the old node and
     * the other half to the new.
 */

    temp_pointers = malloc( (order + 1) * sizeof(node *) );
    if (temp_pointers == NULL) {
        perror("Temporary pointers array for splitting nodes.");
        exit(EXIT_FAILURE);
    }
    temp_keys = malloc( order * sizeof(int) );
    if (temp_keys == NULL) {
        perror("Temporary keys array for splitting nodes.");
        exit(EXIT_FAILURE);
    }

    for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pointers[j] = old_node->pointers[i];
    }

    for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = old_node->keys[i];
    }

    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
    split = cut(order);
    new_node = make_node();
    old_node->num_keys = 0;
    for (i = 0; i < split - 1; i++) {
        old_node->pointers[i] = temp_pointers[i];
        old_node->keys[i] = temp_keys[i];
        old_node->num_keys++;
    }
    old_node->pointers[i] = temp_pointers[i];
    k_prime = temp_keys[split - 1];
    for (++i, j = 0; i < order; i++, j++) {
        new_node->pointers[j] = temp_pointers[i];
        new_node->keys[j] = temp_keys[i];
        new_node->num_keys++;
    }
    new_node->pointers[j] = temp_pointers[i];
    free(temp_pointers);
    free(temp_keys);
    new_node->parent = old_node->parent;
	for (i = 0; i <= new_node->num_keys; i++) {
        child = new_node->pointers[i];
        child->parent = new_node;
		if(child->next ==NULL)
			enqueue(child);
    }
    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */
    return insert_into_parent(root, old_node, k_prime, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node * insert_into_parent(node * root, node * left, int64_t key, node * right) {

    int left_index;
    node * parent;

    parent = left->parent;

	enqueue(right);
	enqueue(left);
	/* Case: new root. */

    if (parent == NULL)
        return insert_into_new_root(left, key, right);

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */

    left_index = get_left_index(parent, left);


    /* Simple case: the new key fits into the node. 
     */

    if (parent->num_keys < (INTERNAL_ORDER-1))
        return insert_into_node(root, parent, left_index, key, right);

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */

    return insert_into_node_after_splitting(root, parent, left_index, key, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
node * insert_into_new_root(node * left, int64_t key, node * right) {

    node * root = make_node();
    root->keys[0] = key;
    root->pointers[0] = left;
    root->pointers[1] = right;
    root->num_keys++;
    root->parent = NULL;
    left->parent = root;
    right->parent = root;
	enqueue(root);
    return root;
}



/* First insertion:
 * start a new tree.
 */
node * start_new_tree(int64_t key, record * pointer) {

    node * root = make_leaf();
    root->keys[0] = key;
    root->pointers[0] = pointer;
    root->pointers[(LEAF_ORDER-1)] = NULL;
    root->parent = NULL;
    root->num_keys++;
	enqueue(root);
    return root;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
node * insert( node * root, int64_t key, char *value ) {

    record * pointer;
    node * leaf;

    /* The current implementation ignores
     * duplicates.
     */

    if (find(root, key, false) != NULL)
        return root;

    /* Create a new record for the
     * value.
     */
    pointer = make_record(value);


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root == NULL) 
        return start_new_tree(key, pointer);


    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    leaf = find_leaf(root, key, false);

    /* Case: leaf has room for key and pointer.
     */

    if (leaf->num_keys < LEAF_ORDER - 1) {
        leaf = insert_into_leaf(leaf, key, pointer);
        enqueue(leaf);
		return root;
    }


    /* Case:  leaf must be split.
     */

    return insert_into_leaf_after_splitting(root, leaf, key, pointer);
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

    key_record = find(root, key, false);
    key_leaf = find_leaf(root, key, false);
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

