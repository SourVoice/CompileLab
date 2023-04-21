// Node.h
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NODE_H
#define NODE_H
typedef enum nodetype {
    INT_TOKEN,
    FLOAT_TOKEN,
    TYPE_TOKEN,
    ID_TOKEN,
    OTHER_TOKEN,
    NOT_A_TOKEN,
} NodeType;

typedef struct NODE {
    int             lineno;
    NodeType        type;

    char *          typeName;
    char *          tokenContent; // token内容

    struct NODE *   child; // first child node of non-terminals node
    struct NODE *   next;  // next brother node, 同级节点
} Node;

Node *newNode(int line, NodeType type, char *typeName, int argc, ...) {
    // printf("line = %d, type = %s, ", line, typeName);
    Node *curNode = (Node *)malloc(sizeof(Node));
    curNode->lineno = line;
    curNode->type = type;

    int len = strlen(typeName) + 1;
    curNode->typeName = (char *)malloc(sizeof(char) * len);
    strncpy(curNode->typeName, typeName, len);

    va_list args;
    va_start(args, argc);

    Node *childNode = va_arg(args, Node *);
    // printf("\tline = %d, type = %s, ", childNode->lineno, childNode->typeName);
    curNode->child = childNode;
    for (int i = 1; i < argc; i++) {
        childNode->next = va_arg(args, Node *);
        if (childNode->next != NULL) {
            childNode = childNode->next;
            // printf("\t\tline = %d, type = %s, ", childNode->lineno, childNode->typeName);
        }
    }
    // printf("\n");

    va_end(args);
    return curNode;
}

// 储存终结符
Node *newTokenNode(int line, NodeType type, char *typeName, char *tokenContent) {
    Node *tokenNode = (Node *)malloc(sizeof(Node));

    // printf("line = %d, type = %s, content = %s, \n", line, typeName, tokenContent);
    tokenNode->lineno = line;
    tokenNode->type = type;
    int tokenName_len = strlen(typeName) + 1;
    tokenNode->typeName = (char *)malloc(sizeof(char) * tokenName_len);
    strncpy(tokenNode->typeName, typeName, tokenName_len);

    int tokenText_len = strlen(tokenContent) + 1;
    tokenNode->tokenContent = (char *)malloc(sizeof(char) * tokenText_len);
    strncpy(tokenNode->tokenContent, tokenContent, tokenText_len);

    tokenNode->child = NULL;
    tokenNode->next = NULL;

    return tokenNode;
}

// TODO: 实现树形打印, need optimize
void printAST(Node *root, int height) {
    if (root == NULL) { return; }
    for (int i = 0; i < height; i++) { printf("    "); }
    if      (root->type == INT_TOKEN)   { printf("%s",      root->typeName); }
    else if (root->type == FLOAT_TOKEN) { printf("%s: %s",  root->typeName, root->tokenContent); } 
    else if (root->type == TYPE_TOKEN)  { printf("%s: %s",  root->typeName, root->tokenContent); } 
    else if (root->type == ID_TOKEN)    { printf("%s: %s",  root->typeName, root->tokenContent); } 
    else if (root->type == OTHER_TOKEN) { printf("%s",      root->typeName); } 
    else if (root->type == NOT_A_TOKEN) { printf("%s (%d)", root->typeName, root->lineno); }
    printf("\n");
    printAST(root->child, height + 1);
    printAST(root->next, height);
}
#endif