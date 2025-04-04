#include "list.h"



static int remove_node_recursive(node_t**, node_t*);
static void remove_all_node_recursive(node_t* ptr);



list_t* initialize_list()
{
    list_t* list = get_mem(sizeof(list_t));

    list->pointer_in_head = NULL;
    list->count = 0;

    return list;
}

void free_list(list_t* list)
{
    remove_all_node(list);

    free(list);
}



void create_node(list_t* list, void* ptr)
{
    node_t* new_node = get_mem(sizeof(node_t));

    new_node->data = ptr;
    new_node->next = list->pointer_in_head;

    list->pointer_in_head = new_node;
    list->count++;
}

void remove_node(list_t* list, node_t* ptr)
{
    if (remove_node_recursive(&list->pointer_in_head, ptr))
        list->count--;
}

void remove_all_node(list_t* list)
{
    remove_all_node_recursive(list->pointer_in_head);
    
    list->count = 0;
    list->pointer_in_head = NULL;
}



static int remove_node_recursive(node_t** current, node_t* ptr)
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
            
            free(tmp->data);
            free(tmp);

            return 1;
        }
        else 
            return remove_node_recursive(&(*current)->next, ptr);
    }
}

static void remove_all_node_recursive(node_t* ptr)
{
    if (ptr == NULL)
        return;

    remove_all_node_recursive(ptr->next);

    free(ptr->data);
    free(ptr);
}