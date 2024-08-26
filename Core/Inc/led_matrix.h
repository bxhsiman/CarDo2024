#ifndef __led_matirx_H
#define	__led_matirx_H
#include "main.h"

void led_matrix_init( void ) ;
void show_led_matrix( void ) ;
void move_led_matrix( void ) ;

#define LED_SHIFT_CLK_Pin GPIO_PIN_0
#define LED_SHIFT_CLK_GPIO_Port GPIOC
#define LED_LATCH_CLK_Pin GPIO_PIN_1
#define LED_LATCH_CLK_GPIO_Port GPIOC
#define LED_SHIFT_DATA_Pin GPIO_PIN_2
#define LED_SHIFT_DATA_GPIO_Port GPIOC

#define SENSOR_PWR_ON_Pin GPIO_PIN_13
#define SENSOR_PWR_ON_GPIO_Port GPIOC

#endif /* __I2C_MMA_H */


