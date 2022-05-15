#ifndef __GENTREE_H_
#define __GENTREE_H_

#include "list.h"
#include "kheap.h"
#include "system.h"

typedef struct gentree_node
{
    list_t *children;
    void *data;
}gentree_node_t;

typedef struct gentree
{
    gentree_node_t *root;
}gentree_t;

gentree_t* gentree_create();

gentree_node_t* gentree_node_create(void *data);
gentree_node_t* gentree_insert(gentree_t *tree, gentree_node_t *subroot, void *data);
gentree_node_t* gentree_findParent(gentree_t *tree, gentree_node_t *remove_node, int* child_index);
gentree_node_t* gentree_findParent_recur(gentree_t *tree, gentree_node_t *remove_node, gentree_node_t *subroot, int *child_index);

void gentree_remove(gentree_t *tree, gentree_node_t *remove_node);
void tree2list_recur(gentree_node_t *subroot, list_t *list);
void tree2list(gentree_t *tree, list_t *list);
void tree2array(gentree_t *tree, void **array, int *size);
void tree2array_recur(gentree_node_t *subroot, void **array, int *size);

#endif