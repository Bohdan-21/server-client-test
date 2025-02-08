#include "dialog.h"

void init_default_dialog_list()
{
    dialog_t* welcome; /*start*/
    dialog_t* question1;
    dialog_t* question2;
    dialog_t* best_wishes; /*exit*/

    welcome = create_dialog_t(welcome_state, "Hello dear client!\n");
    question1 = create_dialog_t(food_state, "What is your favourite food?\n");
    question2 = create_dialog_t(town_state, "Where are you from?\n");
    best_wishes = create_dialog_t(exit_state, "Bye dear client!\n");


    create_node(dialog_type, welcome);
    create_node(dialog_type, question1);
    create_node(dialog_type, question2);
    create_node(dialog_type, best_wishes);
}

dialog_t* create_dialog_t(dialog_state_t state, const char* msg)
{
    dialog_t* new_dialog = malloc(sizeof(dialog_t));

    if (new_dialog != NULL)
    {
        (*new_dialog).state = state;
        (*new_dialog).write_msg = msg;
    }

    return new_dialog;
}

void free_dialog_data(dialog_t* dialog)
{
    free((void*)dialog->write_msg);
}