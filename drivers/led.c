#include "led.h"
#include "stdint.h"

void delay(uint32_t count)
{
  for(uint32_t i = 0 ; i < count ; i++);
}

void init_all_leds(void)
{

	// 1. Port A saatini aktif et
	RCC_APB2ENR |= (1 << 2);

	// 2. PA0, PA1, PA2, PA3 pinlerini Output (Max 10MHz) olarak ayarla
	// Her pin için 4 bit: CNF(2 bit) + MODE(2 bit).
    // 0011 (0x3) -> Push-Pull Output, 10MHz
    GPIOA_CRL &= ~(0xFFFF); // İlk 4 pini temizle
    GPIOA_CRL |=  (0x3333);  // 0x3333 -> PA0-PA3 = 10MHz Output

    led_off(LED_GREEN);
    led_off(LED_WHITE);
    led_off(LED_RED);
    led_off(LED_BLUE);
}

void led_on(uint32_t led_num)
{
	GPIOA_ODR |= (1 << led_num);
}

void led_off(uint32_t led_num)
{
	GPIOA_ODR &= ~(1 << led_num);
}
