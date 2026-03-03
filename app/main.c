#include <stdint.h>
#include "scheduler.h"
#include "bsp_stm32f103c8t6.h"

// #include "scheduler_priv.h" // debug 

void tsk_dht11_read(void);
void tsk_i2c_lcd_write(void);

static uint32_t stack_array_tsk_dht11_read[128];
static uint32_t stack_array_tsk_i2c_lcd_write[256];

int main(void)
{
    if(sched_init(HSI_CLOCK) != OK)
    {
        // ERROR LOG
    }

    if(sched_add_task(&tsk_dht11_read, &stack_array_tsk_dht11_read[0], 128) != OK)
    {
        // ERROR LOG
    }

    if(sched_add_task(&tsk_i2c_lcd_write, &stack_array_tsk_i2c_lcd_write[0], 256) != OK)
    {
        // ERROR LOG
    }


    sched_start();
}

void tsk_dht11_read(void)
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

        //     tsk_dht11_read();
        // }
        // else
        // {
        //     while(1);
        // }

        task_delay_tick(1000);
    }
}

void tsk_i2c_lcd_write(void)
{
    while(1)
    {
        task_delay_tick(1000);
    }
}