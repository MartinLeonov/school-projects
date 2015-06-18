/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   parser.c
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#include "scanner.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char table[9][9];
string *s;
int c;
t_stItemV *VTableRootGlob, *VTableCurrent, CurrentTablePtr;
t_stItemF *FTableRootGlob;
tInstList *InstListGlob, *InstListCurrent;

int
parse (t_stItemV * VTableRoot, t_stItemF * FTableRoot,
tInstList * GlobalInstList)
{

	//Prevzatie datovych struktur
	CurrentTablePtr = NULL;
	VTableRootGlob = VTableRoot;
	FTableRootGlob = FTableRoot;
	InstListGlob = GlobalInstList;
	expr_analysis_init (&table);
	int result;
	VTableCurrent = VTableRootGlob;
	InstListCurrent = InstListGlob;


// Inicializacia struktur pre scanner
	if ((s = malloc (sizeof (string))) == NULL)
		return RET_ERR;
	if (init_string (s) == RET_ERR)
		{
			free (s);
			return RET_ERR;
		}

// Kontrola uvodneho znaku
	if ((c = get_token (f, s)) != php_entry)
		{
			if (c == SYNT_ERR)
	{
	  free_string (s);
	  free (s);
	  return SYNT_ERR;
	}
			if (c == LEX_ERR)
	{
	  free_string (s);
	  free (s);
	  return LEX_ERR;
	}
			if (c == RET_ERR)
	{
	  free_string (s);
	  free (s);
	  return RET_ERR;
	}
			free_string (s);
			free (s);
			return SYNT_ERR;
		}

	//Syntakticka analyza programu
	result = program ();


//Kontrola ci boli vsetky funkcie deklarovane
	if (stTableFCheckInstListPtr (*FTableRootGlob) == SEM_ERR)
		{
			free_string (s);
			free (s);
			return SEM_ERR;
		}

	// Uvolnění paměti po scanneru
	free_string (s);
	free (s);
	return result;




}


//Funckia systematicky generuje retazce podla indexu, ktory dostane
//Tieto retazce sa vyuzivaju na ukladanie dat potrebnych pre interpretaciu

char *
GenerateStringExpr (int index)
{

	int indexSize = snprintf (NULL, 0, "%d", index) + 1;

	char *num = malloc (sizeof (char) * indexSize);
	sprintf (num, "%d", index);

	char *str = malloc (sizeof (char) * (3 + indexSize));

	if ((num == NULL) || (str == NULL))
		return NULL;

	str[0] = 'G';
	str[1] = 'E';
	str[2] = 'N';

	for (int i = 0; i < indexSize; i++)
		{
			str[3 + i] = num[i];
		}

	free (num);
	str[2 + indexSize] = '\0';

	return str;
}


//To iste ako funkcia vyssie, s rozdielom nazvu generujuceho stringu
char *
GenerateString (int index)
{

	int indexSize = snprintf (NULL, 0, "%d", index) + 1;

	char *num = malloc (sizeof (char) * indexSize);
	sprintf (num, "%d", index);

	char *str = malloc (sizeof (char) * (3 + indexSize));

	if ((num == NULL) || (str == NULL))
		return NULL;

	str[0] = 'P';
	str[1] = 'A';
	str[2] = 'R';

	for (int i = 0; i < indexSize; i++)
		{
			str[3 + i] = num[i];
		}

	free (num);
	str[2 + indexSize] = '\0';

	return str;
}


// Funkcia vlozi danu instrukciu na koniec globalneho zoznamu
int
GenerateInstruction (int inst_type, void *addr1, void *addr2, void *result)
{
	tInst instruction;

	instruction.type = inst_type;
	instruction.adr1 = addr1;
	instruction.adr2 = addr2;
	instruction.result = result;
	if (listInsertInst (InstListCurrent, instruction) == RET_ERR)
		return RET_ERR;
	return SYNT_OK;

}

// To iste ako vyssie, s pouzitim konkretneho zoznamu
int
GenerateInstructionList (int inst_type, void *addr1, void *addr2,
			 void *result, tInstList * InstList)
{
	tInst instruction;

	instruction.type = inst_type;
	instruction.adr1 = addr1;
	instruction.adr2 = addr2;
	instruction.result = result;
	if (listInsertInst (InstList, instruction) == RET_ERR)
		return RET_ERR;
	return SYNT_OK;

}

void
expr_analysis_init (char (*table)[9][9])
{

	/* table [top zasobniku][akt. vstup] */
	//Inicializacia tabulky prechodov
	(*table)[0][0] = RIGHT_ARROW;
	(*table)[0][1] = RIGHT_ARROW;
	(*table)[0][2] = RIGHT_ARROW;
	(*table)[0][3] = RIGHT_ARROW;
	(*table)[0][4] = RIGHT_ARROW;
	(*table)[0][5] = LEFT_ARROW;
	(*table)[0][6] = RIGHT_ARROW;
	(*table)[0][7] = RIGHT_ARROW;
	(*table)[0][8] = LEFT_ARROW;

	(*table)[1][0] = LEFT_ARROW;
	(*table)[1][1] = RIGHT_ARROW;
	(*table)[1][2] = RIGHT_ARROW;
	(*table)[1][3] = RIGHT_ARROW;
	(*table)[1][4] = RIGHT_ARROW;
	(*table)[1][5] = LEFT_ARROW;
	(*table)[1][6] = RIGHT_ARROW;
	(*table)[1][7] = RIGHT_ARROW;
	(*table)[1][8] = LEFT_ARROW;

	(*table)[2][0] = LEFT_ARROW;
	(*table)[2][1] = RIGHT_ARROW;
	(*table)[2][2] = RIGHT_ARROW;
	(*table)[2][3] = RIGHT_ARROW;
	(*table)[2][4] = RIGHT_ARROW;
	(*table)[2][5] = LEFT_ARROW;
	(*table)[2][6] = RIGHT_ARROW;
	(*table)[2][7] = RIGHT_ARROW;
	(*table)[2][8] = LEFT_ARROW;

	(*table)[3][0] = LEFT_ARROW;
	(*table)[3][1] = LEFT_ARROW;
	(*table)[3][2] = LEFT_ARROW;
	(*table)[3][3] = RIGHT_ARROW;
	(*table)[3][4] = RIGHT_ARROW;
	(*table)[3][5] = LEFT_ARROW;
	(*table)[3][6] = RIGHT_ARROW;
	(*table)[3][7] = RIGHT_ARROW;
	(*table)[3][8] = LEFT_ARROW;

	(*table)[4][0] = LEFT_ARROW;
	(*table)[4][1] = LEFT_ARROW;
	(*table)[4][2] = LEFT_ARROW;
	(*table)[4][3] = LEFT_ARROW;
	(*table)[4][4] = RIGHT_ARROW;
	(*table)[4][5] = LEFT_ARROW;
	(*table)[4][6] = RIGHT_ARROW;
	(*table)[4][7] = RIGHT_ARROW;
	(*table)[4][8] = LEFT_ARROW;

	(*table)[5][0] = LEFT_ARROW;
	(*table)[5][1] = LEFT_ARROW;
	(*table)[5][2] = LEFT_ARROW;
	(*table)[5][3] = LEFT_ARROW;
	(*table)[5][4] = LEFT_ARROW;
	(*table)[5][5] = LEFT_ARROW;
	(*table)[5][6] = EQUALITY;
	(*table)[5][7] = EXCLAMATION;
	(*table)[5][8] = LEFT_ARROW;

	(*table)[6][0] = RIGHT_ARROW;
	(*table)[6][1] = RIGHT_ARROW;
	(*table)[6][2] = RIGHT_ARROW;
	(*table)[6][3] = RIGHT_ARROW;
	(*table)[6][4] = RIGHT_ARROW;
	(*table)[6][5] = EXCLAMATION;
	(*table)[6][6] = RIGHT_ARROW;
	(*table)[6][7] = RIGHT_ARROW;
	(*table)[6][8] = EXCLAMATION;

	(*table)[7][0] = LEFT_ARROW;
	(*table)[7][1] = LEFT_ARROW;
	(*table)[7][2] = LEFT_ARROW;
	(*table)[7][3] = LEFT_ARROW;
	(*table)[7][4] = LEFT_ARROW;
	(*table)[7][5] = LEFT_ARROW;
	(*table)[7][6] = DOLLAR_SIGN;
	(*table)[7][7] = DOLLAR_SIGN;
	(*table)[7][8] = LEFT_ARROW;

	(*table)[8][0] = RIGHT_ARROW;
	(*table)[8][1] = RIGHT_ARROW;
	(*table)[8][2] = RIGHT_ARROW;
	(*table)[8][3] = RIGHT_ARROW;
	(*table)[8][4] = RIGHT_ARROW;
	(*table)[8][5] = EXCLAMATION;
	(*table)[8][6] = RIGHT_ARROW;
	(*table)[8][7] = RIGHT_ARROW;
	(*table)[8][8] = EXCLAMATION;
}


int
program ()
{

	//Nonterminal MAIN
	// Na zaklade vstupneho tokenu rozhodneme o aku konstrukciu pojde
	while ((c = get_token (f, s)) != RET_ERR)
		{
			switch (c)
	{
	case variable:		//premenna
	  if ((c = StateID ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR_MISS_PAR)
	    return SEM_ERR_MISS_PAR;
	  break;

	case kw_if:		// if
	  if ((c = StateIf ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  break;

	case kw_while:		//while
	  if ((c = StateWhile ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  break;

	case kw_function:	//function
	  if ((c = StateFunction ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR_OTHER)
	    return SEM_ERR_OTHER;
	  if (c == SEM_ERR_MISS_PAR)
	    return SEM_ERR_MISS_PAR;
	  VTableCurrent = VTableRootGlob;
	  InstListCurrent = InstListGlob;
	  break;

	case kw_return:	//return, koniec programu
	  if (StateReturn () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  if (c != semicolon)
	    return SYNT_ERR;

	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;
	  break;

	case semicolon:
	  break;

	case EOF:
	  //Vygeneruje sa zaverecny return, posledna instrukcia v programe           
	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;

	  return SYNT_OK;

	case LEX_ERR:
	  return LEX_ERR;
	  break;

	default:
	  return SYNT_ERR;	//Syntakticka chyba
	}


		}
	return RET_ERR;
}

//Jedna sa o Prikaz priradenia
//Musime rozhodnut ci sa jedna o vyraz
//alebo volanie funkcie
int
StateID ()
{
	int retV;
	// Identifikator sa vyhlada v tabulke
	// Pripadne sa tam vlozi --DOIMPLEMENTOVAT

	//pravidlo, ID -> "=" -> "Vyraz" OR FuncID(); -> ";"
	// Vlozenie noveho prvko do tabulky symbolov premennych
	t_stItemV LValueItem, RetValueItem;

	if (stInsertV (VTableCurrent, s->strg, type_undef, 0, 0, NULL, false) ==
			RET_ERR)
		return RET_ERR;
	if ((LValueItem = stSearchV (VTableCurrent, s->strg)) == NULL)
		return RET_ERR;


	if ((c = get_token (f, s)) != assign)
		{				//Chybovy stav
			if (c == LEX_ERR)
	return LEX_ERR;

			return SYNT_ERR;
		}

	//Rozhodneme ci sa jedna o volanie funkcie, alebo moze ist o vyraz
	if ((c = get_token (f, s)) != identificator)
		{
			if (c == RET_ERR)
	return RET_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			//Zistime ci moze ist o vyraz
			if (!
	  ((c == variable) || (c == const_true) || (c == const_false)
	   || (c == const_null) || (c == int_num) || (c == double_num)
	   || (c == string_literal) || (c == left_parent)))
	return SYNT_ERR;
			//Zavolame analyzu vyrazu
			if ((retV = StateExpression ()) != SYNT_OK)
	{
	  if (retV == RET_ERR)
	    return RET_ERR;

	  if (retV == LEX_ERR)
	    return LEX_ERR;
	  if (retV == SYNT_ERR)
	    return SYNT_ERR;
	  if (retV == SEM_ERR)
	    return SEM_ERR;
	}
			//Instrukcia priradenia vysledku vyrazu
			if (GenerateInstruction
	  (INST_LOAD, "ExpressionResult", NULL, LValueItem->Key) == RET_ERR)
	return RET_ERR;

		}
	else
		{				// Ide o volanie funkcie
			if ((retV = StateFunctionCall ()) != SYNT_OK)
	{
	  if (retV == RET_ERR)
	    return RET_ERR;
	  if (retV == LEX_ERR)
	    return LEX_ERR;
	  if (retV == SYNT_ERR)
	    return SYNT_ERR;
	  if (retV == SEM_ERR)
	    return SEM_ERR;
	  if (retV == SEM_ERR_MISS_PAR)
	    return SEM_ERR_MISS_PAR;
	}
			// Priradenie navratovej hodnoty
			if ((RetValueItem = stSearchV (VTableRootGlob, "ReturnValue")) == NULL)
	return RET_ERR;

			//Vygenerujeme instrukciu, ktorou sa ziska navratova hodnota funkcie
			if (GenerateInstruction
	  (INST_GTOLLOAD, RetValueItem, NULL, LValueItem->Key) == RET_ERR)
	return RET_ERR;

		}
	if (c != semicolon)
		{
			if (c == RET_ERR)
	return RET_ERR;

			if (c == LEX_ERR)
	return LEX_ERR;

			return SYNT_ERR;
		}

	return SYNT_OK;		//Syntax priradenia je v poriadku
}


// Jedna sa o volanie funkcie v programe
int
StateFunctionCall ()
{
	int counter = 0;
	t_stItemF HelpItem, HelpItem2, FunctionPtr;
	t_stItemV CurrentOperand, CurrentTarget;
	char *parameters;
//Otestujeme ci sa funkcia nachadza v tabulke
	if ((HelpItem = stSearchF (FTableRootGlob, s->strg)) == NULL)
		//Funkcia sa nenachadza v tabulke, je treba ju tam pridat
		{
			if (stInsertF (FTableRootGlob, s->strg, 0, NULL, NULL) == RET_ERR)
	return RET_ERR;

			if ((HelpItem2 = stSearchF (FTableRootGlob, s->strg)) == NULL)
	return RET_ERR;
			FunctionPtr = HelpItem2;

		}
	else
		{				//Funkcia sa v tabulke nachadza
			FunctionPtr = HelpItem;
		}
	//Musi nasledovat lava zatvorka a dalej vstupne parametre
	if ((c = get_token (f, s)) != left_parent)
		{
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;

			return SYNT_ERR;

		}
	while ((c = get_token (f, s)) != LEX_ERR)
		{
			if (c == RET_ERR)
	return RET_ERR;

			if ((c == right_parent) && counter == 0)	//Zoznam parametrov je prazdny
	{
	  if (HelpItem != NULL)	//funkcia bola pred tym definovana
	    {
	      if (counter < (HelpItem->paramCount))	//pocet parametrov je mensi ako minimaly zisteny pocet
		{
		  if ((HelpItem->instListPtr) != NULL)
		    {		//Semanticka chyba, funkcia bola definovana
		      //a volame ju s mensim poctom parametrov
		      return SEM_ERR_MISS_PAR;

		    }
		  else
		    {		//Ulozime najmensi zisteny pocet parametrov
		      HelpItem->paramCount = counter;
		    }

		}
	    }
	  else			//funkciu volame s 0 parametrami
	    {
	      FunctionPtr->paramCount = 0;
	    }
	  //Vygenerujeme zarazku za parametre
	  if (stInsertV
	      (VTableCurrent, "UNDEF", type_end_par, 0, 0, NULL,
	       false) == RET_ERR)
	    return RET_ERR;

	  if ((parameters = GenerateString (counter + 1)) == NULL)
	    return RET_ERR;

	  if ((CurrentOperand = stSearchV (VTableCurrent, "UNDEF")) == NULL)
	    return RET_ERR;
	  if (stInsertV
	      (VTableRootGlob, parameters, type_null, 0, 0, NULL,
	       false) == RET_ERR)
	    {
	      free (parameters);
	      parameters = NULL;
	      return RET_ERR;
	    }
	  if ((CurrentTarget =
	       stSearchV (VTableRootGlob, parameters)) == NULL)
	    {
	      free (parameters);
	      parameters = NULL;
	      return RET_ERR;
	    }
	  free (parameters);
	  parameters = NULL;

	  if (GenerateInstruction
	      (INST_LTOGLOAD, CurrentOperand->Key, NULL,
	       CurrentTarget) == RET_ERR)
	    return RET_ERR;
	  if (GenerateInstruction (INST_CALL, NULL, NULL, FunctionPtr) ==
	      RET_ERR)
	    return RET_ERR;
	  c = get_token (f, s);	//nacitame dalsi token pre volajucu funkciu
	  return SYNT_OK;	//V poriadku (0 parametrov)
	}
			counter++;		//zvysime pocidaldo zistenych parametrov
			// podmienka, ktora zistuje ci sa jedna o platny
			// vstupny paremeter funkcie
			if ((c == variable) || (c == int_num) || (c == double_num) ||
	  (c == string_literal) || (c == const_null) || (c == const_true) ||
	  (c == const_false))
	{
	  switch (c)
	    {

// JEDNA SA O PARAMETER NAZVU PREMENNEJ
	    case variable:
	      //test ci vstupna premenna bola deklarovana

	      if (stInsertV
		  (VTableCurrent, s->strg, type_undef, 0, 0, NULL,
		   false) == RET_ERR)
		return RET_ERR;
	      CurrentOperand = stSearchV (VTableCurrent, s->strg);
	      // Polozka sa nenachadza v lokalnej TS
	      // Nikdy pred tym nebola pouzita ako L-hodnota a preto nie je mozne
	      // aby bola definovana

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_undef, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
//vygenerovanie instrukcie, kde sa ulozi parameter n
// do tabulky symbolov s nazvom "par"+"n"
	      free (parameters);
	      parameters = NULL;
//Pouzijeme instrukciu na presun z lokalnej do globalnej tabulky symbolov
	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;
	      break;

// JEDNA SA O PARAMETER INTEGER
// Zvysok je na rovnakom principe ako parameter premennej
	    case int_num:

	      if (stInsertV
		  (VTableCurrent, s->strg, type_int, atoi (s->strg), 0, NULL,
		   false) == RET_ERR)
		return RET_ERR;

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;

	      if ((CurrentOperand =
		   stSearchV (VTableCurrent, s->strg)) == NULL)
		return RET_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      free (parameters);
	      parameters = NULL;

	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;

	      break;

	    case double_num:

	      if (stInsertV
		  (VTableCurrent, s->strg, type_double, 0, atof (s->strg),
		   NULL, false) == RET_ERR)
		return RET_ERR;

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;

	      if ((CurrentOperand =
		   stSearchV (VTableCurrent, s->strg)) == NULL)
		return RET_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      free (parameters);
	      parameters = NULL;

	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;


	      break;

	    case string_literal:

	      if (stInsertV
		  (VTableCurrent, s->strg, type_string, 0, 0, s->strg,
		   false) == RET_ERR)
		return RET_ERR;

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;

	      if ((CurrentOperand =
		   stSearchV (VTableCurrent, s->strg)) == NULL)
		return SEM_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      free (parameters);
	      parameters = NULL;

	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;



	      break;

	    case const_null:

	      if (stInsertV
		  (VTableCurrent, s->strg, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		return RET_ERR;

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;

	      if ((CurrentOperand =
		   stSearchV (VTableCurrent, s->strg)) == NULL)
		return SEM_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      free (parameters);
	      parameters = NULL;

	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;




	      break;

	    case const_true:

	      if (stInsertV
		  (VTableCurrent, s->strg, type_boolean, 0, 0, NULL,
		   true) == RET_ERR)
		return RET_ERR;

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;

	      if ((CurrentOperand =
		   stSearchV (VTableCurrent, s->strg)) == NULL)
		return SEM_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      free (parameters);
	      parameters = NULL;

	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;



	      break;
	    case const_false:

	      if (stInsertV
		  (VTableCurrent, s->strg, type_boolean, 0, 0, NULL,
		   false) == RET_ERR)
		return RET_ERR;

	      if ((parameters = GenerateString (counter)) == NULL)
		return RET_ERR;

	      if ((CurrentOperand =
		   stSearchV (VTableCurrent, s->strg)) == NULL)
		return SEM_ERR;
	      if (stInsertV
		  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
		   false) == RET_ERR)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      if ((CurrentTarget =
		   stSearchV (VTableRootGlob, parameters)) == NULL)
		{
		  free (parameters);
		  parameters = NULL;
		  return RET_ERR;
		}
	      free (parameters);
	      parameters = NULL;

	      if (GenerateInstruction
		  (INST_LTOGLOAD, CurrentOperand->Key, NULL,
		   CurrentTarget) == RET_ERR)
		return RET_ERR;



	      break;

	    default:
	      return SYNT_ERR;

	    }
	  //Zistili sme platny parameter
	  // Musi nasledovat ciarka alebo prava zatvorka
	  if ((c = get_token (f, s)) != comma)
	    {
	      if (c == LEX_ERR)
		return LEX_ERR;
	      if (c == RET_ERR)
		return RET_ERR;

	      if (c == right_parent)
		{
		  if ((c = get_token (f, s)) == semicolon)
		    {
		      if (HelpItem != NULL)
			{
			  if ((HelpItem->instListPtr) != NULL)
			    {	//funkcia bola definovana a sucastne ma menej parametrov
			      //jedna sa o semanticku chybu
			      if (counter < HelpItem->paramCount)
				return SEM_ERR_MISS_PAR;

			    }
			  else
			    {	//Ulozime najmensi zisteny pocet parametrov
			      if (counter < HelpItem->paramCount)
				HelpItem->paramCount = counter;

			    }

			}
		      else
			{
			  FunctionPtr->paramCount = counter;
			}

//NAKONIEC VLOZIME UKONCOVACI PARAMETER TYPU type_end_par
		      if (stInsertV
			  (VTableCurrent, "UNDEF", type_end_par, 0, 0, NULL,
			   false) == RET_ERR)
			return RET_ERR;

		      if ((parameters = GenerateString (counter + 1)) == NULL)
			return RET_ERR;

		      if ((CurrentOperand =
			   stSearchV (VTableCurrent, "UNDEF")) == NULL)
			return RET_ERR;
		      if (stInsertV
			  (VTableRootGlob, parameters, type_null, 0, 0, NULL,
			   false) == RET_ERR)
			{
			  free (parameters);
			  parameters = NULL;
			  return RET_ERR;
			}
		      if ((CurrentTarget =
			   stSearchV (VTableRootGlob, parameters)) == NULL)
			{
			  free (parameters);
			  parameters = NULL;
			  return RET_ERR;
			}
		      free (parameters);
		      parameters = NULL;

		      if (GenerateInstruction
			  (INST_LTOGLOAD, "UNDEF", NULL,
			   CurrentTarget) == RET_ERR)
			return RET_ERR;

		      //Syntakticka analyza prebehla v poriadku
		      //Nagenerujeme si instrukciu, ktora vola danu funkciu
		      if (GenerateInstruction
			  (INST_CALL, NULL, NULL, FunctionPtr) == RET_ERR)
			return RET_ERR;


		      return SYNT_OK;

		    }
		  else
		    {
		      if (c == LEX_ERR)
			return LEX_ERR;
		      return SYNT_ERR;

		    }
		}
	      else
		{
		  return SYNT_ERR;
		}
	    }
	  continue;
	}
			return SYNT_ERR;
		}
	return LEX_ERR;

}


int
StateIf ()
{
	tInstListItem *JumpInst;
	int RetV;
	//pravidlo if -> "(" -> EXPRESSION -> ")" -> "{"
	if ((c = get_token (f, s)) != left_parent)
		{
			if (c == RET_ERR)
	return RET_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			return SYNT_ERR;
		}
	c = get_token (f, s);
	if (c == RET_ERR)
		return RET_ERR;
	if (c == LEX_ERR)
		return LEX_ERR;
	//jedna sa o prazdnu podmienku, skace sa vzdy
	if (c == right_parent)
		{


			//Prazdny prikaz IF vygenerujeme priamy skok
			if (GenerateInstruction (INST_DJUMP, NULL, NULL, NULL) == RET_ERR)
	return RET_ERR;

			JumpInst = InstListCurrent->Last;

			if ((c = get_token (f, s)) != left_braces)
	{
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  return SYNT_ERR;
	}

			return StateIfCore (JumpInst);	//Zaciatok je syntakticky v poriadku

		}


	//vykona sa syntakticka analyza vyrazu 
	RetV = StateExpression ();	// VYRAZ
	if (RetV == LEX_ERR)
		return LEX_ERR;
	if (RetV == SYNT_ERR)
		return SYNT_ERR;
	if (RetV == SEM_ERR)
		return SEM_ERR;
	if (RetV == RET_ERR)
		return RET_ERR;

	//Vytvorime instrukciu, kde void *result==(Ukazatel na prvok v tabulke, kde sa nachadza vysledok vyrazu)
	//                          void *addr1==(Ukazatel na label pre false)
	//                          void *addr2==NULL
	if (GenerateInstruction (INST_JUMP, NULL, NULL, "ExpressionResult") ==
			RET_ERR)
		return RET_ERR;
	//V JumpInst sa ulozi pointer na instrukciu INST_JUMP
	//aby sa do nej vedeli dopisat ukazatele na Labely
	JumpInst = InstListCurrent->Last;

	if (c != right_parent)
		{
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			return SYNT_ERR;
		}

	if ((c = get_token (f, s)) != left_braces)
		{
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			return SYNT_ERR;
		}
	return StateIfCore (JumpInst);	//Zaciatok je syntakticky v poriadku

}

// Dostali sme sa do tela podmienky if
// Prikazy v tele mozu zacinat na ID, Keyword, "}"
int
StateIfCore (tInstListItem * JumpInst)
{
	while ((c = get_token (f, s)) != RET_ERR)
		{
			//NONTERMINAL STATEMENTS
			switch (c)
	{
	case variable:
	  if ((c = StateID ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  break;
	case kw_if:
	  if ((c = StateIf ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;

	case semicolon:
	  break;

	case kw_while:
	  if ((c = StateWhile ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;

	case kw_return:
	  if (StateReturn () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  if (c != semicolon)
	    return SYNT_ERR;

	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;
	  break;

	case right_braces:
	  return StateElse (JumpInst);	// Opustame sekvenciu prikazov IF
	  break;


	case EOF:
	  return SYNT_ERR;
	case LEX_ERR:
	  return LEX_ERR;
	  break;
	default:
	  return SYNT_ERR;	//Syntakticka chyba
	}
		}

	return RET_ERR;



}

int
StateElse (tInstListItem * JumpInst)
{
// Pravidlo else -> "{" -> sek.prikazov -> "}"

	tInstListItem *JumpInst2;

	if ((c = get_token (f, s)) != kw_else)
		{
			if (c == RET_ERR)
	return RET_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			return SYNT_ERR;
		}
	if ((c = get_token (f, s)) != left_braces)
		{
			if (c == RET_ERR)
	return RET_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			return SYNT_ERR;
		}
	// Vygenerujeme instrukciu, ktorou skocime za vetvu "else"
	// teda ju preskocime
	if (GenerateInstruction (INST_DJUMP, NULL, NULL, NULL) == RET_ERR)
		return RET_ERR;
	JumpInst2 = InstListCurrent->Last;
	// Label, na ktory sa skace v pripade vetvy "else"
	if (GenerateInstruction (INST_LABEL, NULL, NULL, NULL) == RET_ERR)
		return RET_ERR;

	(JumpInst->instruction).adr1 = InstListCurrent->Last;

	return StateElseCore (JumpInst2);
}

int
StateElseCore (tInstListItem * JumpInst2)
{

	//NONTERMINAL STATEMENTS
	while ((c = get_token (f, s)) != RET_ERR)
		{
			switch (c)
	{
	case variable:
	  if ((c = StateID ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  break;
	case kw_if:
	  if ((c = StateIf ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;

	case kw_while:
	  if ((c = StateWhile ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;

	case semicolon:
	  break;

	case kw_return:
	  if (StateReturn () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  if (c != semicolon)
	    return SYNT_ERR;

	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;
	  break;

	case LEX_ERR:
	  return LEX_ERR;
	  break;

	case right_braces:
	  //Label na ktory sa skace po vetve "if" je to posledna instrukcia vo vetve else
	  if (GenerateInstruction (INST_LABEL, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;

	  (JumpInst2->instruction).adr1 = InstListCurrent->Last;


	  //Koniec uplneho vetvenia,
	  return SYNT_OK;	//ktore je syntakticky spravne
	  break;


	case EOF:
	  return SYNT_ERR;

	default:
	  return SYNT_ERR;	//Syntakticka chyba
	}

		}
	return RET_ERR;

}

int
StateWhile ()
{
	tInstListItem *JumpInstCont, *JumpInstBreak;
	int RetV;
//Pravidlo while -> "(" -> vyraz -> ")" -> "{" -> sek.prikazov -> "}"
	if ((c = get_token (f, s)) != left_parent)
		{
			if (c == RET_ERR)
	return RET_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			return SYNT_ERR;
		}
	if ((c = get_token (f, s)) == LEX_ERR)
		return LEX_ERR;
	if (c == RET_ERR)
		return RET_ERR;

	//Vytvori sa Label na ktory sa bude skakat na konci cyklu
	//za ktorym nasleduju instrukcie vyhodnotenia podmienky    
	if (GenerateInstruction (INST_LABEL, NULL, NULL, NULL) == RET_ERR)
		return RET_ERR;
	JumpInstCont = InstListCurrent->Last;

	if (c == right_parent)	//Prazdna podmienka cyklus sa vzdy preskoci
		{
			if ((c = get_token (f, s)) != left_braces)
	{
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  return SYNT_ERR;
	}

			//Priamy skok za podmienku
			if (GenerateInstruction (INST_DJUMP, NULL, NULL, NULL) == RET_ERR)
	return RET_ERR;
			JumpInstBreak = InstListCurrent->Last;
			return StateWhileCore (JumpInstCont, JumpInstBreak);
		}

//vyhodnoti sa vyraz

	RetV = StateExpression ();

	if (RetV == SYNT_ERR)
		return SYNT_ERR;
	if (RetV == LEX_ERR)
		return LEX_ERR;
	if (RetV == SEM_ERR)
		return SEM_ERR;
	if (RetV == RET_ERR)
		return RET_ERR;

	if (c != right_parent)
		{
			if (c == SEM_ERR)
	return SEM_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			return SYNT_ERR;
		}
	if ((c = get_token (f, s)) != left_braces)
		{
			if (c == SEM_ERR)
	return SEM_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			return SYNT_ERR;
		}
	if (GenerateInstruction (INST_JUMP, NULL, NULL, "ExpressionResult") ==
			RET_ERR)
		return RET_ERR;
	JumpInstBreak = InstListCurrent->Last;



	return StateWhileCore (JumpInstCont, JumpInstBreak);


}

int
StateWhileCore (tInstListItem * JumpInstCont, tInstListItem * JumpInstBreak)
{
	//NONTERMINAL STATEMENTS
	while ((c = get_token (f, s)) != RET_ERR)
		{
			switch (c)
	{
	case variable:
	  if ((c = StateID ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;

	  break;
	case kw_if:
	  if ((c = StateIf ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  break;

	case kw_while:
	  if (StateWhile () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  break;

	case kw_return:
	  if (StateReturn () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  if (c != semicolon)
	    return SYNT_ERR;

	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;
	  break;
	case semicolon:
	  break;

	case LEX_ERR:
	  return LEX_ERR;
	  break;
	case right_braces:

	  //Vygeneruje sa instrukcia ktora skace spat na podmienku vzdy ked sa
	  //dostaneme program do tela cyklu a vola sa posledna v cykle
	  if (GenerateInstruction (INST_DJUMP, JumpInstCont, NULL, NULL) ==
	      RET_ERR)
	    return RET_ERR;

	  //Vytvori sa label na ktory program skace v pripade konca cyklu
	  //Teda hned po nesplneni podmienky cyklu
	  if (GenerateInstruction (INST_LABEL, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;

	  (JumpInstBreak->instruction).adr1 = InstListCurrent->Last;





	  return SYNT_OK;	// Opustame sekvenciu prikazov iteracie
	  break;		// Syntakticky je to v poriadku


	default:
	  return SYNT_ERR;	//Syntakticka chyba
	}
		}
	return RET_ERR;		//scanner vratil lexikalnu chybu





}

// Jedna sa o deklaraciu funkcie
int
StateFunction ()
{
	t_stItemV CurrentOperand, CurrentTarget;
	t_stItemF FuncPtr;
	tInstList *InstListptr;
	int counter = 0;
	char *parameters;
// Pravidlo function -> Identifikator -> zoznam parametrov -> "{" -> sek.prikazov -> "}"
	if ((c = get_token (f, s)) != identificator)
		{
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			return SYNT_ERR;
		}

////////////////////////////////////INICIALIZACIE DATOVYCH STRUKTUR PRE FUNKCIE

	if ((FuncPtr = stSearchF (FTableRootGlob, s->strg)) != NULL)
		{
			if ((FuncPtr->instListPtr) != NULL)
	return SEM_ERR;		//DVOJITA DEFINICIA FUNKCIE
			//Funkcia uz bola volana ale zatial nebola inicializovana

		}
	else
		{				//funkcia este nebola volana
			//Vytvori sa zaznam o symbole v tabulke
			if (stInsertF (FTableRootGlob, s->strg, INT_MAX, NULL, NULL) == RET_ERR)
	return RET_ERR;

			if ((FuncPtr = stSearchF (FTableRootGlob, s->strg)) == NULL)
	return RET_ERR;

		}				//Vytvori sa zoznam instrukcii danej funkcie

	if ((InstListptr = (tInstList *) malloc (sizeof (tInstList))) == NULL)
		return RET_ERR;
	// Zoznam sa inicializuje
	listInit (InstListptr);
	// Nakoniec sa ukazatel na jeho strukturu ulozi do tabulky symbolov
	// ako atribut funkcie

	InstListCurrent = InstListptr;
	stInitV (&CurrentTablePtr);
	VTableCurrent = &CurrentTablePtr;
	FuncPtr->instListPtr = InstListptr;

//
/////////////////////CAST FUNKCIE S PARAMETRAMI
	if ((c = get_token (f, s)) != left_parent)
		{
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			return SYNT_ERR;
		}
	while ((c = get_token (f, s)) != LEX_ERR)
		{
			if (c == RET_ERR)
	return RET_ERR;

			if ((c == right_parent) && counter == 0)	//Zoznam parametrov je prazdny
	{
	  FuncPtr->paramCount = counter;
	  return StateFunctionCore (FuncPtr);
	}
			counter++;
			if (c == variable)
	{
	  //Uspesne sme identifikovali parameter je treba
	  //tuto skutocnost obsluzit instrukciami, ktore inicializuju
	  //Tabulku symbolov funkcie, teda predaju parametre

	  if (stSearchV (VTableCurrent, s->strg) != NULL)	//Parameter uz bol definovany
	    return SEM_ERR_OTHER;

	  if (stInsertV
	      (VTableCurrent, s->strg, type_undef, 0, 0, NULL,
	       false) == RET_ERR)
	    return RET_ERR;

	  CurrentTarget = stSearchV (VTableCurrent, s->strg);
	  if ((parameters = GenerateString (counter)) == NULL)
	    return RET_ERR;
	  if (stInsertV
	      (VTableRootGlob, parameters, type_undef, 0, 0, NULL,
	       false) == RET_ERR)
	    {
	      free (parameters);
	      parameters = NULL;
	      return RET_ERR;
	    }
	  if ((CurrentOperand =
	       stSearchV (VTableRootGlob, parameters)) == NULL)
	    {
	      free (parameters);
	      parameters = NULL;
	      return RET_ERR;
	    }
	  free (parameters);
	  parameters = NULL;
	  // Vygenerujeme instrukciu, ktora presunie hodnotu z priameho ukazatela
	  // na prvok v globalnej tabulke symbolov do prvku v lokalnej tabulke
	  // ktory je predany ako nazov prvku
	  if (GenerateInstruction
	      (INST_GTOLLOAD, CurrentOperand, NULL,
	       CurrentTarget->Key) == RET_ERR)
	    return RET_ERR;


	  if ((c = get_token (f, s)) != comma)
	    {			//jedna sa o koniec parametrov?

	      if (c == right_parent)	//Uspesne sme identifikovali parametre
		{
		  if ((FuncPtr->paramCount) < counter)	//funkcia bola niekde volana
		    return SEM_ERR_MISS_PAR;	//s mensim poctom parametrov

		  FuncPtr->paramCount = counter;

		  return StateFunctionCore (FuncPtr);
		}
	      else
		{
		  if (c == LEX_ERR)
		    return LEX_ERR;
		  if (c == RET_ERR)
		    return RET_ERR;
		  return SYNT_ERR;
		}
	    }
	  continue;
	}
			return SYNT_ERR;
		}
	return LEX_ERR;

}

int
StateFunctionCore (t_stItemF FunctionPtr)
{
	//NONTERMINAL STATEMENTS

	if ((c = get_token (f, s)) != left_braces)
		{
			if (c == RET_ERR)
	return RET_ERR;
			if (c == LEX_ERR)
	return LEX_ERR;
			return SYNT_ERR;
		}

	while ((c = get_token (f, s)) != RET_ERR)
		{
			switch (c)
	{
	case semicolon:
	  break;
	case variable:
	  if ((c = StateID ()) == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;
	case kw_if:
	  if (StateIf () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;

	case kw_while:
	  if (StateWhile () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;
	  break;

	case LEX_ERR:
	  return LEX_ERR;
	  break;

	case kw_return:
	  if (StateReturn () == SYNT_ERR)
	    return SYNT_ERR;
	  if (c == RET_ERR)
	    return RET_ERR;
	  if (c == SEM_ERR)
	    return SEM_ERR;
	  if (c == LEX_ERR)
	    return LEX_ERR;

	  if (c != semicolon)
	    return SYNT_ERR;

	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;
	  break;

	case right_braces:;
	  //Vygenerujeme navratovu hodnotu NULL
	  t_stItemV ReturnPointer;
	  //Vytorime polozku NULL
	  if (stInsertV (VTableCurrent, "NULL", type_null, 0, 0, NULL, false)
	      == RET_ERR)
	    return RET_ERR;
	  //Vytvorime polozku navratovej adresy
	  if (stInsertV
	      (VTableRootGlob, "ReturnValue", type_undef, 0, 0, NULL,
	       false) == RET_ERR)
	    return RET_ERR;
	  if ((ReturnPointer =
	       stSearchV (VTableRootGlob, "ReturnValue")) == NULL)
	    return RET_ERR;
	  //Vygenerujeme instrukciu, ktora na navratovu
	  //adresu vlozi hodnotu NULL
	  if (GenerateInstruction (INST_LTOGLOAD, "NULL", NULL, ReturnPointer)
	      == RET_ERR)
	    return RET_ERR;

	  if (GenerateInstruction (INST_RETURN, NULL, NULL, NULL) == RET_ERR)
	    return RET_ERR;


	  //Hmmm nejaka magia s pointrami
	  FunctionPtr->stItemVPtr = *VTableCurrent;
	  VTableCurrent = VTableRootGlob;
	  InstListCurrent = InstListGlob;

	  return SYNT_OK;	// Opustame sekvenciu prikazov funkcie
	  break;		// Syntakticky je to v poriadku


	default:
	  return SYNT_ERR;	//Syntakticka chyba
	}
		}
	return RET_ERR;

}

int
StateReturn ()
{
	int RetV;
	t_stItemV ReturnPointer;
	if ((c = get_token (f, s)) != semicolon)
		{
			if (c == LEX_ERR)
	return LEX_ERR;
			if (c == RET_ERR)
	return RET_ERR;
			//Vyhodnotime vyraz za return               
			RetV = StateExpression ();
			if (RetV == LEX_ERR)
	return LEX_ERR;
			if (RetV == RET_ERR)
	return RET_ERR;
			if (RetV == SYNT_ERR)
	return SYNT_ERR;
			if (RetV == SEM_ERR)
	return SEM_ERR;

			if (stInsertV
	  (VTableRootGlob, "ReturnValue", type_undef, 0, 0, NULL,
	   false) == RET_ERR)
	return RET_ERR;
			if ((ReturnPointer = stSearchV (VTableRootGlob, "ReturnValue")) == NULL)
	return RET_ERR;

			if (GenerateInstruction
	  (INST_LTOGLOAD, "ExpressionResult", NULL, ReturnPointer) == RET_ERR)
	return RET_ERR;
			return SYNT_OK;
		}

	if (stInsertV (VTableCurrent, "NULL", type_null, 0, 0, NULL, false) ==
			RET_ERR)
		return RET_ERR;
	//Vytvorime polozku navratovej adresy
	if (stInsertV (VTableRootGlob, "ReturnValue", type_undef, 0, 0, NULL, false)
			== RET_ERR)
		return RET_ERR;
	if ((ReturnPointer = stSearchV (VTableRootGlob, "ReturnValue")) == NULL)
		return RET_ERR;
	//Vygenerujeme instrukciu, ktora na navratovu
	//adresu vlozi hodnotu NULL
	if (GenerateInstruction (INST_LTOGLOAD, "NULL", NULL, ReturnPointer) ==
			RET_ERR)
		return RET_ERR;

	return SYNT_OK;
}


int
StateExpression ()
{				// Zacatek vyhodnocovani vyrazu
	tDLList L;
	DLInitList (&L);		// inicializace dvousmerneho seznamu
	DLInsertFirst (&L, semicolon, NULL, NULL, NULL);	// vlozeni prvniho prvku 
	DLFirst (&L);			// aktivita na prvnim prvku
	int result;
	if ((result = (StateExpressionAnalysis (&L))) != SYNT_OK)	// Inicializace hotova, probiha vyhodnocovani...
		{
			DLDisposeList (&L);
			if (result == SYNT_ERR)
	return SYNT_ERR;
			else if (result == SEM_ERR)
	return SEM_ERR;
			else if (result == LEX_ERR)
	return LEX_ERR;
			else
	return RET_ERR;

		}
	DLDisposeList (&L);
	return SYNT_OK;
}

int
StateExpressionAnalysis (tDLList * L)
{
	int i, j, x, evaluator, type;	// i je index radku ve staticke tabulce, j je index sloupce
	// evaluator je promenna pro vyhodnocovani vyrazu, urcuje ktere pravidlo bylo pouzito
	// type je promenna pro cteni 'typu instrukce' z dvousmerneho seznamu
	static int index = 0;		// index mezivysledku
	double y;
	tInst top;
	t_stItemV var_pointer0 = NULL;	// pointery pro manipulaci se seznamy
	t_stItemV var_pointer1 = NULL;
	t_stItemV var_pointer2 = NULL;
	char *str;			// pomocna promenna pri prirazovani nazvu mezivysledkovym hodnotam pri pridani do TS
	DLCopy (L, &top);

	while (1)
		{
			switch (top.type)
	{			// Vrchol zasobniku
	case multiply:
	case divide:
	  i = 0;
	  break;
	case plus:
	case minus:
	  i = 1;
	  break;
	case concatenate:
	  i = 2;
	  break;
	case greater:
	case less:
	case greater_equal:
	case less_equal:
	  i = 3;
	  break;
	case equal:
	case not_equal:
	  i = 4;
	  break;
	case left_parent:
	  i = 5;
	  break;
	case right_parent:
	  i = 6;
	  break;
	case semicolon:
	  i = 7;
	  break;
	case variable:
	case const_null:
	case const_false:
	case const_true:
	case int_num:
	case double_num:
	case string_literal:
	  i = 8;
	  break;
	default:
	  return SYNT_ERR;
	}

			switch (c)
	{			// Aktualni vstup
	case multiply:
	case divide:
	  j = 0;
	  break;
	case plus:
	case minus:
	  j = 1;
	  break;
	case concatenate:
	  j = 2;
	  break;
	case greater:
	case less:
	case greater_equal:
	case less_equal:
	  j = 3;
	  break;
	case equal:
	case not_equal:
	  j = 4;
	  break;
	case left_parent:
	  j = 5;
	  break;
	case right_parent:
	  j = 6;
	  break;
	case semicolon:
	  j = 7;
	  break;
	case variable:
	case const_null:
	case const_false:
	case const_true:
	case int_num:
	case double_num:
	case string_literal:
	  j = 8;
	  break;
	default:
	  return SYNT_ERR;
	}

			switch (table[i][j])	// vyhledani odpovidajici hodnoty ve staticke tabulce
	{
	case LEFT_ARROW:
	  DLCopy (L, &top);
	  if (c == variable || (c >= const_true && c <= const_null)
	      || c == int_num || c == double_num || c == string_literal)
	    {
	      if (c != variable)
		{		// Pokud to neni promenna, je potreba tuto konstantu pridat do tabulky jako lokalni promennou
		  /*
		     V tabulce se vytvori nova polozka, jejiz klic je ziskan prevodem konstanty na jeji
		     retezcovou reprezentaci. Pokud je napriklad potreba pridat konstantu s hodnotou 10,
		     je do tabulky pridana polozka s klicem "10" a hodnotou 10. Toto by delalo problemy 
		     v pripade vyskytu numerical stringu ve vysledne vystupni tabulce, tento pripad je
		     osetren pozdeji.
		   */
		  if (c == const_true)
		    {
		      if ((stInsertV
			   (VTableCurrent, s->strg, type_boolean, 0, 0, NULL,
			    true)) == RET_ERR)
			return RET_ERR;
		    }
		  else if (c == const_false)
		    {
		      if ((stInsertV
			   (VTableCurrent, s->strg, type_boolean, 0, 0, NULL,
			    false)) == RET_ERR)
			return RET_ERR;
		    }
		  else if (c == const_null)
		    {
		      if ((stInsertV
			   (VTableCurrent, s->strg, type_null, 0, 0, NULL,
			    false)) == RET_ERR)
			return RET_ERR;
		    }
		  else if (c == int_num)
		    {
		      x = atoi (s->strg);
		      if ((stInsertV
			   (VTableCurrent, s->strg, type_int, x, 0, NULL,
			    false)) == RET_ERR)
			return RET_ERR;
		    }
		  else if (c == double_num)
		    {
		      y = atof (s->strg);
		      if ((stInsertV
			   (VTableCurrent, s->strg, type_double, 0, y, NULL,
			    false)) == RET_ERR)
			return RET_ERR;
		    }
		  else if (c == string_literal)
		    {
		      if ((stInsertV
			   (VTableCurrent, s->strg, type_string, 0, 0,
			    s->strg, false)) == RET_ERR)
			return RET_ERR;
		    }
		  var_pointer0 = stSearchV (VTableCurrent, s->strg);
		}
	      else
		{
		  if ((stInsertV
		       (VTableCurrent, s->strg, type_undef, 0, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  var_pointer0 = stSearchV (VTableCurrent, s->strg);

		}
	      while (top.type == INST_ASSIGN)
		{		// navraceni aktivniho prvku pred neterminalni znaky pro spravne vlozeni '<'
		  DLPred (L);
		  DLCopy (L, &top);
		}
	      // vlozeni '<'
	      DLPostInsert (L, LEFT_ARROW, NULL, NULL, NULL);
	      // na konec dvousmerneho seznamu je vlozena prazdna instrukce odkazujici do tabulky symbolu na prislusnou konstantu
	      DLInsertLast (L, INST_NOP, NULL, NULL, &var_pointer0);
	    }
	  else
	    {			// pokud to neni promenna ani cislo, je to znamenko
	      while (top.type == INST_ASSIGN)
		{
		  DLPred (L);
		  DLCopy (L, &top);
		}
	      DLPostInsert (L, LEFT_ARROW, NULL, NULL, NULL);
	      // na konec dvousmerneho seznamu je vlozena instrukce indikujici znamenko
	      DLInsertLast (L, c, NULL, NULL, NULL);
	    }
	  DLLast (L);
	  DLCopy (L, &top);	// aktivita je zpatky na poslednim prvku seznamu, muzeme nacist dalsi znak
	  if (c != semicolon)
	    {
	      c = get_token (f, s);
	      if (c == LEX_ERR)
		return LEX_ERR;
	      if (c == RET_ERR)
		return RET_ERR;
	    }
	  break;


	case RIGHT_ARROW:
	  // pro korektni vyhodnoceni je na konec seznamu vlozena instrukce '>'
	  DLInsertLast (L, RIGHT_ARROW, NULL, NULL, NULL);
	  if ((evaluator = StateExpressionEvaluation (L)) == SYNT_ERR)	// overeni splneni nektereho z pravidel
	    {
	      return SYNT_ERR;
	    }
	  DLPred (L);
	  DLPostDelete (L);	// sipka vpravo smazana
	  DLCopy (L, &top);
	  // pokud existuje jiz jiny mezivysledek typu string, je treba jej uvolnit pred nahranim nove hodnoty
	  var_pointer1 = stSearchV (VTableCurrent, "ExpressionResult");
	  if (var_pointer1 != NULL)
	    {			// existuje
	      if (var_pointer1->Content.Type == type_string && var_pointer1->Content.Data.sData != NULL)	// je typu string
		free (var_pointer1->Content.Data.sData);	// je uvolnen, typ nastaven na undef
	      var_pointer1->Content.Type = type_undef;
	    }
	  if (evaluator == 1)
	    {			// evaluator == 1 znaci pravidlo C -> i, jelikoz v seznamu je na konci posloupnost znaku: <i>
	      switch (var_pointer0->Content.Type)
		{		// jaky je typ identifikatoru?
		case type_boolean:	// bool konstanta
		  if ((stInsertV
		       (VTableCurrent, var_pointer0->Key,
			var_pointer0->Content.Type, 0, 0, NULL,
			var_pointer0->Content.Data.bData)) == RET_ERR)
		    return RET_ERR;
		  if ((stInsertV
		       (VTableCurrent, "ExpressionResult",
			var_pointer0->Content.Type, 0, 0, NULL,
			var_pointer0->Content.Data.bData)) == RET_ERR)
		    return RET_ERR;
		  break;
		case type_int:	// int konstanta
		  if ((stInsertV
		       (VTableCurrent, var_pointer0->Key,
			var_pointer0->Content.Type,
			var_pointer0->Content.Data.iData, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  if ((stInsertV
		       (VTableCurrent, "ExpressionResult",
			var_pointer0->Content.Type,
			var_pointer0->Content.Data.iData, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  break;
		case type_double:	// double konstanta
		  if ((stInsertV
		       (VTableCurrent, var_pointer0->Key,
			var_pointer0->Content.Type, 0,
			var_pointer0->Content.Data.dData, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  if ((stInsertV
		       (VTableCurrent, "ExpressionResult",
			var_pointer0->Content.Type, 0,
			var_pointer0->Content.Data.dData, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  break;
		case type_string:	// string literal konstanta
		  /* Zde je potreba generovat nazev mezivysledku kvuli "numerical strings". Pokud by existovala napriklad
		     konstanta typu integer s hodnotou 10 a klicem "10", a v programu by pozdeji byla pouzita konstanta
		     typu string s hodnotou "10", tak by prepsala integerovou konstantu. Proto se pro string konstanty
		     generuji specialni nazvy ("GEN0", "GEN1", ...). Pote se generovane nazvy uvolni.
		   */
		  str = GenerateStringExpr (index);
		  if ((stInsertV
		       (VTableCurrent, str, var_pointer0->Content.Type, 0, 0,
			var_pointer0->Content.Data.sData, false)) == RET_ERR)
		    {
		      free (str);
		      return RET_ERR;
		    }
		  if ((stInsertV
		       (VTableCurrent, "ExpressionResult",
			var_pointer0->Content.Type, 0, 0,
			var_pointer0->Content.Data.sData, false)) == RET_ERR)
		    {
		      free (str);
		      return RET_ERR;
		    }
		  var_pointer2 = stSearchV (VTableCurrent, str);
		  free (str);
		  index++;
		  break;
		case type_null:	// null konstanta
		  if ((stInsertV
		       (VTableCurrent, var_pointer0->Key,
			var_pointer0->Content.Type, 0, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  if ((stInsertV
		       (VTableCurrent, "ExpressionResult",
			var_pointer0->Content.Type, 0, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  break;
		case type_undef:	// type undef - jedna se o promennou
		  if ((stInsertV
		       (VTableCurrent, var_pointer0->Key,
			var_pointer0->Content.Type, 0, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  if ((stInsertV
		       (VTableCurrent, "ExpressionResult",
			var_pointer0->Content.Type, 0, 0, NULL,
			false)) == RET_ERR)
		    return RET_ERR;
		  break;
		default:
		  return RET_ERR;
		}
	      DLPred (L);
	      DLPostDelete (L);	// smazani '<' ve dvousmernem seznamu
	      DLCopy (L, &top);


	      //  Rozpoznana instrukce je nakonec vlozena na konec obou seznamu. 
	      if (var_pointer0->Content.Type != type_string)
		{
		  var_pointer1 = stSearchV (VTableCurrent, var_pointer0->Key);
		  DLInsertLast (L, INST_ASSIGN, var_pointer0, NULL,
				var_pointer1->Key);
		  if ((GenerateInstruction
		       (INST_ASSIGN, var_pointer0->Key, NULL,
			var_pointer1->Key)) == RET_ERR)
		    return RET_ERR;
		}
	      else
		{
		  var_pointer1 = stSearchV (VTableCurrent, var_pointer2->Key);
		  DLInsertLast (L, INST_ASSIGN, var_pointer0, NULL,
				var_pointer1->Key);
		  if ((GenerateInstruction
		       (INST_ASSIGN, var_pointer2->Key, NULL,
			var_pointer1->Key)) == RET_ERR)
		    return RET_ERR;
		}


	    }
	  else if (evaluator == 3)
	    {			// jinymy slovy pravidlo: C -> C operator C
	      if ((str = GenerateStringExpr (index)) == NULL)	// klic pro mezivysledek do tabulky symbolu je generovan
		return RET_ERR;
	      index++;
	      // seznam je nyni postupne cten od konce aby byly nacteny operatory a typ instrukce

	      // nacten druhy operator dane operace v tabulce symbolu
	      var_pointer2 = stSearchV (VTableCurrent, (char *) top.result);
	      DLPred (L);
	      DLPostDelete (L);	// smazan v dvousmernem seznamu 
	      DLCopy (L, &top);

	      // nacten typ instrukce (scitani, odcitani, relace,...)
	      type = top.type;
	      DLPred (L);
	      DLPostDelete (L);	// smazan v dvousmernem seznamu
	      DLCopy (L, &top);

	      // nacten druhy operator dane operace v tabulce symbolu
	      var_pointer0 = stSearchV (VTableCurrent, (char *) top.result);
	      DLPred (L);
	      DLPostDelete (L);	// smazan v dvousmernem seznamu
	      DLCopy (L, &top);

	      if ((stInsertV (VTableCurrent, str, type_null, 0, 0, NULL, 0))
		  == RET_ERR)
		{		// vlozeni mezivysledku do tabulky symboly promennych 
		  free (str);
		  return RET_ERR;
		}
	      var_pointer1 = stSearchV (VTableCurrent, str);	// nasledne nalezeni, pro ucel tvorby instrukce
	      // nasledne je instrukce vlozena do obou seznamu
	      DLInsertLast (L, type, var_pointer0, var_pointer2,
			    var_pointer1->Key);
	      GenerateInstruction (type, var_pointer0->Key, var_pointer2->Key,
				   var_pointer1->Key);
	      free (str);
	    }
	  var_pointer0 = NULL;
	  // Smazani '<'
	  if (top.type == LEFT_ARROW)
	    {
	      DLPred (L);
	      DLPostDelete (L);
	    }

	  // priprava dvousmerneho seznamu pro prijem dalsiho znaku
	  DLSucc (L);
	  DLActualize (L, INST_ASSIGN);
	  type = top.type;
	  DLLast (L);
	  DLCopy (L, &top);
	  while (top.type == INST_ASSIGN)
	    {
	      DLPred (L);
	      DLCopy (L, &top);
	    }
	  break;


	case EQUALITY:		// k leve zavorce nalezena prava, ani se nemusi vkladat do seznamu
	  DLLast (L);
	  DLCopy (L, &top);
	  if (top.type == left_parent && c == right_parent)	// pokud je ve vyrazu toto: '()' jedna se o SYNT_ERR
	    return SYNT_ERR;
	  while (top.type == INST_ASSIGN)
	    {
	      DLPred (L);
	      DLCopy (L, &top);
	    }
	  DLPred (L);
	  DLPostDelete (L);	// smazani '('
	  DLPred (L);
	  DLPostDelete (L);	// smazani '<'

		// priprava dvousmerneho seznamu pro prijem dalsiho znaku
		while (top.type == INST_ASSIGN)
			{
				DLPred (L);
				DLCopy (L, &top);
			}
		DLCopy (L, &top);
		// nacteni dalsiho znaku
		if (c != semicolon)
		{
			c = get_token (f, s);
			if (c == LEX_ERR)
				return LEX_ERR;
			if (c == RET_ERR)
				return RET_ERR;
		}
		break;


	case EXCLAMATION:	// nepovolena posloupnost znaku, syntakticka chyba
		return SYNT_ERR;


	case DOLLAR_SIGN:	// vyraz korektni, vysledek je v tabulce symbolu pod klicem "ExpressionResult"
		GenerateInstruction (INST_LOAD, var_pointer1->Key, NULL, "ExpressionResult");
		return SYNT_OK;


	default:
		return SYNT_ERR;
	}
			if (!
		((c >= const_true && c <= const_null) || c == variable
		|| c == int_num || c == double_num || c == string_literal
		|| c == concatenate || c == equal || c == not_equal || (c >= less
		&& c <= greater)
		|| (c >= plus && c <= divide) || c == left_parent
		|| c == right_parent || c == semicolon))
	{			// pokud dalsi nacteny znak neni korektni, vracena syntakticka chyba
		return SYNT_ERR;
	}
		}
	// vyraz korektni, vysledek je v tabulce symbolu pod klicem "ExpressionResult"
	GenerateInstruction (INST_LOAD, var_pointer1->Key, NULL, "ExpressionResult");
	return SYNT_OK;
}

int
StateExpressionEvaluation (tDLList * L)
{				// funkce vyhodnocuje, jestli je mozne aplikovat pravidlo
	tInst ev;			// pomocna promenna pro pohyb v dvousmernem seznamu
	DLLast (L);
	DLCopy (L, &ev);
	// nejdrive se aktivni prvek posune za '<'
	while (ev.type != LEFT_ARROW)
		{
			DLPred (L);
			DLCopy (L, &ev);
		}
	DLSucc (L);
	DLCopy (L, &ev);

/*
**     PRAVIDLO            C -> C operator C
*/
	if (ev.type == INST_ASSIGN)
		{				// prvni znak musi byt nonterminal
			DLSucc (L);
			DLCopy (L, &ev);
			if ((ev.type >= plus && ev.type <= divide) || ev.type == concatenate
			|| ev.type == equal || ev.type == not_equal || (ev.type >= less
			&& ev.type <= greater))
	{			// dalsim znakem musi byt operator
		DLSucc (L);
		DLCopy (L, &ev);
		if (ev.type == INST_ASSIGN)
		{			// posledni znak opet nonterminal
				DLSucc (L);
				DLCopy (L, &ev);
				if (ev.type == RIGHT_ARROW)	// a nakonec '>', pravidlo splneno, je mozne provest redukci
		return 3;	// identifikator pouziteho pravidla
				else
		return SYNT_ERR;
			}
		else
			return SYNT_ERR;
	}
			else
	return SYNT_ERR;
		}
/*
**     PRAVIDLO            C -> i
*/
	else if (ev.type == variable
		|| (ev.type >= const_true && ev.type <= const_null)
		|| ev.type == int_num || ev.type == double_num
		|| ev.type == string_literal)
		{				// prvnim znakem je terminal
			DLSucc (L);
			DLCopy (L, &ev);
			if (ev.type == RIGHT_ARROW)	// nasleduje '>', pravidlo splneno, je mozne provest redukci
	{
	  return 1;		// identifikator pouziteho pravidla
	}
			else
	return SYNT_ERR;
		}
/*
**     PRAVIDLO            C -> (C)
*/
	else if (ev.type == left_parent)
		{				// prvnim znakem je '('
			DLSucc (L);
			DLCopy (L, &ev);
			if (ev.type == INST_ASSIGN)
	{			// nasleduje nonterminal
		DLSucc (L);
		DLCopy (L, &ev);
		if (ev.type == right_parent)	// poslednim znakem je ')', pravidlo splenno, je mozne provest redukci
			return 2;		// identifikator pouziteho pravidla
		else
			return SYNT_ERR;
	}
			else
	return SYNT_ERR;
		}
	else
		return SYNT_ERR;
}

// Konec! :-)
