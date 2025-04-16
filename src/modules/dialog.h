#ifndef DIALOG_H
#define DIALOG_H

#include "list.h"
#include "base.h"

typedef struct 
{
    int dialog_id;/*key*/
    const char* msg;
} dialog_t;



dialog_t* create_dialog(int dialog_id, char* msg);

void free_dialog(void*);




#endif