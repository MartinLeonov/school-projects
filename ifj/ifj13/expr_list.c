/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   expr_list.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include "instlist.h"
#include "expr_list.h"

int solved;
int errflg;

void DLError()
{
		printf ("*ERROR* This program has performed an illegal operation.\n");
		errflg = TRUE;
		exit(0);
		return;
}

void DLInitList (tDLList *L)
{ // Inicializace seznamu
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L)
{ // Zruseni vsech prvku seznamu
	if((L->Act = L->First) == NULL) return;
	while(L->Act != L->Last)
	{
		L->Act = L->Act->rptr;
		free(L->Act->lptr);
	}
	free(L->Act);
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val, void *op1, void *op2, void *result)
{ // Vlozeni prvku na zacatek seznamu
	tDLElemPtr element;
	if((element = (tDLElemPtr) malloc(sizeof(struct tDLElem))) == NULL)
	{
		DLError();
		return;
	}
	element->data.type = val;
	element->data.adr1 = op1;
	element->data.adr2 = op2;
	element->data.result = result;
	element->lptr = NULL;
	if(L->First == NULL)
	{
		element->rptr = NULL;
		L->Last = element;
	}
	else
	{
		element->rptr = L->First;
		element->rptr->lptr = element;
	}
	L->First = element;
	return;
}

void DLInsertLast(tDLList *L, int val, void *op1, void *op2, void *result)
{
	tDLElemPtr element;
	if((element = (tDLElemPtr) malloc(sizeof(struct tDLElem))) == NULL)
	{
		DLError();
		return;
	}
	element->data.type = val;
	element->data.adr1 = op1;
	element->data.adr2 = op2;
	element->data.result = result;
	element->rptr = NULL;
	if(L->First == NULL)
	{
		element->lptr = NULL;
		L->First = element;
	}
	else
	{
		element->lptr = L->Last;
		element->lptr->rptr = element;
	}
	L->Last = element;
	return;
}

void DLFirst (tDLList *L)
{
	L->Act = L->First;
	return;
}

void DLLast (tDLList *L)
{
	L->Act = L->Last;
	return;
}

void DLCopyFirst (tDLList *L, tInst *val)
{
	if(L->First == NULL)
	{
		DLError();
		return;
	}
	*val = L->First->data;
	return;
}

void DLCopyLast (tDLList *L, tInst *val)
{
	if(L->First == NULL)
	{
		DLError();
		return;
	}
	*val = L->Last->data;
	return;
}

void DLDeleteFirst (tDLList *L)
{
	if(L->First != NULL)
	{
		if(L->Act == L->First)
			L->Act = NULL;
		if(L->First == L->Last)
		{
			free(L->First);
			L->First = L->Act = L->Last = NULL;
		}
		else
		{
			L->First = L->First->rptr;
			free(L->First->lptr);
			L->First->lptr = NULL;
		}
	}
	return;
}

void DLDeleteLast (tDLList *L)
{ // Smaze posledni prvek
	if(L->First != NULL)
	{
		if(L->Act == L->Last)
			L->Act = NULL;
		if(L->First == L->Last)
		{
			free(L->Last);
			L->First = L->Act = L->Last = NULL;
		}
		else
		{
			L->Last = L->Last->lptr;
			free(L->Last->rptr);
			L->Last->rptr = NULL;
		}
	}
}

void DLPostDelete (tDLList *L)
{ // Smaze prvek za aktivnim prvkem
	if(L->Act != NULL && L->Act != L->Last)
	{
		if((L->Act->rptr) != L->Last)
		{
			L->Act->rptr = L->Act->rptr->rptr;
			free(L->Act->rptr->lptr);
			L->Act->rptr->lptr = L->Act;
		}
		else
		{
			free(L->Act->rptr);
			L->Act->rptr = NULL;
			L->Last = L->Act;
		}
	}
	return;
}

void DLPreDelete (tDLList *L)
{ // Smaze prvek pred aktivnim prvkem
	if(L->Act != NULL && L->Act != L->First)
	{
		if((L->Act->lptr) != L->First)
		{
			L->Act->lptr = L->Act->lptr->lptr;
			free(L->Act->lptr->rptr);
			L->Act->lptr->rptr = L->Act;
		}
		else
		{
			free(L->Act->lptr);
			L->Act->lptr = NULL;
			L->First = L->Act;
		}
	}
	return;
}

void DLPostInsert (tDLList *L, int val, void *op1, void *op2, void *result)
{ // Vlozi prvek za aktivni prvek
	if(L->Act != NULL)
	{
		tDLElemPtr element;
		if((element = (tDLElemPtr) malloc(sizeof(struct tDLElem))) == NULL)
		{
			DLError();
			return;
		}
		element->data.type = val;
		element->data.adr1 = op1;
		element->data.adr2 = op2;
		element->data.result = result;
		element->lptr = L->Act;
		element->rptr = L->Act->rptr;
		if(L->Act->rptr != NULL)
			L->Act->rptr = L->Act->rptr->lptr = element;
		else
			L->Act->rptr = L->Last = element;
	}

}

void DLPreInsert (tDLList *L, int val, void *op1, void *op2, void *result)
{ // Vlozi prvek pred aktivni prvek
	if(L->Act != NULL)
	{
		tDLElemPtr element;
		if((element = (tDLElemPtr) malloc(sizeof(struct tDLElem))) == NULL)
		{
			DLError();
			return;
		}
		element->data.type = val;
		element->data.adr1 = op1;
		element->data.adr2 = op2;
		element->data.result = result;
		element->rptr = L->Act;
		element->lptr = L->Act->lptr;
		if(L->Act->lptr != NULL)
			L->Act->lptr = L->Act->lptr->rptr = element;
		else
			L->Act->lptr = L->First = element;
	}
}

void DLCopy (tDLList *L, tInst *val)
{ // Vraci hodnotu aktivniho prvku
	if(L->Act == NULL)
	{
		DLError();
		return;
	}
	*val = L->Act->data;
	return;
}

void DLActualize (tDLList *L, int val)
{  // Prepise obsah aktivniho prvku
	if(L->Act != NULL)
	{
		L->Act->data.type = val;
	}
	return;
}

void DLSucc (tDLList *L)
{ // Posune aktivitu na dalsi prvek
	if(L->Act != NULL)
		L->Act = L->Act->rptr;
	return;
}

void DLPred (tDLList *L)
{ // Posune aktivitu na predchozi prvek
	if(L->Act != NULL)
		L->Act = L->Act->lptr;
	return;
}

int DLActive (tDLList *L)
{ // Zjisti aktivitu seznamu
	return (L->Act != NULL);
}

