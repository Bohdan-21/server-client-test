#ifndef DIALOG_H
#define DIALOG_H

#include "list.h"
#include "dialog_state.h"
#include "base.h"

typedef struct 
{
    int dialog_id;/*key*/
    const char* msg;
} dialog_t;



dialog_t* create_dialog_t(int dialog_id, const char* msg);

void free_dialog_data(void*);




#endif