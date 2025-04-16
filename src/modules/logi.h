#ifndef LOGI_H
#define LOGI_H

#include "session.h"

int is_logging();

void push_data(const session_t* session);

#endif