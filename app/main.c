#include <stdint.h>
#include "scheduler.h"
#include "bsp_stm32f103c8t6.h"

// #include "scheduler_priv.h" // debug 

void task_led1(void);
void task_led2(void);

static uint32_t stack_array_task_led1[128];
static uint32_t stack_array_task_led2[256];

int main(void)
{
    if(sched_init(HSI_CLOCK) != OK)
    {
        // ERROR LOG
    }

    if(sched_add_task(&task_led1, &stack_array_task_led1[0], 128) != OK)
    {
        // ERROR LOG
    }

    if(sched_add_task(&task_led2, &stack_array_task_led2[0], 256) != OK)
    {
        // ERROR LOG
    }


    sched_start();

    /* Should never reach here */
    while(1);
    return 0;
}

void task_led1(void)
{
    while(1)
    {
        // DEBUG - TEST CHECK OVERFLOW FUNCTİON
        // if(check_task_stack_overflow() == 0U)
        // {
        //     uint32_t test_stack_overflow_array[10];
        //     for(uint32_t i = 0U; i < 10U; i++)
        //     {
        //         test_stack_overflow_array[i] = 0xCACACACAUL;
        //     }

        //     task_led1();
        // }
        // else
        // {
        //     while(1);
        // }

        task_delay_tick(1000);
    }
}

void task_led2(void)
{
    while(1)
    {
        task_delay_tick(1000);
    }
}