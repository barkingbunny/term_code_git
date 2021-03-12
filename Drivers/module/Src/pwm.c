/*
 * pwm.c
 *
 *  Created on: Jan 27, 2019
 *      Author: jakub
 */
/**
 * tento kod pisu pro moznost obslhovat PWM LED pomoci par prikazu
 */
#include "pwm.h"


/**
 * start the PWM channel with coding:
 *  first number - timer; second number is channel (21 - tim2, channel 1)
 *  // vent -board
 *
 */
static Flags_PWM running;

void PWM_start(uint8_t pwm_channel)
{
	if (FALSE == PWM_status(pwm_channel)) // jestlize kanal jiz bezi, tak se nic nevykonna
	{
		switch (pwm_channel)
		{

#ifdef TIMER2_PWM
		case (21):
		{
			/*##-3- Start PWM signals generation #######################################*/
			/* Start channel 1 */
			if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
			{
				/* PWM Generation Error */
				Error_Handler();
			}
			running.TIM2CH1 = TRUE;
			break;
		}
		case (22):
		{
			if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
				Error_Handler();
			running.TIM2CH2 = TRUE;
			break;
		}
		case (23):
		{
			if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK)
				Error_Handler();
			running.TIM2CH3 = TRUE;
			break;
		}
		case (24):
		{
			if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4) != HAL_OK)
				Error_Handler();
			running.TIM2CH4 = TRUE;
			break;
		}
#endif

#ifdef TIMER3_PWM
		case (31):
		{
			if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK)
				Error_Handler();
			running.TIM3CH1 = TRUE;
			break;
		}
		case (32):
		{
			if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2) != HAL_OK)
				Error_Handler();
			running.TIM3CH2 = TRUE;
			break;
		}
		case (33):
		{
			if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3) != HAL_OK)
				Error_Handler();
			running.TIM3CH3 = TRUE;
			break;
		}
		case (34):
		{
			if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4) != HAL_OK)
				Error_Handler();
			running.TIM3CH4 = TRUE;
			break;
		}
#endif
		}
	}
}

void PWM_stop(uint8_t pwm_channel){

	if (PWM_status(pwm_channel))
	{
		switch (pwm_channel)
		{
#ifdef TIMER2_PWM
		case (21):
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			running.TIM2CH1 = FALSE;
			break;
		}
		case (22):
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
			running.TIM2CH2 = FALSE;
			break;
		}
		case (23):
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
			running.TIM2CH3 = FALSE;
			break;
		}
		case (24):
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
			running.TIM2CH4 = FALSE;
			break;
		}
#endif
#ifdef TIMER3_PWM
		case (31):
		{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			running.TIM3CH1 = FALSE;
			break;
		}
		case (32):
		{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
			running.TIM3CH2 = FALSE;
			break;
		}
		case (33):
		{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			running.TIM3CH3 = FALSE;
			break;
		}
		case (34):
		{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
			running.TIM3CH4 = FALSE;
			break;
		}
#endif

		} // SWITCH end
	}
}


void PWM_togle(uint8_t pwm_channel){
	uint8_t running_bool = FALSE;
	switch(pwm_channel){
#ifdef TIMER2_PWM
	case (21): {
		running_bool = running.TIM2CH1;
		break; }
	case (22): {
		running_bool = running.TIM2CH2;
		break; }
	case (23): {
		running_bool = running.TIM2CH3;
		break;}
	case (24): {
		running_bool = running.TIM2CH4;
		break;}
#endif
#ifdef TIMER3_PWM
	case (31):{
		running_bool = running.TIM3CH1;
		break; }
	case (32):{
		running_bool = running.TIM3CH2;
		break; }
	case (33):{
		running_bool = running.TIM3CH3;
		break; }
	case (34):{
		running_bool = running.TIM3CH4;
		break; }
#endif

	}

	if(TRUE==running_bool){
		PWM_stop(pwm_channel);
	}
	else
		PWM_start(pwm_channel);

}



void PWM_duty_change(uint8_t pwm_channel, uint16_t new_duty){
		
	PWM_stop(pwm_channel);

	switch(pwm_channel){
#ifdef TIMER2_PWM
	case (21): {
		htim2.Instance->CCR1 = new_duty; 
		break; }
	case (22): {
		htim2.Instance->CCR2 = new_duty; 
		break; }
	case (23): {
		htim2.Instance->CCR3 = new_duty; 
		break; }
	case (24): {
		htim2.Instance->CCR4 = new_duty; 
		break;}
#endif
#ifdef TIMER3_PWM
	case (31):{
		htim3.Instance->CCR1 = new_duty; 
		break; }
	case (32):	{
		htim3.Instance->CCR2 = new_duty; 
		break; 	}
	case (33):	{
		htim3.Instance->CCR3 = new_duty;
		break;	}
	case (34):	{
		htim3.Instance->CCR4 = new_duty;
		break; 	}
#endif
	}
	PWM_start(pwm_channel);

}
/**
 * vrati zpet stav PWM kanalu (Zda bezi ci ne)
 * 
 * return: FALSE - kanal neberi
 * 			TRUE PWM kanal je zapnut
 */

uint8_t PWM_status(uint8_t pwm_channel){
		
uint8_t running_bool = FALSE;
	switch(pwm_channel){
#ifdef TIMER2_PWM
	case (21): {
		running_bool = running.TIM2CH1;
		break; }
	case (22): {
		running_bool = running.TIM2CH2;
		break; }
	case (23): {
		running_bool = running.TIM2CH3;
		break;}
	case (24): {
		running_bool = running.TIM2CH4;
		break;}
#endif
#ifdef TIMER3_PWM
	case (31):{
		running_bool = running.TIM3CH1;
		break; }
	case (32):{
		running_bool = running.TIM3CH2;
		break; }
	case (33):{
		running_bool = running.TIM3CH3;
		break; }
	case (34):{
		running_bool = running.TIM3CH4;
		break; }
#endif

	}
	return running_bool;

}
/**
 * @note Funkce zjisti jakou hodnotu PWM ma dany kanal a vrati jej
 * 
*/

uint16_t PWM_duty_read(uint8_t pwm_channel){
	uint16_t pwm_duty = 0;	

	switch(pwm_channel){
#ifdef TIMER2_PWM
	case (21): {
		pwm_duty = htim2.Instance->CCR1;
		break; }
	case (22): {
		pwm_duty = htim2.Instance->CCR2; 
		break; }
	case (23): {
		pwm_duty = htim2.Instance->CCR3; 
		break; }
	case (24): {
		pwm_duty = htim2.Instance->CCR4; 
		break;}
#endif
#ifdef TIMER3_PWM
	case (31):{
		pwm_duty = htim3.Instance->CCR1;
		break; }
	case (32):	{
		pwm_duty = htim3.Instance->CCR2;
		break; 	}
	case (33):	{
		pwm_duty = htim3.Instance->CCR3;
		break;	}
	case (34):	{
		pwm_duty = htim3.Instance->CCR4;
		break; 	}
#endif
	}
	
return pwm_duty;

}