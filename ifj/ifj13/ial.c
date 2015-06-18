/**
 * Predmet:  IFJ / IAL
 * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
 * Tym:      099
 * Varianta: a/2/I
 * Soubor:   ial.c
 * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
 *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
 *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
 *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
 *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
 */

 #include <string.h>
 #include <limits.h>
 #include <stdarg.h>
 #include <ctype.h>

 #include "scanner.h"
 #include "parser.h"
 #include "inc_func.h"
 #include "ial.h"

	// Funkce pro praci s pomocnym zasobnikem pri operaci Dispose nad TS promennych
	//-------------------------------
	void sInitV(t_stStackV *s)
	{
		*s = NULL;
	}

	int sPushV(t_stStackV *s, t_stItemV p, tInstListItem *NextReturnAddr, tInstList *NextReturnList)
	{
		t_stStackV new;
		if((new = malloc(sizeof(struct stStackV))) == NULL) return RET_ERR;

		// Vkladani prvniho prvku
		if(*s == NULL) {
			*s = new;
			(*s)->Content = p;
			(*s)->returnAddr = NextReturnAddr;
			(*s)->returnList = NextReturnList;
			(*s)->Prev = NULL;
		}
		else {
			// Vkladani dalsich prvku
			new->Content = p;
			new->Prev = *s;
			new->returnAddr = NextReturnAddr;
			new->returnList = NextReturnList;
			*s = new;
		}
		return RET_OK;
	}

	void sPopV(t_stStackV *s)
	{
		// Odstranuje z vrcholu jen pokud zasobnik neni prazdny
		if(*s != NULL) {
			t_stStackV del = *s;
			*s = (*s)->Prev;
			free(del);
			del = NULL;
		}
	}

	/* Funkce pro odstraneni celeho zasobniku pouzivana,
	   pokud doslo k chyba a je treba uvolnit veskerou pamet */
	void sDispose(t_stStackV* s)
	{
		while(*s != NULL)
		{
			sPopV(s);
		}
	}

	//Funkce pro praci s TS promennych
	//-------------------------------
	void stInitV(t_stItemV *ptr)
	{
		*ptr = NULL;
	}

	// Projde binarni strom a vrati ukazatel na hledany prvek
	t_stItemV stSearchV(t_stItemV *ptr, char *Key)
	{
		t_stItemV p = *ptr;

		while(p != NULL)
		{
			if(!strcmp(p->Key, Key))
				return p;
			else if(strcmp(p->Key, Key)>0)
				p = p->LPtr;
			else
				p = p->RPtr;
		}
		return NULL;
	}

	/* Projde binarni strom a podle toho zda nasel hodnotu se stejnym klicem
	   budto updatuje jeji polozky nebo prida novou */
	int stInsertV(t_stItemV *ptr, char *Key, int Type, int iData, double dData, char* sData, int bData)
	{
		t_stItemV p = *ptr, p2;

		while(p != NULL)
		{
			if(strcmp(p->Key, Key) == 0) {
				// Nuluje vsechna pripadna predesla data
				if((p->Content.Data.sData!=NULL) && (p->Content.Type == type_string)){
					// Pokud byla uchovavana promena typu string, je treba jej dealokovat
					free(p->Content.Data.sData);
					p->Content.Data.sData=NULL;
				}
				p->Content.Data.iData = 0;
				p->Content.Data.dData = 0;
				p->Content.Data.sData = NULL;
				p->Content.Data.bData = false;
				p->Content.Type = Type;
				// Podle datoveho typu nastavi nova
				switch(Type) {
					case type_int: p->Content.Data.iData = iData; break;
					case type_double: p->Content.Data.dData = dData; break;
					case type_string: 
							if((p->Content.Data.sData = malloc(sizeof(char)*(strlen(sData)+1))) == NULL) {
								stDisposeV(ptr);
								return RET_ERR;
							}
							strcpy(p->Content.Data.sData,sData); break;
					case type_boolean: p->Content.Data.bData = bData; break;
					default: break;
				}
				return RET_OK;
			}
			else if(strcmp(p->Key, Key)>0) {
				p2 = p;
				p = p->LPtr;
			}
			else {
				p2 = p;
				p = p->RPtr;
			}
		}

		if((p = malloc(sizeof(struct stItemV))) == NULL) {
			stDisposeV(ptr);
			return RET_ERR;
		}
		// Alokace pameti pro retezec predstavujici jmeno promenne
		if((p->Key = malloc(sizeof(char)*(strlen(Key)+1))) == NULL) {
			stDisposeV(ptr);
			return RET_ERR;
		}
		strcpy(p->Key, Key);
		// Nastaveni vsech dat na nulove hodnoty
		p->Content.Data.iData = 0;
		p->Content.Data.dData = 0;
		p->Content.Data.sData = NULL;
		p->Content.Data.bData = false;
		p->Content.Type = Type;
		// Podle datoveho typu naplni prislusnou promennou v unionu
		switch(Type) {
			case type_int: p->Content.Data.iData = iData; break;
			case type_double: p->Content.Data.dData = dData; break;
			case type_string: 
					if((p->Content.Data.sData = malloc(sizeof(char)*(strlen(sData)+1))) == NULL) {
						stDisposeV(ptr); // OSETRENI DEALOKACE PRI CHYBE
						return RET_ERR;
					}
					strcpy(p->Content.Data.sData,sData); break;
			case type_boolean: p->Content.Data.bData = bData; break;
			default: break;
		}

		p->LPtr = NULL;
		p->RPtr = NULL;

		// Podminka pokud slo o prnvi prvek seznamu
		if(*ptr == NULL)
			*ptr = p;
		else if(strcmp(p2->Key, Key)>0)
			p2->LPtr = p;
		else
			p2->RPtr = p;

		return RET_OK;
	}

	// Pomocna funkce prochazenici binarni strom k nejlevejsimu prvku
	void leftMostV(t_stItemV ptr, t_stStackV *stack)
	{
		while(ptr != NULL) {
			sPushV(stack, ptr, NULL, NULL);
			ptr = ptr->LPtr;
		}
	}

	// Funkce okopiruje obsah z naplnene tabulky do prazdne
	int stInorderV(t_stItemV tableItem, t_stItemV *tableItemEmpty)
	{
		if(tableItem != NULL) {
			t_stStackV stack;
			sInitV(&stack);

			leftMostV(tableItem, &stack);

			while(stack != NULL) {
				tableItem = stack->Content;
				sPopV(&stack);

				if (stInsertV(tableItemEmpty, tableItem->Key, tableItem->Content.Type,
					tableItem->Content.Data.iData, tableItem->Content.Data.dData,
					tableItem->Content.Data.sData, tableItem->Content.Data.bData) == RET_ERR) {
					stDisposeV(&tableItem); // OSETRENI DEALOKACE PRI CHYBE
						return RET_ERR; // chyba alokace
				}

				leftMostV(tableItem->RPtr, &stack);
			}
		}
		return RET_OK;
	}

	// Odstrani cely binarni strom
	int stDisposeV(t_stItemV *ptr)
	{
		t_stStackV stack;
		sInitV(&stack);
		t_stItemV p = *ptr, p2;

		while(*ptr != NULL)
		{
			if(p->RPtr != NULL) {
				if(sPushV(&stack, p, NULL, NULL) == RET_ERR) return RET_ERR;
				p = p->RPtr;
			}
			else if(p->LPtr != NULL) {
				if(sPushV(&stack, p, NULL, NULL) == RET_ERR) return RET_ERR;
				p = p->LPtr;
			}
			else {
				if(stack != NULL) {
					p2 = p;
					p = stack->Content;
					sPopV(&stack);

					// Nastavi spravny ukazatel predesleho prvku na null
					if(p->RPtr == p2)
						p->RPtr = NULL;
					else
						p->LPtr = NULL;

					// Je treba uvolnit i pamet naalokovanou pro retezce
					free(p2->Key);
					if(p2->Content.Type == type_string)
						free(p2->Content.Data.sData);
					free(p2);
					p2 = NULL;
				}
				//Pokud se jedna o posledni prvek seznamu
				else {
					// Je treba uvolnit i pamet naalokovanou pro retezce
					free(p->Key);
					if(p->Content.Type == type_string)
						free(p->Content.Data.sData);

					free(p);
					p = NULL;
					*ptr = NULL;
				}
			}
		}
		return RET_OK;
	}

	// Funkce pro praci s pomocnym zasobnikem pri operaci Dispose nad TS funkci
	//-------------------------------
	void sInitF(t_stStackF *s)
	{
		*s = NULL;
	}

	int sPushF(t_stStackF *s, t_stItemF p)
	{
		t_stStackF new; 
		if((new = malloc(sizeof(struct stStackF))) == NULL) return RET_ERR;

		// Vkladani prvniho prvku
		if(*s == NULL) {
			*s = new;
			(*s)->Content = p;
			(*s)->Prev = NULL;
		}
		else {
			// Vkladani dalsich prvku
			new->Content = p;
			new->Prev = *s;
			*s = new;
		}
		return RET_OK;
	}

	void sPopF(t_stStackF *s)
	{
		// Odstranuje z vrcholu jen pokud zasobnik neni prazdny
		if(*s != NULL) {
			t_stStackF del = *s;
			*s = (*s)->Prev;
			free(del);
			del = NULL;
		}
	}

	//Funkce pro praci s TS funkci
	//-------------------------------

	/* Funkce provede inicializaci tabulky funkci. U tabulky symbolu pro funkce k tomu
	   patri i naplneni tabulky vestavenymi funkcemi IFJ13 */
	int stInitF(t_stItemF *ptr,t_stItemV *VTableRootGl)
	{
		t_stItemV ReturnPointer;
		int i = 0;
		*ptr = NULL;
		tInstList *InstListptr[10];
		for (i=0;i<10;i++)
		{
			if ((InstListptr[i] =(tInstList *) malloc(sizeof(tInstList)))==NULL)
				return RET_ERR;
			listInit(InstListptr[i]);
		}
		// Vytvoření proměnné pro vrácení hodnoty
		if (stInsertV(VTableRootGl,"ReturnValue",type_undef,0,0,NULL,false)==RET_ERR)
			return RET_ERR;
		if ((ReturnPointer = stSearchV(VTableRootGl,"ReturnValue"))==NULL)
			return RET_ERR;

		// Funkce boolval
		if (GenerateInstructionList(INST_BOOLVAL,NULL,NULL,NULL,InstListptr[0]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[0])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[0])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "boolval", 1, InstListptr[0], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce doubleval
		if (GenerateInstructionList(INST_DOUBVAL,NULL,NULL,NULL,InstListptr[1]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[1])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[1])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "doubleval", 1, InstListptr[1], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce intval
		if (GenerateInstructionList(INST_INTVAL,NULL,NULL,NULL,InstListptr[2]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[2])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[2])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "intval", 1, InstListptr[2], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce strval
		if (GenerateInstructionList(INST_STRVAL,NULL,NULL,NULL,InstListptr[3]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[3])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[3])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "strval", 1, InstListptr[3], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce get string
		if (GenerateInstructionList(INST_GETSTR,NULL,NULL,NULL,InstListptr[4]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[4])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[4])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "get_string", 0, InstListptr[4], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce put string
		if (GenerateInstructionList(INST_PUTSTR,NULL,NULL,NULL,InstListptr[5]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[5])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[5])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "put_string", 0, InstListptr[5], NULL) == RET_ERR) 
			return RET_ERR;

		// Funkce get string
		if (GenerateInstructionList(INST_STRLEN,NULL,NULL,NULL,InstListptr[6]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[6])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[6])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "strlen", 1, InstListptr[6], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce get substring
		if (GenerateInstructionList(INST_GETSUB,NULL,NULL,NULL,InstListptr[7]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[7])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[7])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "get_substring", 3, InstListptr[7], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce find substring
		if (GenerateInstructionList(INST_FINDSTR,NULL,NULL,NULL,InstListptr[8]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[8])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[8])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "find_string", 2, InstListptr[8], NULL) == RET_ERR)
			return RET_ERR;

		// Funkce sort string
		if (GenerateInstructionList(INST_SORTSTR,NULL,NULL,NULL,InstListptr[9]) == RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_LTOGLOAD,"FunctionResult",NULL,ReturnPointer,InstListptr[9])==RET_ERR)
			return RET_ERR;
		if (GenerateInstructionList(INST_RETURN,NULL,NULL,NULL,InstListptr[9])==RET_ERR)
			return RET_ERR;
		if(stInsertF(ptr, "sort_string", 1, InstListptr[9], NULL) == RET_ERR)
			return RET_ERR;

		return RET_OK;
	}

	// Projde binarni strom a vrati ukazatel na hledany prvek
	t_stItemF stSearchF(t_stItemF *ptr, char *Key)
	{
		t_stItemF p = *ptr;

		while(p != NULL)
		{
			if(!strcmp(p->Key, Key))
				return p;
			else if(strcmp(p->Key, Key)>0)
				p = p->LPtr;
			else
				p = p->RPtr;
		}
		return NULL;
	}

	/* Funkce prohleda tabulku symbolu funkci a pokud byla jiz funkce nalezena vrati chybu
	   jinak ji vlozi do tabulky */
	int stInsertF(t_stItemF *ptr, char *Key, int paramCount, tInstList *instListPtr, t_stItemV stItemVPtr)
	{
		t_stItemF p = *ptr, p2;

		while(p != NULL)
		{
			if(strcmp(p->Key, Key)>0) {
				p2 = p;
				p = p->LPtr;
			}			
			else {
				p2 = p;
				p = p->RPtr;
			}
		}

		if((p = malloc(sizeof(struct stItemF))) == NULL) {
			stDisposeF(ptr);
			return RET_ERR;
		}
		// Alokace pameti pro retezec predstavujici jmeno promenne
		if((p->Key = malloc(sizeof(char)*(strlen(Key)+1))) == NULL) {
			stDisposeF(ptr);
			return RET_ERR;
		}
		strcpy(p->Key, Key);
		p->paramCount = paramCount;
		p->instListPtr = instListPtr;
		p->stItemVPtr = stItemVPtr;
		p->LPtr = NULL;
		p->RPtr = NULL;

		if(*ptr == NULL)
			*ptr = p;
		else if(strcmp(p2->Key, Key)>0)
			p2->LPtr = p;
		else
			p2->RPtr = p;

		return RET_OK;
	}

	// Pomocna funkce prochazenici binarni strom k nejlevejsimu prvku
	void leftMostF(t_stItemF ptr, t_stStackF *stack)
	{
		while(ptr != NULL) {
			sPushF(stack, ptr);
			ptr = ptr->LPtr;
		}
	}

	/* Pomocna funkce parseru, ktera kontroluje zda u kazde polozky tabulky symbolu
	   existuje ukazatel na prislusny seznam instrukci */
	int stTableFCheckInstListPtr(t_stItemF ptr)
	{
		if(ptr != NULL) {
			t_stStackF stack;
			sInitF(&stack);

			leftMostF(ptr, &stack);

			while(stack != NULL) {
				ptr = stack->Content;
				sPopF(&stack);
				if(ptr->instListPtr == NULL) {
					while(stack != NULL)
						sPopF(&stack);

					//stDisposeF(&ptr); // OSETRENI DEALOKACE PRI CHYBE
					return SEM_ERR;
				}

				leftMostF(ptr->RPtr, &stack);
			}
		}
		return RET_OK;
	}

	/* Funkce pro odstraneni cele tabulky symbolu funkci s polu s na ni navazujicimi
	   tabulkami symbolu pro lokalni promenne a prislusnymi seznamy instrukci */
	int stDisposeF(t_stItemF *ptr)
	{
		t_stStackF stack;
		sInitF(&stack);
		t_stItemF p = *ptr, p2;

		while(*ptr != NULL)
		{
			if(p->RPtr != NULL) {
				if(sPushF(&stack, p) == RET_ERR) return RET_ERR;
				p = p->RPtr;
			}
			else if(p->LPtr != NULL) {
				if(sPushF(&stack, p) == RET_ERR) return RET_ERR;
				p = p->LPtr;
			}
			else {
				if(stack != NULL) {
					p2 = p;
					p = stack->Content;
					sPopF(&stack);

					if(p->RPtr == p2)
						p->RPtr = NULL;
					else
						p->LPtr = NULL;

					// Je treba uvlonit i pamet naalokovanou pro retezec
					free(p2->Key);
					/* Pokud existoval ukazatel an seznam instrukci
					(ten napriklad u integrovanych funkci jazyka existova nemusi)
					Valoa uvolnovaci funkci seznamu a pak uvolni i jeho polozku struktury */
					if(p2->instListPtr != NULL) {
						listDestroy(p2->instListPtr);
						free(p2->instListPtr);
					}
					if(p2->stItemVPtr != NULL){
						stDisposeV(&(p2->stItemVPtr));
					}
					free(p2);
					p2 = NULL;
				}
				// Uvolneni pameti v pripade posledniho prvku seznamu
				else {
					//Je treba uvlonit i pamet naalokovanou pro retezec
					free(p->Key);
					/* Pokud existoval ukazatel an seznam instrukci
					(ten napriklad u integrovanych funkci jazyka existova nemusi)
					Valoa uvolnovaci funkci seznamu a pak uvolni i jeho polozku struktury */
					if(p->instListPtr != NULL) {
						listDestroy(p->instListPtr);
						free(p->instListPtr);
					}
					if(p->stItemVPtr != NULL){
						stDisposeV(&(p->stItemVPtr));
					}
					free(p);
					p = NULL;
					*ptr = NULL;
				}
			}
		}
		return RET_OK;
	}

	/* Funkce vyhleda v retezci, predanem jako prvni parametr, prvni
	   vyskyt podretezce predaneho jako druhy parametr a vrati jeho index(od 0) */
	int find_string(char *string, char *find){

		int string_length;
			if ((string_length = strLen(string)) == SEM_ERR_OTHER) // kontrola jestli to je NULL
			{
				return SEM_ERR_OTHER;
			}
		int find_length;
			if ((find_length = strLen(find)) == SEM_ERR_OTHER)
			{
				return SEM_ERR_OTHER;
			}
			if (find_length == 0)
				return 0;
		int string_indx = 0,find_indx = 0;
		int position = 2,cnt = 0;
		int fail[find_length+1];

		fail[0]=-1;
		fail[1]=0;
		while (position<find_length){
			if(find[position-1]==find[cnt]){
				fail[position++]=++cnt;
			}
			else if(cnt>0){
				cnt=fail[cnt];
			}
			else{
				fail[position++]=0;
			}
		}



		while(string_indx + find_indx<string_length){
			if(find[find_indx] == string[string_indx+find_indx]) {
				if(find_indx==find_length-1)
					return string_indx;
				find_indx++;
			}
			else{
				string_indx = string_indx + find_indx -fail[find_indx];
				if(fail[find_indx]>=0)
					find_indx=fail[find_indx];
				else
					find_indx = 0;
			}
		}
		return -1;
	}

	/* Funkce sort string a jeji pomocne funkce. funkce sort string
	   seradi znaky v retezci od nejnizsi po nejvyssi ordinalni hodnotou. */
	void swap(char *s, int r, int l){
		char tmp = s[r];
		s[r] = s[l];
		s[l] = tmp;
	}

	void heap(char *s, int max, int start){
		char temp = s[start];
		int son = start*2 + 1;
		if(son<max && s[son]<s[son+1]) son++;

		while(son<=max && temp<s[son]){
			s[start]=s[son];
			start=son;
			son = son*2+1;
			if(son<max && s[son]<s[son+1]) son++;
		}
		s[start]=temp;
		return;
	}

	int sort_string(char **string){

		int size;

		if(*string == NULL) return SEM_ERR_OTHER;

		if((size = strlen(*string)) <= 1)
			return RET_OK;


		for(int i = size/2-1;i>=0;i--)
		{
			heap(*string,size-1,i);
		}
		for(int j = size-1;j>0;j--)
		{	
			swap(*string,j,0);
			heap(*string,j-1,0);
		}

		return RET_OK;
	}

