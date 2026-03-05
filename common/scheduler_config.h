#ifndef SCHEDULER_CONFIG_H_
#define SCHEDULER_CONFIG_H_

#define MAX_TASKS 4U // your task count + 1 (idle task), if you are 3 tasks then you write 4
#define TICK_HZ 1000U
#define CONFIG_MAX_TICK_HZ 5000U

#define MIN_STACK_SIZE 64U

#endif