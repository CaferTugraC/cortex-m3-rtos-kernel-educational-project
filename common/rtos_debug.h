#ifndef RTOS_DEBUG_H_
#define RTOS_DEBUG_H_

#include "scheduler_config.h"
#include <stdint.h>

#if (USE_RTOS_ASSERT == 1)

    /* Extern definition for disabling interrupts. 
        This function should be provided by the port layer. */
    extern void interrupt_disable(void);

    /*
     * RTOS_ASSERT macro:
     * if the expression is false (0), interrupt are disabled,
     * and the system enters an infinite loop, halting execuiton.
     * The file name and line number are recorded in volatile variables
     * so they can be easely inspected with a debugger.
     */
    #define RTOS_ASSERT(expr)                                   \
        do {                                                    \
            if ((expr) == 0) {                                  \
                interrupt_disable();                            \
                volatile const char * assert_file = __FILE__;   \
                volatile int assert_line = __LINE__;            \
                (void)assert_file; /* Prevent unused warn */    \
                (void)assert_line;                              \
                while (1) {                                     \
                    __asm volatile("nop");                      \
                }                                               \
            }                                                   \
        } while(0)

#else

    /* If asserts are disabled, the macro does nothing */
    #define RTOS_ASSERT(expr) ((void)0)

#endif /* USE_RTOS_ASSERT */


#endif /* RTOS_DEBUG_H_ */