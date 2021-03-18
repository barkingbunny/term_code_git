/*
 * menu.c
 *
 *  Created on: Jan 23, 2017
 *      Author: jakub
 */

#include "MenuDef.h"
#include "menu.h"
#include "rtc_api.h" // for working with RTC
#include <Time.h>
//#include "usbd_cdc_if.h"

menu_item_t *ActualMenu;
Compare_t menu_compare;
int8_t position, position_x;
static uint8_t temp_index;
static uint8_t last_temp_index;
uint16_t backlite_value = LCD_BACKLITE_DUTY;

int16_t set_temperature = 2000;
RTC_TimeTypeDef set_stimestructureget;
RTC_HandleTypeDef set_RtcHandle;
RTC_DateTypeDef set_datestruct;
/**
 * @brief Tato funkce se vzdy spousti pred aktivaci MENU
 * 			zde se tedy vse pripravi pro dalsi funkcnost.
 * 
 * @return uint8_t jestli probehla inicializace vporadku.
 */
uint8_t activation_memu()
{
	menu_compare.overflow = actual_HALtick.overflow;
	fill_comparer(MENU_TIMOUT, &menu_compare);
	position = 0;
	position_x = 0;
	en_count = 0;
	temp_index = 0;
	last_temp_index = temp_index;
	ActualMenu = &MainMenu;
	flags.menu_activate = 0;
	flags.menu_running = 1;
	return 0;
}

/** MENU action - function that cure all function in the MENU
 *
 * return values:
 * 0 - success finished menu, exit
 * 1 - success, continue in the MENU
 * 20 - success finished menu, exit, LOG mem erassed
 *
 */
uint8_t menu_action()
{

	char buffer_menu[32];
#ifdef DEBUG_TERMOSTAT
	lcd_setCharPos(7, 14);
	char buffer_s[32];
	snprintf(buffer_s, 12, "enc%03i", (en_count));
	lcd_printString(buffer_s);
#endif

	if (flags.enc_changed)						   // move by encoder was detected - action on display
		fill_comparer(MENU_TIMOUT, &menu_compare); //enlarge time in menu

	switch (ActualMenu->action)
	{
	case (next):
	{
		if (flags.enc_changed)
		{ // move by encoder was detected - action on display
			position = en_count;
			if ((position) < 0)
				en_count = 0;
			if ((position) > ActualMenu->numberOfChoices)
				en_count = ActualMenu->numberOfChoices;
			position = en_count;
			for (uint8_t j = 0; j < 8; j++)
			{
				lcd_setCharPos(j, 0);
				_putc(' ');
				_putc(' ');
			}
			lcd_setCharPos((position + 1), 0);
			_putc(0x083);
			_putc(0x084);
			copy_to_lcd();
			flags.enc_changed = FALSE;
		} //if COUNTER!=0

		if (pushed_button == BUT_ENC)
		{							  // co se bude dit po stisknuti tlactika???
			flags.enc_changed = TRUE; // jenom pomoc aby se zobrazila sipka vzdy...
			if (position > (ActualMenu->numberOfChoices - 1))
			{									// probably the end of the choices - back / exit
				if (ActualMenu->upmenu == NULL) // in main manu - exit
				{
					return 0; // exit the menu
				}
				else
					ActualMenu = ActualMenu->upmenu;
			}
			else
			{ // if is not chosen the exit or back.
				ActualMenu = ActualMenu->submenu[position];
				switch (ActualMenu->action)
				{
				case (clock):
				{
					HAL_RTC_GetTime(&hrtc, &set_stimestructureget, RTC_FORMAT_BIN);
					position_x = 0;
					break;
				} // end case CLOCK

				case (date):
				{
					HAL_RTC_GetDate(&hrtc, &set_datestruct, RTC_FORMAT_BIN);
					position_x = 0;
					break;
				} // end case date
				case (setTemperature):
				{ // inicializace promene pred vlezenim do tohoto menu
					position_x = 0;
					break;
				}
				case (backlight_intensity):
				{
					backlite_value = PWM_duty_read(LCD_LIGHT);
					break;
				}

				} // end SWITCH
			}
			lcd_clear();
			en_count = 0;
		} // end IF - PUSHED BUTTON
		break;
	} // END case (next)

	case (clock):
	{
		if (pushed_button == BUT_ENC)
		{
			position_x++;
		}
		if (position_x == 0)
		{
			if (set_stimestructureget.Hours + en_count < 0)
				set_stimestructureget.Hours = set_stimestructureget.Hours + 24 + en_count;
			else
			{
				set_stimestructureget.Hours = set_stimestructureget.Hours + en_count;
			}
			if (set_stimestructureget.Hours > 23)
				set_stimestructureget.Hours -= 24;
		}
		if (position_x == 1)
		{
			if (set_stimestructureget.Minutes + en_count < 0)
				set_stimestructureget.Minutes = set_stimestructureget.Minutes + 60 + en_count;
			else
			{
				set_stimestructureget.Minutes = set_stimestructureget.Minutes + en_count;
			}
			if (set_stimestructureget.Minutes > 59)
				set_stimestructureget.Minutes -= 60;
		}
		if (position_x == 2)
		{
			if (set_stimestructureget.Seconds + en_count < 0)
				set_stimestructureget.Seconds = set_stimestructureget.Seconds + 60 + en_count;
			else
			{
				set_stimestructureget.Seconds = set_stimestructureget.Seconds + en_count;
			}
			if (set_stimestructureget.Seconds > 59)
				set_stimestructureget.Seconds -= 60;
		}
		if (position_x == 3)
		{ // last click with encoder
			HAL_RTC_SetTime(&hrtc, &set_stimestructureget, RTC_FORMAT_BIN);
			position_x = 0;
			flags.menu_running = 0;
			lcd_clear();
			return 0; //exit menu
		}
		if (position_x > 3)
			position_x = 0;

		en_count = 0;
		break;
	}

	case (date):
	{
		if (pushed_button == BUT_ENC)
		{
			position_x++;
		}
		if (position_x == 0)
		{
			if (set_datestruct.Date + en_count < 1)
				set_datestruct.Date = set_datestruct.Date + 31 + en_count;
			else
			{
				set_datestruct.Date = set_datestruct.Date + en_count;
			}
			if (set_datestruct.Date > 31)
				set_datestruct.Date -= 31;
		}
		if (position_x == 1)
		{
			if (set_datestruct.Month + en_count < 1)
				set_datestruct.Month = set_datestruct.Month + 12 + en_count;
			else
			{
				set_datestruct.Month = set_datestruct.Month + en_count;
			}
			if (set_datestruct.Month > 12)
				set_datestruct.Month -= 12;
		}
		if (position_x == 2)
		{
			if (set_datestruct.Year + en_count < 1)
				set_datestruct.Year = set_datestruct.Year + 100 + en_count;
			else
			{
				set_datestruct.Year = set_datestruct.Year + en_count;
			}
			if (set_datestruct.Year > 99)
				set_datestruct.Year -= 100;
		}
		if (position_x == 3)
		{ // last click with encoder
			HAL_RTC_SetDate(&hrtc, &set_datestruct, RTC_FORMAT_BIN);
			position_x = 0;
			flags.menu_running = 0;
			lcd_clear();
			return 0; //exit menu
		}
		if (position_x > 3)
			position_x = 0;

		en_count = 0;
		break;
	}

	case (setTemperature):
	{
		if (AUTO == heat_mode)
		{
			if (pushed_button == BUT_ENC)
			{
				position_x++;
			}
			switch (position_x)
			{
			case 0:
			{
				temp_auto.status[temp_index].valid_timer += en_count;
				break;
			}
			case 1:
			{
				if (FALSE == temp_auto.status[temp_index].valid_timer)
					position_x = 5;
				else
					temp_auto.status[temp_index].state += en_count;
				break;
			}
			case 2:
			{
				if (TRUE == temp_auto.status[temp_index].state) // pokud se ma topit
				{
					if (temp_auto.tempOn[temp_index] < 1800) // a pokud je nastavena hodnota na teplotu mensi nez 18C
						temp_auto.tempOn[temp_index] = 2000; // prednastav tam 20 C
				}
				else
				{
					temp_auto.tempOn[temp_index] = 0;
				}

				if (temp_auto.time_s[temp_index].hours + en_count < 0)
					temp_auto.time_s[temp_index].hours = temp_auto.time_s[temp_index].hours + 24 + en_count;
				else
				{
					temp_auto.time_s[temp_index].hours = temp_auto.time_s[temp_index].hours + en_count;
				}
				if (temp_auto.time_s[temp_index].hours > 23)
					temp_auto.time_s[temp_index].hours -= 24;
				break;
			}
			case 3:
			{
				if (temp_auto.time_s[temp_index].minutes + en_count * 10 < 0)
					temp_auto.time_s[temp_index].minutes = temp_auto.time_s[temp_index].minutes + 60 + en_count * 10;
				else
				{
					temp_auto.time_s[temp_index].minutes = temp_auto.time_s[temp_index].minutes + en_count * 10;
				}
				if (temp_auto.time_s[temp_index].minutes > 59)
					temp_auto.time_s[temp_index].minutes -= 60;
				break;
			}
			case 4:
			{
				if (FALSE == temp_auto.status[temp_index].state) // kdyz je stav VYPNI, neni potreba nastavovat teplotu.
				{
					position_x = 5;
				}
				else
				{
					temp_auto.tempOn[temp_index] = temp_auto.tempOn[temp_index] + en_count * 50;
					if (temp_auto.tempOn[temp_index] > TEMPERATURE_MAX) // if the chosen temperature is higher than maximum allowed temperature
					{
						temp_auto.tempOn[temp_index] = TEMPERATURE_MAX;
					}
					if (temp_auto.tempOn[temp_index] < TEMPERATURE_MIN) // if the chosen temperature is lower than minimum allowed temperature
					{
						temp_auto.tempOn[temp_index] = TEMPERATURE_MIN;
					}
				}
				break;
			}
			case 5:
			{
				temp_index++;
				mode_auto_sort(&temp_auto);
				if (temp_index >= AUTO_TIMERS)
				{
					//DEBUG
					lcd_clear();

					for (uint8_t index_a = 0; index_a < (AUTO_TIMERS); index_a++)
					{

						lcd_setCharPos(index_a, 1);
						snprintf(buffer_menu, 33, "%02d:%02d;%3s;%3ld.%02ld C", temp_auto.time_s[temp_auto.sortIndex[index_a]].hours, temp_auto.time_s[temp_auto.sortIndex[index_a]].minutes, temp_auto.status[temp_auto.sortIndex[index_a]].state ? "Zap" : "Vyp", temp_auto.tempOn[temp_auto.sortIndex[index_a]] / 100, abs(temp_auto.tempOn[temp_auto.sortIndex[index_a]] % 100));
						lcd_printString(buffer_menu);
					}
					HAL_Delay(3500);
					//DEBUG

					// last click with encoder
					position_x = 0;
					flags.menu_running = 0;
					flags.temp_new_set = TRUE; // naznaceni nove nastavene temploty
					flags.heating_up = TRUE;   // if the button was pushed, turn-on the heater, even if the temperature is reached.
					lcd_clear();
					return 0; //exit menu
				}
				else
				{
					position_x = 0;
					lcd_clear();
				}
				break;
			}
			default:
			{
				position_x = 0;
			}
			}

			en_count = 0;
		}
		break;
	}
	case (printLogUSB): //BLOKOVACI
	{
		uint8_t post = 0;
		uint8_t usbTransmit = 0;

		//LCD vypis
		lcd_clear();
		lcd_setCharPos(1, 1);
		snprintf(buffer_menu, 16, "Vypisuji na USB");
		lcd_printString(buffer_menu);
		//debug

		uint8_t buffer_menu2[32] = "Vypisuji na USB";
		char buffer_menu3[32];

		post = CDC_Transmit_FS(&buffer_menu2[0], 16);

		lcd_setCharPos(0, 4);
		snprintf(buffer_menu, 12, "return %i", post);
		lcd_printString(buffer_menu);
		uint8_t log_flag = 0;

		for (uint16_t index = 0; index < LOG_DATA_LENGTH; index++)
		{
			log_flag = log_data[index].enable_flag.heating_up;
			log_flag |= log_data[index].enable_flag.regulation_temp << 1;
			log_flag |= log_data[index].enable_flag.heating_instant << 2;

			snprintf(buffer_menu3, 30, "%03i;%02u;%02u;%02u;%02u;%03ld.%02d;%03ld%;%02x\r\n ", (index + 1), log_data[index].month, log_data[index].day, log_data[index].hour, log_data[index].minute, log_data[index].temp_1 / 100, abs(log_data[index].temp_1 % 100), (log_data[index].hum_1), log_flag);
			for (int j = 0; j < 32; j++)
			{
				buffer_menu2[j] = (uint8_t *)buffer_menu3[j];
			}
			{
				HAL_Delay(3);
				usbTransmit = CDC_Transmit_FS(&buffer_menu2[0], 30);
				//if(usbTransmit==USBD_BUSY) HAL_Delay(500);
				/*mel jsem problem, ze se pri vypisovani USB dostavalo do BUSY modu. Bylo hodne dat a nemel jsem Delay funkci. Tak busy bylo porad.*/
			}
			while (usbTransmit == USBD_BUSY)
				;
		}
		return 0; //exit menu

		break;
	}

	case (printLogLCD): // toto zaruci, ze se po dobu vypisu bude stale provadet normalni rutina, ale zaroven se neukonci vypisovani
	{

		char buffer_menu5[19];
		uint8_t log_readings = Log_To_String(&buffer_menu5, 18);
		if (0 == log_readings) // no data to read
			snprintf(buffer_menu5, 19, "NO DATA TO READ");
		lcd_setCharPos(3, 0);
		lcd_printString(buffer_menu5);
		HAL_Delay(1000);
		fill_comparer(1500, &menu_compare); // Pricteni mi vyprsel cas pro menu. tak ho zvetsuji.

		if (pushed_button == BUT_ENC)
		{
			flags_log.read_request = FALSE;
			return 0; //exit menu; INTERUPTED by user
		}
		if (1 == log_readings) // all data read
			return 0;		   //exit menu, finished
		if (0 == log_readings) // no data to read
			return 0;		   //exit menu, finished

		break;
	}

	case (eraseLogMem):
	{
		char buffer_menu[32];
		lcd_clear();
		lcd_setCharPos(2, 0);
		snprintf(buffer_menu, 20, "Pocet logu %3u/%u", Log_memory_fullness(), LOG_DATA_LENGTH);
		lcd_printString(buffer_menu);

		//vymazani logovaci databaze/pameti
		Log_errase_database();
		//LCD vypis
		lcd_setCharPos(3, 1);
		snprintf(buffer_menu, 20, "Log memory - erased");
		lcd_printString(buffer_menu);
		lcd_setCharPos(5, 0);
		snprintf(buffer_menu, 20, "Pocet logu %3u/%u", Log_memory_fullness(), LOG_DATA_LENGTH);
		lcd_printString(buffer_menu);

		HAL_Delay(1000);

		return 20; //exit menu, finished
		break;
	}

	case (menuReset):
	{
		NVIC_SystemReset();
		break;
	}
	case (backlight_intensity):
	{

		if (pushed_button == BUT_ENC)
		{

			flags.menu_running = 0;
			lcd_clear();
			return 0; //exit menu
		}
		break;
	}

	case (information):
	{

		if (pushed_button == BUT_ENC)
		{
			flags_log.read_request = FALSE;
			return 0; //exit menu
		}
		break;
	}

	default:
	{
		lcd_setCharPos(3, 5);
		lcd_printString("ERROR 0100");

		break;
	}
	}		  // end of switch
	return 1; // correct end of function- continue.
}

Bool menu_timout(void)
{
	if (comparer_timeout(&menu_compare))
	{
		return TRUE;
	}
	return FALSE;
}

void display_menu(menu_item_t *display_menu)
{
	char buffer_menu[32];
	uint8_t i = 0;

	lcd_setCharPos(0, 1);
	lcd_printString(display_menu->menuHeader);

	if (display_menu->numberOfChoices == 0)
	{ //set value
		lcd_setCharPos(2, 2);
		switch (display_menu->action)
		{
		case (clock):
		{
			RTC_TimeShow_time(&set_stimestructureget, buffer_menu);
			lcd_setCharPos(1, 10);
			lcd_printString(buffer_menu);

			// aktivni text oznacen podtrzenim
			line_pixel_clear(17);
			// pouzivam switch - rychlejsi operace nez pouziti vzorce - viz default.
			switch (position_x)
			{
			case 0:
				line((59), 17, (71), 17, 1);
				break;
			case 1:
				line((77), 17, (89), 17, 1);
				break;
			case 2:
				line((95), 17, (107), 17, 1);
				break;
			default:
				line((59 + position_x * 18), 17, (71 + position_x * 18), 17, 1);
			}
			break;
		}
		case (date):
		{
			RTC_DateShow_date(&set_datestruct, buffer_menu);
			lcd_setCharPos(1, 10);
			lcd_printString(buffer_menu);
			// aktivni text oznacen podtrzenim
			line_pixel_clear(17);
			// pouzivam switch - rychlejsi operace nez pouziti vzorce - viz default.
			switch (position_x)
			{
			case 0:
				line((59), 17, (71), 17, 1);
				break;
			case 1:
				line((77), 17, (89), 17, 1);
				break;
			case 2: // rok ma zmenu jen na dvou poslednich
				line((107), 17, (119), 17, 1);
				break;
			default:
				line((59 + position_x * 18), 17, (71 + position_x * 18), 17, 1);
			}
			break;
		}
		case (setTemperature):
		{
			if ((AUTO == heat_mode)) // pouze vypisovaci konzole
			{
				lcd_setCharPos(1, 0);
				snprintf(buffer_menu, 18, "funkcni:  %3s", temp_auto.status[temp_index].valid_timer ? "ANO" : "NE");
				lcd_printString(buffer_menu);

				lcd_setCharPos(2, 0);
				snprintf(buffer_menu, 14, "stav:   %5s", temp_auto.status[temp_index].state ? "Zapni" : "Vypni");
				lcd_printString(buffer_menu);
				lcd_setCharPos(3, 0);
				snprintf(buffer_menu, 14, "cas:    %02d:%02d", temp_auto.time_s[temp_index].hours, temp_auto.time_s[temp_index].minutes);
				lcd_printString(buffer_menu);
				lcd_setCharPos(4, 0);
				snprintf(buffer_menu, 18, "temp:  %3ld.%02ld C ", temp_auto.tempOn[temp_index] / 100, abs(temp_auto.tempOn[temp_index] % 100));
				lcd_printString(buffer_menu);
				lcd_setCharPos(0, 15);
				snprintf(buffer_menu, 8, "ID: %2u", temp_index + 1);
				lcd_printString(buffer_menu);

				// aktivni cast - sipka
				switch (position_x)
				{
				case 0:
					lcd_setCharPos((1), 13);
					_putc(0x085);
					copy_to_lcd();
					break;
					case 1:
					lcd_setCharPos((1), 13);
					_putc(' '); // delete symbol
					lcd_setCharPos((2), 13);
					_putc(0x085);
					copy_to_lcd();
					break;
					case 2:
					lcd_setCharPos((2), 13);
					_putc(' '); // delete symbol
					lcd_setCharPos((3), 13);
					_putc(0x085);
					copy_to_lcd();
					break;
					case 3:
					lcd_setCharPos((3), 13);
					_putc(' '); // delete symbol
					lcd_setCharPos((3), 15);
					_putc(0x085);
					copy_to_lcd();
					break;
					case 4:
					lcd_setCharPos((3), 15);
					_putc(' '); // delete symbol
					lcd_setCharPos((4), 13);
					_putc(0x085);
					copy_to_lcd();
					break;
				default:
					lcd_setCharPos((position_x), 13);
					_putc(' '); // delete symbol
					lcd_setCharPos((position_x + 1), 13);
					_putc(0x085);
					copy_to_lcd();
					break;
				}

				/* SEM BUDU PSAT proceduru pro vygresleni GRAFU, co znazorni zap/vyp topeni v jednodenim cyklu*/
				if (last_temp_index != temp_index) // pokud nastala zmena - dalsi nastaveni pameti
				{
					mode_auto_graph(&temp_auto, &hrtc);
					last_temp_index = temp_index;
				}
				// Prekreslit caru - znaceni zapnuti a vypnut doby 120/24 = 5tecek na hodinu
			}
			else
			{
				if (!flags.temp_new_set)
				{
					{
						lcd_setCharPos(3, 0);
						char_magnitude(1);
						snprintf(buffer_menu, 16, "Set temperature");
						lcd_printString(buffer_menu);

						set_temperature = temperature_manual + en_count * 50;
						if (set_temperature > TEMPERATURE_MAX) // if the chosen temperature is higher than maximum allowed temperature
						{
							set_temperature = TEMPERATURE_MAX;
							en_count = (set_temperature - temperature_manual) / 50; // calculate how much it should be at temp max
						}
						if (set_temperature < TEMPERATURE_MIN) // if the chosen temperature is lower than minimum allowed temperature
						{
							set_temperature = TEMPERATURE_MIN;
							en_count = (set_temperature - temperature_manual) / 50;
						}
						lcd_setCharPos(5, 3);
						char_magnitude(2);
						snprintf(buffer_menu, 12, "%3ld.%02ld C ", set_temperature / 100, abs(set_temperature % 100));
						lcd_printString(buffer_menu);
						char_magnitude(1);
					}
				}
				if (pushed_button == BUT_ENC)
				{ // BUTTONE PUSHED
					temperature_manual = (int32_t)set_temperature;
					flags.temp_new_set = TRUE;
					flags.heating_up = TRUE; // if the button was pushed, turn-on the heater, even if the temperature is reached.

					// end of the Menu
					flags.menu_running = 0;
					lcd_clear();
				} // end of BUTTONE PUSHED
			}

			break;
		}

		case (printLogLCD): // Zde se bude vypisovat zalogovane hodnoty
		{

			/*
 * Sem chci narvat vypisovani logu po radku na displej. Cas mezi vypisovanim by mel byt definovan nekde dale.
 * Po vypsani posledniho radku se bude cekat, nez obsluha zmackne encoder, nebo na vyprseni casove konstanty
 *
 * zde se bude pravdepodobne nachazet jen jedna funkce, ktera da vedet vypisovaci funkci, ze je cas na dalsi promneou.
 *
 */
			break;
		}

		case (backlight_intensity): // intenzita podsviceni
		{
			if (en_count != 0)
			{
				PWM_duty_change(LCD_LIGHT, PWM_duty_read(LCD_LIGHT) + en_count);
				en_count = 0;
			}
			lcd_setCharPos(1, 0);
			lcd_printString(" Nastaveni podsviceni\r");
			lcd_printString("\r");
			char_magnitude(2);
			snprintf(buffer_menu, 10, " %3u", PWM_duty_read(LCD_LIGHT));
			lcd_printString(buffer_menu);
			char_magnitude(1);

			break;
		}

		case (information): // Zde se budou vypisovat informace ohledne FW/HW
		{
			lcd_setCharPos(1, 0);
			lcd_printString("Termostat_git\r");
			lcd_printString("\r");
			snprintf(buffer_menu, 20, "Verze:  0.%d \r", SW_VERSION);
			lcd_printString(buffer_menu);
			snprintf(buffer_menu, 20, "Pocet logu %3u/%u", Log_memory_fullness(), LOG_DATA_LENGTH);
			lcd_printString(buffer_menu);

			break;
		}

		default:
		{

			break;
		}
		} // end switch
	}
	else
	{ // write a list of menu choices

		for (i = 0; i < display_menu->numberOfChoices; i++)
		{
			lcd_setCharPos((i + 1), 2);
			lcd_printString(display_menu->submenu[i]->menuHeader);
		}
		lcd_setCharPos(display_menu->numberOfChoices + 1, 2);
		if (display_menu->menuHeader == MainMenu.menuHeader)
			lcd_printString("EXIT");
		else
			lcd_printString("Back");
	}
} // end function

void menu_comparer_overflow_erase(void)
{
	menu_compare.overflow = FALSE;
}
