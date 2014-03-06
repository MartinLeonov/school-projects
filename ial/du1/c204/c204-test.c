
/* ***************************** c204-test.c ******************************** */
/*  P�edm�t: Algoritmy (IAL) - FIT VUT v Brn�                                 */
/*  �kol: c204 - P�evod infixov�ho v�razu na postfixov� (s vyu�it�m c202)     */
/*  Referen�n� implementace: Petr P�ikryl, listopad 1994                      */
/*  P�epis do jazyka C: Luk� Mar��k, prosinec 2012                           */
/*  Upravil: Bohuslav K�ena, ��jen 2013                                       */
/* ************************************************************************** */

/* Z�kladn� testy pro c204.c */

#include "c204.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

char postExpr[MAX_LEN];            /* v�stupn� �et�zec pro konvertovan� v�raz */

int solved;                                        /* indik�tor �e�en� funkce */
int err_flag;                                              /* indik�tor chyby */

/****************************************************************************** 
 * Speci�ln� o�et�en� testovan�ch funkc�.                                     *
 ******************************************************************************/

/* Covert infix expression and verify result */ 
void convert_and_verify(char* infExpr, char* postExprOk)
{
  /* Arguments assertation */
  assert(infExpr != NULL);
  assert(postExprOk != NULL);
  assert(strlen(infExpr) <= MAX_LEN);
  assert(strlen(postExprOk) <= MAX_LEN);
  
  solved = 1;
  
  /* Print expression to be convert */
  printf("Input infix expression:    %s\n", infExpr);
  
  /* Perform conversion */
  char* postExpr = infix2postfix(infExpr);
  
  /* Has function already started to be solved? */
  if (!solved) {
    printf("[W] Function infix2postfix hasn't been implemented yet.\n\n");
    return;
  }
  
  /* Test postfix string */
  if (postExpr == NULL) { /* bad allocation or bad pointer returned */
    printf("ERROR: NULL pointer returned.\n   You may have forgotten to return a correct pointer or\n   an error inside infix2postfix() occured.\n\n");
  }
  else { /* string is probably allocated */
  
    /* Try to find zero character */
    char lastChar = postExpr[MAX_LEN-1]; /* stored for future test */
    postExpr[MAX_LEN-1] = 0; /* just in case to have properly terminated string */
    if (strlen(postExpr) >= (MAX_LEN-1) && lastChar != '0') { /* charater array doesn't contain zero character, this case probably never happend (zero character is often present in unitialized array) */
      printf("ERROR: String appears to be too long (exceeds %d characters).\n   Haven't you forgotten to include zero character?\n\n", MAX_LEN-1);
    }
    else { /* string is OK in C language meaning */
    
      /* Print converted expression and conversion result */
      printf("Output postfix expression: %s\n", postExpr);
      printf("Conversion result match:   "); 
    
      /* Test strings match */
      if (strcmp(postExpr, postExprOk) != 0) { /* conversion test failed */
        printf("FAILED\n\n");
      }
      else { /* conversion is finally OK */
        printf("OK\n\n");
      }
    }
      
    free(postExpr); /* free memory */
  }
}
  

/****************************************************************************** 
 * Vlastn� testov�n�                                                          *
 ******************************************************************************/

int main (int argc, char* argv[]) {
  printf ("C204 - Infix to Postfix Expression Conversion - Basic Tests\n");
  printf ("-----------------------------------------------------------\n\n");

  printf("[TEST01] Upper and lower case characters with plus operator\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("a+B=", "aB+=");
  
  printf("[TEST02] Digits with minus operator\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("0-1=", "01-=");
  
  printf("[TEST03] Mixed operands with multiplication operator\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("a*0=", "a0*=");
  
  printf("[TEST04] Mixed operands with division operator\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("B/1=", "B1/=");
  
  printf("[TEST05] Parentheses support\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("(a+b)=", "ab+=");
  
  printf("[TEST06] Expression evaluation from the left to the right\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("a+b+c=", "ab+c+=");
  
  printf("[TEST07] Minus operator does not have higher priority than plus\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("a+b-c=", "ab+c-=");
  
  printf("[TEST08] Plus operator does not have higher priority than minus\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("A-B+C=", "AB-C+=");
  
  printf("[TEST09] Division operator does not have higher priority than multiplication\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("1*2/3=", "12*3/=");
  
  printf("[TEST10] Multiplication operator does not have higher priority than division\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("a/B*C=", "aB/C*=");
  
  printf("[TEST11] Multiplication operator has higher priority than plus\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("a*B+c=", "aB*c+=");
  
  printf("[TEST12] Parentheses change operator priority\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("A+(B-c)=", "ABc-+=");
  
  printf("[TEST13] Parentheses change operator priority\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("A*(b/c)=", "Abc/*=");
  
  printf("[TEST14] Parentheses change operator priority\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("A*(b-C)=", "AbC-*=");
  
  printf("[TEST15] Complex expression conversion\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  convert_and_verify("(A*0+b)*((c*(1+D))-(e/(3*f+g)))=", "A0*b+c1D+*e3f*g+/-*=");
  
  printf("\n----- C204 - The End of Basic Tests -----\n");

  return(0);
}

/* Konec c204-test.c */
