	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                             Úpravy: Bohuslav Køena, øíjen 2013
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* This program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/

	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L)	{
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/

	tDLElemPtr Temp;

	// postupne uvolnovani prvku
	while (L->First != NULL) {
		Temp = L->First->rptr;
		free(L->First);
		L->First = Temp;
	}

	L->Act = NULL;
	L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
L->Last = NewLast;
L->Last = NewLast;
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr NewFirst;

	// alokace pameti pro novy prvek
	if ((NewFirst = malloc(sizeof(struct tDLElem))) == NULL) {
		DLError();
		return;
	}

	// vlozeni noveho prvku do seznamu
	NewFirst->data = val;
	if (L->First == NULL) {
		// pokud se jedna o prvni prvek
		L->First = NewFirst;
		L->Last = NewFirst;
		NewFirst->lptr = NULL;
		NewFirst->rptr = NULL;
	} else {
		// pokud se nejedna o prvni prvek
		L->First->lptr = NewFirst;
		NewFirst->lptr = NULL;
		NewFirst->rptr = L->First;
		L->First = NewFirst;
	}
}

void DLInsertLast(tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	
	
	tDLElemPtr NewLast;

	// alokace pameti pro novy prvek
	if ((NewLast = malloc(sizeof(struct tDLElem))) == NULL) {
		DLError();
		return;
	}

	// vlozeni noveho prvku do seznamu
	NewLast->data = val;
	if (L->First == NULL) {
		// pokud se jedna o prvni prvek
		L->First = NewLast;
		L->Last = NewLast;
		NewLast->lptr = NULL;
		NewLast->rptr = NULL;
	} else {
		// pokud se nejedna o prvni prvek
		L->Last->rptr = NewLast;
		NewLast->rptr = NULL;
		NewLast->lptr = L->Last;
		L->Last = NewLast;
	}
}

void DLFirst (tDLList *L)	{
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
	// nastaveni prvniho prvku seznamu jako aktivni
	L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	
	// nastaveni posledniho prvku seznamu jako aktivni
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

	if (L->First == NULL) {
		DLError();
		return;
	} else {
		*val = L->First->data;
	}
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
	if (L->First == NULL) {
		DLError();
		return;
	} else {
		*val = L->Last->data;
	}
}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
	
	tDLElemPtr FirstToDel;

	// kontrola jestli neni prazdny
	if (L->First != NULL) {
		FirstToDel = L->First;
		if (L->First == L->Last) {
			// obsahuje jediny prvek
			L->First = NULL;
			L->Last = NULL;
			L->Act = NULL;
		} else {
			if (L->First == L->Act) {
				// pokud se rusi aktivni
				L->Act = NULL;
			}
			L->First = FirstToDel->rptr;
			L->First->lptr = NULL;
		}
		free(FirstToDel);
	}
}	

void DLDeleteLast (tDLList *L)	{
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 

	tDLElemPtr LastToDel;

	// kontrola jestli neni prazdny
	if (L->Last != NULL) {
		LastToDel = L->Last;
		if (L->First == L->Last) {
			// obsahuje jediny prvek
			L->First = NULL;
			L->Last = NULL;
			L->Act = NULL;
		} else {
			if (L->Last == L->Act) {
				// pokud se rusi aktivni
				L->Act = NULL;
			}
			L->Last = LastToDel->lptr;
			L->Last->rptr = NULL;
		}
		free(LastToDel);
	}
}

void DLPostDelete (tDLList *L)	{
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
	// pokud je neaktivni nebo aktivni je posledni v seznamu
	// nic se nedeje
	if ((L->Act == NULL) || (L->Act == L->Last)) {
		return;
	} else {
		// zjisteni jestli je co rusit
		if (L->Act->rptr != NULL) {
			tDLElemPtr TempElement;
			// ukazatel na zruseny
			TempElement = L->Act->rptr;
			// preklenuti ruseneho
			L->Act->rptr = TempElement->rptr;
			if (TempElement == L->Last) {
				// jestli je ruseny posledni, poslednim
				// se stane aktivni
				L->Last = L->Act;
			} else {
				// prvek za rusenym ukazuje
				// doleva na aktivni prvek
				TempElement->rptr->lptr = L->Act;
			}
			free(TempElement);
		}
	}
}

void DLPreDelete (tDLList *L)	{
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
	
	// pokud je neaktivni nebo aktivni je prvni v seznamu
	// nic se nedeje
	if ((L->Act == NULL) || (L->Act == L->First)) {
		return;
	} else {
		// zjisteni jestli je co rusit
		if (L->Act->lptr != NULL) {
			tDLElemPtr TempElement;
			// ukazatel na ruseny
			TempElement = L->Act->lptr;
			// preklenuti ruseneho
			L->Act->lptr = TempElement->lptr;
			if (TempElement == L->First) {
				// jestli je ruseny prvni, prvnim se
				// stane aktivni
				L->First= L->Act;
			} else {
				// prvek pred zrusenym ukazuje 
				// doprava na aktivni prvek
				TempElement->lptr->rptr = L->Act;
			}
			free(TempElement);
		}
	}
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	// pokud je seznam neaktivni, nic se nedeje
	if (L->Act != NULL) {

		tDLElemPtr NewPostElem;
		// alokace pameti pro novy prvek
		if ((NewPostElem = malloc(sizeof(struct tDLElem))) == NULL) {
			DLError();
			return;
		}

		NewPostElem->data = val;
		NewPostElem->rptr = L->Act->rptr;
		NewPostElem->lptr = L->Act;
		L->Act->rptr = NewPostElem;
		if (L->Act == L->Last) {
			L->Last = NewPostElem;
		} else {
			NewPostElem->rptr->lptr = NewPostElem;
		}
	}
}

void DLPreInsert (tDLList *L, int val)		{
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	
	// pokud je seznam neaktivni, nic se nedeje
	if (L->Act != NULL) {

		tDLElemPtr NewPostElem;
		// alokace pameti pro novy prvek
		if ((NewPostElem = malloc(sizeof(struct tDLElem))) == NULL) {
			DLError();
			return;
		}

		NewPostElem->data = val;
		NewPostElem->lptr = L->Act->lptr;
		NewPostElem->rptr = L->Act;
		L->Act->lptr = NewPostElem;
		if (L->Act == L->First) {
			L->First = NewPostElem;
		} else {
			NewPostElem->lptr->rptr = NewPostElem;
		}
	}
}

void DLCopy (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	
	if (L->Act == NULL) {
		DLError();
		return;
	} else {
		*val = L->Act->data;
	}
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/

	if (L->Act != NULL) {
		L->Act->data = val;
	}
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/

	// kontrola, zda je seznam aktivni
	if (L->Act != NULL) {
		if (L->Act == L->Last) {
			// aktivita na poslednim prvku
			L->Act = NULL;
		} else {
			L->Act = L->Act->rptr;
		}
	}
}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/

	// kontrola, zda je seznam aktivni
	if (L->Act != NULL) {
		if (L->Act == L->First) {
			// aktivita na prvnim prvku
			L->Act = NULL;
		} else {
			L->Act = L->Act->lptr;
		}
	}
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/

	return (L->Act == NULL) ? 0 : 1;
}

/* Konec c206.c*/
