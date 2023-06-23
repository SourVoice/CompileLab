%option yylineno
%option noyywrap
%{
#include <unistd.h>

#include "Node.h"
#include "syntax.tab.h" 

int yycolumn = 1;

#define TRUE 1
#define FALSE 0

unsigned lexError = FALSE;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng; 

%}

ws [ \t]
digit [0-9]
letter [_a-zA-Z]
ld[0-9_a-zA-Z]
nonletter [^_a-zA-Z]
RELOP   >|<|>=|<=|==|!=
IF if
ELSE else
WHILE while
TYPE int|float
STRUCT struct
RETURN return
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
NOT "!"
DOT "."
SEMI ";"
COMMA ","
ASSIGNOP "="
COMMENT "\/\*[\s\S]*?\*\/"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
ID {letter}{ld}*

D_DIGIT [0-9]
O_DIGIT [0-7]
H_DIGIT[0-9a-fA-F]

D_INT 0|[1-9]{D_DIGIT}*
O_INT 0[1-7]{O_DIGIT}+
H_INT 0("x"|"X"){H_DIGIT}+
INT {D_INT}|{O_INT}|{H_INT}
FLOAT {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+

%%
{ws}+       {  ;  }
\n|\r       {  yycolumn = 1;  }

{IF}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "IF", yytext); return IF;  }
{ELSE}      {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "ELSE", yytext); return ELSE; }
{WHILE}     {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "WHILE", yytext);return WHILE; }
{TYPE}      {  yylval.node = newTokenNode(yylineno, TYPE_TOKEN, "TYPE", yytext);return TYPE; }
{STRUCT}    {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "STRUCT", yytext);return STRUCT; }
{RETURN}    {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "RETURN", yytext);return RETURN; }
{RELOP}     {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "RELOP", yytext);return RELOP; }

{DOT}       {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "DOT", yytext);return DOT; }
{SEMI}      {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "SEMI", yytext);return SEMI; }
{COMMA}     {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "COMMA", yytext);return COMMA; }
{COMMENT}   {  ;  }
{ASSIGNOP}  {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "ASSIGNOP", yytext);return ASSIGNOP; }

{PLUS}      {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "PLUS", yytext);return PLUS; }
{MINUS}     {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "MINUS", yytext);return MINUS; }
{STAR}      {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "STAR", yytext);return STAR; }
{DIV}       {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "DIV", yytext);return DIV; }
{AND}       {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "AND", yytext);return AND; }
{OR}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "OR", yytext);return OR; }
{NOT}       {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "NOT", yytext);return NOT; }

{LP}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "LP", yytext);return LP; }
{RP}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "RP", yytext);return RP; }
{LB}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "LB", yytext);return LB; }
{RB}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "RB", yytext);return RB; }
{LC}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "LC", yytext);return LC; }
{RC}        {  yylval.node = newTokenNode(yylineno, OTHER_TOKEN, "RC", yytext);return RC; }

{ID}        {  yylval.node = newTokenNode(yylineno, ID_TOKEN, "ID", yytext);return ID;  }
{INT}       {  yylval.node = newTokenNode(yylineno, INT_TOKEN, "INT", yytext);return INT;  }
{FLOAT}     {  yylval.node = newTokenNode(yylineno, FLOAT_TOKEN, "FLOAT", yytext);return FLOAT;  }

. { lexError = TRUE;  printf("Error type (Lexical) at line %d: Mysterious character \"%s\".\n", yylineno, yytext); }
%%

// no main