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

#include "list.h"

#include "kheap.h"

list_t *list_create(void)
{
    list_t *list = kcalloc(sizeof(list_t), 1);
    return list;
}

uint32_t list_size(list_t *list)
{
    if(list == NULL)
        return 0;
    return list->size;
}

listnode_t *list_insert_front(list_t *list, void *val)
{
    listnode_t *node = kcalloc(sizeof(listnode_t), 1);
    list->head->prev = node;
    node->next = list->head;
    node->val = val;

    if(!list->head)
        list->tail = node;
    list->head = node;
    list->size++;
    return node;
}

void list_insert_back(list_t *list, void *val)
{
    listnode_t *node = kcalloc(sizeof(listnode_t), 1);
    node->prev = list->tail;
    if(list->tail)
        list->tail->next = node;
    node->val = val;
    if(!list->head)
        list->head = node;
    list->tail = node;
    list->size++;
}

void * list_remove_front(list_t * list)
{
    if(!list->head)
        return NULL;
    listnode_t *node = list->head;
    void* val = node->val;
    list->head = node->next;
    if(list->head)
        list->head->prev = NULL;
    kfree(node);
    list->size--;
    return val;
}

void * list_remove_back(list_t * list)
{
    if(!list->head)
        return NULL;
    listnode_t *node = list->tail;
    void* val = node->val;
    list->tail = node->prev;
    if(list->tail)
        list->tail->next = NULL;
    kfree(node);
    list->size--;
    return val;
}

void list_push(list_t * list, void * val)
{
    list_insert_back(list, val);
}

listnode_t * list_pop(list_t * list)
{
    if(!list->head)
        return NULL;
    listnode_t * node = list->tail;
	list->tail = node->prev;
	if(list->tail)
		list->tail->next = NULL;
	list->size--;
	return node;
}

void list_enqueue(list_t * list, void * val)
{
    list_insert_front(list, val);
}

listnode_t * list_dequeue(list_t * list)
{
    return list_pop(list);
}

void * list_peek_front(list_t * list)
{
    if(!list->head)
        return NULL;
    return list->head->val;
}

void* list_peek_back(list_t * list)
{
    if(!list->tail)
        return NULL;
    return list->tail->val;
}

int list_contain(list_t * list, void * val)
{
    int index = 0;
    foreach(listnode, list)
    {
        if(listnode->val == val)
            return index;
        index++;
    }
    return -1;
}

listnode_t * list_get_node_by_index(list_t * list, int index)
{
    if(index < 0 || index >= list_size(list))
        return NULL;
    int i = 0;
    foreach(listnode, list)
    {
        if(i == index)
            return listnode;
        i++;
    }
    return NULL;
}

void* list_remove_by_index(list_t * list, int index)
{
    listnode_t * node = list_get_node_by_index(list, index);
    list_remove_node(list, node);
}

void * list_remove_node(list_t * list, listnode_t * node)
{
    void * val = node->val;
    if(list->head == node)
        return list_remove_front(list);
    else if(list->tail == node)
        return list_remove_back(list);
    else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        list->size--;
        kfree(node);
    }
    return val;
}
void list_destroy(list_t * list)
{
    if(!list)
        return;
    listnode_t * node = list->head;
    while(node)
    {
        listnode_t * save = node;
        node = node->next;
        kfree(node);
    }
    kfree(list);
}

void listnode_destroy(listnode_t * node)
{
    if(!node)
        return;
    kfree(node);
}