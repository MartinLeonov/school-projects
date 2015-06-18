/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I 
  * Soubor:   interpret.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include <stdlib.h>
#include <string.h>
#include "interpret.h"

int
interpret (t_stItemV GlobSymbTable, tInstList * List)
{
  tInstListItem *instItem;	//  polozka v seznamu instrukci
  tInst Inst;			//  instrukce
  listFirstActive (List);	//  nastaveni aktivity na prvni instrukci


  int return_value_int;
  int return_value_int1;
  double return_value_double;

  char *parameters;
  char *string = NULL;		//  pro ulozeni vystupniho retezce
  char *string1 = NULL;		//  pro ulozeni vystupniho retezce

  t_stItemV ptrAdr1, ptrAdr2, ptrResult;

  t_stStackV stack;		//  zasobnik (INST_CALL)
  sInitV (&stack);		//  inicializace zasobniku

  sPushV (&stack, GlobSymbTable, NULL, NULL);

  while (1)
    {
      ptrAdr1 = NULL;
      ptrAdr2 = NULL;
      ptrResult = NULL;
      // nacteni instrukce
      instItem = listGetActive (List);
      Inst = instItem->instruction;
      switch (Inst.type)
	{

	case INST_BOOLVAL:	// boolval()
	  ptrResult = stSearchV (&GlobSymbTable, "PAR1");	//Nacitanie parametru z globalnej tabulky
	  if ((return_value_int = boolval (ptrResult)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }			//Polozka pomocou, ktorej sa odovzda navratova hodnota
	  if (stInsertV
	      (&(stack->Content), "FunctionResult", type_boolean, 0, 0, NULL,
	       return_value_int) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_DOUBVAL:	// doubleval()
	  ptrResult = stSearchV (&GlobSymbTable, "PAR1");
	  if (ptrResult == NULL)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if ((return_value_double = doubleval (ptrResult)) == SEM_ERR_RETYPE)
	    {
	      sDispose (&stack);
	      return SEM_ERR_RETYPE;
	    }
	  if (stInsertV
	      (&(stack->Content), "FunctionResult", type_double, 0,
	       return_value_double, NULL, false) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_INTVAL:	// intval()
	  ptrResult = stSearchV (&(GlobSymbTable), "PAR1");
	  if (ptrResult == NULL)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  return_value_int = intval (ptrResult);
	  if (stInsertV
	      (&(stack->Content), "FunctionResult", type_int,
	       return_value_int, 0, NULL, false) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_STRVAL:	// strval()
	  ptrResult = stSearchV (&(GlobSymbTable), "PAR1");
	  if (strval (ptrResult, &string) != RET_OK)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if (stInsertV
	      (&(stack->Content), "FunctionResult", type_string, 0, 0, string,
	       false) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_GETSTR:	// get_string()
	  if ((return_value_int = get_string (&string)) != RET_OK)
	    {
	      sDispose (&stack);
	      return return_value_int;
	    }
	  if ((stInsertV
	       (&(stack->Content), "FunctionResult", type_string, 0, 0,
		string, false)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_PUTSTR:	// put_string()
	  return_value_int = 1;
	  //Neznamy pocet paratrov sa riesi nekonecnym cyklom,ktory
	  //nacitava hodnoty kym nenarazi na zarazku typ_end_par
	  while (1)
	    {			//Vygenerujeme nazov parametru
	      parameters = GenerateString (return_value_int);
	      if (parameters == NULL)
		{
		  sDispose (&stack);
		  return RET_ERR;
		}
	      return_value_int++;	//najdeme polozku v tabulke
	      ptrAdr1 = stSearchV (&GlobSymbTable, parameters);
	      free (parameters);
	      //musime pretypovat?
	      if ((ptrAdr1->Content.Type) != type_string)
		{
		  if (ptrAdr1->Content.Type == type_end_par)
		    {
		      if ((stInsertV
			   (&(stack->Content), "FunctionResult", type_int,
			    return_value_int - 2, 0, NULL, false)) == RET_ERR)
			{
			  sDispose (&stack);
			  return RET_ERR;
			}
		      break;
		    }
		  if (strval (ptrAdr1, &string) != RET_OK)
		    {
		      sDispose (&stack);
		      return RET_ERR;
		    }
		  printf ("%s", string);	//VYpis pretypovaneho parametru
		  continue;
		}		// Vypis parametru, ktory nie je treba pretypovat
	      printf ("%s", ptrAdr1->Content.Data.sData);
	    }
	  break;

	case INST_STRLEN:	// strlen()
	  ;
	  int length;
	  ptrAdr1 = stSearchV (&(GlobSymbTable), "PAR1");
	  if (strval (ptrAdr1, &string) != RET_OK)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if ((length = strLen (string)) < RET_OK)
	    {
	      sDispose (&stack);
	      return length;
	    }
	  if ((stInsertV
	       (&(stack->Content), "FunctionResult", type_int, length, 0,
		NULL, false)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_GETSUB:	// get_substring()
	  ptrResult = stSearchV (&(GlobSymbTable), "PAR1");
	  if (strval (ptrResult, &string) != RET_OK)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  ptrAdr1 = stSearchV (&(GlobSymbTable), "PAR2");
	  if ((return_value_int = intval (ptrAdr1)) < 0)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  ptrAdr2 = stSearchV (&(GlobSymbTable), "PAR3");
	  if ((return_value_int1 = intval (ptrAdr2)) < 0)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if ((return_value_int =
	       get_substring (&string, return_value_int,
			      return_value_int1)) != RET_OK)
	    {
	      sDispose (&stack);
	      return return_value_int;
	    }
	  if ((stInsertV
	       (&(stack->Content), "FunctionResult", type_string, 0, 0,
		string, false)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }

	  break;

	case INST_FINDSTR:	// find_string()
	  ptrAdr1 = stSearchV (&(GlobSymbTable), "PAR1");
	  if (strval (ptrAdr1, &string) != RET_OK)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  ptrAdr2 = stSearchV (&(GlobSymbTable), "PAR2");
	  if (strval (ptrAdr2, &string1) != RET_OK)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if ((return_value_int = find_string (string, string1)) < -1)
	    {
	      sDispose (&stack);
	      return return_value_int;
	    }
	  if ((stInsertV
	       (&(stack->Content), "FunctionResult", type_int,
		return_value_int, 0, NULL, false)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_SORTSTR:	// sort_string()
	  ptrAdr1 = stSearchV (&(GlobSymbTable), "PAR1");
	  if (strval (ptrAdr1, &string) != RET_OK)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if ((return_value_int = sort_string (&string)) != RET_OK)
	    {
	      sDispose (&stack);
	      return return_value_int;
	    }
	  if ((stInsertV
	       (&(stack->Content), "FunctionResult", type_string, 0, 0,
		string, false)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  break;

	case INST_NONTERM:
	case INST_NOP:
	case INST_LABEL:	// prazdne operace
	  break;

	case INST_JUMP:	// jump

	  //Najdeme vysledok podmienky, na jeho zaklade rozhodneme ci skakat alebo nie
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  if ((return_value_int = boolval (ptrResult)) == RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if (return_value_int == false)	//Skaceme
	    {			//Nastavenie labelu ako dalsej instrukcie
	      listSetActiveOn (List, (tInstListItem *) Inst.adr1);
	    }
	  break;

	case INST_DJUMP:	// jump primo bez kontroly
	  listSetActiveOn (List, (tInstListItem *) Inst.adr1);
	  break;

	case INST_CALL:	// volani funkci

	  ;
	  t_stItemF SymbTableResult;

	  SymbTableResult = (t_stItemF) Inst.result;
	  // ulozeni ukazatele na tabulku na vrchol zasobniku
	  // listGetActive -> ulozi adresu funkce aby bylo mozne
	  // pri returnu nacist nasledujici za instrukci CALL
	  sPushV (&stack, NULL, listGetActive (List), List);
	  stInitV (&(stack->Content));
	  // SymbTablePtr je prazdne -> lze nahravat symboly ->
	  // prekopirovani stItemVPtr ze struktury t_stItemF do 
	  // SymbolTablePtr
	  if (stInorderV (SymbTableResult->stItemVPtr, &(stack->Content)) ==
	      RET_ERR)
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  // Nastavenie noveho zoznamu, kde su instrukcie volanej funkcie
	  // nastavenie aktivneho prvku na zaciatok zoznamu
	  List = SymbTableResult->instListPtr;
	  listFirstActive (List);
	  break;

	case INST_RETURN:

	  //Jedna sa o return celeho programu?
	  if (stack->Content == GlobSymbTable)
	    {
	      sPopV (&stack);
	      return SYNT_OK;
	    }
	  List = stack->returnList;	//Obnovime navratovy list
	  listSetActiveOn (List, stack->returnAddr);	//aj navratovu adresu
	  stDisposeV (&stack->Content);	//Uvlonime zasobnik
	  sPopV (&stack);	//popneme zasobnik
	  break;

	case INST_GTOLLOAD:
	  //Instrukcia, ktora nacita z globalnej tabulky do lokalnej
	  //Funguje ako prostrednik prenosu parametrov do funckie, ako aj
	  //navratovej hodnoty  von z funkcie
	  ptrAdr1 = (t_stItemV) Inst.adr1;	//Zdroj je predany priamym ukazatelom
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);	//Ciel je predany retazcom
	  //Zvysok instrukcie je to iste ako u instrukcie LOAD
	  switch (ptrAdr1->Content.Type)
	    {
	    case type_undef:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);

	      sDispose (&stack);
	      return SEM_ERR_UNDEF;
	    case type_end_par:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      break;
	    case type_boolean:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.bData = ptrAdr1->Content.Data.bData;
	      break;
	    case type_int:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.iData = ptrAdr1->Content.Data.iData;
	      break;
	    case type_null:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      break;
	    case type_double:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.dData = ptrAdr1->Content.Data.dData;
	      break;
	    case type_string:
	      if (ptrResult->Key != ptrAdr1->Key)
		{
		  if ((ptrResult->Content.Type == type_string))
		    {
		      free (ptrResult->Content.Data.sData);
		    }
		  ptrResult->Content.Type = ptrAdr1->Content.Type;

		  if ((ptrResult->Content.Data.sData =
		       (char *) malloc (sizeof (char) *
					strlen (ptrAdr1->Content.Data.sData) +
					1)) == NULL)
		    {
		      sDispose (&stack);
		      return RET_ERR;
		    }
		  strcpy (ptrResult->Content.Data.sData,
			  ptrAdr1->Content.Data.sData);
		}
	      break;
	    }
	  break;

	case INST_LTOGLOAD:
	  // Opacna operacia od GTOLLOAD
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrResult = (t_stItemV) Inst.result;

	  switch (ptrAdr1->Content.Type)
	    {
	    case type_undef:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);

	      sDispose (&stack);
	      return SEM_ERR_UNDEF;
	    case type_end_par:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      break;
	    case type_boolean:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.bData = ptrAdr1->Content.Data.bData;
	      break;
	    case type_int:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.iData = ptrAdr1->Content.Data.iData;
	      break;
	    case type_null:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      break;
	    case type_double:
	      if ((ptrResult->Content.Type == type_string)
		  && (ptrResult->Content.Data.sData) != NULL)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.dData = ptrAdr1->Content.Data.dData;
	      break;
	    case type_string:
	      if (ptrResult->Key != ptrAdr1->Key)
		{
		  if ((ptrResult->Content.Type == type_string)
		      && (ptrResult->Content.Data.sData != NULL))
		    free (ptrResult->Content.Data.sData);

		  ptrResult->Content.Type = ptrAdr1->Content.Type;

		  if ((ptrResult->Content.Data.sData =
		       (char *) malloc (sizeof (char) *
					strlen (ptrAdr1->Content.Data.sData) +
					1)) == NULL)
		    {
		      sDispose (&stack);
		      return RET_ERR;
		    }
		  strcpy (ptrResult->Content.Data.sData,
			  ptrAdr1->Content.Data.sData);
		}

	      break;
	    }
	  break;

	case INST_LOAD:	// nacteni hodnoty do promenne
	  //Obe polozky tabulky su predane retazcom
	  //Jedna sa o presun z lokalnej do lokalnej tabulky

	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  //Na zaklade typu zdrojovej polozky rozhodneme ktore data prepiseme
	  switch (ptrAdr1->Content.Type)
	    {
	    case type_boolean:
	      if ((ptrResult->Content.Type == type_string))
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.bData = ptrAdr1->Content.Data.bData;
	      break;
	    case type_int:
	      if ((ptrResult->Content.Type == type_string))
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.iData = ptrAdr1->Content.Data.iData;
	      break;
	    case type_null:
	      if ((ptrResult->Content.Type == type_string))
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      break;
	    case type_double:
	      if ((ptrResult->Content.Type == type_string))
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.dData = ptrAdr1->Content.Data.dData;
	      break;
	    case type_string:
	      if (ptrResult->Key != ptrAdr1->Key)
		{
		  if ((ptrResult->Content.Type == type_string)
		      && (ptrResult->Content.Data.sData != NULL))
		    free (ptrResult->Content.Data.sData);

		  ptrResult->Content.Type = ptrAdr1->Content.Type;

		  if ((ptrResult->Content.Data.sData =
		       (char *) malloc (sizeof (char) *
					strlen (ptrAdr1->Content.Data.sData) +
					1)) == NULL)
		    {
		      sDispose (&stack);
		      return RET_ERR;
		    }

		  strcpy (ptrResult->Content.Data.sData,
			  ptrAdr1->Content.Data.sData);
		}
	      break;

	    case type_undef:
	      sDispose (&stack);
	      return SEM_ERR_UNDEF;
	      break;
	    }
	  break;


	case INST_CONCAT:	// . konkatenacia retazcov
	  string = NULL;
	  //nacitanie operandov
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_string;
	  if (ptrAdr1->Content.Type != type_string)	// nejde o string, chyba
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if (strval (ptrAdr2, &string) != RET_OK)	// Pretypovanie druheho operandu
	    {
	      sDispose (&stack);
	      return RET_ERR;
	    }
	  if (ptrResult->Content.Data.sData != NULL)	//najskor uvolnime string, na ktory ukazuje
	    free (ptrResult->Content.Data.sData);	//adresa cielovej polozky, ideme ju prepisat
	  ptrResult->Content.Data.sData =
	    stringConcatenate (ptrAdr1->Content.Data.sData, string);
	  break;

	case INST_EQUAL:	// ===
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_boolean;
	  if (ptrAdr1->Content.Type != ptrAdr2->Content.Type)	// ruzny typ automaticky false
	    ptrResult->Content.Data.bData = false;
	  else
	    {
	      if (ptrAdr1->Content.Type == type_boolean)
		{		// porovnani bool hodnot
		  if (ptrAdr1->Content.Data.bData ==
		      ptrAdr2->Content.Data.bData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_int)
		{		// porovnani int hodnot
		  if (ptrAdr1->Content.Data.iData ==
		      ptrAdr2->Content.Data.iData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_double)
		{		// porovnani double hodnot
		  if (ptrAdr1->Content.Data.dData ==
		      ptrAdr2->Content.Data.dData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_string)
		{		// porovnani stringu
		  if ((strcmp
		       (ptrAdr1->Content.Data.sData,
			ptrAdr2->Content.Data.sData)) == 0)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_null)	// null automaticky true
		ptrResult->Content.Data.bData = true;
	    }
	  break;

	case INST_NEQUAL:	// !==
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_boolean;
	  if (ptrAdr1->Content.Type != ptrAdr2->Content.Type)	// ruzny typ automaticky true
	    ptrResult->Content.Data.bData = true;
	  else
	    {
	      if (ptrAdr1->Content.Type == type_boolean)
		{		// porovnani bool hodnot
		  if (ptrAdr1->Content.Data.bData ==
		      ptrAdr2->Content.Data.bData)
		    ptrResult->Content.Data.bData = false;
		  else
		    ptrResult->Content.Data.bData = true;
		}
	      else if (ptrAdr1->Content.Type == type_int)
		{		// porovnani int hodnot
		  if (ptrAdr1->Content.Data.iData ==
		      ptrAdr2->Content.Data.iData)
		    ptrResult->Content.Data.bData = false;
		  else
		    ptrResult->Content.Data.bData = true;
		}
	      else if (ptrAdr1->Content.Type == type_double)
		{		// porovnani double hodnot
		  if (ptrAdr1->Content.Data.dData ==
		      ptrAdr2->Content.Data.dData)
		    ptrResult->Content.Data.bData = false;
		  else
		    ptrResult->Content.Data.bData = true;
		}
	      else if (ptrAdr1->Content.Type == type_string)
		{		// porovnani stringu
		  if ((strcmp
		       (ptrAdr1->Content.Data.sData,
			ptrAdr2->Content.Data.sData)) == 0)
		    ptrResult->Content.Data.bData = false;
		  else
		    ptrResult->Content.Data.bData = true;
		}
	      else if (ptrAdr1->Content.Type == type_null)	// null automaticky false
		ptrResult->Content.Data.bData = false;
	    }
	  break;

	case INST_ASSIGN:	// =

	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  switch (ptrAdr1->Content.Type)
	    {			// podle typu ptrAdr1 je prirazen typ vysledku...pokud byl typ vysledku string, je tento string uvolnen
	      // nasleduje prirazeni hodnoty
	    case type_boolean:
	      if (ptrResult->Content.Data.sData != NULL
		  && ptrResult->Content.Type == type_string)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.bData = ptrAdr1->Content.Data.bData;
	      break;
	    case type_null:
	      if (ptrResult->Content.Data.sData != NULL
		  && ptrResult->Content.Type == type_string)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      break;
	    case type_int:
	      if (ptrResult->Content.Data.sData != NULL
		  && ptrResult->Content.Type == type_string)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.iData = ptrAdr1->Content.Data.iData;
	      break;
	    case type_double:
	      if (ptrResult->Content.Data.sData != NULL
		  && ptrResult->Content.Type == type_string)
		free (ptrResult->Content.Data.sData);
	      ptrResult->Content.Type = ptrAdr1->Content.Type;
	      ptrResult->Content.Data.dData = ptrAdr1->Content.Data.dData;
	      break;
	    case type_string:
	      if (ptrResult->Key != ptrAdr1->Key)
		{
		  if (ptrResult->Content.Data.sData != NULL
		      && ptrResult->Content.Type == type_string)
		    free (ptrResult->Content.Data.sData);
		  ptrResult->Content.Type = ptrAdr1->Content.Type;

		  if ((ptrResult->Content.Data.sData =
		       (char *) malloc (sizeof (char) *
					strlen (ptrAdr1->Content.Data.sData) +
					1)) == NULL)
		    {
		      sDispose (&stack);
		      return RET_ERR;
		    }

		  strcpy (ptrResult->Content.Data.sData,
			  ptrAdr1->Content.Data.sData);
		}
	      break;
	    case type_undef:
	      sDispose (&stack);
	      return SEM_ERR_UNDEF;
	      break;
	    }

	  break;

	case INST_LESSER:	// <
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_boolean;
	  if (ptrAdr1->Content.Type != ptrAdr2->Content.Type)
	    {			// nekompatibilni typy, chyba
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  else
	    {
	      if (ptrAdr1->Content.Type == type_boolean)
		{		// porovnani bool hodnot
		  if (ptrAdr1->Content.Data.bData <
		      ptrAdr2->Content.Data.bData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_int)
		{		// porovnani int hodnot
		  if (ptrAdr1->Content.Data.iData <
		      ptrAdr2->Content.Data.iData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_double)
		{		// porovnani double hodnot
		  if (ptrAdr1->Content.Data.dData <
		      ptrAdr2->Content.Data.dData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_string)
		{		// porovnani stringu
		  if ((strcmp
		       (ptrAdr1->Content.Data.sData,
			ptrAdr2->Content.Data.sData)) < 0)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_null)	// null automaticky false
		ptrResult->Content.Data.bData = false;
	    }
	  break;

	case INST_LESSERQ:	// <=
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_boolean;
	  if (ptrAdr1->Content.Type != ptrAdr2->Content.Type)
	    {			// nekompatibilni typy, chyba
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  else
	    {
	      if (ptrAdr1->Content.Type == type_boolean)
		{		// porovnani bool hodnot
		  if (ptrAdr1->Content.Data.bData <=
		      ptrAdr2->Content.Data.bData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_int)
		{		// porovnani int hodnot
		  if (ptrAdr1->Content.Data.iData <=
		      ptrAdr2->Content.Data.iData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_double)
		{		// porovnani double hodnot
		  if (ptrAdr1->Content.Data.dData <=
		      ptrAdr2->Content.Data.dData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_string)
		{		// porovnani stringu
		  if ((strcmp
		       (ptrAdr1->Content.Data.sData,
			ptrAdr2->Content.Data.sData)) <= 0)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_null)	// null automaticky false
		ptrResult->Content.Data.bData = false;
	    }
	  break;

	case INST_GREATER:	// >
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_boolean;
	  if (ptrAdr1->Content.Type != ptrAdr2->Content.Type)
	    {			// nekompatibilni typy, chyba
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  else
	    {
	      if (ptrAdr1->Content.Type == type_boolean)
		{		// porovnani bool hodnot
		  if (ptrAdr1->Content.Data.bData >
		      ptrAdr2->Content.Data.bData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_int)
		{		// porovnani int hodnot
		  if (ptrAdr1->Content.Data.iData >
		      ptrAdr2->Content.Data.iData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_double)
		{		// porovnani double hodnot
		  if (ptrAdr1->Content.Data.dData >
		      ptrAdr2->Content.Data.dData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_string)
		{		// porovnani stringu
		  if ((strcmp
		       (ptrAdr1->Content.Data.sData,
			ptrAdr2->Content.Data.sData)) > 0)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_null)	// null automaticky false
		ptrResult->Content.Data.bData = false;
	    }
	  break;

	case INST_GREATERQ:	// >=
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  ptrResult->Content.Type = type_boolean;
	  if (ptrAdr1->Content.Type != ptrAdr2->Content.Type)
	    {			// nekompatibilni typy, chyba
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  else
	    {
	      if (ptrAdr1->Content.Type == type_boolean)
		{		// porovnani bool hodnot
		  if (ptrAdr1->Content.Data.bData >=
		      ptrAdr2->Content.Data.bData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_int)
		{		// porovnani int hodnot
		  if (ptrAdr1->Content.Data.iData >=
		      ptrAdr2->Content.Data.iData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_double)
		{		// porovnani double hodnot
		  if (ptrAdr1->Content.Data.dData >=
		      ptrAdr2->Content.Data.dData)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_string)
		{		// porovnani stringu
		  if ((strcmp
		       (ptrAdr1->Content.Data.sData,
			ptrAdr2->Content.Data.sData)) >= 0)
		    ptrResult->Content.Data.bData = true;
		  else
		    ptrResult->Content.Data.bData = false;
		}
	      else if (ptrAdr1->Content.Type == type_null)	// null automaticky false
		ptrResult->Content.Data.bData = false;
	    }
	  break;

	case INST_PLUS:	// +
	  return_value_double = -1.0;
	  return_value_int = -1;
	  // Nacitani operandu a cilove polozky
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  if ((ptrAdr1->Content.Type != type_double)
	      && (ptrAdr1->Content.Type != type_int))
	    {			// typ prvniho operandu nevyhovuje, chyba
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if ((ptrAdr2->Content.Type != type_double)
	      && (ptrAdr2->Content.Type != type_int))
	    {			// typ druheho operandu nevyhovuje, chyba
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if ((ptrAdr1->Content.Type == type_double)
	      || (ptrAdr2->Content.Type == type_double))
	    {			// pokud je jeden z operandu double, vysledek je typu double
	      ptrResult->Content.Type = type_double;
	      if ((ptrAdr1->Content.Type == type_int)
		  && (ptrAdr2->Content.Type == type_double))
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.iData + ptrAdr2->Content.Data.dData;
	      else if ((ptrAdr1->Content.Type == type_double)
		       && (ptrAdr2->Content.Type == type_int))
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData + ptrAdr2->Content.Data.iData;
	      else
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData + ptrAdr2->Content.Data.dData;
	    }
	  else
	    {			// vysledek je typu int, pokud jsou oba operandy typu int
	      ptrResult->Content.Type = type_int;
	      ptrResult->Content.Data.iData =
		ptrAdr1->Content.Data.iData + ptrAdr2->Content.Data.iData;
	    }
	  break;

	case INST_MINUS:	// -
	  return_value_double = -1.0;
	  return_value_int = -1;
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  if ((ptrAdr1->Content.Type != type_double)
	      && (ptrAdr1->Content.Type != type_int))
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if ((ptrAdr2->Content.Type != type_double)
	      && (ptrAdr2->Content.Type != type_int))
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if ((ptrAdr1->Content.Type == type_double)
	      || (ptrAdr2->Content.Type == type_double))
	    {
	      ptrResult->Content.Type = type_double;
	      if ((ptrAdr1->Content.Type == type_int)
		  && (ptrAdr2->Content.Type == type_double))
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.iData - ptrAdr2->Content.Data.dData;
	      else if ((ptrAdr1->Content.Type == type_double)
		       && (ptrAdr2->Content.Type == type_int))
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData - ptrAdr2->Content.Data.iData;
	      else
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData - ptrAdr2->Content.Data.dData;
	    }
	  else
	    {
	      ptrResult->Content.Type = type_int;
	      ptrResult->Content.Data.iData =
		ptrAdr1->Content.Data.iData - ptrAdr2->Content.Data.iData;
	    }
	  break;

	case INST_MULTIP:	// *
	  return_value_double = -1.0;
	  return_value_int = -1;
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  if ((ptrAdr1->Content.Type != type_double)
	      && (ptrAdr1->Content.Type != type_int))
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if ((ptrAdr2->Content.Type != type_double)
	      && (ptrAdr2->Content.Type != type_int))
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if ((ptrAdr1->Content.Type == type_double)
	      || (ptrAdr2->Content.Type == type_double))
	    {
	      ptrResult->Content.Type = type_double;
	      if ((ptrAdr1->Content.Type == type_int)
		  && (ptrAdr2->Content.Type == type_double))
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.iData * ptrAdr2->Content.Data.dData;
	      else if ((ptrAdr1->Content.Type == type_double)
		       && (ptrAdr2->Content.Type == type_int))
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData * ptrAdr2->Content.Data.iData;
	      else
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData * ptrAdr2->Content.Data.dData;
	    }
	  else
	    {
	      ptrResult->Content.Type = type_int;
	      ptrResult->Content.Data.iData =
		ptrAdr1->Content.Data.iData * ptrAdr2->Content.Data.iData;
	    }
	  break;

	case INST_DIVIDE:
	  ptrAdr1 = stSearchV (&(stack->Content), (char *) Inst.adr1);
	  ptrAdr2 = stSearchV (&(stack->Content), (char *) Inst.adr2);
	  ptrResult = stSearchV (&(stack->Content), (char *) Inst.result);
	  if (ptrAdr1->Content.Type != type_double
	      && ptrAdr1->Content.Type != type_int)
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if (ptrAdr2->Content.Type != type_double
	      && ptrAdr2->Content.Type != type_int)
	    {
	      sDispose (&stack);
	      return SEM_ERR_ARITH;
	    }
	  if (ptrAdr2->Content.Type == type_double
	      && ptrAdr2->Content.Data.dData == 0.0)
	    {			// osetreni deleni double nulou
	      sDispose (&stack);
	      return SEM_ERR_NULL_DIV;
	    }
	  else if (ptrAdr2->Content.Type == type_int
		   && ptrAdr2->Content.Data.iData == 0)
	    {			// osetreni deleni int nulou
	      sDispose (&stack);
	      return SEM_ERR_NULL_DIV;
	    }
	  ptrResult->Content.Type = type_double;
	  // vysledek vzdy typu double
	  if (ptrAdr2->Content.Type == type_double)
	    {
	      if (ptrAdr1->Content.Type == type_double)
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData / ptrAdr2->Content.Data.dData;
	      else
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.iData / ptrAdr2->Content.Data.dData;
	    }
	  else
	    {
	      if (ptrAdr1->Content.Type == type_double)
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.dData / ptrAdr2->Content.Data.iData;
	      else
		ptrResult->Content.Data.dData =
		  ptrAdr1->Content.Data.iData / ptrAdr2->Content.Data.iData;
	    }
	  break;
	}

      // uvolneni pomocnych stringu pro dalsi pouziti
      if (string != NULL)
	free (string);
      if (string1 != NULL)
	free (string1);
      string = NULL;
      string1 = NULL;
      // posunuti aktivity na dalsi instrukci
      // Ak sa jedna o instrukciu volania funkcie, neposuvame sa
      // pretoze dalsia instrukcia sa nacita v Case prikazoch
      if (Inst.type != INST_CALL)
	{
	  listNextInst (List);
	}
    }

}

char *
stringConcatenate (char *adr1, char *adr2)
{				// funkce pro konkatenaci stringu
  char *result;
  result =
    (char *) malloc ((sizeof (char)) * (strlen (adr1) + strlen (adr2) + 1));
  if (result == NULL)
    {
      return NULL;
    }
  strcpy (result, adr1);
  strcat (result, adr2);
  return result;
}
