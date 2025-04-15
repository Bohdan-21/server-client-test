#ifndef LIST_H
#define LIST_H


#include <stdlib.h>
#include <stdio.h>
#include "base.h"



typedef struct node node_t;
typedef struct list list_t;
typedef void (*destroy_function_t) (void*);

struct list
{
    node_t* pointer_in_head;

    size_t count;
};


struct node
{
    void* data;
    node_t* next;
};



list_t* create_list();

void free_list(list_t*, destroy_function_t);


void create_node(list_t*, void*);

void remove_node(list_t*, node_t*, destroy_function_t);

void remove_all_node(list_t*, destroy_function_t);

#endif