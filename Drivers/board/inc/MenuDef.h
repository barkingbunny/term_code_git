/*
 * MenuDef.h
 *  projekt - TERMOSTAT
 *  Created on: Jan 24, 2017
 *      Author: jakub
 */
/**
 * definition of MENU items.
 * The lower level needs to be defined as first.
 */
#ifndef MODULE_MENUDEF_H_
#define MODULE_MENUDEF_H_

#include "menu.h"

// submenu

//define all structures first:
const menu_item_t MainMenu;
const menu_item_t setTime;
const menu_item_t setTemp;
const menu_item_t put_menuReset;
const menu_item_t extLOG;
const menu_item_t about;
//TIME submenus
const menu_item_t setDate;
const menu_item_t setClock;
// LOG submenus
const menu_item_t i_printLogLCD;
const menu_item_t i_printLogUSB;
const menu_item_t i_eraseLogMem;


/*
 * uint8_t *menuHeader;
	uint8_t numberOfChoices;
	struct menu_item_s *submenu[MAX_MENU_ITEM];
	struct menu_item_s *upmenu;
	EnPushMenu action;
 */


// MAIN MENU - root
const menu_item_t MainMenu = {
		"MENU",
		5,
		{&setTemp,&setTime,&extLOG,&put_menuReset,&about},
		NULL,
		next
};

const menu_item_t setTime = {
		"TIME",
		2,
		{&setClock,&setDate},
		&MainMenu,
		next
};

const menu_item_t setTemp = {
		"TEMPERATURE",
		0,
		{NULL},
		&MainMenu,
		setTemperature
};

const menu_item_t put_menuReset = {
		"RESET",
		0,
		{NULL},
		&MainMenu,
		menuReset
};

const menu_item_t extLOG = {
		"LOGs",
		3,
		{&i_printLogLCD,&i_printLogUSB,&i_eraseLogMem},
		&MainMenu,
		next
};
const menu_item_t about = {
		"ABOUT",
		0,
		{NULL},
		&MainMenu,
		information
};


/* submenu for TIME */

const menu_item_t setClock = {
		"Clock",
		0,
		{NULL},
		&setTime,
		clock
};

const menu_item_t setDate = {
		"Date",
		0,
		{NULL},
		&setTime,
		date
};
/* submenu for LOG */
const menu_item_t i_printLogLCD = {
		"Print data to LCD",
		0,
		{NULL},
		&extLOG,
		printLogLCD
};

const menu_item_t i_printLogUSB = {
		"Print data to USB",
		0,
		{NULL},
		&extLOG,
		printLogUSB
};

const menu_item_t i_eraseLogMem = {
		"Erase log memory",
		0,
		{NULL},
		&extLOG,
		eraseLogMem
};






#endif /* MODULE_MENUDEF_H_ */
