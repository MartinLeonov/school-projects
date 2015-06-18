/**
	* Predmet:  IFJ / IAL
	* Projekt:  Implementace interpretu imperativniho jazyka IFJ13
	* Tym:      099
	* Varianta: a/2/I
	* Soubor:   expr_list.h
	* Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
	*           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
	*           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
	*           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
	*           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
	*/

#ifndef EXPR_LIST_H_INCLUDED
#define EXPR_LIST_H_INCLUDED

#include "ial.h"

#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>


#define FALSE 0
#define TRUE 1

extern int errflg;
extern int solved;

typedef struct tDLElem {                /* prvek dvouosměrně vázaného seznamu */ 
	tInst data;                                                /* užitečná data */
	struct tDLElem *lptr;                /* ukazatel na předchozí prvek seznamu */
	struct tDLElem *rptr;              /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
	tDLElemPtr First;                        /* ukazatel na první prvek seznamu */
	tDLElemPtr Act;                       /* ukazatel na aktuální prvek seznamu */
	tDLElemPtr Last;                      /* ukazatel na poslední prvek seznamu */
} tDLList;

/* prototypy jednotlivých funkcí */
void DLInitList (tDLList *);
void DLDisposeList (tDLList *);
void DLInsertFirst (tDLList *, int, void *, void *, void *);
void DLInsertLast(tDLList *, int, void *, void *, void *);
void DLFirst (tDLList *);
void DLLast (tDLList *);
void DLCopyFirst (tDLList *, tInst *);
void DLCopyLast (tDLList *, tInst *);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostDelete (tDLList *);
void DLPreDelete (tDLList *);
void DLPostInsert (tDLList *, int, void *, void *, void *);
void DLPreInsert (tDLList *, int, void *, void *, void *);
void DLCopy (tDLList *, tInst *);
void DLActualize (tDLList *, int);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);

#endif

