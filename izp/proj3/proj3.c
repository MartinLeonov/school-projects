/*
 * Projekt c.2 - Reseni osmismerky
 * Autor:   Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Skupina: 11 (1BIA)
 * Datum:   21.11.2012
 * Soubor:  proj3.c
 *
 * Popis:
 * Ukolem programu je najit a oznacit v osmismerce vsechna zadana slova.
 * Zbyle neoznacene znaky tvori tajenku, ktera se cte postupne zleva
 * doprava po radcich shora dolu
 */

// VLOZENI HLAVICKOVYCH SOUBORU ////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>

// VYTVORENI STRUKTUR A KONSTANT ///////////////////////////////////////////////

typedef struct {int row; int column; char *arrayCW; char *words;} Tstructure;

enum tecodes
{
 EOK = 0,
 ECLWRONG,
 EUNKNOWN,
 EFREADCW,
 EFREADW,
 EFREADNUM,
 EMALLOC,
 EWRONGCW,
 ELETT,
};

const char const *ecode[] =
{
 [EOK] = "Vse je OK",
 [ECLWRONG] = "Chybne parametry prikazoveho radku\n",
 [EUNKNOWN] = "Nastala neznama chyba\n",
 [EFREADCW] = "Soubor s matici se nepodarilo otevrit\n",
 [EFREADW] = "Soubor se slovy se nepodarilo otevrit\n",
 [EFREADNUM] = "Nepodarilo se nacist cisla\n",
 [EMALLOC] = "Chyba pri alokaci pameti\n",
 [EWRONGCW] = "Osmismerka neni spravne zapsana\n",
 [ELETT] = "Pocet zapsanych znaku neodpovida rozmerum osmismerky\n",
};

const char const *HELPMSG =

  "Projekt 3 - Reseni osmismerky\n"
  "Autor: Roman Blanco\n"
  "Program:   Projekt 3. - Reseni osmismerky\n"
  "Autor:     Roman Blanco, xblanc01@stud.fit.vutbr.cz\n"
  "Popis:     Program pro vypis cetnosti znaku\n"
  "Pouziti:   ./proj3 [PREPINAC] [SOUBOR] [SOUBOR]\n\n"
  "Popis parametru:\n\n"
  "  -h                      - Vypise tuto obrazovku s napovedou.\n"
  "  --test [file]           - Otestovani spravnosti osmismerky v souboru\n"
  "  --search=[word] [file]  - Vyhledani slova v osmismerce zapsane v souboru\n"
  "  --solve [file] [file]  - Vyreseni osmismerky zapsane v souboru\n"
;

// FUNKCNI PROTOTYPY ///////////////////////////////////////////////////////////

int readCrossword(FILE *soubor, int *row, int *column);
int getMatrix(FILE *soubor, Tstructure *strct);
void printMatrix(Tstructure *strct);
void getWords(FILE *soubor, Tstructure *strct, int maxSize);
int searchWord(Tstructure *strct, char *tf_str, int size);

// HLAVNI FUNKCE - MAIN ////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  FILE *soubor; // ukazatel na soubor s datovym typem FILE
  int row, column, maxSize, size, errnum;
  Tstructure strct;
  char *str;

  if ((argc >= 2) && (argc <= 4))
  {
    if (argc == 2)
    {
      if (strcmp(argv[1], "-h") == 0)
      {
        printf("%s", HELPMSG); // napoveda
        return 0;
      }
      else
      {
        fprintf(stderr,"%s", ecode[ECLWRONG]); // chyba, spatne parametry
        return 1;
      }
    }
    else if (argc == 3)
    {
      if (strcmp(argv[1], "--test") == 0)

// OTESTOVANI OSMISMERKY ///////////////////////////////////////////////////////

      {

        // NACTENI SOUBORU S OSMISMERKOU ///////////////////////////////////////

        soubor = fopen(argv[2], "r"); // otevreni souboru s pravy read
        if (soubor == NULL)  // kontrola nacteni (NULL->*soubor nikam neukazuje)
        {
          fprintf(stderr,"%s", ecode[EFREADCW]);
          return 1;
        }

        // NACTENI OSMISMERKY DO STRUKTURY /////////////////////////////////////

        errnum = readCrossword(soubor, &row, &column);
        if (errnum != EOK) // pokud nastala chyba
        {
          fprintf(stderr,"%s", ecode[errnum]); // nepodarilo se nacist cisla
          return 1;
        }

        strct.row = row;       // prirazeni radku (row) do struktury
        strct.column = column; // prirazeni sloupcu (column) do struktury
        strct.arrayCW = NULL;

        /* alokuje pole o velikosti sloupce * radky*/
        strct.arrayCW = malloc(strct.row * strct.column);

        if ((strct.arrayCW == NULL))
        {
          fprintf(stderr,"%s", ecode[EMALLOC]); // nepodarilo se alokovat misto
          return 1;
        }

        errnum = getMatrix(soubor, &strct); // nacteni matice ze souboru do pole
        if (errnum != EOK) // pokud nastala chyba
        {
          fprintf(stderr,"%s", ecode[errnum]); // nepodarilo se nacist cisla
          return 1;
        }

        fclose(soubor); // uzavreni souboru

        // VYPSANI OSMISMERKY //////////////////////////////////////////////////

        printMatrix(&strct); // vypsani matice (ve spravnem formatu)
        free(strct.arrayCW); // uvolneni alokovane pameti

        return 0;
      }

      else if (strncmp(argv[1], "--search=", 9) == 0)

// VYHLEDANI 1 SLOVA ///////////////////////////////////////////////////////////

      {

        str = argv[1]+9; // nacteni hledaneho slova
        size = strlen(str);


        // PREVOD CH NA # //////////////////////////////////////////////////////

        char *tf_str;
        int p = 0;
        int q = 0;

        tf_str = malloc(size*sizeof(char));

        if ((tf_str == NULL))
        {
          fprintf(stderr,"%s", ecode[EMALLOC]); // nepodarilo se alokovat
          return EMALLOC;
        }

        while (str[p] != '\0')
        {
          if ((str[p] == 104) && (str[p-1] == 99))
          {
            tf_str[q-1] = 35;
            p++;
          }
          if ((str[p] >= 97) && (str[p] <= 122))
          {
            tf_str[q] = str[p];
            q++;
            p++;
          }
          else
          {
            fprintf(stderr,"%s", ecode[ECLWRONG]); // chyba, spatne parametry
            return 1;
          }
        }

        size = strlen(tf_str); // prepocitani nove delky (s CH -> # se meni)

        // NACTENI SOUBORU S OSMISMERKOU ///////////////////////////////////////

        soubor = fopen(argv[2], "r"); // otevreni souboru s pravy read
        if (soubor == NULL)  // kontrola nacteni (NULL->*soubor nikam neukazuje)
        {
          fprintf(stderr,"%s", ecode[EFREADCW]); // chyba v nacteni souboru
          return 1;
        }

        // NACTENI OSMISMERKY DO STRUKTURY /////////////////////////////////////

        errnum = readCrossword(soubor, &row, &column);
        if (errnum != EOK) // pokud nastala chyba
        {
          fprintf(stderr,"%s", ecode[errnum]); // nepodarilo se nacist cisla
          return 1;
        }

        strct.row = row;       // prirazeni radku (row) do struktury
        strct.column = column; // prirazeni sloupcu (column) do struktury
        strct.arrayCW = NULL;

        /* alokuje pole o velikosti sloupce * radky*/
        strct.arrayCW = malloc(strct.row * strct.column);

        if ((strct.arrayCW == NULL))
        {
          fprintf(stderr,"%s", ecode[EMALLOC]); // nepodarilo se alokovat
          return 1;
        }

        errnum = getMatrix(soubor, &strct); // nacteni matice ze souboru do pole
        if (errnum != EOK) // pokud nastala chyba
        {
          fprintf(stderr,"%s", ecode[errnum]); // nepodarilo se nacist cisla
          return 1;
        }

        fclose(soubor); // uzavreni souboru

        // NALEZENI SLOVA A VYPSANI V OSMISMERCE ///////////////////////////////

        searchWord(&strct, tf_str, size);
        printMatrix(&strct);

        free(strct.arrayCW); // uvolneni alokovane pameti
        free(tf_str);

        return 0;
      }
      else
      {
        fprintf(stderr,"%s", ecode[ECLWRONG]); // chyba, spatne parametry
        return 1;
      }
    }
    else if (argc == 4)
    {
      if (strcmp(argv[1], "--solve") == 0)

// VYRESENI OSMISMERKY /////////////////////////////////////////////////////////

      {

        // NACTENI SOUBORU S OSMISMERKOU ///////////////////////////////////////

        soubor = fopen(argv[2], "r"); // otevreni souboru s pravy read
        if (soubor == NULL)  // kontrola nacteni (NULL->*soubor nikam neukazuje)
        {
          fprintf(stderr,"%s", ecode[EFREADCW]); // chyba v nacteni souboru
          return 1;
        }

        // NACTENI OSMISMERKY DO STRUKTURY /////////////////////////////////////

        errnum = readCrossword(soubor, &row, &column);
        if (errnum != EOK) // pokud nastala chyba
        {
          fprintf(stderr,"%s", ecode[errnum]); // nepodarilo se nacist cisla
          return 1;
        }

        strct.row = row;       // prirazeni radku (row) do struktury
        strct.column = column; // prirazeni sloupcu (column) do struktury
        strct.arrayCW = NULL;

        /* alokuje pole o velikosti sloupce * radky*/
        strct.arrayCW = malloc(strct.row * strct.column);

        if ((strct.arrayCW == NULL))
        {
          fprintf(stderr,"%s", ecode[EMALLOC]); // nepodarilo se alokovat
          return 1;
        }

        errnum = getMatrix(soubor, &strct); // nacteni matice ze souboru do pole
        if (errnum != EOK) // pokud nastala chyba
        {
          fprintf(stderr,"%s", ecode[errnum]); // nepodarilo se nacist cisla
          return 1;
        }

        fclose(soubor); // uzavreni souboru

        // VYPSANI OSMISMERKY //////////////////////////////////////////////////

        printMatrix(&strct); // vypsani matice (ve spravnem formatu)

        // NACTENI SOUBORU SE SLOVY ////////////////////////////////////////////

        soubor = fopen(argv[3], "r");
        if (soubor == NULL) // kontrola, jestli je soubor spravne nacteny
        {
          fprintf(stderr,"%s", ecode[EFREADW]);
          return 1;
        }

        // NACTENI SLOV ZE SOUBORU A ULOZENI DO STRUKTURY //////////////////////

        strct.words = NULL;

        if (strct.row > strct.column) //maxSize = nejvetsi moznou velikost slova
        { maxSize = strct.row; }
        else
        { maxSize = strct.column; }

        /*alokuje pole o velikosti maximalni velikosti slova*/
        strct.words = malloc(maxSize * sizeof(char));

        getWords(soubor, &strct, maxSize); // nacteni slov ze souboru do pole
        fclose(soubor); // uzavreni souboru

        return 0;
      }
      else
      {
        fprintf(stderr,"%s", ecode[ECLWRONG]); // chyba, spatne parametry
        return 1;
      }
    }
  }
  else
  {
    fprintf(stderr,"%s", ecode[ECLWRONG]); // chyba, spatne parametry
    return 1;
  }
}

// FUNKCE NACTE POCET RADKU / SLOUPCU //////////////////////////////////////////

/**
 * Funkce nacte ze souboru pocet radku a pocet sloupcu zadane osmismerky.
 * Pokud se cisla nepodari nacist, (pocet nactenych cisel neni roven 2),
 * funkce vypise chybove hlaseni
 */

int readCrossword(FILE *soubor, int *row, int *column)
{
  int read = fscanf(soubor, "%d %d", row, column);

  if (read != 2) // pokud pocet zapsanych cisel neni roven 2, vypise se chyba
    { return EFREADNUM; }
  else
    { return EOK; }
}

// FUNKCE NACTE OSMISMERKU /////////////////////////////////////////////////////

/**
 * Funkce nacte ze souboru jednotlive pismena z matice, a ulozi je
 * do promenne arrayCW ve strukture.
 * Rozlisuje se "ch" a "c h"
 */

int getMatrix(FILE *soubor, Tstructure *strct)
{
  int i, ch;
  int temp = 0;
  for (i = 0; ((ch = fgetc(soubor)) != EOF); )

  {
    if ((ch == 104) && (temp == 99)) // je-li nacteny znak = "h" a predchozi "c"
    {
      strct->arrayCW[i-1] = 35; // zapsani "#" na misto kde ma byt "ch"
    }
    else
    {
      if ((ch >= 97) && (ch <= 122)) // zapsani jen pokud je znak male pismeno
      {
        if  (( (temp >= 97) && (temp <= 122) ) && ((ch >= 97) && (ch <= 122)))
        { // pokud byly pismena zapsane bez mezery -> chyba
          return EWRONGCW;
        }
        else
        {
          strct->arrayCW[i] = ch;
          i++;
        }
      }
    }
    temp = ch; // ulozi se hodnota minuleho precteneho znaku (kvuli ch)
  }

  /* kontrola ze rozmery odpovidaji poctu zapsanych pismen */
  if (i != (strct->row * strct->column))
    { return ELETT; }
  else
    { return EOK; }

}

// FUNKCE VYPISE OSMISMERKU ////////////////////////////////////////////////////

/**
 * Funkce vypise ze strct->arrayCW pismena. Pokud pri cteni z pole narazi na
 * znak "#" (35), vypise misto nej "ch "
 *
 */

void printMatrix(Tstructure *strct)
{
  printf("%d %d", strct->row, strct->column);
  for (int i = 0; i < (strct->row * strct->column); i++)
  {
    if ((i % strct->column) == 0)  // i / slpupce = cele cislo -> konec radku
    {
      printf("\n");
    }

    if (strct->arrayCW[i] == 35) // pokud je zapsan # nahradi se pri vypisu "ch"
    {
      printf("ch ");
    }
    else if (strct->arrayCW[i] == 36) // pokud je zapsan $ nahradi se "CH"
    {
      printf("CH ");
    }
    else
    {
      printf("%c ", strct->arrayCW[i]);
    }
  }
  printf("\n");
}

// NACTENI SLOV ZE SOUBORU /////////////////////////////////////////////////////

/**
 * Funkce nacte ze souboru jednotlive pismena z matice, a ulozi je
 * do promenne words ve strukture strct. Pri kazde iteraci se take
 * alokuje misto pro nove slovo
 */

void getWords(FILE *soubor, Tstructure *strct, int maxSize)
{
  int i;

  for ( i = 0; (fscanf(soubor, "%s%*[\n]", &strct->words[i])) != EOF  ; i++)
  {
    printf("%s\n", &strct->words[i]); // TODO + FIXME: ch & male pismena

    strct->words =
    realloc(strct->words, (maxSize + strlen(&strct->words[i]) + 1));
  }
}

// VYHLEDAVANI V MATICI ////////////////////////////////////////////////////////

/**
* Kdyz funkce nacte znak, ktery je stejny jako prvni znak hledaneho slova,
* nejdrive otestuje, zda je pokracovanim hledane slovo, pak az vyznaci slovo v
* osmismerce.
*
* (TODO: (?) Misto 8 smeru mit 4 a slovo prevracet)
*
* Vysvetleni podminek na konci
*/

int searchWord(Tstructure *strct, char *tf_str, int size)
{
  int i, j = 0;
  int index;
  int temp_index, temp_j;
  bool match;

  for (i = 0; i < (strct->row * strct->column); i++)
  {
    if (strct->arrayCW[i] == tf_str[j])
    {

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI DOPRAVA //////////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if (!((temp_index +1) % strct->column == 0))
        { temp_index += 1; } // posun doprava
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if (!((index + 1) % strct->column == 0))
          { index += 1; } // posun doprava
        }
      }

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI DOLU /////////////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if (!(((strct->row - 1) * strct->column) <= temp_index))
        { temp_index += strct->column; } // posun dolu
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {

          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] += 1) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if (!(((strct->row - 1) * strct->column) <= index))
          { index += strct->column; } // posun dolu
        }
      }

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI DOLEVA ///////////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if (!((temp_index) % strct->column == 0))
        { temp_index -= 1 ; } // posun doleva
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if (!((index) % strct->column == 0))
          { index -= 1 ; } // posun doleva
        }
      }


      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI NAHORU ///////////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if (!(temp_index < strct->column))
        { temp_index -= strct->column ; } // posun nahoru
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if (!(index < strct->column))
          { index -= strct->column ; } // posun nahoru
        }
      }

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI NAHORU DOLEVA/////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if ((!(temp_index < strct->column)) &&
           (!((temp_index) % strct->column == 0)))
        { temp_index = temp_index - strct->column - 1; } // posun doleva nahoru
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if ((!(index < strct->column)) &&
           (!((index) % strct->column == 0)))
          { index = index - strct->column - 1; } // posun doleva nahoru
        }
      }

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI NAHORU DOPRAVA ///////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if ((!(temp_index < strct->column)) &&
           (!((temp_index + 1) % strct->column == 0)))
        { temp_index = temp_index - strct->column + 1; } // posun doprava nahoru
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if ((!(index < strct->column)) &&
           (!((index + 1) % strct->column == 0)))
          { index = index - strct->column + 1; } // posun doprava nahoru
        }
      }

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI DOLU DOPRAVA /////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if ((!(((strct->row - 1) * strct->column) <= temp_index)) &&
           (!((temp_index + 1) % strct->column == 0)))
        { temp_index = temp_index + strct->column + 1; } // posun doprava dolu
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if ((!(((strct->row - 1) * strct->column) <= index)) &&
           (!((index + 1) % strct->column == 0)))
          { index = index + strct->column + 1; } // posun doprava dolu
        }
      }

      // NASTAVENI POCATECNICH HODNOT //////////////////////////////////////////

      j = 0;
      index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
      temp_index = index;
      temp_j = j;
      match = true;

      // NACITANI DOLU DOLEVA //////////////////////////////////////////////////

      for(temp_j = 0; temp_j < size ; temp_j++)
      {
        if (strct->arrayCW[temp_index] == tf_str[temp_j])
        { match *= true; }
        else
        { match *= false; }

        if ((!(((strct->row - 1) * strct->column) <= temp_index)) &&
           (!((temp_index) % strct->column == 0)))
        { temp_index = temp_index + strct->column - 1; } // posun doleva dolu
      }

      if (match == true)
      {
        for (j = 0; j < size ; j++)
        {
          (strct->arrayCW[index] == tf_str[j]) ?
          ((strct->arrayCW[index] == 35) ?
            (strct->arrayCW[index] = 36) :
            (strct->arrayCW[index] -= 32)) :
          0;

          if ((!(((strct->row - 1) * strct->column) <= index)) &&
           (!((index) % strct->column == 0)))
          { index = index + strct->column - 1; } // posun doleva dolu
        }
      }
    }

    // NASTAVENI POCATECNICH HODNOT ////////////////////////////////////////////

    j = 0;
    index = i; // prelozeni i do indexu abych pak mohl dal hledat v matici
    temp_index = index;
    temp_j = j;
    match = true;

  }
  return EOK;
}

/*******************Pohyb v osmismerce v jednorozmernem poli********************

n = pocet sloupcu
m = pocet radku
i = index pismena v poli (!: pole se cisluje od 0):

                             /--------------------\
                             | -n-1 |  -n  | -n+1 |
                             |------|------|------|
                             |  -1  |  +0  |  +1  |
                             |------|------|------|
                             | +n-1 |  +n  | +n+1 |
                             \--------------------/
Omezeni:

Zabraneni posunu doleva pokud : i % n == 0
Zabraneni posunu doprava pokud: (i + 1) % n == 0
Zabraneni posunu nahoru pokud : i < n
Zabraneni posunu dolu pokud   : (m-1) * n <= i

Zabraneni posunu do sikma se urci sloucenim podminek dvou zakladnich smeru

*******************************************************************************/
