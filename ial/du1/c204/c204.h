
/* ******************************* c204.h *********************************** */
/*  P�edm�t: Algoritmy (IAL) - FIT VUT v Brn�                                 */
/*  �kol: c204 - P�evod infixov�ho v�razu na postfixov� (s vyu�it�m c202)     */
/*  Referen�n� implementace: Petr P�ikryl, listopad 1994                      */
/*  P�epis do jazyka C: Luk� Mar��k, prosinec 2012                           */
/*  Upravil: Bohuslav K�ena, ��jen 2013                                       */
/* ************************************************************************** */

/* TENTO SOUBOR, PROS�ME, NEUPRAVUJTE! */

#ifndef _INFIX2POSTFIX_H_
#define _INFIX2POSTFIX_H_

#include <stdio.h>
#include <stdlib.h>

#include "c202.h"                          /* vyu��v� p��klad c202 - z�sobn�k */

#define MAX_LEN 64                          /* maxim�ln� d�lka �et�zce v�razu */

extern int solved;                      /* Indikuje, zda byla operace �e�ena. */

/* Konverzn� funkce */
char* infix2postfix (const char* infExpr);

#endif

/* Konec hlavi�kov�ho souboru c204.h */
