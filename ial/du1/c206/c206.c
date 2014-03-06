	
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                             �pravy: Bohuslav K�ena, ��jen 2013
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu, 
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek, 
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku, 
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu, 
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem, 
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep�e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu, 
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam 
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/	
    printf ("*ERROR* This program has performed an illegal operation.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/

	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L)	{
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free. 
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
** Vlo�� nov� prvek na za��tek seznamu L.
L->Last = NewLast;
L->Last = NewLast;
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
	
	// nastaveni prvniho prvku seznamu jako aktivni
	L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
	
	// nastaveni posledniho prvku seznamu jako aktivni
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Prost�ednictv�m parametru val vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
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
** Prost�ednictv�m parametru val vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
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
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita 
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
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
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
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
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
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
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
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
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Prost�ednictv�m parametru val vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
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
** P�ep�e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/

	if (L->Act != NULL) {
		L->Act->data = val;
	}
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
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
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
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
** Je-li seznam aktivn�, vrac� true. V opa�n�m p��pad� vrac� false.
** Funkci implementujte jako jedin� p��kaz.
**/

	return (L->Act == NULL) ? 0 : 1;
}

/* Konec c206.c*/
