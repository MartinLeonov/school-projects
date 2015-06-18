/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   inc_func.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "scanner.h"  //vlozi stdiio.h a stdlib.h
#include "inc_func.h"

/* Funkce vracejici delku retezce ktery je ji predan parametrem */
unsigned long strLen(const char inp[])
{
	if(inp == NULL) return SEM_ERR_OTHER;

	int i = 0;

	while(inp[i] != '\0') {
		i++;
	}

	return i;
}

/* Funkce vracejici podretezec retezce predaneho prvnim parametrem.
   Podretezec je urcen indexy, ktere predstavuji druhy a treti parametr
   Pri chybnych parametrech vraci NULL					 */
int get_substring(char **ptr, int start, int end)
{
	if(*ptr == NULL) return SEM_ERR_OTHER;

	int i = start, j = 0;
	char *out;


	if(start<0 || end<0 || start>end) {
		return SEM_ERR_OTHER;
	}
	if((out = malloc(sizeof(char)*(end-start)+1)) == NULL) {
		return RET_ERR;
	}
	if(start>=(int)strLen(*ptr) || end>(int)strLen(*ptr)) {
		free(out);
		return SEM_ERR_OTHER;
	}

	while(i<end) {
		out[j] = (*ptr)[i];
		i++; j++;
	}
	out[j] = '\0';

	free(*ptr);
		*ptr = out;
	return RET_OK;
}

/**
 * Funkce: get_string
 * ------------------
 * Nacte jeden radek ze stdin a vrati jej jako
 * hodnotu typu string. Nacitani je ukonceno znakem
 * konce radku nebo EOF. Pokud je prvnim nactenym
 * znakem EOF, funkce vraci prazdny retezec.
 */
int get_string(char **ptr)
{
	char *string;;
	int stringSize = 0;
	int allocatedSize;
	int ch;
	// alokace
	if ((string = (char *) malloc(STR_ALLOC)) == NULL) {
	return RET_ERR;
	} else {
		allocatedSize = STR_ALLOC;
	}

	while ((ch = getchar()) != EOF) {
		if (ch != '\n') {

			// realokace
			if (stringSize + NULL_TERM >= allocatedSize) {
				if ((string = (char *) realloc(string,
				 stringSize + STR_ALLOC)) == NULL) {
			free(string);
			return RET_ERR;
				} else {
					allocatedSize = stringSize + STR_ALLOC;
				}
			}

			// nacitani retezce
			string[stringSize] = ch;
			stringSize++;

		} else {
			string[stringSize] = '\0';
		*ptr = string;
			return RET_OK;
		}
	}
// zahrnuje i osetreni EOF
	string[stringSize] = '\0';
	*ptr = string;
	return RET_OK;
}

/**
 * Funkce: strval
 * --------------
 * Prevede a vrati hodnotu poslanou v
 * parametru na retezec
 */
int strval(t_stItemV ptr, char **string)
{

	if(ptr == NULL)
	return RET_ERR;
	int stringSize = 1; // kvuli false + koncove nule

	// zjisteni delky cisla jako retezce
	if((ptr)->Content.Type == type_int)
	stringSize = snprintf(NULL, 0, "%d", (ptr)->Content.Data.iData) + 1;
	else if((ptr)->Content.Type == type_double)
		stringSize = snprintf(NULL, 0, "%g", (ptr)->Content.Data.dData) + 1;
	else if((ptr)->Content.Type == type_boolean && (ptr)->Content.Data.bData == true)
		stringSize = 2;
	else if((ptr)->Content.Type == type_string)
		stringSize = strlen((ptr)->Content.Data.sData) + 1;


	// alokace
	if ((*string = (char *) malloc(stringSize)) == NULL) {
		return RET_ERR;
	}
	// nacteni retezce do promenne 'string'
	if((ptr)->Content.Type == type_int){
	sprintf(*string, "%d", (ptr)->Content.Data.iData);
		(*string)[stringSize - 1] = '\0';
	}	
	else if((ptr)->Content.Type == type_double){
		sprintf(*string, "%g", (ptr)->Content.Data.dData);
		(*string)[stringSize - 1] = '\0';
	}
	else if((ptr)->Content.Type == type_boolean) {
		if ((ptr)->Content.Data.bData == true) {
			strcpy(*string, "1\0");
		} else {
			strcpy(*string, "\0");
		}
	}
	else if((ptr)->Content.Type == type_string)
	{
		strcpy(*string, (ptr)->Content.Data.sData);
	}
	else if((ptr)->Content.Type == type_null){
		strcpy(*string, "\0");
	}
	return RET_OK;
}

/**
 * Funkce: boolval
 * ---------------
 * Prevede a vrati hodnotu poslanou v
 * parametru na pravdivostni hodnotu
 * TRUE nebo FALSE                
 */
int boolval(t_stItemV ptr)
{
	int a = 0;
	if (ptr == NULL) {
		return RET_ERR;
	}
	if((ptr)->Content.Type == type_boolean)
		return ptr->Content.Data.bData;
	else if ((ptr)->Content.Type == type_int) {
			if ((ptr)->Content.Data.iData == 0) {
				a = false;
			} else {
				a = true;
			}

	}
	else if ((ptr)->Content.Type == type_double) {
			if ((ptr)->Content.Data.dData == 0.0) {
				a = false;
			} else {
				a = true;
			}

	}
	else if ((ptr)->Content.Type == type_string) {
		
			if (strcmp((ptr)->Content.Data.sData, "") == 0) {
				a = false;
			} else {
				a = true;
			}
		
		}
	else if((ptr)->Content.Type == type_null){
		a = false;
	}
	
	return a;
}

/**
 * Funkce: intval
 * --------------
 * Prevede a vrati hodnotu poslanou v
 * parametru na integer
 */
int intval(t_stItemV ptr)
{
	if (ptr == NULL)
		return RET_ERR;
	if((ptr)->Content.Type == type_int)
		return ptr->Content.Data.iData;
	else if ((ptr)->Content.Type == type_null){
		return 0;
	}
	else if ((ptr)->Content.Type == type_string) {
		int i = 0, ret = 0;
		while((ptr)->Content.Data.sData[i] != '\0' && isspace((ptr)->Content.Data.sData[i])) {
			i++;}
		while(isdigit((ptr)->Content.Data.sData[i])) {
			ret = ret*10 + ((ptr)->Content.Data.sData[i] - '0');i++;
		}
	return ret;
	}
	else if ((ptr)->Content.Type == type_double) {
	return (int)(ptr)->Content.Data.dData;
	}
	else if((ptr)->Content.Type == type_boolean) {
		if ((ptr)->Content.Data.bData == true) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}

/**
 * Funkce: doubleval
 * --------------
 * Prevede a vrati hodnotu poslanou v
 * parametru na double
 */
double doubleval(t_stItemV ptr)
{
	double returnval;
	if(ptr == NULL)
	return (double)RET_ERR;
	if((ptr)->Content.Type == type_double)
		return ptr->Content.Data.dData;
	else if((ptr)->Content.Type == type_null){
	return 0.0;
	}
	else if((ptr)->Content.Type == type_string){	
		int i = 0, exp = 0;
	double over_zero = 0.0, under_zero = 0.0, div = 10.0;
	bool plus = true;

		while((ptr)->Content.Data.sData[i] != '\0' && isspace((ptr)->Content.Data.sData[i]))
			i++;

		while((ptr)->Content.Data.sData[i] != '\0' && isdigit((ptr)->Content.Data.sData[i]))
	{
				over_zero = over_zero*10.0 + ((ptr)->Content.Data.sData[i] - '0');
		i++;
		}

	if((ptr)->Content.Data.sData[i] == '.')
	{
		i++;
		if(!isdigit((ptr)->Content.Data.sData[i]))
			return (double) SEM_ERR_RETYPE;

		else{
			while((ptr)->Content.Data.sData[i] != '\0' && isdigit((ptr)->Content.Data.sData[i]))
			{
				under_zero = under_zero + ((ptr)->Content.Data.sData[i] - '0')/div;
				i++;
				div*=10.0;
			}
		}
	}

	if((ptr)->Content.Data.sData[i] == 'e' || (ptr)->Content.Data.sData[i] == 'E')
	{
		i++;
		if(!isdigit((ptr)->Content.Data.sData[i]) && (ptr)->Content.Data.sData[i] != '+' && (ptr)->Content.Data.sData[i] != '-')
			return (double) SEM_ERR_RETYPE;

		else
		{
			if((ptr)->Content.Data.sData[i] == 'e' || (ptr)->Content.Data.sData[i] == 'E')
				i++;

			if((ptr)->Content.Data.sData[i] == '-')
			{
				plus = false;
				i++;
			}
			else if((ptr)->Content.Data.sData[i] == '+')
				i++;
			
			if(!isdigit((ptr)->Content.Data.sData[i]))
				return (double) SEM_ERR_RETYPE;

			while((ptr)->Content.Data.sData[i] != '\0' && isdigit((ptr)->Content.Data.sData[i]))
			{
				exp = exp*10.0 + ((ptr)->Content.Data.sData[i] - '0');
				i++;
			}
		}
	}


	over_zero+=under_zero;
	for(;exp>0;exp--)
	{
		if(plus == true)
			over_zero*=10;
		else if (plus == false)
			over_zero/=10;
	}

	returnval = over_zero;
	}

	else if((ptr)->Content.Type == type_int){
	returnval = (double)(ptr)->Content.Data.iData;
	}
	else if((ptr)->Content.Type == type_boolean) {
	if ((ptr)->Content.Data.bData == true) {
	    returnval = 1.0;
	} else {
	    returnval = 0.0;
	}
	}

	return returnval;
}
