#ifndef RETURN_ENUM_H_
#define RETURN_ENUM_H_

typedef enum
{
    /* Success & Warnings*/
    KERNEL_OK = 0,
    KERNEL_WARNING = 1,

    /* Task & Scheduling Errors*/
    KERNEL_ERROR_INIT = 2,
    KERNEL_ERROR_STACK_OVERFLOW = 3,
    KERNEL_REACHED_MAX_TASK = 4,

    /* Parameter & Memory Errors*/
    KERNEL_ERROR_INVALID_PARAM = 5,
    KERNEL_ERROR_NULL_PTR = 6,
    KERNEL_ERROR_NO_MEMORY = 7,

     /* Context & State Errors */
    ERR_NOT_IN_ISR = 13,   /* Execution not allowed from ISR context */
    ERR_IN_ISR = 14,       /* Execution only allowed from ISR context */

} System_Status_t;


#endif