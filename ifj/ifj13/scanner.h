/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   scanner.h
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#define ALLOC_SIZE_BASE 8
#define RET_OK 0


/////////////////////////////////////
//Struktura, pomocí které se bude vracet token
/////////////////////////////////////



typedef struct{
        char *strg;
        int alok_velikost;
        int konec;
}string;


/////////////////////////////////////
//Výčtový typ pro návratové hodnoty jednotlivých tokenů
/////////////////////////////////////
enum states{
        start = 1,
	php_entry,
///////////////
/////Základ
        identificator,
        kw_if,
	kw_else,
	kw_while,
	kw_function,
	kw_return,
        const_true,
	const_false,
	const_null,
        type_int,
	type_double,
	type_string,
	type_boolean,
	type_null,
        type_undef,
        type_end_par,
        variable,
///////////////
/////Čísla a typ čísla (asi je to jedno, o jaký jde typ -> možno zredukovat)
        int_num = 20,		// int
        double_num,		// doube
        double_num_exp,		// double
///////////////
/////Řetězec a konkatenace
        string_literal = 30,	// "abcd"
        concatenate,		// .
///////////////
/////Logicke operatory
        equal = 40,		// ===
 	not_equal,		// !==
	assign,			// =
 
        less,			// <
        less_equal,		// <=
        greater_equal,		// >=
        greater,		// >
///////////////
/////Komentář
        comment = 50, 		// // or /**/
///////////////
/////Aritmetika
        plus = 60, 		// +
        minus, 			// -
        multiply, 		// *
        divide, 		// /
///////////////
/////Závorky a středníky
        left_braces = 70, 	// {
        right_braces,		// }
        left_parent,  		// (
        right_parent, 		// )
	comma,    		// ,
        semicolon, 		// ;
	php_entry_wrong
};

int init_string(string *s);

int add_string(string *s, char add);

void free_string(string *s);

int del_string(string *s);

int is_keyword(char* string);

int is_const(char* string);

int is_type(char* string);

int get_token(FILE *f, string *str);

#endif

