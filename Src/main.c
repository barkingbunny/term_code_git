/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/**popis
 * 28.9.2020 - testovani Logovani
 *
 * Ukoly:
 * pridej MENU, s nastavenim casu a teplot. funkcni
 * nastveni - 115200-8-N-1, /dev/ttyACM0 // pouzivam "cutecom"
 */
#include "global.h"
#include "pinmap.h"
#include "peripherals.h"
#include "BME280.h"
#include "lcd_12864.h"
#include "ds18b20.h"
#include <stdio.h>
#include "usbd_cdc_if.h"
#include "rtc_api.h"
#include "Time.h"
#include "menu.h"
#include "log.h"
#include "sleep.h"
#include "waiter.h"
#include "heater.h"
#include "usb_api.h"
#include "pwm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static States_loop current_state;
static Screen show;
Heating_mode heat_mode;
Mode_auto_s temp_auto; // struktura pro automaticky rezim
Flags_main flags;
uint32_t citac = 0;
int8_t en_count = 0;
int32_t temperature = -20000;
int32_t humid = -5;
uint32_t presure = 0;

// Temp_seting and driving
int32_t temperature_set;
int32_t temperature_manual = 2000;
;
// END Temp_seting and driving

Buttons pushed_button; //cleared each main cycle

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	char buffer_s[32];
	//char buffer_usb [32];

	uint16_t T_led = 20;
	actual_HALtick.tick = 0, past_HALtick.tick = 0;
	uint32_t InputVoltage = 0;
	uint8_t en_count_last = 0;
	int16_t count1;
	uint16_t sirka = 0;
	char aShowTime[50] = {0};
	uint8_t heat_instant = 0;

	//debug
	Bool show_time = TRUE;
	//debug

	//timeouts
	Compare_t backlite_compare, measure_compare, led_compare, time_compare, button_compare, heating_compare, logging_compare, show_timeout, heating_instant_timeout;
	backlite_compare.overflow = FALSE, measure_compare.overflow = FALSE, led_compare.overflow = FALSE, time_compare.overflow = FALSE, button_compare.overflow = FALSE, heating_compare.overflow = FALSE, logging_compare.overflow = FALSE, show_timeout.overflow = FALSE, heating_instant_timeout.overflow = FALSE;
	actual_HALtick.overflow = FALSE;
	past_HALtick.overflow = FALSE;

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC_Init();
	MX_I2C1_Init();
	MX_RTC_Init();
	MX_SPI1_Init();
	MX_TIM21_Init();
	MX_TIM22_Init();
	MX_USB_DEVICE_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	backlite_init();
	fill_comparer(BACKLITE_TIMEOUT, &backlite_compare);

	lcd12864_init(&hspi1);
	line(0, 60, 110, 60, 1);
	lcd_setCharPos(0, 0);
	lcd_printString("Initialization unit\r");
	lcd_printString("termostat_git\r");
	snprintf(buffer_s, 11, "SW v 0.%d", SW_VERSION);
	lcd_printString(buffer_s);

	HAL_TIM_Encoder_Start(&htim22, TIM_CHANNEL_1);

	htim22.Instance->EGR = 1; // Generate an update event
	htim22.Instance->CR1 = 1; // Enable the counter

	BME280_init(&hi2c1, DEFAULT_SLAVE_ADDRESS); // initialization of temp/humid sensor BOSH

#ifdef DEBUG_TERMOSTAT //debug

#endif

	HAL_Delay(1500);
	lcd_clear();

	//settings variables to default values
	flags.heating_up = FALSE;

	current_state = MEASURING;
	show = desktop;
	flags.log_enable = TRUE; // logging of the actions

	hrej();
	//Init of timers
	fill_comparer(LED_PERIODE, &led_compare);
	fill_comparer(MEASURE_PERIODE, &measure_compare);
	fill_comparer(TIME_PERIODE, &time_compare);
	fill_comparer(BUT_DELAY, &button_compare);
	fill_comparer(10, &heating_compare);		 // check it immediately
	fill_comparer_seconds(30, &logging_compare); // chci zapisovat hned po zmereni hodnot / ZAPNUTI

	mode_auto_init(&temp_auto); // inicializace automatickeho temperovani - nulovani

	show_timeout.tick = 0xfffffffe;
	heating_instant_timeout.tick = 0;
	// ADC - calibration
	if (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	for (;;)
	{

		switch (current_state)
		{
		case SLEEP:
		{ //			neni potreba spanku
			Go_To_Sleep(&hrtc);
			current_state = MEASURING;
			show_time = TRUE;
			break;
		}
		case IDLE:
		{
			break;
		}
		case MEASURING:
		{

			temperature = BME280_getTemperature();
			humid = BME280_getHumidity();
			//	presure=BME280_getPressure();
			current_state = IDLE;
#ifdef DEBUG_TERMOSTAT //debug
			current_state = VOLTAGE;
#endif
			flags.new_data_to_show = TRUE;

			fill_comparer(MEASURE_PERIODE, &measure_compare);
			break;
		}
		case VOLTAGE:

		{
			HAL_ADC_Start(&hadc);
			if (HAL_ADC_PollForConversion(&hadc, 10) == HAL_OK)
			{
				InputVoltage = HAL_ADC_GetValue(&hadc);
			}

			flags.new_data_to_show = TRUE;
			current_state = IDLE;

			break;
		}
		/* there is a process, of heating with the heater.
		 *The heater is turned on by OUT1, when the temperature is below temperature_set-Hysteresis
		 */
		case HEATING:
		{

			if (AUTO == heat_mode)
			{
				temperature_set = mode_auto_temperature(&temp_auto, &hrtc); // teplota na jakou se ma topit

				if (temperature_set == -20000) // vypni topeni
				{
					flags.heating_up = FALSE;
					flags.regulation_temp = FALSE;
				}
				else if (temperature_set == -21000)
				{
					flags.heating_up = FALSE;
					flags.regulation_temp = FALSE;
				}
				else
				{
					flags.regulation_temp = TRUE;
				}
			}

			if (flags.heating_instant) // INSTANTni vyhrivani - vzdy jen max 2 hodiny
			{
				if (!comparer_timeout(&heating_instant_timeout))
					turnOnHeater(temperature);
				else
				{
					flags.heating_instant = FALSE;
					flags.heating_instant_req = TRUE;
					heat_instant = 0;
				}
			}
			else // bezny provoz
			{
				if (flags.regulation_temp & (temperature < temperature_set))
				{
					if (flags.heating_up) // if the heater is turned on, keep continue with heating.
					{
						turnOnHeater(temperature);
					}
					else
					{ // when the heater is not active, wait until the temperature fall below threshold-Hysteresis
						if (temperature < (temperature_set - HEATING_HYSTERESIS))
						{
							flags.heating_up = TRUE;
							turnOnHeater(temperature);
						}
					}
				} // end-if

				else // vypni topeni
				{

					turnOffHeater();
					flags.heating_up = FALSE;
				}
			}
			fill_comparer(HEATING_PERIODE, &heating_compare);
			current_state = IDLE;
			break;
		}
#ifdef LOG_ENABLE
		case LOG:
		{
			if (flags.log_enable)
			{
				Log_Temperature(&hrtc, temperature, humid);
			}
			fill_comparer_seconds(LOG_PERIODE, &logging_compare);
			sirka = Log_memory_fullness() * LCD_WIDTH / LOG_DATA_LENGTH;
			current_state = IDLE;
			break;
		}
#endif
		default:
		{
			break;
		}

		} // end of switch CURRENT STATE

		/* **** SCREEN **** */
		switch (show)
		{

		case blind:
		{
			lcd_clear();
			show = idle;
			break;
		}
		case desktop:
		{ // showing main screen - temperatures and Hum
			if (flags.new_data_to_show == TRUE)
			{
				// BME280 sensor
				lcd_setCharPos(1, 2);
				char_magnitude(2);
				snprintf(buffer_s, 12, "%3ld.%02d C", temperature / 100, abs(temperature % 100));
				lcd_printString(buffer_s);
#ifdef DEBUG_TERMOSTAT //debug
				char_magnitude(1);
#endif
				char_magnitude(1);
				lcd_setCharPos(5, 2);
				snprintf(buffer_s, 21, "Humidity  %3ld.%02ld%%", (humid / 1024), humid % 1024 * 100 / 1024);
				lcd_printString(buffer_s);
#ifdef DEBUG_TERMOSTAT //debug
				char_magnitude(2);
#endif
				// Marking - heating is active/not active
				lcd_setCharPos(3, 4);
				lcd_printString(MENU_SET_TEMP_AUTO_STATE_TEXT);
				lcd_printString(" ");
				switch (heat_mode)
				{
				case OFF:
					lcd_printString("OFF ");
					break;
				case MANUAL:
					lcd_printString("ON  ");
					break;
				case AUTO:
					lcd_printString("Auto");
					break;
				default:
					lcd_printString("Err"); //ERROR dostal jsem se mimo definovanou skupinu
					break;
				}

				if (flags.heating_instant_req)
				{
					flags.heating_instant_req = FALSE;
					if (!flags.heating_instant)
					{
						lcd_setCharPos(4, 19);
						lcd_printString(" ");
						char_magnitude(1);
						lcd_setCharPos(6, 15);
						snprintf(buffer_s, 8, "        ");
						lcd_printString(buffer_s);
					}
				}

				// END Marking - heating is active/not active

				char_magnitude(1);
				lcd_setCharPos(4, 3);
				if (-20000 == temperature_set)
					snprintf(buffer_s, 24, MENU_DESKTOP_TEMP_SET_TEXT"  OFF       ");
				else if (-21000 == temperature_set)
				{
					snprintf(buffer_s, 24, MENU_DESKTOP_TEMP_SET_TEXT" NO PRESET");
				}
				else
					snprintf(buffer_s, 24, MENU_DESKTOP_TEMP_SET_TEXT" %3ld.%02d C ", temperature_set / 100, abs(temperature_set % 100));
				lcd_printString(buffer_s);

				/*	lcd_setCharPos(6,4);
  					snprintf(buffer_s, 18, "Pres %d.%02d hp",presure/100,presure%100);
  					lcd_printString(buffer_s);
				 */

#ifdef DEBUG_TERMOSTAT //debug
				lcd_setCharPos(5, 0);
				snprintf(buffer_s, 13, "%lu-> %lu.%02luV", InputVoltage, InputVoltage * 66 / 2550, (InputVoltage * 66 % 2550 * 100 / 255) / 10); // get two numbers for voltage
				lcd_printString(buffer_s);
#endif
				flags.new_data_to_show = FALSE; // the data was showed.

				/* Ukazka plnosti pameti na logovani */
				// cara bude na spodni strane a bude tri tecky siroka
				{
					line(0, 62, sirka, 62, 1);
				}

				if ((heat_mode == AUTO) & (!flags.heating_instant))
					mode_auto_graph(&temp_auto, &hrtc); // zakresleni casoveho diagramu zapnuti/ vypnuti topeni

			} // end if - new data to show
			if (show_time)
			{
				RTC_DateShow(&hrtc, aShowTime);
				lcd_setCharPos(0, 11);
				lcd_printString(aShowTime);
				RTC_TimeShow(&hrtc, aShowTime);
				lcd_setCharPos(0, 1);
				lcd_printString(aShowTime);

				if (flags.heating_instant) // odpocet casu pro instant heating
				{
					char_magnitude(2);
					lcd_setCharPos(4, 19);
					_putc(0x07f);
					char_magnitude(1);
					lcd_setCharPos(6, 15);
					uint16_t instantEndTime = end_of_timeout(&heating_instant_timeout);
					snprintf(buffer_s, 8, "%3u:%02u", instantEndTime / 60, instantEndTime % 60);
					lcd_printString(buffer_s);
				}
				fill_comparer(TIME_PERIODE, &time_compare);
				show_time = FALSE;
			}
			break;
		}
		case idle:
		{
			break;
		}

		case menu:
		{
			display_menu(ActualMenu);
			break;
		}

		case debug:
		{
			lcd_clear();
			RTC_DateShow(&hrtc, aShowTime);
			lcd_setCharPos(1, 10);
			lcd_printString(aShowTime);

			RTC_TimeShow(&hrtc, aShowTime);
			lcd_setCharPos(1, 0);
			lcd_printString(aShowTime);

			//CITAC
			uint32_t sekundy = (SlaveCounter << 16) / 1000000;

			lcd_setCharPos(4, 1);
			snprintf(buffer_s, 20, "%03lu:%02lu", sekundy / 60, sekundy % 60);
			lcd_printString(buffer_s);

			// reading the actual number from encoder

			count1 = htim22.Instance->CNT;
			lcd_setCharPos(5, 0);
			snprintf(buffer_s, 25, "s %i \r t %i ms", (count1 + T_led) * MAIN_LOOP * 2, MAIN_LOOP * 2 * T_led);
			lcd_printString(buffer_s);

			lcd_setCharPos(7, 0);
			snprintf(buffer_s, 9, "EN%03i", (count1));
			lcd_printString(buffer_s);

			snprintf(buffer_s, 12, "; enc%03i", (en_count));
			lcd_printString(buffer_s);

			HAL_Delay(500);
			break;
		}
		default:
		{
			lcd_setCharPos(1, 1);
			lcd_printString("ERROR display -default");
		}
		} // switch show
		/**
		 * 2.1.2018 -
  	1) objevil jsem chybu pri cteni time elapsing. Mam to spatne. Muze se stat, ze mi timer bude blbe fungovat, kdyz budu pricitat testne pred pretecenim promene a vykonavani se bude konat mozna az po preteceni timeru...

  	2) Cteni tlacitek by se melo provadet pokazde a po stisknuti by se melo dalsi cteni oddalit na delsi cas - 200-300 ms?
		 *
		 */

		/* *------ TIME ELAPSING CHECK -------* */
		get_actual_HAL_tick(); // put the actual number of ms counter to variable actual_HALtic.tick

		if (comparer_timeout(&logging_compare)) //log data after defined period.
		{
			current_state = LOG;
		}
		if (comparer_timeout(&heating_compare)) //measure after defined period.
		{
			current_state = HEATING;
		}
		if (comparer_timeout(&measure_compare)) //measure after defined period.
		{
			current_state = MEASURING;
		}
		if (comparer_timeout(&time_compare)) //change time after defined period.
		{
			show_time = TRUE;
		}

		if (comparer_timeout(&show_timeout)) //change time after defined period.
		{
			show = desktop;
		}
		// MENU TIMEOUT
		if (TRUE == flags.menu_running)
		{ // je to takhle slozite , protoze jsem neprisel na jiny efektivni zpusob, jak smazat displej, po zkonceni menu
			if (!menu_timout())
			{
				uint8_t menu_exit_code = menu_action();
				if (1 != menu_exit_code)
				{ // exit from menu condition

					switch (menu_exit_code)
					{
					case 20: // exit after log erasse
						sirka = Log_memory_fullness() * LCD_WIDTH / LOG_DATA_LENGTH;
						break;
					}
					flags.menu_running = 0;
					lcd_clear();
					show = desktop;
				}
				else
					show = menu;
			} // if menu - TIMEOUT
			else
			{
				flags.menu_running = 0;
				lcd_clear();
				show = desktop;
			}
			fill_comparer(BACKLITE_TIMEOUT, &backlite_compare); // Pro udrzeni rozsviceneho displeje v menu! NETESTOVANO.
		}
		if (flags.temp_new_set)
		{
			flags.temp_new_set = FALSE;
			temperature_set = temperature_manual;
			show = desktop;
		}
		if (comparer_timeout(&backlite_compare))
		{
			backliteOff();
		}

		/* *---- READ KEYBOARD -----* */

		pushed_button = BUT_NONE;
		if (comparer_timeout(&button_compare)) //every delay time
		{
			pushed_button = checkButtons();
			fill_comparer(BUT_DELAY, &button_compare);

			// reading the actual number from encoder
			uint32_t actual_counter_CNT = htim22.Instance->CNT;
			if (en_count_last != actual_counter_CNT)
			{
				en_count += actual_counter_CNT - en_count_last;
				en_count_last = actual_counter_CNT;
				flags.enc_changed = TRUE;
				if (pushed_button == BUT_NONE) // enabling light/ increase time constants.
					pushed_button = ENCODER;
			}
		}
		if (pushed_button != BUT_NONE) // any button pushed?
		{
			backliteOn();
			fill_comparer(BUT_DELAY * 300, &button_compare); // 200x - zpozdeni cteni pri stisknuti
			fill_comparer(BACKLITE_TIMEOUT, &backlite_compare);
			fill_comparer(SHOW_TIMEOUT, &show_timeout);
			// neco se zmenilo uzivatel - prekreslit cely displej
			flags.new_data_to_show = TRUE;
		}

		// -- BUTTON PROCCESS
		switch (pushed_button)
		{
		case BUT_1:
		{ // activate heater

			heat_mode++; // cyklovani modu pro termostat
			switch (heat_mode)
			{
			case OFF:
			{
				flags.regulation_temp = FALSE;
#ifndef DEBUG_TERMOSTAT								  ///// NOT - NEGOVANE!
				temperature_set = temperature_manual; // kvuli zobrazovani, jinak tam muze byt -210 apodobne
#endif
				mode_auto_graph_delete();
				break;
			}
			case MANUAL:
			{
				flags.regulation_temp = TRUE;
				flags.heating_up = TRUE;
				temperature_set = temperature_manual;
				mode_auto_graph_delete();
				break;
			}
			case AUTO:
			{
				flags.regulation_temp = TRUE;
				flags.heating_up = TRUE;
				break;
			}
			default:
			{
				flags.regulation_temp = FALSE;
				heat_mode = OFF;
				break;
			}
			}

			break;
		}
		case BUT_2:
		{ // Immediattely heating for 15 minutes

			flags.heating_instant = TRUE;
			flags.heating_instant_req = TRUE;
			heat_instant++;
			if (6 < heat_instant)
			{
				flags.heating_instant = FALSE;
				heat_instant = 0;
			}
			fill_comparer_seconds(HEATING_INSTANT * heat_instant, &heating_instant_timeout);
			break;
		}
		case BUT_ENC:
		{
			flags.enc_changed = TRUE; // jenom pomoc aby se zobrazila sipka vzdy...
			if (0 == flags.menu_running)
			{
				flags.menu_activate = 1;
				en_count = 0;
				en_count_last = 0;
				htim22.Instance->CNT = 0;
				activation_memu();
				lcd_clear();
				show = menu;

				// if this command means go to menu, That there shouldn't be no more pressed.
				pushed_button = BUT_NONE;
			}
			break;
		}
		case ENCODER:
		{
			break;
		}
		default:
		{

			break;
		}

		} // switch pushed button

		HAL_Delay(MAIN_LOOP);

#ifdef DEBUG_TERMOSTAT
		debug_led_heartbeat();
#endif
		/*
#ifdef DEBUG_TERMOSTAT
uint8_t buffer_menu2 [32] = "";
		cas_konce = HAL_GetTick();
		cas_index++;
		cas_aktualni = cas_konce - cas_zacatku;
		cas_celkem += cas_aktualni;
		cas_prumer = cas_celkem / cas_index;
if(0==cas_index%100){
		snprintf(buffer_s, 18, "aktualni %08lu", cas_aktualni);
		for (int j=0; j<32;j++){
						buffer_menu2[j] = (uint8_t *)buffer_s[j];
					}
		{
			HAL_Delay(3);
			cas_transmit = CDC_Transmit_FS(&buffer_menu2[0],18);
		}while (cas_transmit == USBD_BUSY);
		cas_transmit = 1;
		snprintf(buffer_s, 16, "prumer %08lu", cas_prumer);
		for (int j=0; j<32;j++){
						buffer_menu2[j] = (uint8_t *)buffer_s[j];
					}
		{
			HAL_Delay(3);
			cas_transmit = CDC_Transmit_FS(&buffer_menu2[0],16);
		}while (cas_transmit == USBD_BUSY);
}

#endif
*/
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Configure the main internal regulator output voltage
  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Configure LSE Drive Capability
  */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
	/** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_12;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
  */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_USB;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
