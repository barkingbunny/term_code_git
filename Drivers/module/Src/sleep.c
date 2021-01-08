/*
 * sleep.c
 *
 *  Created on: Jul 29, 2018
 *      Author: jakub
 */
/*
 * this file is written to keep all procedures in during going to sleep on the one place
 */
#include "sleep.h"
#include "peripherals.h"
#include "global.h"


void Go_To_Sleep(RTC_HandleTypeDef* RtcHandle)
{

	//* Disable all used wakeup sources*/
	HAL_RTCEx_DeactivateWakeUpTimer(RtcHandle);
	//* Re-enable all used wakeup sources*/
	//* ## Setting the Wake up time ############################################*/
	/*  RTC Wakeup Interrupt Generation:
					  				    Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
					  				    Wakeup Time = Wakeup Time Base * WakeUpCounter
					  				      = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
					  				      ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

					  				    To configure the wake up timer to 4s the WakeUpCounter is set to 0x1FFF:
					  				    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
					  				    Wakeup Time Base = 16 /(~39.000KHz) = ~0,410 ms
					  				    Wakeup Time = ~4s = 0,410ms  * WakeUpCounter
					  				      ==> WakeUpCounter = ~4s/0,410ms = 9750 = 0x2616 */

	if ( HAL_RTCEx_SetWakeUpTimer_IT(RtcHandle, 9250, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
		lcd_setCharPos(2,1);
		lcd_printString(" ERR - no wake set" );
		HAL_Delay(2500);
	}
	// Clear all related wakeup flags
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	//*Suspend Tick increment to prevent wakeup by Systick interrupt.Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)
	HAL_SuspendTick();

	// Enable Power Control clock
	__HAL_RCC_PWR_CLK_ENABLE();

	// Enter Sleep Mode , wake up is done once User push-button is pressed
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	// Resume Tick interrupt if disabled prior to sleep mode entry
	HAL_ResumeTick();
}
