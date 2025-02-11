#include "list.h"

void initialize();

void print();

void remove_node_from_list();

void add();


int main()
{
    print();

    initialize();

    print();

    remove_node_from_list();

    print();

    add();

    print();


    return 0;
}

void initialize()
{
    create_node(10);
    create_node(20);
    create_node(30);
    create_node(40);
    create_node(50);
}

void print()
{
    struct node* tmp;

    printf("\n\n");

    for (reset_current();(tmp = get_current()); move_next())
    {
        printf("%d\n", (*tmp).sd);
    }
    printf("\n\n");
}

void remove_node_from_list()
{
    remove_node(30);
    remove_node(10);
    remove_node(50);
}

void add()
{
    create_node(5);
    create_node(-20);
    create_node(-100);
}
