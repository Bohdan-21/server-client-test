#include "list.h"



static int remove_node_recursive(node_t**, node_t*, destroy_function_t);
static void remove_all_node_recursive(node_t*, destroy_function_t);
static node_t** get_last(node_t**);



list_t* initialize_list()
{
    list_t* list = get_mem(sizeof(list_t));

    list->pointer_in_head = NULL;
    list->count = 0;

    return list;
}

void free_list(list_t* list, destroy_function_t destroy_function)
{
    remove_all_node(list, destroy_function);

    free(list);
}



void create_node(list_t* list, void* ptr)
{
    node_t* new_node;
    node_t** pointer_in_last;

    new_node = get_mem(sizeof(node_t));
    pointer_in_last = get_last(&list->pointer_in_head);

    new_node->data = ptr;
    new_node->next = NULL;

    *pointer_in_last = new_node;
    list->count++;
}

void remove_node(list_t* list, node_t* ptr, destroy_function_t destroy_function)
{
    if (remove_node_recursive(&list->pointer_in_head, ptr, destroy_function))
        list->count--;
}

void remove_all_node(list_t* list, destroy_function_t destroy_function)
{
    remove_all_node_recursive(list->pointer_in_head, destroy_function);
    
    list->count = 0;
    list->pointer_in_head = NULL;
}



static int remove_node_recursive(node_t** current, node_t* ptr, 
                                 destroy_function_t destroy_function)
{
    node_t *tmp;

    if (*current == NULL)
        return 0;
    else
    {
        if (*current == ptr)/*if address equal*/
        {
            tmp = *current;
            *current = (*current)->next;
            
            if (destroy_function)
                destroy_function(tmp->data);

            free(tmp);

            return 1;
        }
        else 
            return remove_node_recursive(&(*current)->next, ptr, destroy_function);
    }
}

static void remove_all_node_recursive(node_t* ptr, destroy_function_t destroy_function)
{
    if (ptr == NULL)
        return;

    remove_all_node_recursive(ptr->next, destroy_function);
    if (destroy_function)
        destroy_function(ptr->data);
        
    free(ptr);
}

static node_t** get_last(node_t** ptr)
{
    if (!(*ptr))
        return ptr;
    return get_last(&(*ptr)->next);
}