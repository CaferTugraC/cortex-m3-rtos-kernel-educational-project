 #ifndef GPIO_H_
 #define GPIO_H_

#define <stdint.h>

typedef enum{
    GPIO_HIGH
    GPIO_LOW
} GPIO_State_t;

typedef enum{
    GPIO_MODE_INPUT_AI
    GPIO_MODE_INPUT_IF
    GPIO_MODE_INPUT_PD
    GPIO_MODE_INPUT_PU

    GPIO_MODE_OUTPUT_PP
    GPIO_MODE_OUTPUT_OD

    GPIO_MODE_AF_PP
    GPIO_MODE_AF_OD
} GPIO_Mode_t;

typedef struct{
    uint32_t pin_number;
    GPIO_Mode_t Mode;
    GPIO_State_t State;
} GPIO_Configuration_t;

void gpio_init(GPIO_Port_t port, GPIO_Pin_Number_t pin, GPIO_Mode_t mode);
void gpio_set_mode(GPIO_Port_t port, GPIO_Pin_Number_t pin, GPIO_Mode_t mode);
void gpio_write(GPIO_Port_t port, GPIO_Pin_Number_t pin, GPIO_State_t state);
void gpio_toggle(GPIO_Port_t port, GPIO_Pin_Number_t pin);

 #endif