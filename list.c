#include "list.h"

list_t* list_for_session = NULL;
list_t* list_for_dialog = NULL; 


static node_t** get_last(node_t**);
static void remove_session_node(int, node_t**);
static void recursive_remove(node_t**);
static void clear_list(list_t*);

void initialize_lists()
{
    list_for_session = malloc(sizeof(list_t));
    list_for_dialog = malloc(sizeof(list_t));

    if (list_for_session)
        clear_list(list_for_session);
    if (list_for_dialog)
        clear_list(list_for_dialog);
}



void create_node(list_type_t lt, void* pointer)
{
    node_t* new_node = malloc(sizeof(node_t));

    if (new_node == NULL)
    {
        perror("create_node");
        return;
    }

    (*new_node).next = NULL;
    (*new_node).pointer_on_data = pointer;

    if (lt == session_type)
    {
        (*get_last(&(*list_for_session).pointer_in_head)) = new_node;
        (*list_for_session).count++;
    }
    else
    {
        (*get_last(&(*list_for_dialog).pointer_in_head)) = new_node;
        (*list_for_dialog).count++;
    }
}

void remove_session(int sd)
{
    remove_session_node(sd, &(*list_for_session).pointer_in_head);
}

static void remove_session_node(int sd, node_t** pointer)
{
    session_t* session;
    node_t* rm_node;

    if ((*pointer) != NULL)
    {
        session = (session_t*)(**pointer).pointer_on_data;
        
        if ((*session).sd == sd)
        {
            rm_node = *pointer;
            *pointer = (**pointer).next;

            (*list_for_session).count--;

            free(session);
            free(rm_node);

            return;
        }
        else
            remove_session_node(sd, &(**pointer).next);
    }
}

void remove_all(list_type_t lt)
{
    if (lt == session_type)
    {
        if (list_for_session)
        {
            recursive_remove(&(*list_for_session).pointer_in_head);
            clear_list(list_for_session);
        }
    }
    else
    {
        if (list_for_dialog)
        {
            recursive_remove(&(*list_for_dialog).pointer_in_head);
            clear_list(list_for_dialog);
        }
    }
}

static void recursive_remove(node_t** pointer)
{
    if (*pointer != NULL)
        recursive_remove(&(**pointer).next);

    free((**pointer).pointer_on_data);
    free(*pointer);
}

static void clear_list(list_t* pointer)
{
    (*pointer).pointer_in_head = NULL;
    (*pointer).pointer_in_current = NULL;
    (*pointer).count = 0;
}



void reset_current(list_type_t lt)
{
    if (lt == session_type)
        (*list_for_session).pointer_in_current = (*list_for_session).pointer_in_head;
    else
        (*list_for_dialog).pointer_in_current = (*list_for_dialog).pointer_in_head;
}

void move_next(list_type_t lt)
{
    if (lt == session_type)
    {
        if ((*list_for_session).pointer_in_current != NULL)
        {
            (*list_for_session).pointer_in_current = 
            (*(*list_for_session).pointer_in_current).next;
        }
    }
    else
    {
        if ((*list_for_dialog).pointer_in_current != NULL)
        {
            (*list_for_dialog).pointer_in_current = 
            (*(*list_for_dialog).pointer_in_current).next;
        }
    }
}

void* get_current(list_type_t lt)
{
    if (lt == session_type)
    {
        if ((*list_for_session).pointer_in_current == NULL)
            return NULL;
        return (*(*list_for_session).pointer_in_current).pointer_on_data;
    }
    else
    {
        if ((*list_for_dialog).pointer_in_current == NULL)
            return NULL;
        return (*(*list_for_dialog).pointer_in_current).pointer_on_data;
    }
}



static node_t** get_last(node_t** pointer)
{
    if ((*pointer) != NULL)
        return get_last(&(**pointer).next);    
    
    return pointer;   
}