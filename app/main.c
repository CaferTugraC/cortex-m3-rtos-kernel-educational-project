#include <stdint.h>
#include "scheduler.h"
#include "bsp_stm32f103c8t6.h"
#include "led.h"

// #include "scheduler_priv.h" // debug 

// task handlers prototype
void task_led1(void);
void task_led2(void);
void task_led3(void);

// task stack arrays
static uint32_t stack_array_task_led1[128];
static uint32_t stack_array_task_led2[128];
static uint32_t stack_array_task_led3[128];

int main(void)
{
    if(sched_init(HSI_CLOCK) != OK)
    {
        // ERROR LOG
    }

    init_all_leds();

    if(sched_add_task(&task_led1, &stack_array_task_led1[0], 128) != OK)
    {
        // ERROR LOG
    }

    if(sched_add_task(&task_led2, &stack_array_task_led2[0], 128) != OK)
    {
        // ERROR LOG
    }

    if(sched_add_task(&task_led3, &stack_array_task_led3[0], 128) != OK)
    {
        // ERROR LOG
    }

    sched_start();

    /* Should never reach here */
    while(1);
    return 0;
}

// task handlers
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

        led_on(LED_BLUE);
        task_delay_tick(500);
        led_off(LED_BLUE);
        task_delay_tick(500);
    }
}

void task_led2(void)
{
    while(1)
    {
        led_on(LED_WHITE);
        task_delay_tick(250);
        led_off(LED_WHITE);
        task_delay_tick(250);
    }
}

void task_led3(void)
{
    while(1)
    {
        led_on(LED_RED);
        task_delay_tick(1000);
        led_off(LED_RED);
        task_delay_tick(1000);
    }
}