/*
 * Projekt c.2 - Iteracni vypocty
 * Autor:   Roman Blanco, xblanc01@stud.fit.vutbr.cz
 * Skupina: 11 (1BIA)
 * Datum:   2.11.2012
 * Soubor:  proj2.c
 *
 * Popis:
 */

//// VKLÁDÁNÍ KNIHOVEN A KONSTANT //////////////////////////////////////////////

#include <stdio.h>   // Knihovna pro vstup a vystup
#include <stdlib.h>  // Standartni funkce jazyka C
#include <math.h>    // Knihovna matematických funkcí - kvůli fabs()
#include <string.h>  // Kvůli funkci strcmp()
#include <stdbool.h> // pro datový typ bool

const double IZP_PI_2 = 1.57079632679489661923; // Konstanta (pi/2)
const double IZP_PI_4 = 0.78539816339744830962; // Konstanta (pi/4)

//// FUNKČNÍ PROTOTYPY /////////////////////////////////////////////////////////

void napoveda(void);
double powxa(double x, float koeficient_eps, float eps, double a);
double arctg(double x, float koeficient_eps, float eps);
double argsinh(double x, float koeficient_eps, float eps);
double ln(double x, float koeficient_eps, float eps);
double odmocnina(double x, float koeficient_eps, float eps);

//// HLAVNÍ FUNKCE /////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    double sigdig;
    float eps = 0.1;
    float koeficient_eps;

    int scan;
    double x;

    double vysledek;

    if ((argc >= 2) && (argc < 5))
    {
        if (argc == 2)
        {
            if (strcmp(argv[1], "-h") == 0)
            {
                napoveda();
                return 0;
            }
            else
            {
                fprintf(stderr, "Byly nesprávně zapsané parametry.\n");
                return 1;
            }
        }
        if (argc == 3)
        {

//// VYPOCET SIGDIG - EPS //////////////////////////////////////////////////////

            sigdig = atoi(argv[2]); // pokud narazí na znak, dál nečte
            for (int i = 0; i < sigdig; i++)
            {
                eps *= 0.1; // pro sigdig n vypocita eps s n+1 desetinnymi misty
            }
            koeficient_eps = eps;

////////////////////////////////////////////////////////////////////////////////

            if (strcmp(argv[1], "--arctg") == 0)
            {
                while ((scan = scanf("%lf", &x)) != EOF)
                {
                    if (scan != 0)
                    {
                        vysledek = arctg(x, koeficient_eps, eps);
                        printf("%.10e\n", vysledek);
                    }
                    else
                    {
                        scanf("%*s");
                        vysledek = arctg(NAN, koeficient_eps, eps);
                        printf("%.10e\n", vysledek);
                    }
                }
            }

            else if (strcmp(argv[1], "--argsinh") == 0)
            {
                while ((scan = scanf("%lf", &x)) != EOF)
                {
                    if (scan != 0)
                    {
                        vysledek = argsinh(x, koeficient_eps, eps);
                        printf("%.10e\n", vysledek);
                    }
                    else
                    {
                        scanf("%*s");
                        vysledek = argsinh(NAN, koeficient_eps, eps);
                        printf("%.10e\n", vysledek);
                    }
                }
            }
            else
            {
                fprintf(stderr, "Byly nesprávně zapsané parametry.\n");
                return 1;
            }
        }
        if (argc == 4)
        {

//// VYPOCET SIGDIG - EPS //////////////////////////////////////////////////////

            sigdig = atoi(argv[2]); // pokud narazí na znak, dál nečte
            for (int i = 0; i < sigdig; i++)
            {
                eps *= 0.1; // pro sigdig n vypocita eps s n+1 desetinnymi misty
            }
            koeficient_eps = eps;

////////////////////////////////////////////////////////////////////////////////

            if (strcmp(argv[1], "--powxa") == 0)
            {
                double a = atof(argv[3]); // pokud narazí na znak, dál nečte

                while ((scan = scanf("%lf", &x)) != EOF)
                {
                    if (scan != 0)
                    {
                        vysledek = powxa(x, koeficient_eps, eps, a);
                        printf("%.10e\n", vysledek);
                    }
                    else
                    {
                        scanf("%*s");
                        vysledek = powxa(NAN, koeficient_eps, eps, a);
                        printf("%.10e\n", vysledek);
                    }
                }
            }
            else
            {
                fprintf(stderr, "Byly nesprávně zapsané parametry.\n");
                return 1;
            }
        }
    }
    else
    {
        fprintf(stderr, "Byly nesprávně zapsané parametry.\n");
        return 1;
    }

    return 0;
}

//// FUNKCE - NÁPOVĚDA /////////////////////////////////////////////////////////

void napoveda(void)
{
    printf("Program:   proj2.c\n"
            "Autor:     Roman Blanco, xblanc01@stud.fit.vutbr.cz\n"
            "Popis:     Program pro vypis cetnosti znaku\n"
            "Použití:   proj2 [PREPINAC] [CISLO1] [CISLO2]\n\n"
            "Popis parametrù:\n\n"
            "  -h                    - Vypíše tuto obrazovku s nápovědou.\n"
            "  --powxa [sigdig] [a]  - Mocninná funkce s realnym exponentem\n"
            "                        - sigdig - přesnost výpočtu\n"
            "                        - a - exponent\n"
            "  --arctg [sigdig]      - Funkce arkustangen\n"
            "                        - sigdig - přesnost výpočt\n"
            "  --argsinh [sigdig]    - Funkce převrácený arkussinus\n"
            "                        - sigdig - přesnost výpočtu\n"
            );
}

//// FUNKCE - MOCNINNA S REALNYM EXPONENTEM ////////////////////////////////////

/*
 * Pro mocninnou funkci s realným exponentem platí: x^a = e^(a*ln(x))
 *
 * Součtová řada pro e^x vypadá: 1 + r + r^2/(2!) + r^3/(3!) + r^4/(4!) + ...
 *
 * za r dosadím (a*ln(x)), a dostanu požadovanou součtovou řadu pro funkci x^a
 * Přírůstek kterým se bude násobit minulé t (tp) pro získání nového t je: (r/i)
 */

double powxa(double x, float koeficient_eps, float eps, double a)
{
    double t = 1.0; // První prvek součtové řady
    double tp = 0.0; // Vytvoření proměnné kam se ukládá T pro porovnání

    double suma = t; // Přičtení prvního prvku do celkové sumy
    double i = 1.0; // Inicializace I, slouží jako faktorial

    double ln_x;
    double r; // Mezivýpočet pro usnadnění výpočtu v cyklu

    if (a == 0.0) // Pokud je exponent nulový ...
    {
        if (x != 0.0) // ... a X není nula, vysledek bude 1;
        {
            suma = 1;
            return suma;
        }
        else // ... a X je nula, -> 0^0 není definovaná -> NAN
        {
            suma = NAN;
            return suma;
        }
    }

    else if  (x == 0.0) // Pokud je X = 0, výsledek  je 0
    {
        suma = 0;
        return suma;
    }
////////////////////////////////////////////////////////////////////////////////
    else if (x < 0)
    {
		suma = NAN;
        return suma; // Vrácení hodnoty proměnné SUMA
    }

    else //výpočet pro X > 0
    {
        ln_x = ln(x, koeficient_eps, eps); // vypočítání logaritmu
        r = a * ln_x; // Dosazení (ř. 191)

        while (fabs(t - tp) > eps) // Porovná přesnost rozdílem T s předchozím T
        {
            tp = t; // Do tp se ulozi hodnota posledního t pro porovnání
            t *= (r / i); // Vynásobení minulého T přírůstkem -> dostanu nové T

            suma += t; // Přičtení nového T do sumy
            i++; // Inkrementace I

            eps = suma * koeficient_eps; // Vypočítání nové přesnosti
        }

        return suma; // Vrácení hodnoty proměnné SUMA
    }
}

//// FUNKCE - ARKUSTANGENS /////////////////////////////////////////////////////

/*
 * Pro funkci arctg platí: arct(x) = PI/2 - 1/(x^1) + 1/(3*x^3) - 1/(5*x^5) +...
 *
 * Pro interval (-1;1) platí jiný součtový rozvoj:
 * x - (x^3)/3 + (x^5)/5 - (x^7)/7 ...
 *
 * Přírůstek, kterým se bude násobit tp pro získání t je: - (i - 2) / (i * x^2)
 */

double arctg(double x, float koeficient_eps, float eps)
{
    double t = 0.0;
    double tp = 0.0;

    double suma = 0.0;
    double i; // i = 3, protože první krok již proběhl (ř. 232)

    double x2; // Mezivýpočet, pro zjednodušení výpočtu v cyklu
    bool zmena;

    if (x == 0) //  {0}
    { return suma; }

	else if (fabs(x) < 1) // (-1;1)
	{
        if (x < 0) // pokud je x v intervalu (-1;0), převedu, a zapisu zmenu
        {
            zmena = 1; // zapsani, ze probehla zmena v znamenku
            x *= -1;
        }

        t = x;

        suma = t;
        i = 3;

        x2 = x * x;

        while(fabs(t - tp) > eps)
        {
            tp = t;
            t *= -1 * ((i - 2) * x2) / i;

            suma += t;
            i += 2;

            eps = suma * koeficient_eps;
        }

        if (zmena == 1)
        { suma *= -1; } // nazpět změním znaménko, pokud došlo ke změně

        return suma;
	}

    else if (fabs(x) == 1) // {-1},{1}
    {
        if (x == 1)
        { suma = 1*IZP_PI_4; } // pokud je X = 1, vysledek je PI/4
        else
        { suma = -1*IZP_PI_4; }

        return suma;
    }

    else // x v (-inf;-1) U (1;inf)
    {
        if (x < 0)
        {
            zmena = 1;
            x *= -1;
        }

		t = -1 / x; // (-1/x) je první člen součtového rozvoje (zde je i = 1)
		tp = 0.0; // Vytvoření proměnné kam se ukládá t pro porovnání

		suma = IZP_PI_2 + t; // PI/2 + (první člen součtového rozvoje)
		i = 3;

		x2 = x * x;

        while (fabs(t - tp) > eps)
        {
            tp = t;
            t *= -(i - 2) / (x2 * i);

            suma += t;
            i += 2;

            eps = suma * koeficient_eps;
        }

        if (zmena == 1)
        { suma *= -1; }

        return suma;
    }


}

//// FUNKCE - PREVRACENY ARGSIN ////////////////////////////////////////////////

/*
 * Pro funkci argsinh platí:
 * argsinh(x) = ln(2x) + 1!!/2*(2!!)*(1/x^2) - 3!!/4*(4!!)*(1/x^4) +
 * + 5!!/6*(6!!)*(1/x^6) ... (př.: 6!! -> dvojnásobný faktorial 6 = 6*4*2)
 *
 * Přírůstek, kterým se bude násobit tp pro získání t je:
 * - ((i - 1) * (i - 2)) / ((i^2) * (x^2))
 */

double argsinh(double x, float koeficient_eps, float eps)
{
    double t; // první člen rozvoje (zde i = 2)
    double tp = 0.0; // Vytvoření proměnné kam se ukládá t pro porovnání

    double suma = 0.0;
    double i = 4.0; // i=4 protože první krok není v součtovém rozvoji (ř. 407)

    double x2 = x * x; // Mezivýpočet, pro zjednodušení výpočtu v cyklu
    bool zmena; // Záznam o změně

    if (x == 0)
    { return suma; }

    else
    {
        if (x < 0)
        {
            zmena = 1;
            x *= -1;
        }

		t = 1 / (4 * x * x); // první prvek rozvoje, (X se mění dle intervalu)
        suma = ln(2 * x, koeficient_eps, eps) + t; // ln(2*x)+(první člen)

        while (fabs(t - tp) > eps)
        {
            tp = t;
            t *= -((i - 1) * (i - 2)) / (i * i * x2);

            suma += t;
            i += 2;

            eps = suma * koeficient_eps;
        }

        if (zmena == 1)
        { suma *= -1; }

        return suma;
    }

}

//// FUNKCE - PRIROZENY LOGARITMUS /////////////////////////////////////////////

/*
 * Pro funkci logaritmus platí: ln(x) =
 * = 2 * ( ([{x-1}/{x+1}]^1/1) + ([{x-1}/{x+1}]^3/3) + ([{x-1}/{x+1}]^5/5) +...)
 *
 * Platí pro x > 0
 *
 * Přírůstek, kterým se bude násobit tp pro získání t je:
 * -((i - 1) * (i - 2)) / ((i^2) * (x^2))
 */

double ln(double x, float koeficient_eps, float eps)
{

	double odm;
    odm = odmocnina(x, koeficient_eps, eps);

    double t = (odm - 1) / (odm + 1); // Výpočet prvního členu (i = 1)
    double tp = 0.0;

    double suma = t; // Do sumy se zapíše hodnota prvního členu
    double i = 3.0; // První krok proběhl (ř. 443) -> i = 3

    double moc_temp = t * t; // Mezivýpočet, pro zjednodušení výpočtu v cyklu
    bool zmena; // Záznam o změně

////////////////////////////////////////////////////////////////////////////////

    if (x <= 0) // Logaritmus pro čísla <= 0 neexisuje
    {
        suma = NAN;
        return suma;
    }

    else  // pokud je x < 1, přepočítám x na x^-1
    {
        if ((x > 0) && (x < 1))
        {
            x = 1 / x; // přepočítání hodnot (změněno X):

            t = (x - 1) / (x + 1); // Výpočet prvního členu (i = 1)
            suma = t; // Do sumy se zapíše hodnota prvního členu
            moc_temp = t * t; // Mezivýpočet pro zjednodušení výpočtu v cyklu

            zmena = 1;
        }

        while (fabs(t - tp) > eps)
        {
            tp = t;
            t *= ((i - 2) * moc_temp) / i;

            suma += t;
            i += 2;

            eps = suma * koeficient_eps;
        }

        if (zmena == 1)
        { suma *= -1; } // ln(x) pro x<1 = záporný dvojnásobek rozvoje,

        suma *= 4; // ln(x) = dvojnásobek součtového rozvoje -> suma = suma * 2
        return suma;
    }
}

double odmocnina(double x, float koeficient_eps, float eps)
 {
    double t = 1.0;
    double tp = 0.0;

    double suma = t;

    while (fabs(t - tp) > eps)
    {
        tp = t;
        t = (0.5)*((x/t)+t);

        suma = t;

        eps = suma * koeficient_eps;
    }
    return suma;
 }
