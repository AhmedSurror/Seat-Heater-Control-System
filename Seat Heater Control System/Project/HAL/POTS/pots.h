/*
 * pot.h
 *
 *  Created on: March , 2025
 *  Author    : Ahmed Surror
 */

#ifndef HAL_POTS_POTS_H_
#define HAL_POTS_POTS_H_

#include <stdint.h>

#define POT1_MAX_VALUE   4096
#define POT2_MAX_VALUE   4096

void POT1_init(void);
uint32_t POT1_getValue();

void POT2_init(void);
uint32_t POT2_getValue();


#endif /* HAL_POTS_POTS_H_ */
