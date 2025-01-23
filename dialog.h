#ifndef DIALOG_H
#define DIALOG_H

#include "list.h"
#include "dialog_state.h"

typedef struct 
{
    dialog_state_t state;/*key*/
    const char* write_msg;
} dialog_t;


void init_default_dialog_list();




#endif