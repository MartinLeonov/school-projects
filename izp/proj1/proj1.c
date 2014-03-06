/* 
 * Projekt c.1 - Histogram
 * Autor:   Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Skupina: 11 (1BIA)
 * Datum:   19.10.2012
 * Soubor:  proj1.c
 * Projekt: Tvorba histogramu, projekt c. 1 pro predmet IZP
 *
 * Popis:   Program vypocitava cetnost znaku, ktere se vyskytuji ve
 * vstupnim textu, a nasledne vypise prehled danych znaku s jejich cetnosti
 * Program take pracuje s pripadnym parametrem, kterym uzivatel udava
 * kolik radku chce vypsat.
 */

/* Knihovna pro vstup a vystup */
#include <stdio.h>
/* Standartni funkce jazyka C */
#include <stdlib.h>
/* Kvuli porovnavaci operaci strcmp */
#include <string.h>
/* Kvuli operaci isprint, isdigit */
#include <ctype.h>

const int UNS_CHAR = 256;

/* Vytvoreni funkcnich prototypu */
void napoveda(void);
int prevodParametru(char *parametr);
void nulovaniPole(unsigned char pole[UNS_CHAR]);
void pripraveniAscii(unsigned char ascii[UNS_CHAR]);
void spocitaZnaky(unsigned char pole[UNS_CHAR]);
void vypisePole(unsigned char pole[UNS_CHAR]);
void seradiPole(unsigned char pole[UNS_CHAR], unsigned char pole_ascii[UNS_CHAR]);
void vypiseSerazenePole(unsigned char pole[], unsigned char ascii[], int parametr);

int main(int argc, char *argv[])
{
    int parametr;
    unsigned char pole[UNS_CHAR];

    /*
     * Porovna pripadny parametr a pote budto prevede a nasledne vypise znaky podle cetnosti,
     * Vypise napovedu a nebo chybove hlaseni. Jsou-li zadany dva parametry, vypise se chybove hlaeni.
     * V pripade ze neni zadany parametr, pokracuje sectenim znaku a vypsani podle cisla znaku v ACII
     */
    if (argc == 2) /* Overuje, zda je zadán jeden parametr */
     {
        /* Je-li parametrem je -h nebo --help, vypise napovedu */
        if ((argc == 2 && strcmp(argv[1], "-h") == 0) || (argc == 2 && strcmp(argv[1], "--help") == 0))
        {
            napoveda();
            return 0;
        }
        else
        {
            parametr = prevodParametru(argv[1]);
            /* Pokud je parametr v intervalu 1 az UNS_CHAR, spocitaji se znaky, a vypise znaky serazene podle cetnosti */
            if ((parametr >= 1) && (parametr <= UNS_CHAR))
            {
                /* Definovani pole, ve kterém budou uloženy pozice prohozených znaků, viz radek 243 */
                unsigned char ascii[UNS_CHAR];

                nulovaniPole(pole);
                pripraveniAscii(ascii);
                spocitaZnaky(pole);
                seradiPole(pole, ascii);
                printf("\n");
                vypiseSerazenePole(pole, ascii, parametr);
            }
            else
            {
                /* Pokud je zadan parametr mimo interval, vypise se chybove hlaseni */
                fprintf(stderr, "Byla zadaná hodnota, se kterou nelze pracovat\n");
                return 1;
            }
        }
    }
    else if (argc > 2)
    {
        /* Pokud je parametru vice nez 1, vypise se chybove hlaseni */
        fprintf(stderr, "Byl zadán chybný počet parametrů\n");
        return 1;
    }
    else
    {
        /* Paklize neni zadany zadny parametr, program secte znaky a vypise je podle cisla znaku v ASCII */
        nulovaniPole(pole);
        spocitaZnaky(pole);
        printf("\n");
        vypisePole(pole);
    }
}

void napoveda(void)
/* Funkce, ktera slouzi k vypsani napovedy */
{
    printf("Program:	proj1.c\n"
            "Autor:		Roman Blanco, xblanc01@stud.fit.vutbr.cz\n"
            "Popis:		Program pro vypis cetnosti znaku\n"
            "Použití:	proj1 [PREPINAC] [CISLO]\n\n"
            "Popis parametrù:\n"
            "[CISLO] - parametr, ktery udava kolik radku se ma vypsat\n"
            "	- je mozne zadat hodnty 1 az %d\n\n"
            "Prepinace napovedy:\n"
            "  -h, --help		Vypíše tuto obrazovku s nápovědou.\n", UNS_CHAR);
}

int prevodParametru(char *parametr)
/* Prevede znakovy retezec na dane cislo */
{
    int i = 0;

    for (int a = 0; parametr[a] != '\0'; a++)
        /* Cyklus, ktery se bude opakovat, dokud v poli nedojde na ukoncovaci nulu */
    {
        /*
         * K i se priradi hodnota i vynasobena deseti,
         * zvetsena o rozdil ciselne hodnoty znaku na pozici 'a' v poli 'parametr'
         * a ciselne hodnoty znaku 0 (-> 48)
         */
        if (isdigit(parametr[a])) /* Kontrola, zda je parametr cislice */
        {
            i = i * 10 + parametr[a] - '0';
        }
        else
        {
            return -1;
        }
    }
    return i;
}

void nulovaniPole(unsigned char pole[UNS_CHAR])
/* vynuluje pole, aby bylo zajisteno, ze se budou znaky pricitat od 0 */
{
    int i;

    for (i = 0; i < UNS_CHAR; i++)
    {
        pole[i] = 0;
    }
}

void pripraveniAscii(unsigned char ascii[UNS_CHAR])
/* nastavi hodnoty v poli, aby mohlo být použito po serazeni na identifikaci znaku */
{
    int i;

    for (i = 0; i < UNS_CHAR; i++)
    {
        ascii[i] = i;
    }
}

void spocitaZnaky(unsigned char pole[])
/* spocita znaky (umisteni v poli = desitkova hodnota v ascii tabulce) */
{
    int c;

    while ((c = getchar()) != EOF)
        /* Cyklus, ktery pricita znaky, dokud nedojde na EOF (konec vstupu) */
    {
        if (pole[c] <= UNS_CHAR - 2)
        {
            /*
             * Hodnoty se budou přičítat, dokud nebude počet znaků větší než UNS_CHAR - 2,
             * pote priradi znaku pocet UNS_CHAR - 1 (pri vypisu misto cisla UNS_CHAR - 1 bude zobrazeno "NNN")
             */
            pole[c]++;
        }
        else
        {
            pole[c] = UNS_CHAR - 1;
        }
    }
}

void vypisePole(unsigned char pole[])
{
    /*
     * v pripade, ze byl program spusten bez parametru,
     *  se vypise pole podle desitkove hodnoty v ascii
     */
    for (int i = 0; i < UNS_CHAR; i++)
    {
        /* Zjisteni, zda je znak tisknutelny */
        if (isprint(i))
        {
            /*
             * Pokud je soucet znaku jinehodnoty nez UNS_CHAR - 1,
             * vypise se cislo, jinak bude vypsano "NNN" (r. 203)
             */
            if (pole[i] != UNS_CHAR - 1)
            {
                /* Pokud bude soucet znaku rovny 0, nebude znak vypsany */
                if (pole[i] != 0)
                {
                    printf("%d '%c' %u\n", i, i, pole[i]);
                }
            }
            else
            {
                printf("%d '%c' NNN\n", i, i);
            }
        }
        else
        {
            if (pole[i] != UNS_CHAR - 1)
            {
                if (pole[i] != 0)
                {
                    printf("%d: %u\n", i, pole[i]);
                }
            }
            else
            {
                printf("%d: NNN\n", i);
            }
        }
    }
}

void seradiPole(unsigned char pole[], unsigned char ascii[])
 {
    /* Serazeni pole podle cetnosti */
    int i, j;

    for (i = UNS_CHAR - 1; i > 0; i--)
    {
        for (j = 0; j < i; j++)
        {
            /*
             * Pokud bude hodnota v naslednem poli vetsi nez v aktualnim,
             *  hodnoty v poli se preradi
             */
            if (pole[j] < pole[j + 1])
            {
                int docasna;
                /* Serazeni pole s pomoci docasne promenne */
                docasna = pole[j];
                pole[j] = pole[j + 1];
                pole[j + 1] = docasna;
                /*
                 * Protoze prehozenim hodnot v poli dojde k problemu,
                 * ze pozice v poli nebude udavat cislo znaku v ASCII,
                 * je vytvorene pole cislovane od 0 do UNS_CHAR - 1,
                 * kde se budou cisla prehazovat zaroven pri prehozeni souctu znaku
                 */
                docasna = ascii[j];
                ascii[j] = ascii[j + 1];
                ascii[j + 1] = docasna;
            }
        }
    }
}

void vypiseSerazenePole(unsigned char pole[], unsigned char ascii[], int parametr)
 {
    /* Funkce vypise pole serazene podle cetnosti znaku. V pripade, ze byl program spusten s parametrem */

    /*
     * Cyklus, ktery se bude opakovat, dokud nebude hodnota i mensi nez cislo zadane v parametru
     * Vypis je obdobny, jako u funkce ktera vypisuje neserazene pole, rozdil je v tom, ze pri vypisu
     * nesarazeneho pole je znak rozpoznan pozici v poli. Zde je znad rozpoznan podle hodnoty v poli,
     * ktera je na stejne pozici jako prehozeny soucet znaku
     */
    for (int i = 0; i < parametr; i++)
    {
        if (isprint(ascii[i]))
        {
            if (pole[i] != UNS_CHAR - 1)
            {
                if (pole[i] != 0)
                {
                    printf("%d '%c' %u\n", ascii[i], ascii[i], pole[i]);
                }
            }
            else
            {
                printf("%d '%c' NNN\n", ascii[i], ascii[i]);
            }
        }
        else
        {
            if (pole[i] != UNS_CHAR - 1)
            {
                if (pole[i] != 0)
                {
                    printf("%d: %u\n", ascii[i], pole[i]);
                }
            }
            else
            {
                printf("%d: NNN\n", ascii[i]);
            }
        }
    }
}
