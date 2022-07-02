/**
 * Copyright (C) 2022 Arun007coder
 * 
 * This file is part of SectorOS-RE2.
 * 
 * SectorOS-RE2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SectorOS-RE2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gentree.h"

gentree_t* gentree_create()
{
    return (gentree_t *)kcalloc(sizeof(gentree_t), 1);
}

gentree_node_t *gentree_node_create(void *data)
{
    gentree_node_t *n = (gentree_node_t*)kcalloc(sizeof(gentree_node_t), 1);
    n->data = data;
    n->children = list_create();
    return n;
}

gentree_node_t *gentree_insert(gentree_t *tree, gentree_node_t *subroot, void *data)
{
    gentree_node_t *treenode = (gentree_node_t*)kcalloc(sizeof(gentree_node_t), 1);
    treenode->children = list_create();
    treenode->data = data;

    if (!tree->root)
    {
        tree->root = treenode;
        return treenode;
    }
    list_insert_front(subroot->children, treenode);
    return treenode;
}

gentree_node_t *gentree_findParent(gentree_t *tree, gentree_node_t *remove_node, int *child_index)
{
    if (remove_node == tree->root)
        return NULL;
    return gentree_findParent_recur(tree, remove_node, tree->root, child_index);
}

gentree_node_t *gentree_findParent_recur(gentree_t *tree, gentree_node_t *remove_node, gentree_node_t *subroot, int *child_index)
{
    int idx;
    if ((idx = list_contain(subroot->children, remove_node)) != -1)
    {
        *child_index = idx;
        return subroot;
    }
    foreach (child, subroot->children)
    {
        gentree_node_t *ret = gentree_findParent_recur(tree, remove_node, (gentree_node_t*)child->val, child_index);
        if (ret != NULL)
        {
            return ret;
        }
    }
    return NULL;
}

void gentree_remove(gentree_t *tree, gentree_node_t *remove_node)
{
    int child_index = -1;
    gentree_node_t *parent = gentree_findParent(tree, remove_node, &child_index);
    if (parent != NULL)
    {
        gentree_node_t *freethis = (gentree_node_t*)list_remove_by_index(parent->children, child_index);
        kfree(freethis);
    }
}

void tree2list_recur(gentree_node_t *subroot, list_t *list)
{
    if (subroot == NULL)
        return;
    foreach (child, subroot->children)
    {
        gentree_node_t *curr_treenode = (gentree_node_t *)child->val;
        void *curr_val = curr_treenode->data;
        list_insert_back(list, curr_val);
        tree2list_recur((gentree_node_t*)child->val, list);
    }
}

void tree2list(gentree_t *tree, list_t *list)
{
    tree2list_recur(tree->root, list);
}

void tree2array(gentree_t *tree, void **array, int *size)
{
    tree2array_recur(tree->root, array, size);
}

void tree2array_recur(gentree_node_t *subroot, void **array, int *size)
{
    if (subroot == NULL)
        return;
    void *curr_val = (void *)subroot->data;
    array[*size] = curr_val;
    *size = *size + 1;
    foreach (child, subroot->children)
    {
        tree2array_recur((gentree_node_t*)child->val, array, size);
    }
}