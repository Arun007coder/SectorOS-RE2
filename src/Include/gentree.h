// Copyright (C) 2022 Arun007coder
// 
// This file is part of SectorOS-RE2.
// 
// SectorOS-RE2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SectorOS-RE2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

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