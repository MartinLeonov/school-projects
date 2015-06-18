/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   instlist.h
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#ifndef INSTLIST_H_INCLUDED
#define INSTLIST_H_INCLUDED

// typy jednotlivych instrukci
enum InstType {
	// vestavene funkce
	INST_BOOLVAL = 0,     // boolval()
	INST_DOUBVAL,         // doubleval()
	INST_INTVAL,          // intval()
	INST_STRVAL,          // strval()
	INST_GETSTR,          // get_string()
	INST_PUTSTR,          // put_string()
	INST_STRLEN,          // strlen()
	INST_GETSUB,          // get_substring()
	INST_FINDSTR,         // find_string()
	INST_SORTSTR,         // sort_string()
	// vyrazy
	INST_NONTERM,         // prirazeni - nonterminalni polozky
	INST_NOP,             // prirazeni - znamenko (prazdna instrukce)
	// skoky
	INST_JUMP,            // jump
	INST_DJUMP,           // Priamy skok na addr1 bez kontroly
	INST_LABEL,           // navesti (prazdna instrukce)
	INST_CALL,            // volani funkci
	INST_LOAD,            // nacteni hodnoty do promenne
	INST_RETURN,
	INST_LTOGLOAD,        // presun z lokalnej tabulky do globalnej
	                      // ciel reprezentovany priamym ukazatelom

	INST_GTOLLOAD,        // presun z Globalnej tabulky do lokalnej
	                      // Zdroj reprezentovany priamym ukazatelom

	// operace s retezci
	INST_CONCAT = 31,     // .
	// logicke operace
	INST_EQUAL = 40,      // ===
	INST_NEQUAL,          // !=
	INST_ASSIGN,          // =
	INST_LESSER,          // <
	INST_LESSERQ,         // <=
	INST_GREATERQ,        // >=
	INST_GREATER,         // >
	// aritmeticke operace
	INST_PLUS = 60,       // +
	INST_MINUS,           // -
	INST_MULTIP,          // *
	INST_DIVIDE           // /
};


// instrukce
typedef struct {
	int type;
	void *adr1;
	void *adr2;
	void *result;
} tInst;

// polozka v seznamu instrukci
typedef struct InstListItem{
	tInst instruction;
	struct InstListItem *prevInst;
	struct InstListItem *nextInst;
} tInstListItem;

// seznam instrukci
typedef struct {
	tInstListItem *First;
	tInstListItem *Active;
	tInstListItem *Last;
} tInstList;

// prototypy funkci
void listInit(tInstList *);
int  listInsertInst(tInstList *, tInst);
tInstListItem* listGetActive(tInstList *);
void listSetActiveOn(tInstList *List,tInstListItem *addr1);
void listNextInst(tInstList *);
void listPrevInst(tInstList *);
void listFirstActive(tInstList *);
void listDestroy(tInstList *);

#endif

