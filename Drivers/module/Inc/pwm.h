/*
 * pwm.h
 *
 *  Created on: Jan 27, 2019
 *      Author: jakub
 */

#ifndef MODULE_INC_PWM_H_
#define MODULE_INC_PWM_H_

#include "tim.h"
#include "global.h"
#include "peripherals.h"


typedef struct {
	uint8_t TIM2CH1:1;
	uint8_t TIM2CH2:1;
	uint8_t TIM2CH3:1;
	uint8_t TIM2CH4:1;
	uint8_t TIM3CH1:1;
	uint8_t TIM3CH2:1;
	uint8_t TIM3CH3:1;
	uint8_t TIM3CH4:1;

}Flags_PWM;


void PWM_start(uint8_t pwm_channel);
void PWM_stop(uint8_t pwm_channel);
void PWM_togle(uint8_t pwm_channel);
void PWM_duty_change(uint8_t pwm_channel, uint16_t new_duty);
uint8_t PWM_status(uint8_t pwm_channel);
uint16_t PWM_duty_read(uint8_t pwm_channel);


#endif /* MODULE_INC_PWM_H_ */
