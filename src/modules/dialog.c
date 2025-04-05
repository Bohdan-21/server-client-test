#include "dialog.h"



dialog_t* create_dialog_t(int dialog_id, const char* str)
{
    dialog_t* new_dialog = get_mem(sizeof(dialog_t));

    new_dialog->dialog_id = dialog_id;
    new_dialog->msg = make_copy_string(str);

    return new_dialog;
}

void free_dialog_data(void* data)
{
    dialog_t* dialog = (dialog_t*)data;

    free((void*)dialog->msg);
    free(dialog);
}