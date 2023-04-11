/**
 * language.h

 pro zvoleni jazyku
  projekt - TERMOSTAT
 *  Created on: Dec 29, 2016
 *      Author: jakub
 */

#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#ifdef LANGUAGE_DEF == EN
#define HEAD_SET_BACKLITE " Backlite settings\r"
#define SW_VERSION_TEXT "Version:"

#define MENU_SET_TEMP_AUTO_ENABLE_TEXT "Enable"
#define MENU_SET_TEMP_AUTO_ENABLE_VARIABLE_A "YES"
#define MENU_SET_TEMP_AUTO_ENABLE_VARIABLE_B "NO"
#define MENU_SET_TEMP_AUTO_STATE_TEXT "Heater"
#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_A "Run  "
#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_B "OFF  "

#define MENU_SET_TEMP_AUTO_TIME_TEXT "time"

#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_PRINT_A "ON "
#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_PRINT_B "OFF"

#define MENU_LOG_COUNT_TEXT "Log  count"

#define MENU_DESKTOP_TEMP_SET_TEXT "Set to"

#else
#define HEAD_SET_BACKLITE " Nastaveni podsviceni\r"
#define SW_VERSION_TEXT "Verze:"
#define MENU_SET_TEMP_AUTO_ENABLE_TEXT "Funkcni"
#define MENU_SET_TEMP_AUTO_ENABLE_VARIABLE_A "ANO"
#define MENU_SET_TEMP_AUTO_ENABLE_VARIABLE_B "NE"
#define MENU_SET_TEMP_AUTO_STATE_TEXT "topeni"
#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_A "Zapni"
#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_B "Vypni"
#define MENU_SET_TEMP_AUTO_TIME_TEXT "cas "

#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_PRINT_A "Zap"
#define MENU_SET_TEMP_AUTO_STATE_VARIABLE_PRINT_B "Vyp"

#define MENU_LOG_COUNT_TEXT "Pocet logu"

#define MENU_DESKTOP_TEMP_SET_TEXT "Nastaveno"

#endif

#endif /* LANGUAGE_H_*/