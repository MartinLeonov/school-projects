/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   instlist.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include <stdlib.h>
#include "instlist.h"
#include "scanner.h"
#include "ial.h"
#include "parser.h"

// inicializace seznamu instrukci
void listInit(tInstList *List) {
	List->First  = NULL;
	List->Active = NULL;
	List->Last   = NULL;
}

// vlozeni instrukce na konec seznamu
int listInsertInst(tInstList *List, tInst receivedInst) {
	tInstListItem *newInst;
	if ((newInst = malloc(sizeof(tInstListItem))) == NULL) {
		return RET_ERR;
	} else {
		// nacteni hodnot nove instrukce
		newInst->instruction = receivedInst;
		newInst->nextInst = NULL;
		if (List->First == NULL) {
			// jedna se o prvni instrukci v seznamu
			newInst->prevInst = NULL;
			List->First  = newInst;
			List->Active = newInst;
			List->Last   = newInst;
		} else {
			// pridani instrukce na konec
			newInst->prevInst = List->Last;
			List->Last->nextInst = newInst;
			List->Last = newInst;
		}
		return RET_OK;
	}
}

// vrati adresu aktivniho prvku
tInstListItem *listGetActive(tInstList *List) {
	if (List->Active != NULL) {
		return List->Active;
	} else {
		return NULL;
	}
}

// nastavi aktivitu na prvek poslany v parametru
void listSetActiveOn(tInstList *List,tInstListItem *addr1) {
	List->Active = addr1;
}
// posune aktivitu na nasledujici instrukci
void listNextInst(tInstList *List) {
	List->Active = List->Active->nextInst;
}

// posune aktivitu na predchozi instrukci
void listPrevInst(tInstList *List) {
	List->Active = List->Active->prevInst;
}

// posune aktivitu na prvni instrukci
void listFirstActive(tInstList *List) {
	List->Active = List->First;
}

// zruseni seznamu instrukci
void listDestroy(tInstList *List) {
	tInstListItem *tmp;
	tInstListItem *InstInList = List->First;

	while (InstInList != NULL) {
		tmp = InstInList;
		InstInList = InstInList->nextInst;
		free(tmp);
	}
}
