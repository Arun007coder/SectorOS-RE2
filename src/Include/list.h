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

#ifndef __LIST_H__
#define __LIST_H__

#include "system.h"

typedef struct listnode
{
    void *val;
    struct listnode *next;
    struct listnode *prev;
} listnode_t;

typedef struct list
{
    listnode_t *head;
    listnode_t *tail;
    uint32_t size;
} list_t;

#define foreach(t, list) for(listnode_t * t = list->head; t != NULL; t = t->next)

list_t *list_create();
uint32_t list_size(list_t *list);

listnode_t* list_insert_front(list_t *list, void *val);
void list_insert_back(list_t *list, void *val);

void * list_remove_node(list_t * list, listnode_t * node);
void * list_remove_front(list_t * list);
void * list_remove_back(list_t * list);

void list_push(list_t * list, void * val);
listnode_t * list_pop(list_t * list);

void list_enqueue(list_t * list, void * val);
listnode_t * list_dequeue(list_t * list);

void * list_peek_front(list_t * list);
void * list_peek_back(list_t * list);

void list_destroy(list_t * list);
void listnode_destroy(listnode_t * node);

int list_contain(list_t * list, void * val);

listnode_t * list_get_node_by_index(list_t * list, int index);
void * list_remove_by_index(list_t * list, int index);


#endif