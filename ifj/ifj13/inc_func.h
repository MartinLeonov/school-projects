/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   inc_func.h
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#ifndef INC_FUNC_H_INCLUDED
#define INC_FUNC_H_INCLUDED

#include "parser.h"

// Definice  pro funkci get_string
#define STR_ALLOC 8
#define NULL_TERM 1

unsigned long strLen(const char inp[]);
int get_substring(char **ptr, int start, int end);
int get_string(char **ptr);
int strval(t_stItemV ptr, char **string);
int boolval(t_stItemV ptr);
int intval(t_stItemV ptr);
double doubleval(t_stItemV ptr);
int put_string(int count, char *format, ...);

#endif

