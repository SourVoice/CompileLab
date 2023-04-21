%option yylineno
%option noyywrap
%{
/* Token kinds.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INT = 258,                     /* INT  */
    FLOAT = 259,                   /* FLOAT  */
    ID = 260,                      /* ID  */
    TYPE = 261,                    /* TYPE  */
    COMMA = 262,                   /* COMMA  */
    DOT = 263,                     /* DOT  */
    SEMI = 264,                    /* SEMI  */
    RELOP = 265,                   /* RELOP  */
    ASSIGNOP = 266,                /* ASSIGNOP  */

    PLUS = 267,                    /* PLUS  */
    MINUS = 268,                   /* MINUS  */
    STAR = 269,                    /* STAR  */
    DIV = 270,                     /* DIV  */
    AND = 271,                     /* AND  */
    OR = 272,                      /* OR  */
    NOT = 273,                     /* NOT  */

    LP = 274,                      /* LP  */
    RP = 275,                      /* RP  */
    LB = 276,                      /* LB  */
    RB = 277,                      /* RB  */
    LC = 278,                      /* LC  */
    RC = 279,                      /* RC  */
    IF = 280,                      /* IF  */
    ELSE = 281,                    /* ELSE  */
    WHILE = 282,                   /* WHILE  */
    STRUCT = 283,                  /* STRUCT  */
    RETURN = 284,                  /* RETURN  */
    LOWER_THAN_ELSE = 285,          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

typedef struct msg {
  int line;
  char type[100];
  char text[100];
  struct msg* next;
}msg, *Msg; // 简单链表保存每行的token结果

Msg m, p;

void store(int line,char *type, char *text) {
    Msg newmsg = (Msg)malloc(sizeof(msg));
    newmsg->line = line;
    strcpy(newmsg->type,type);
    strcpy(newmsg->text,text);
    p->next = newmsg;
    p = p->next;
}

void printToken(int token) {
  switch (token) {
    case INT             : {
      //printf("line%d:(INT, %s)\n", yylineno, yytext);
      store(yylineno, "INT", yytext);
      break;
    }
    case FLOAT             : {
      //printf("line%d:(float, %s)\n", yylineno, yytext);
      store(yylineno, "float", yytext);
      break;
    }
    case ID          : {
      //printf("line%d:(ID, %s)\n", yylineno, yytext);
      store(yylineno, "ID", yytext);
      break;
    }
    case TYPE     :{
      //printf("line%d:(TYPE, %s)\n", yylineno, yytext);
      store(yylineno, "TYPE", yytext);
      break;
    }
    case COMMA     :{
      //printf("line%d:(COMMA, %s)\n", yylineno, yytext);
      store(yylineno, "COMMA", yytext);
      break;
    }
    case DOT     :{
      //printf("line%d:(DOT, %s)\n", yylineno, yytext);
      store(yylineno, "DOT", yytext);
      break;
    }
    case SEMI     :{
      //printf("line%d:(SEMI, %s)\n", yylineno, yytext);
      store(yylineno, "SEMI", yytext);
      break;
    }
    case RELOP     :{
      //printf("line%d:(RELOP, %s)\n", yylineno, yytext);
      store(yylineno, "RELOP", yytext);
      break;
    }
    case ASSIGNOP     :{
      //printf("line%d:(ASSIGNOP, %s)\n", yylineno, yytext);
      store(yylineno, "ASSIGNOP", yytext);
      break;
    }
    case PLUS     :{
      //printf("line%d:(PLUS, %s)\n", yylineno, yytext);
      store(yylineno, "PLUS", yytext);
      break;
    }
    case MINUS     :{
      //printf("line%d:(MINUS, %s)\n", yylineno, yytext);
      store(yylineno, "MINUS", yytext);
      break;
    }
    case STAR     :{
      //printf("line%d:(STAR, %s)\n", yylineno, yytext);
      store(yylineno, "STAR", yytext);
      break;
    }
    case DIV     :{
      //printf("line%d:(DIV, %s)\n", yylineno, yytext);
      store(yylineno, "DIV", yytext);
       break;
    }
    case AND     :{
      //printf("line%d:(AND, %s)\n", yylineno, yytext);
      store(yylineno, "AND", yytext);
      break;
    }
    case OR     :{
      //printf("line%d:(OR, %s)\n", yylineno, yytext);
      store(yylineno, "OR", yytext);
        break;
    }
    case NOT     :{
      //printf("line%d:(NOT, %s)\n", yylineno, yytext);
      store(yylineno, "NOT", yytext);
      break;
    }
    case LP         :{
      //printf("line%d:(LP, %s)\n", yylineno, yytext);
      store(yylineno, "LP", yytext);
        break;
    }
    case RP         :{
      //printf("line%d:(RP, %s)\n", yylineno, yytext);
      store(yylineno, "RP", yytext);
      break;
    }
    case LB        :{
      //printf("line%d:(LB, %s)\n", yylineno, yytext);
      store(yylineno, "LB", yytext);
        break;
    }
    case RB       :{
      //printf("line%d:(RB, %s)\n", yylineno, yytext);
      store(yylineno, "RB", yytext);
      break;
    }
    case LC      :{
      //printf("line%d:(LC, %s)\n", yylineno, yytext);
      store(yylineno, "LC", yytext);
        break;
    }
    case RC      :{
      //printf("line%d:(RC, %s)\n", yylineno, yytext);
      store(yylineno, "RC", yytext);
      break;
    }
    case IF      :{
      //printf("line%d:(IF, %s)\n", yylineno, yytext);
      store(yylineno, "IF", yytext);
        break;
    }
    case ELSE      :{
      //printf("line%d:(ELSE, %s)\n", yylineno, yytext);
      store(yylineno, "ELSE", yytext);
      break;
    }
    case WHILE      :{
      //printf("line%d:(WHILE, %s)\n", yylineno, yytext);
      store(yylineno, "WHILE", yytext);
      break;
    }
    case STRUCT      :{
      //printf("line%d:(STRUCT, %s)\n", yylineno, yytext);
      store(yylineno, "STRUCT", yytext);
      break;
    }
    case RETURN      :{
      //printf("line%d:(RETURN, %s)\n", yylineno, yytext);
      store(yylineno, "RETURN", yytext);
      break;
    }
    default:
       printf("[E]>>>Unknown TokenType %d\n", token);
  }
}
int lines = 1;
int yycolumn = 1;

#define TRUE 1
#define FALSE 0

unsigned lexError = FALSE;

#define YY_USER_ACTION \
    yycolumn+=yyleng;

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
COMMENT [\\]
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
ID {letter}{ld}*
INT 0|[1-9]+[0-9]*
FLOAT {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+

%%
{ws}+ {;}
\n|\r { yycolumn = 1; }

{IF} {  return IF; }
{ELSE} {  return ELSE; }
{WHILE} {  return WHILE; }
{TYPE} {  return TYPE; }
{STRUCT} {  return STRUCT; }
{RETURN} {  return RETURN; }
{RELOP} {  return RELOP; }

{DOT} {  return DOT; }
{SEMI} {  return SEMI; }
{COMMA} {  return COMMA; }
{COMMENT} {  ;  }
{ASSIGNOP} { return ASSIGNOP; }

{PLUS} { return PLUS; }
{MINUS} {  return MINUS; }
{STAR} {  return STAR; }
{DIV} {  return DIV; }
{AND} {  return AND; }
{OR} {  return OR; }
{NOT} {  return NOT; }

{LP} {  return LP; }
{RP} {  return RP; }
{LB} {  return LB; }
{RB} {  return RB; }
{LC} {  return LC; }
{RC} {  return RC; }

{ID} { return ID;}
{INT} { return INT;}
{FLOAT} { return FLOAT;}

{digit}+{ID} {lexError = TRUE;  printf("Error type (Lexical) at line %d: Illegal ID \"%s\".\n", yylineno, yytext); }
"."{digit}+ { lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); }
{digit}+"." { lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); }
{digit}*"."{digit}+[eE] { lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); }
{digit}+"."{digit}*[eE] { lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); }
{digit}+[eE][+-]?{digit}* { lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); }
"."[eE][+-]?{digit}+ { lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); }
. { lexError = TRUE;  printf("Error type (Lexical) at line %d: Mysterious character \"%s\".\n", yylineno, yytext); }
%%

int NextToken() {
    /*
    每当调用yylex()时，它都会从全局输入文件yyin（默认为stdin）中扫描token。
    它一直持续到到达文件结尾（此时它返回值0）或它的其中一个动作执行return语句为止。
    */
    return yylex();
};

int main(int argc, char **argv)
{
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
  m = (Msg)malloc(sizeof(msg));
  p = m;
  
  int token = 0;
  while (token = NextToken()) {
    printToken(token);
  }
  p = m;
  p = p->next;
  if(lexError != TRUE){
    while(p != NULL){
        printf("line%d:(%s, %s)\n", p->line, p->type, p->text);
        p = p->next;
      }
  }
    // yylex();
    // // printf("%d\n", lines);
    return 0;
}
