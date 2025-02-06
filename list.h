/*
Note: The list provides functionality to remove specific sessions 
but does not support removing individual dialog states. This is intentional, 
as sessions can be created and removed dynamically during the program's 
lifetime, while dialog states persist.
*/

#ifndef LIST_H
#define LIST_H


#include <stdlib.h>
#include <stdio.h>
#include "session.h"


typedef struct node node_t;
typedef struct list list_t;


extern list_t* list_for_session;
extern list_t* list_for_dialog;




typedef enum
{
    session_type,
    dialog_type
} list_type_t;


struct list
{
    node_t* pointer_in_head;
    node_t* pointer_in_current;

    size_t count;
};


struct node
{
    void* pointer_on_data;
    node_t* next;
};





void initialize_lists();

void free_lists();


void create_node(list_type_t, void*);

void remove_session(int);

void remove_all(list_type_t);



void reset_current(list_type_t);

void move_next(list_type_t);

void* get_current(list_type_t);

#endif