#ifndef RETURN_ENUM_H
#define RETURN_ENUM_H

typedef enum
{
    OK = 0,
    ERROR_INIT = 1,
    WARNING = 2,
    ERROR_STACK_OVERFLOW = 3,
    INVALID_PARAM = 4,
    REACHED_MAX_TASK = 5,

} System_Status_t;


#endif