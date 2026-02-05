#include <stdint.h>
#include 'scheduler.h'

uint64_t tsk_dht11_read(void);
void tsk_i2c_lcd_write(uint32_t data);

uint32_t stack_array_tsk_dth11_read[128];
uint32_t stack_array_tsk_i2c_lcd_write[256];

int main(void)
{
    init_fault();
    sched_init();

    sched_add_task(&tsk_dth11_read, &stack_array_tsk_dth11_read, 128);

    sched_start(&tsk_dth11_read);
}

uint64_t tsk_dht11_read(void)
{

}

void tsk_i2c_lcd_write(uint32_t data)
{

}