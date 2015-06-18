/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   ial.h
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#ifndef IAL_H_INCLUDED 
#define IAL_H_INCLUDED 

#include "instlist.h"
#include <stdbool.h>

// Datove struktury pro TS promennych
union t_stDataV
{
	int iData;
	double dData;
	char* sData;
	bool bData;
};

struct stContentV
{
	int Type;
	union t_stDataV Data;
};

typedef struct stItemV
{
	char *Key;
	struct stContentV Content;
	struct stItemV *LPtr;
	struct stItemV *RPtr;
} *t_stItemV;

//Datove struktury pro TS funkci
typedef struct stItemF
{
	char *Key;
	int paramCount;
	tInstList *instListPtr; // Ukazatel na seznam instrukci
	t_stItemV stItemVPtr;	// Ukazatel na lokalni instanci TS promennych
	struct stItemF *LPtr;
	struct stItemF *RPtr;
} *t_stItemF;

//Pomocny zasobnik pro Dispose TS promennych
typedef struct stStackV
{
	t_stItemV Content;
	tInstListItem *returnAddr;
	tInstList *returnList;
	struct stStackV *Prev;
} *t_stStackV;

//Pomocny zasobnik pro Dispose TS funkci
typedef struct stStackF
{
	t_stItemF Content;
	struct stStackF *Prev;
} *t_stStackF;

//Hlavicky funkci
void sInitV(t_stStackV *s);
int sPushV(t_stStackV *s, t_stItemV p, tInstListItem *NextReturnAddr, tInstList *NextReturnList);
void sPopV(t_stStackV *s);
void sDispose(t_stStackV* s);
void stInitV(t_stItemV *ptr);
t_stItemV stSearchV(t_stItemV *ptr, char *Key);
int stInsertV(t_stItemV *ptr, char *Key, int Type, int iData, double dData, char* sData, int bData);
void leftMostV(t_stItemV ptr, t_stStackV *stack);
int stInorderV(t_stItemV tableItem, t_stItemV *tableItemEmpty);
int stDisposeV(t_stItemV *ptr);
void sInitF(t_stStackF *s);
int sPushF(t_stStackF *s, t_stItemF p);
void sPopF(t_stStackF *s);
int stInitF(t_stItemF *ptr, t_stItemV *VTableRootGl);
t_stItemF stSearchF(t_stItemF *ptr, char *Key);
int stInsertF(t_stItemF *ptr, char *Key, int paramCount, tInstList *instListPtr, t_stItemV stItemVPtr);
int stTableFCheckInstListPtr(t_stItemF ptr);
int stDisposeF(t_stItemF *ptr);


int find_string(char *string, char *find);
int sort_string(char **string);
void swap(char *s, int r, int l);
void heap(char *s, int max, int start);

#endif
