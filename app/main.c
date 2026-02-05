#include <stdint.h>
#include "scheduler.h"

void tsk_dht11_read(void);
void tsk_i2c_lcd_write(void);

static uint32_t stack_array_tsk_dht11_read[128];
static uint32_t stack_array_tsk_i2c_lcd_write[256];

int main(void)
{
    sched_init();

    sched_add_task(&tsk_dht11_read, &stack_array_tsk_dht11_read[0], 128);
    sched_add_task(&tsk_i2c_lcd_write, &stack_array_tsk_i2c_lcd_write[0], 256);


    sched_start(&tsk_dht11_read);
}

void tsk_dht11_read(void)
{
    task_delay(1000);
    task_delay(1000);
}

void tsk_i2c_lcd_write(void)
{
    task_delay(1000);
    task_delay(1000);
}