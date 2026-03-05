#ifndef LED_H_
#define LED_H_

#include <stdint.h>

#define LED_BLUE 0
#define LED_WHITE 1
#define LED_RED 2
#define LED_GREEN 3

// Register Adresleri (F103 için)
#define RCC_BASE      0x40021000
#define GPIOA_BASE    0x40010800

#define RCC_APB2ENR   (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOA_CRL     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define DELAY_COUNT_1MS 		 1250U
#define DELAY_COUNT_1S  		(1000U * DELAY_COUNT_1MS)
#define DELAY_COUNT_500MS  		(500U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_250MS 		(250U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_125MS 		(125U  * DELAY_COUNT_1MS)

void delay(uint32_t count);
void init_all_leds(void);
void led_on(uint32_t led_num);
void led_off(uint32_t led_num);
#endif /* LED_H_ */
