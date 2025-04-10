/*
 * rgb.h
 *
 *  Created on: March , 2025
 *  Author    : Ahmed Surror
 */

#ifndef HAL_RGB_LED_RGB_H_
#define HAL_RGB_LED_RGB_H_

#define RED_PORT_BASE           GPIO_PORTB_BASE
#define GREEN_PORT_BASE         GPIO_PORTB_BASE
#define BLUE_PORT_BASE          GPIO_PORTB_BASE

#define RED_PIN                 GPIO_PIN_1
#define GREEN_PIN               GPIO_PIN_2
#define BLUE_PIN                GPIO_PIN_3


void RGB_init(void);

void RGB_RedLedOn(void);
void RGB_BlueLedOn(void);
void RGB_GreenLedOn(void);

void RGB_RedLedOff(void);
void RGB_BlueLedOff(void);
void RGB_GreenLedOff(void);


#endif /* HAL_RGB_LED_RGB_H_ */
