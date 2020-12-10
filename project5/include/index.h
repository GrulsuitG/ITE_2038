#ifndef __BPT_H__
#define __BPT_H__

// Uncomment the line below if you are compiling on Windows.
// #define WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "buffer.h"
#include "lock_table.h"
#include "transaction.h"

#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif


/* Type representing the record
 * to which a given key refers.
 * In a real B+ tree system, the
 * record would hold data (in a database)
 * or a file (in an operating system)
 * or some other information.
 * Users can rewrite this part of the code
 * to change the type and content
 * of the value field.
 */


/* Type representing a node in the B+ tree.
 * This type is general enough to serve for both
 * the leaf and the internal node.
 * The heart of the node is the array
 * of keys and the array of corresponding
 * pointers.  The relation between keys
 * and pointers differs between leaves and
 * internal nodes.  In a leaf, the index
 * of each key equals the index of its corresponding
 * pointer, with a maximum of order - 1 key-pointer
 * pairs.  The last pointer points to the
 * leaf to the right (or NULL in the case
 * of the rightmost leaf).
 * In an internal node, the first pointer
 * refers to lower nodes with keys less than
 * the smallest key in the keys array.  Then,
 * with indices i starting at 0, the pointer
 * at i + 1 points to the subtree with keys
 * greater than or equal to the key in this
 * node at index i.
 * The num_keys field is used to keep
 * track of the number of valid keys.
 * In an internal node, the number of valid
 * pointers is always num_keys + 1.
 * In a leaf, the number of valid pointers
 * to data is always num_keys.  The
 * last leaf pointer points to the next leaf.
 */
typedef struct node {
	pagenum_t pagenum;    
	void ** pointers;
    int * keys;
    struct node * parent;
    bool is_leaf;
    int num_keys;
    struct node * next; // Used for queue.
} node;


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
extern node * queue;
//page_t *Q;

/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
extern bool verbose_output;

pthread_mutex_t index_latch;
// FUNCTION PROTOTYPES.

// Output and utility.

void enqueue( node * new_node );
node * dequeue( void );

page_t* node_to_page(node* node);
node* page_to_node(page_t *page, pagenum_t pagenum);
node* syncFileAndTree(int table_id);

//int height( node * root );
int path_to_root( node * root, node * child );
void print_tree(int table_id);
/*void print_leaves( node * root );

void find_and_print(node * root, int64_t key, bool verbose); 
void find_and_print_range(node * root, int64_t range1, int64_t range2, bool verbose); 
int find_range( node * root, int64_t key_start, int64_t key_end, bool verbose,
        int64_t returned_keys[], void * returned_pointers[]); */
page_t * find_leaf(int table_id, pagenum_t root, int64_t key);
record* find(int table_id, pagenum_t *root, int64_t key);
int find_record(int table_id, int64_t key, int trx_id,int lock_mode, trxList* t, char* str);
int cut( int length );

int index_init(int num_buf);

int index_open(int table_id, char* filename);
int index_close(int table_id);

// Insertion.

record * make_record(char *value);
node * make_node( void );
node * make_leaf( void );
int get_left_index(page_t *parent, pagenum_t left);
void insert_into_leaf(int table_id,  page_t * leaf, int64_t key, record * pointer );
pagenum_t insert_into_leaf_after_splitting(int table_id, pagenum_t root, page_t *leaf, 
				int64_t key, record * pointer);
void insert_into_node(int table_id, page_t *parent, int left_index, int64_t key, page_t *right);
pagenum_t insert_into_node_after_splitting(int table_id, pagenum_t root, page_t *parent,
		int left_index, int64_t key, page_t *right);
pagenum_t insert_into_parent(int table_id, pagenum_t root, page_t * left, int64_t key, page_t * right);
pagenum_t insert_into_new_root(int table_id, page_t * left, int64_t key, page_t * right);
pagenum_t start_new_tree(int table_id, int64_t key, record * pointer);
void insert(int table_id, pagenum_t root, int64_t key, char *value );

// Deletion.

int get_neighbor_index(int table_id,  page_t *p );
pagenum_t adjust_root(int table_id, page_t *root);
pagenum_t coalesce_nodes(int table_id, pagenum_t root, page_t *p, page_t *parent, page_t *neighbor, int neighbor_index, int64_t k_prime);
pagenum_t redistribute_nodes(int table_id, pagenum_t root, page_t *p, page_t *parent ,page_t *neighbor,
         int neighbor_index,int k_prime_index, int64_t k_prime);
pagenum_t delete_entry(int table_id, pagenum_t root, page_t *p, int64_t key, pagenum_t pagenum);
page_t * remove_entry_from_page(page_t *p, int64_t key, pagenum_t pagenum);
void delete(int table_id, pagenum_t root, int64_t key);

void destroy_tree_nodes(node * root);
node * destroy_tree(node * root);

#endif /* __BPT_H__*/
