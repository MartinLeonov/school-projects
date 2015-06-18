/**
  * Predmet:  IFJ / IAL
  * Projekt:  Implementace interpretu imperativniho jazyka IFJ13
  * Tym:      099
  * Varianta: a/2/I
  * Soubor:   parser.h
  * Autori:   Vlcek Michael   <xvlcek21@stud.fit.vutbr.cz>
  *           Svacek Radim    <xsvace02@stud.fit.vutbr.cz>
  *           Blanco Roman    <xblanc01@stud.fit.vutbr.cz>
  *           Micka Vojtech   <xmicka06@stud.fit.vutbr.cz>
  *           Wolfert Richard <xwolfe00@stud.fit.vutbr.cz>
  */

#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED


//Definicia chybovych stavov
#define LEX_ERR -3
#define SYNT_ERR -4
#define SEM_ERR -5
#define SEM_ERR_MISS_PAR -6
#define SEM_ERR_UNDEF -7
#define SEM_ERR_NULL_DIV -10
#define SEM_ERR_RETYPE -11
#define SEM_ERR_ARITH -12
#define SEM_ERR_OTHER -13
#define RET_ERR -99
#define SYNT_OK 1


#define LEFT_ARROW 120
#define RIGHT_ARROW 121
#define EXCLAMATION 122
#define EQUALITY 123
#define DOLLAR_SIGN 124

#include "ial.h"
#include "expr_list.h"


FILE *f;

int  program();
int  parse(t_stItemV *VTableRoot,t_stItemF *FTableRoot, tInstList *GlobalInstList);
int  StateID();
int  StateKeyWord();
int  StateIf();
int  StateIfCore(tInstListItem *JumpInst);
int  StateElse(tInstListItem *JumpInst);
int  StateElseCore(tInstListItem *JumpInst);
int  StateWhile();
int  StateWhileCore(tInstListItem *JumpInstCont,tInstListItem *JumpInstBreak);
int  StateFunction();
int  StateFunctionCore(t_stItemF FunctionPtr);
int  StateReturn();
int  StateFunctionCall();

void expr_analysis_init(char (*)[9][9]);
int StateExpression();
int StateExpressionAnalysis (tDLList *);
int StateExpressionEvaluation(tDLList *);
int GenerateInstruction(int inst_type,void* addr1,void* addr2, void* result);
int GenerateInstructionList(int inst_type,void* addr1,void* addr2,void* result, tInstList* InstList);
char *GenerateString(int index);
char *GenerateStringExpr (int index);
#endif
