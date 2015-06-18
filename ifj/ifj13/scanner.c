/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   scanner.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include "parser.h"
#include "scanner.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

bool first = true;
/////////////////////////////////////
//Inicializace struktury
/////////////////////////////////////
int init_string(string *s){
	if((s->strg = (char*) malloc(sizeof(ALLOC_SIZE_BASE*sizeof(char)))) == NULL)
		return RET_ERR;
	s->konec = 0;
	s->strg[s->konec] = '\0';
	s->alok_velikost = ALLOC_SIZE_BASE;

	return RET_OK;
}


/////////////////////////////////////
//Přidání znaku do řetězce, v případě překročení vyhrazené paměti realokace paměti
/////////////////////////////////////
int add_string(string *s, char add){	
	if(s->konec+1 >= s->alok_velikost){	
		s->alok_velikost = s->alok_velikost+ALLOC_SIZE_BASE;
		if((s->strg = (char*) realloc(s->strg, s->alok_velikost*sizeof(char)))== NULL)
			return RET_ERR;
	}
	s->strg[s->konec] = add;
	s->konec++;
	s->strg[s->konec] = '\0';

	return RET_OK;
}


/////////////////////////////////////
//Uvolnění struktury
/////////////////////////////////////
void free_string(string *s){
	free(s->strg);
}


/////////////////////////////////////
//Mazání obsahu řetězce
/////////////////////////////////////
int del_string(string *s){
	s->alok_velikost = ALLOC_SIZE_BASE;
	if((s->strg = (char*) realloc(s->strg, s->alok_velikost*sizeof(char)))==NULL)
		return RET_ERR;
	s->konec = 0;
	s->strg[s->konec] = '\0';
	return RET_OK;
}


/////////////////////////////////////
//Funkce vracející, zda jde o klíčové slovo !!
/////////////////////////////////////
int is_keyword(char* string){
	if((strcmp("if",string)) == 0)
		return kw_if;
	else if((strcmp("else",string)) == 0)
		return kw_else;
	else if((strcmp("while",string)) == 0)
		return kw_while;
	else if((strcmp("function",string)) == 0)
		return kw_function;
	else if((strcmp("return",string)) == 0)
		return kw_return;
	else
		return RET_ERR;

//if else while function return - keyword
}

/////////////////////////////////////
//Funkce vracející, zda jde o konstantu !!
/////////////////////////////////////
int is_const(char* string){
	if((strcmp("true",string)) == 0)
		return const_true;
	else if((strcmp("false",string)) == 0)
		return const_false;
	else if((strcmp("null",string)) == 0)
		return const_null;
	else
		return RET_ERR;

//true false null - konstanta
}

/////////////////////////////////////
//Funkce vracející, zda jde o datový typ !!
/////////////////////////////////////
int is_type(char* string){
	if((strcmp("integer",string))==0)
		return type_int;
	else if((strcmp("double",string)) == 0)
		return type_double;
	else if((strcmp("string",string)) == 0)
		return type_string;
	else if((strcmp("boolean",string)) == 0)
		return type_boolean;
	else
		return RET_ERR;
//integer,double, string, boolean - datový typ
}

/////////////////////////////////////
// Funkce get_token
/////////////////////////////////////
int get_token(FILE *f, string *str){
	int c;
	int ret;
	int state = start;
	char hex[] = "00";
	bool must_be_next = false;
	if(del_string(str) == RET_ERR)
		return RET_ERR;

	while((c=fgetc(f)) != EOF){
		switch(state){
			case start:
				////// identifikátor
				if(isalpha(c) || c == '_'){
					state = identificator;
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				////// Proměnná
				else if(c == '$'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					state = variable;
					must_be_next = true;
				}
				////// Řetězcový literál
				else if(c == '"'){
					state = string_literal;
				}
				////// číslo
				else if(isdigit(c)){
					state = int_num;
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				////////////////////////
				////// Rovno, nerovno
				////// jedno nebo tři rovnítka
				else if(c == '='){
					state = equal;
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				////// nerovno
				else if(c == '!'){
					state = not_equal;
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				////// konkatenace - další znaky netřeba
				else if(c == '.'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return concatenate;
				}
				//////////////////
				////// Logické
				////// menší || menší nebo rovno
				else if(c == '<'){
					state = less;
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				////// větší || větší nebo rovno
				else if(c == '>'){
					state = greater;
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				//////////////////
				////// Aritmetické 
				////// pouze znak plus - další znaky netřeba
				else if(c == '+'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return plus;
				}
				////// pouze znak plus - další znaky netřeba
				else if(c == '*'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return multiply;
				}
				////// pouze znak mínus - další znaky netřeba
				else if(c == '-'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return minus;
				}
				////// znak děleno || začátek komentáře
				else if(c == '/'){
					if(first)
						return SYNT_ERR;
					else{
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						state = comment;
					}
				}
				//////////////////
				////// Závorky
				////// pouze znak ( - další znaky netřeba
				else if(c == '('){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return left_parent;
				}
				////// pouze znak ) - další znaky netřeba
				else if(c == ')'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return right_parent;
				}
				////// pouze znak { - další znaky netřeba
				else if(c == '{'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return left_braces;
				}
				////// pouze znak } - další znaky netřeba
				else if(c == '}'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return right_braces;
				}
				////// znak děleno || začátek komentáře
				else if(c == ','){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return comma;
				}
				////// pouze středník - další znaky netřeba
				else if(c == ';'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return semicolon;
				}
				else if(isspace(c)==0){	
					return LEX_ERR;}
				else{
					if(first)
						return SYNT_ERR;
				}
				break;

			//////////////////
			////// Identifikátor
			case identificator:
				if(isalpha(c) || isdigit(c) || c == '_'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				else{
					ungetc(c,f);
					int id = 0;
					if((id = is_keyword(str->strg))!=RET_ERR){
						return id;
					}
					else if((id = is_type(str->strg))!=RET_ERR){
						return id;
					}
					else if((id = is_const(str->strg))!=RET_ERR){
						return id;
					}
					else{
						return identificator;
					}
				}
				break;

			//////////////////
			////// Proměnná
			case variable:	
				if(must_be_next){
					if(isalpha(c) || c == '_'){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						must_be_next = false;
					}
					else 
						return LEX_ERR;
				}
				else{
					if(isalpha(c) || isdigit(c) || c == '_'){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					}
					else{
						ungetc(c,f);
						return variable;
					}	
				}			
				break;

			//////////////////
			////// Řetězcový literál
			case string_literal:
				if(c > 31){
					if(c == '\\'){
						c=fgetc(f);
						if(c == '"'){
							if((ret = add_string(str,'\"')) == RET_ERR) return RET_ERR;
						}
						else if(c == 'n'){
							if((ret = add_string(str,'\n')) == RET_ERR) return RET_ERR;
						}
						else if(c == '$'){
							if((ret = add_string(str,'$')) == RET_ERR) return RET_ERR;
						}
						else if(c == 't'){
							if((ret = add_string(str,'\t')) == RET_ERR) return RET_ERR;
						}
						else if(c == '\\'){
							if((ret = add_string(str,'\\')) == RET_ERR) return RET_ERR;
						}
						else if(c == 'x'){
							hex[0]=fgetc(f); 
							if(hex[0] == EOF)
								return LEX_ERR;
							else if(hex[0] == '\"'){
								if((ret = add_string(str,'\\')) == RET_ERR) return RET_ERR;
								if((ret = add_string(str,'x')) == RET_ERR) return RET_ERR;
								return string_literal;
							}
							else if(hex[0] > 31){
								if(((hex[0]>'f') || (hex[0]<'a')) && ((hex[0]>'F') || (hex[0]<'A')) && ((hex[0]>'9') || (hex[0]<'0'))){
									if((ret = add_string(str,'\\')) == RET_ERR) return RET_ERR;
									if((ret = add_string(str,'x')) == RET_ERR) return RET_ERR;
									ungetc(hex[0],f);
								}
								else{
									hex[1]=fgetc(f); 
									if(hex[1] == EOF)
										return LEX_ERR;
									else if(hex[1] == '\"'){
										if((ret = add_string(str,'\\')) == RET_ERR) return RET_ERR;
										if((ret = add_string(str,'x')) == RET_ERR) return RET_ERR;
										if((ret = add_string(str,hex[0])) == RET_ERR) return RET_ERR;
										return string_literal;
									}
									else if(hex[1] > 31){
										if(((hex[1]>'f') || (hex[1]<'a')) && ((hex[1]>'F') || (hex[1]<'A')) && ((hex[1]>'9') || (hex[1]<'0'))){
											if((ret = add_string(str,'\\')) == RET_ERR) return RET_ERR;
											if((ret = add_string(str,'x')) == RET_ERR) return RET_ERR; 
											ungetc(hex[0],f);ungetc(hex[1],f);

										}
										else{
											char* end = NULL;
											int pomoc = strtol(hex,&end,16); 
											if(pomoc!=0 && *end == '\0'){
												if((ret = add_string(str,pomoc)) == RET_ERR) return RET_ERR;}

										}
									}
									else{
										return LEX_ERR;
									}
								}
							}
							else{
								return LEX_ERR;
							}
						}
						else if(c>31){ ungetc(c,f); c='\\';
							if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						}
						else
							return LEX_ERR;
					}
					else if(c == '"'){
						return string_literal;
					}
					else if(c == '$'){ 
						return LEX_ERR;
					}
					else{
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					}
				}
				else
					return LEX_ERR;
				break;

			//////////////////
			////// Číslo
			case int_num:
				if(isdigit(c)){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				}
				else if(c=='.'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					state = double_num;
					must_be_next = true;
				}
				else if(c=='e'||c=='E'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					must_be_next = true;
					state = double_num_exp;
					c=fgetc(f);
					if(c=='-' || c=='+'){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;}
					else
						ungetc(c,f);
				}
				else{
					ungetc(c,f);
					return int_num;
				}
				break;
			////// desetinné číslo
			case double_num:
				if(must_be_next){
					if(isdigit(c)){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						must_be_next = false;
					}
					else{
						return LEX_ERR;
					}
				}
				else{
					if(isdigit(c)){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						must_be_next = false;
					}
					else if(c=='e'||c=='E'){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						state = double_num_exp;
						c=fgetc(f);
						must_be_next = true;
						if(c=='-' || c=='+'){
							if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;}
						else
							ungetc(c,f);
					}
					else{
						ungetc(c,f);
						return double_num;
					}
				}
				break;
			////// desetinné číslo
			case double_num_exp:
				if(must_be_next){
					if(isdigit(c)){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						must_be_next = false;
					}
					else{
						return LEX_ERR;
					}
				}
				else{
					if(isdigit(c)){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					}
					else{
						ungetc(c,f);
						return double_num;
					}
				}
				break;
			
			//////////////////
			////// Rovno
			case equal:
				if(c == '='){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					c=fgetc(f);
					if(c == '='){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						return equal;
					}
					else{
						ungetc(c,f);
						return LEX_ERR;
					}
				}
				else{
					ungetc(c,f);
					return assign;
				}
				break;

			//////////////////
			////// Nerovno
			case not_equal:
				if(c == '='){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					c=fgetc(f);
					if(c == '='){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						return not_equal;
					}
					else{
						ungetc(c,f);
						return LEX_ERR;
					}
				}
				else{
					ungetc(c,f);
					return LEX_ERR;
				}
				break;

			//////////////////
			////// Větší či většíRovno
			case greater:

				if(c == '='){
				if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return greater_equal;
				}
				else{
					ungetc(c,f);
					return greater;
				}
				break;

			//////////////////
			////// Menší či menšíRovno
			case less:
				if(c == '='){
				if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					return less_equal;
				}
				else if(c == '?'){
				if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
				state=php_entry;
				}
				else{
					ungetc(c,f);
					return less;
				}
				break;

			case php_entry:
				if(c == 'p'){
					if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
					c=fgetc(f);
					if(c == 'h'){
						if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
						c=fgetc(f);
						if(c == 'p'){
							if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
							c=fgetc(f);
							if(c==EOF){
								return php_entry_wrong;}
							else  if(isspace(c)){
								c=fgetc(f);
								if(c==EOF){
									return php_entry_wrong;}
								ungetc(c,f);
								if((ret = add_string(str,c)) == RET_ERR) return RET_ERR;
								first = false;
								return php_entry;
							}

							else{
								first = false;
								return php_entry;
							}
						}
						else{
							return LEX_ERR;
						}
					}
					else{
						return LEX_ERR;
					}
				}
				else{
					ungetc(c,f);
					return LEX_ERR;
				}
				break;
			//////////////////
			////// Komentáře
			case comment:
				if(c == '/'){
					while((c=fgetc(f)) != EOF){
						if(c=='\n')
							break;
					}
					del_string(str);
					state = start;
				}
				else if(c=='*'){
					while((c=fgetc(f)) != EOF){
						if(c=='*'){
							if((c=fgetc(f)) == '/'){
								state = start;
								del_string(str);
								break;
							}
							else
								ungetc(c,f);
						}
					}
					if(c==EOF && state==comment)
						return LEX_ERR;
					
				}
				else{
					ungetc(c,f);
					return divide;
				}
				break;
		}
	}
	return EOF;
}
