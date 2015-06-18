/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   main.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include "scanner.h"
#include "inc_func.h"
#include "interpret.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define FILE_ERR 99

int main(int argc, char *argv[]){

tInstList GlobalInstList;
listInit(&GlobalInstList); // inicializace seznamu

t_stItemV VTableRoot;
stInitV(&VTableRoot);

t_stItemF FTableRoot;
if(stInitF(&FTableRoot,&VTableRoot) != RET_OK) {
	stDisposeF(&FTableRoot);
	stDisposeV(&VTableRoot);
	listDestroy(&GlobalInstList);
	return RET_ERR;
}

	if(argc!=2){
		fprintf(stderr,"Neplatny pocet parametru\n");
		stDisposeF(&FTableRoot);
		stDisposeV(&VTableRoot);
		listDestroy(&GlobalInstList);
		return FILE_ERR;
	}
	else{
		if((f = fopen (argv[1], "r"))==NULL){
			fprintf(stderr,"Nepodarilo se otevrit soubor\n");
			stDisposeF(&FTableRoot);
			stDisposeV(&VTableRoot);
			listDestroy(&GlobalInstList);
			return FILE_ERR;
		}
	}

int result;

result = parse(&VTableRoot,&FTableRoot,&GlobalInstList);
if (result == SYNT_OK)
	result = interpret(VTableRoot,&GlobalInstList);


listDestroy(&GlobalInstList);
stDisposeF(&FTableRoot);
stDisposeV(&VTableRoot);
fclose(f);

switch (result)
{
case SYNT_OK:

return 0;
break;
case SEM_ERR_MISS_PAR:
fprintf(stderr,"Semanticka Chyba, chybajuce parametre\n");
return 4;

case SEM_ERR_UNDEF:
fprintf(stderr,"Semanticka Chyba, nedefinovana premenna\n");
return 5;

case SEM_ERR_NULL_DIV:
fprintf(stderr,"Semanticka Chyba, Delenie nulou\n");
return 10;

case SEM_ERR_RETYPE:
fprintf(stderr,"Semanticka Chyba, Chyba pretypovania\n");
return 11;

case SEM_ERR_ARITH:
fprintf(stderr,"Semanticka Chyba, Chyba v aritmetickej operacii\n");
return 12;

case SEM_ERR_OTHER:
fprintf(stderr,"Semanticka Chyba, Ina chyba\n");
return 13;


case SYNT_ERR:
fprintf(stderr,"Syntakticka chyba!!!\n");
return 2;
break;
case LEX_ERR:
fprintf(stderr,"Lexikalna Chyba!!!\n");
return 1;
break;
case RET_ERR:
fprintf(stderr,"Vnutorna chyba prekladaca!!!\n");
return 99;
case SEM_ERR:
fprintf(stderr,"Semanticka chyba v programe\n");
return 3;
}

	return 0;
}
