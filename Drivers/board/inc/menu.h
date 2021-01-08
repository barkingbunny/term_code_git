/*
 * menu.h
 *  projekt - TERMOSTAT
 *  Created on: Jan 23, 2017
 *      Author: jakub
 */

#ifndef MODULE_MENU_H_
#define MODULE_MENU_H_

#include "global.h"
#include "lcd_12864.h"
#include "peripherals.h"
#include "tim.h"
#include "rtc_api.h"
#include "rtc.h"
#include "usb_device.h"
#include "log.h"
#include "usb_api.h"


#define MAX_MENU_ITEM 5
#define MAX_CHAR_LENGHT 18
#define MENU_TIMOUT 10000 // x second

typedef enum EnSetMenu_t{
	clock,
	date,
	setTemperature,
	printLogUSB,
	next,
	menuReset,
	printLogLCD,
	information,
	eraseLogMem
} EnSetMenu;

typedef struct menu_item_s{
	uint8_t menuHeader[MAX_CHAR_LENGHT];
	uint8_t numberOfChoices;
	struct menu_item_s *submenu[MAX_MENU_ITEM];
	struct menu_item_s *upmenu;
	EnSetMenu action;
}menu_item_t;

extern menu_item_t* ActualMenu;
extern int32_t temperature_set;
extern Buttons pushed_button;
extern int8_t en_count;
extern int32_t temperature;

uint8_t menu_action();
void display_menu(menu_item_t* display_menu);
Bool menu_timout();
uint8_t activation_memu();

#endif /* MODULE_MENU_H_ */


/** NAVOD NA STRUKTURY
 * Struktura

Struktury jsou kontejnery pro heterogenní data. Tak jako pole shromažďuje hodnoty stejného typu, struktury shromažďují data různých typů. Typickým příkladem použití struktury je uchování dat zaměstnance, jako je jméno, věk a plat.

Deklarace struktury

Deklarace struktury má následující syntaxi:

struct <[jméno typu]> {<výčet členů>}<[deklarace]>;
Od deklarace výčtu se liší pouze použitým klíčovým slovem a také výčtem členů. Výše uvedený záznam zaměstnance by se uchoval ve struktuře:

struct zamestnanec
{
  char jmeno[30];
  int vek;
  float plat;
} Pepa;
Pro deklaraci s klíčovým slovem typedef a bez něj platí stejná pravidla jako u výčtového typu. Uvnitř struktury může být jakýkoliv typ včetně ukazatelů na strukturu samotnou nebo jiné vnořené struktury.

Inicializace struktury

Struktura se inicializuje podobně jako pole, tedy výčtem hodnot oddělených čárkami ve složených závorkách. V případě vnořených struktur inicializujeme vnitřní strukturu dalším vnořeným párem složených závorek. Pokud vynecháme některé hodnoty, překladač zbytek doplní nulami stejně jako u polí.

struct zamestnanec Franta = {"Franta Novak", 28, 21000.0};
// chudák Tonda má plat 0
struct zamestnanec Tonda = {"Tonda Voprsalek", 30};
// bezejmenný, bezletý a neohodnocený zaměstnanec
struct zamestnanec Nikdo = {};
Přístup ke členům struktury

Pro přístup ke členům struktury slouží operátor tečka. Máme-li našeho zaměstnance Pepu, změníme mu věk následovně:

Pepa.vek = 31;
Pokud máme pouze ukazatel na strukturu, máme dvě možnosti, jak se dostat ke členským proměnným. Buď ukazatel nejprve dereferencujeme a poté použijeme operátor tečka, nebo použijeme přímo operátor šipka, který provede prvně zmíněné kroky interně.

struct zamestnanec *uk = &Pepa;
(*uk).vek = 31; // použití dereference
uk->vek = 31; // pohodlnější, ekvivalentní dereferenci a tečkování
Kopírování a porovnávání

Proměnné struktury stejného typu lze jednoduše kopírovat (na rozdíl od polí). Stačí pouze přiřadit jednu proměnnou do druhé a automaticky dojde ke zkopírování všech členů struktury.

// Franta a Pepa se stanou jednou osobou
Pepa = Franta;
Problém nastává, když máme ve struktuře dynamicky alokovaná data. Při kopírování se zkopírují pouze ukazatele, ale data zůstanou sdílená (jde o tzv. mělkou kopii). V takovém případě se o zkopírování dat musíme postarat sami.

Pokud chceme proměnné struktur porovnávat, musíme tak učinit člen po členu, nelze jednoduše napsat:

if (Pepa == Franta) //...
 *
 */
