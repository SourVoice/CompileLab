// Makefile doesn't include this, to compile with syntax.tab.c
// need to remove the main part in syntax.y
#include <assert.h>
#include <stdio.h>
// #include "Node.h"
#define FALSE 0
#define TRUE 1
unsigned synError = FALSE;
extern struct NODE *root;
extern int yyparse();
extern void yyrestart(FILE *);
extern void printAST(struct NODE *root, int height);
int main(int argc, char *argv[]) {
    // if(argc <= 1) return 1; // means must read from file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        f = stdin;
        // perror(argv[1]); return 1;
    }
    yyrestart(f);
    yyparse();
    // printf("synError : %d\n", synError);
    assert(root != NULL);
    if (!synError) {
        printAST(root, 0);
    }
    return 0;
}