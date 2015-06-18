/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   interpret.h
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#ifndef INTERPRET_H_INCLUDED
#define INTERPRET_H_INCLUDED

#include "ial.h"
#include "scanner.h"
#include "inc_func.h"


int interpret(t_stItemV GlobSymTable, tInstList *List);
char *stringConcatenate(char *, char *);

#endif
