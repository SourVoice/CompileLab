// Node.h
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;

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
    int lineno;
    NodeType type;

    string typeName;
    string tokenContent; // token内容

    struct NODE *child; // first child node of non-terminals node
    struct NODE *next;  // next brother node, 同级节点
} Node;

typedef Node *pNode;

Node *newNode(int line, NodeType type, string typeName, int argc, ...) {
    printf("line = %d, type = %s, ", line, typeName.c_str());
    Node *curNode = new Node;
    curNode->next = nullptr;
    curNode->lineno = line;
    curNode->type = type;

    curNode->typeName = typeName;

    va_list args;
    va_start(args, argc);

    Node *childNode = va_arg(args, Node *);
    // printf("\tline = %d, type = %s, ", childNode->lineno, childNode->typeName.c_str());
    curNode->child = childNode;
    for (int i = 1; i < argc; i++) {
        childNode->next = va_arg(args, Node *);
        if (childNode->next != nullptr) {
            childNode = childNode->next;
            // printf("\t\tline = %d, type = %s, ", childNode->lineno, childNode->typeName.c_str());
        }
    }
    // printf("\n");

    va_end(args);
    return curNode;
}

// 储存终结符
Node *newTokenNode(int line, NodeType type, string typeName, string &tokenContent) {
    Node *tokenNode = new Node;

    // printf("line = %d, type = %s, content = %s, \n", line, typeName.c_str(), tokenContent.c_str());
    tokenNode->lineno = line;
    tokenNode->type = type;

    tokenNode->tokenContent = tokenContent;
    tokenNode->typeName = typeName;

    tokenNode->child = nullptr;
    tokenNode->next = nullptr;

    return tokenNode;
}

void delNode(Node* node) {
    if (node == nullptr) return;
    while (node->child != nullptr) {
        pNode temp = node->child;
        node->child = node->child->next;
        delNode(temp);
    }
    free(node);
    node = nullptr;
}

// TODO: 实现树形打印, need optimize
void printAST(Node *root, int height) {
    if (root == nullptr) {
        return;
    }
    for (int i = 0; i < height; i++) {
        printf("    ");
    }
    if (root->type == INT_TOKEN) {
        cout << root->typeName << ": "  
             << stoi(root->tokenContent);
    } else if (root->type == FLOAT_TOKEN) {
        cout << root->typeName << ": "
             << root->tokenContent;
    } else if (root->type == TYPE_TOKEN) {
        cout << root->typeName << ": "
             << root->tokenContent;
    } else if (root->type == ID_TOKEN) {
        cout << root->typeName << ": "
             << root->tokenContent;
    } else if (root->type == OTHER_TOKEN) {
        cout << root->typeName << ": "
             << root->tokenContent;
    } else if (root->type == NOT_A_TOKEN) {
        cout << root->typeName << " (" << root->lineno << ")"
             << root->tokenContent;
    }
    printf("\n");
    printAST(root->child, height + 1);
    printAST(root->next, height);
}
#endif