%{
#include <stdio.h>

#include "Node.h"

#include "lex.yy.c"

Node* root;
unsigned synError = FALSE;

#define YYERROR_VERBOSE 1
void yyerror(const char* err_msg){
    synError = TRUE;
    fprintf(stdout, "Error type (Syntactical) at line %d %s.\n",yylineno - 1, err_msg);
}

%}

/* declared types */
// TODO: remove this %%
// %%

%union {
   Node* node;
}

/* declared tokens(终结符, 以%token开头) */
%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP 
%token <node> PLUS
%token <node> MINUS
%token <node> STAR
%token <node> DIV
%token <node> AND
%token <node> OR
%token <node> DOT
%token <node> NOT
%token <node> TYPE

%token <node> LP
%token <node> RP
%token <node> LB
%token <node> RB
%token <node> LC
%token <node> RC

%token <node> STRUCT
%token <node> RETURN
%token <node> IF
%token <node> ELSE
%token <node> WHILE

/* 所有未被定义为%token的符号都会被看作非终结符，
这些非终结符要求必须在 任意产生式的左边至少出现一次。
*/
/* declared non-terminals */

%type <node> Program ExtDefList ExtDef              //  High-level Definitions
%type <node> Specifier                              //  Specifiers
%type <node> VarDec FunDec                          //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp 

/* 优先级&&结合性 */ 
%right      ASSIGNOP
%left       OR         AND
%left       RELOP
%left       PLUS        MINUS
%left       STAR        DIV
%right      NOT
%left       DOT
%left       LB          RB
%left       LP          RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-level Definitions */
Program :           ExtDefList                              {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Program", 1, $1); root = $$; }
    ;                                                       
ExtDefList :        ExtDef ExtDefList                       {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "ExtDefList", 2, $1, $2);  }
    |                                                       {  $$ = NULL;  }
    ;
ExtDef :            Specifier FunDec CompSt                 {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "ExtDef", 3, $1, $2, $3);  }
    |               error SEMI                              {  synError = TRUE;  }
    ;
/* Specifiers */
Specifier :         TYPE /* 类型符 */                       {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Specifier", 1, $1);  }
    ;
/* Declarators */
VarDec :            ID  /*变量名*/                          {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "VarDec", 1, $1);  }
    |               error RB                                {  synError = TRUE;  }
    ;
FunDec :            ID                                      {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "FunDec", 1, $1);  }
    |               ID LP RP                                {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "FunDec", 3, $1, $2, $3);  }
    |               error SEMI                              {  synError = TRUE;  }
    ;
/* Statements */
CompSt :            LC DefList StmtList RC                  {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "CompSt", 4, $1, $2, $3, $4);  }
    |               error RC                                {  synError = TRUE;  }
    ;
StmtList :          Stmt StmtList                           {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "StmtList", 2, $1, $2);  }
    |                                                       {  $$ = NULL;  }
    ;
Stmt :              Exp SEMI                                {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Stmt", 2, $1, $2);  }
    |               RETURN Exp SEMI                         {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Stmt", 3, $1, $2, $3);  }
    |               error SEMI                              {  synError = TRUE;  }
    ;
/* Local Definitions */
DefList :           Def  DefList                            {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "DefList", 2, $1, $2);  }
    |                                                       {  $$ = NULL;  }
    ;
Def :               Specifier DecList SEMI /*类型符 + 一系列变量 + ; */ {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Def", 3, $1, $2, $3);  }
    ; 
DecList :           Dec                                     {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "DecList", 1, $1);  }
    ;
Dec :               VarDec ASSIGNOP Exp                     {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Dec", 3, $1, $2, $3);  }
    |               VarDec                                  {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Dec", 1, $1);  }
    ;
/* Expressions */
Exp :               LP Exp RP                               {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3);  }
    |               NOT Exp                                 {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Exp", 2, $1, $2);  }
    |               INT                                     {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Exp", 1, $1);  }
    |               ID                                      {  $$ = newNode(@$.first_line, NOT_A_TOKEN, "Exp", 1, $1);  }
    |               error RC                                { synError = 1; }
    ;
/* Comments */
/* 四则运算测试 */
// Calc : 
//     | Exp { printf("= %d\n", $1); }
//     ;
// Exp : Factor
//     | Exp ADD Factor { $$ = $1 + $3; }
//     | Exp SUB Factor { $$ = $1 - $3; }
//     ;
// Factor : Term
//     | Factor MUL Term { $$ = $1 * $3; }
//     | Factor DIV Term { $$ = $1 / $3; }
//     ;
// Term : INT
//     | FLOAT
//     ;
%%

int main(int argc, char* argv[]){
    FILE*f = fopen(argv[1], "r");
    if(!f) { f = stdin;}
    yyrestart(f);
    yyparse();
    if(!synError) {
        printAST(root, 0);
        printf("Syntactical Correct.\n");
    }
    return 0;
}