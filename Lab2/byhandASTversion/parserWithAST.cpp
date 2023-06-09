/**
 * @file parserAST.cpp
 * @author 高本涵 (you@domain.com)
  
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "Node.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
enum tokentype {
    INT, FLOAT, ID, SEMI, COMMA, ASSIGNOP, RELOP, PLUS,
    MINUS, STAR, DIV, AND, OR, DOT, NOT, TYPE,
    LP, RP, LB, RB, LC, RC,
    STRUCT, RETURN, IF, ELSE, WHILE,
    LOWER_THAN_ELSE
};
vector<string> keywords = {"else", "for", "if", "return", "struct", "while", "return"};
vector<string> Type = {"int", "void", "char", "double", "short", "float"};
//*********************************************
int line_num_record[200]; // 记录所在行数
int tokens[200];          // 记录词法类型
int tokenSum = 0;              // 记录词法数量
int synError = 0;         // 报错标志
int errornum = 0;         // 错误词法
int tokenIndex = 0;            // 从0走到sum 读完所有词法
//********************************************

struct TokenInfo {
    int line = 0;
    string type;
    string word;
};

vector<TokenInfo> tokenVec;
int line = 1;
char text[1000] = "";
char ch = ' ';
int len = 0;
int i = 0;
int example = 1;
int eline = 0;
string word;
TokenInfo temp;

void push(string s) {
    temp.line = line;
    temp.type = s;
    temp.word = word;
    tokenVec.push_back(temp);
    word.clear();
}

void judgee() // 判断e
{
    word += ch;
    ch = text[++i];
    if (ch == '+' || ch == '-') {
        word += ch;
        ch = text[++i];
    }
    if (ch >= '0' && ch <= '9') {
        word += ch;
        while ((ch = text[++i]) && (ch >= '0' && ch <= '9'))
            word += ch;
        push("FLOAT");
    }
}

void jump() // 判断换行
{
    while (ch == ' ' || ch == '\n' || ch == '\t') {
        if (ch == '\n')
            line++;
        ch = text[++i];
    }
}

void pushDecmial() // 十进制
{
    while (ch >= '0' && ch <= '9') {
        word += ch;
        ch = text[++i];
    }

    if (ch == '.') {
        word += ch;
        ch = text[++i];
        while (ch >= '0' && ch <= '9') {
            word += ch;
            ch = text[++i];
        }
        if (ch == 'e' || ch == 'E')
            judgee();
        else
            push("FLOAT");
    } else if (ch == 'e' || ch == 'E')
        judgee();
    else {
        push("INT");
        if (ch == '-' || ch == '+') {
            if (ch == '-') {
                word += ch;
                ch = text[++i];
                push("MINUS");
            } else {
                word += ch;
                ch = text[++i];
                push("PLUS");
            }
        }
    }
}
void pushOctHex() // 八进制 十六进制
{
    while (ch >= '0' && ch <= '9') {
        word += ch;
        ch = text[++i];
    }
    if ((ch < '0' && ch != '.') || (ch > '9' && ch != 'x' && ch != 'X' && ch != 'e' && ch != 'E')) {
        push("INT");
    } else if (ch == 'e' || ch == 'E')
        judgee();
    else {
        if (ch >= '0' && ch <= '9') {
            while (ch >= '0' && ch <= '9') {
                word += ch;
                ch = text[++i];
            }
            push("INT");
        }
        if (ch == 'x' || ch == 'X') {
            word += ch;
            ch = text[++i];
            while ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
                word += ch;
                ch = text[++i];
            }
            push("INT");
        }
        if (ch == '.')
            pushDecmial();
    }
}
// lexical part
void fun() 
{
    ch = text[i];
    jump();
    while (ch != '\0' && ch != EOF) {
        jump();

        if (ch == '-') {
            word += ch;
            ch = text[++i];
            if (ch == '=') {
                word += ch;
                push("RELOP");
                ch = text[++i];
            } else if ((ch >= '0' && ch <= '9') || ch == '.')
                pushDecmial();
            else
                push("MINUS");
        }
        if (ch == '+') {
            word += ch;
            ch = text[++i];
            if (ch == '=') {
                word += ch;
                push("RELOP");
                ch = text[++i];
            } else if ((ch >= '0' && ch <= '9') || ch == '.')
                pushDecmial();
            else
                push("PLUS");
        }
        if (ch == '.') {
            word += ch;
            ch = text[++i];
            if (ch < '0' || ch > '9') {
                push("DOT");
            } else
                pushDecmial();
        }
        if (ch >= '0' && ch <= '9') {
            if (ch >= '1' && ch <= '9') {
                word += ch;
                ch = text[++i];
                pushDecmial();
            } else if (ch == '0') {
                word += ch;
                ch = text[++i];
                pushOctHex();
            }
        }
        if (isalnum(ch) || ch == '_') {
            while (isalnum(ch) || ch == '_') {
                word += ch;
                ch = text[++i];
            }
            if (find(keywords.begin(), keywords.end(), word) != keywords.end()) {
                if (word == "return")
                    push("RETURN");
                else if (word == "if")
                    push("IF");
                else if (word == "else")
                    push("ELSE");
                else if (word == "while")
                    push("WHILE");
                else if (word == "struct")
                    push("STRUCT");
            } else if (find(Type.begin(), Type.end(), word) != Type.end())
                push("TYPE");
            else {
                push("ID");
                char temp1 = text[i + 1];
                if (ch == '+' && temp1 != '=') {
                    if (temp1 == '+') {
                        word += "++";
                        ++i;
                        ch = text[++i];
                        push("INC");
                    } else {
                        word += ch;
                        ch = text[++i];
                        push("PLUS");
                    }
                }
                if (ch == '-' && temp1 != '=') {
                    if (temp1 == '-') {
                        word += "--";
                        ++i;
                        ch = text[++i];
                        push("DEC");
                    } else {
                        word += ch;
                        ch = text[++i];
                        push("MINUS");
                    }
                }
            }
        }
        if (ch == '/') {
            char temp = text[++i];
            if (temp == '/')
                do {
                    ch = text[++i];
                } while (ch != '\n');
            else if (temp == '*') {
                do {
                    ch = text[++i];
                    jump();
                } while (ch != '/');
                ch = text[++i];
            } else if (temp == '=') {
                word += "/=";
                push("RELOP");
                ch = text[++i];
            } else {
                word += '/';
                push("DIV");
                ch = text[++i];
            }
        }
        if (ch == '|') {
            char temp = text[++i];
            if (temp == '|') {
                word += "||";
                push("OR");
                ch = text[++i];
            }
        }
        if (ch == '&') {
            char temp = text[++i];
            if (temp == '&') {
                word += "&&";
                push("AND");
                ch = text[++i];
            }
        }
        if (ch == '(') {
            word += ch;
            push("LP");
            ch = text[++i];
        }
        if (ch == ')') {
            word += ch;
            push("RP");
            ch = text[++i];
        }
        if (ch == '[') {
            word += ch;
            push("LB");
            ch = text[++i];
        }
        if (ch == ']') {
            word += ch;
            push("RB");
            ch = text[++i];
        }
        if (ch == '{') {
            word += ch;
            push("LC");
            ch = text[++i];
        }
        if (ch == '}') {
            word += ch;
            push("RC");
            ch = text[++i];
        }
        if (ch == ',') {
            word += ch;
            push("COMMA");
            ch = text[++i];
        }
        if (ch == ';') {
            word += ch;
            push("SEMI");
            ch = text[++i];
        }
        if (ch == '<' || ch == '>') {
            word += ch;
            if (text[i + 1] == '=')
                word += text[++i];
            ch = text[++i];
            push("RELOP");
        }
        if (ch == '=') {
            word += ch;
            if (text[i + 1] == '=') {
                word += text[++i];
                ch = text[++i];
                push("RELOP");
            } else {
                ch = text[++i];
                push("ASSIGNOP");
            }
        }
        if (ch == '*') {
            word += ch;
            if (text[i + 1] == '=') {
                word += text[++i];
                ch = text[++i];
                push("RELOP");
            } else {
                ch = text[++i];
                push("STAR");
            }
        }
        if (ch == '!') {
            word += ch;
            if (text[i + 1] == '=') {
                word += text[++i];
                ch = text[++i];
                push("RELOP");
            } else {
                ch = text[++i];
                push("NOT");
            }
        }
        if (ch == '~') {
            eline = line;
            example = 0;
            break;
        }
    }
}
//****************************************************************
// parser part
pNode Program();
pNode ExtDeflist();
pNode ExtDef();
pNode Specifier();
pNode VarDec();
pNode FunDec();
pNode Compst();
pNode StmtList();
pNode Stmt();
pNode DefList();
pNode Def();
pNode DecList();
pNode Dec();
pNode Exp();
// TODO: 重新声明
/**
 * @note
 * Program => ExtDefList
 * @return pNode 
 */
pNode Program() { 
    return newNode(line_num_record[tokenIndex], NOT_A_TOKEN, "Program", 1, ExtDeflist());
}
/**
 * @note
 * ExtDeflist => ExtDef ExtDeflist
 *             | empty
 * @return pNode 
 */
pNode ExtDeflist() {
    if (tokens[tokenIndex] == TYPE) {
        pNode node_ExtDef = ExtDef();
        pNode node_ExtDefList = ExtDeflist();
        return newNode(line_num_record[tokenIndex], NOT_A_TOKEN, "ExtDeflist", 2, node_ExtDef, node_ExtDefList);
    }
    return nullptr;
}
/**
 * @note
 * ExtDef => Specifier FunDec Compst
 * @return pNode 
 */
pNode ExtDef() { 
    pNode node_Specifier = Specifier();
    pNode node_FunDec = FunDec();
    pNode node_Compst = Compst();
    return newNode(line_num_record[tokenIndex], NOT_A_TOKEN, "ExtDef", 3, node_Specifier, node_FunDec, node_Compst);
} 
/**
 * @note
 * Specifier => TYPE
 * @return pNode 
 */
pNode Specifier() { 
    if (tokens[tokenIndex] == TYPE) {
        tokenIndex++;
        pNode token_TYPE = newTokenNode(line_num_record[tokenIndex - 1], TYPE_TOKEN, "TYPE", tokenVec[tokenIndex - 1].word);
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Specifier", 1, token_TYPE);
    } else { // 语法错误 匹配失败
        synError = 1;
        errornum = tokenIndex;
    }
}
/**
 * @note
 * VarDec => ID
 * @return pNode 
 */
pNode VarDec() { 
    if (tokens[tokenIndex] == ID) {
        tokenIndex++;
        pNode token_ID = newTokenNode(line_num_record[tokenIndex- 1], ID_TOKEN, "ID", tokenVec[tokenIndex - 1].word);
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "VarDec", 1, token_ID);
    } else { // 语法错误 匹配失败
        synError = 1;
        errornum = tokenIndex;
    }
}
/**
 * @note
 * FunDec => ID LP RP 
 *         | ID
 * @return pNode 
 */
pNode FunDec() { 
    if (tokens[tokenIndex] == ID) {
        tokenIndex++; 
        pNode token_ID = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "ID", tokenVec[tokenIndex - 1].word);
        if (tokens[tokenIndex] == LP) {
            tokenIndex++;
            pNode token_LP = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "LP", tokenVec[tokenIndex - 1].word);
            if (tokens[tokenIndex] == RP) {
                tokenIndex++;
                pNode token_RP = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "RP", tokenVec[tokenIndex - 1].word);
                return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "FunDec", 3, token_ID, token_LP, token_RP);
            } else { // 语法错误 匹配失败
                synError = 1;
                errornum = tokenIndex;
            }
        }
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "FunDec", 1, token_ID);
    } else { // 语法错误 匹配失败
        synError = 1;
        errornum = tokenIndex;
    }
}
/**
 * @note
 * Compst => LC DefList StmtList RC
 * @return pNode 
 */
pNode Compst() { 
    if (tokens[tokenIndex] == LC) {
        tokenIndex++;
        pNode token_LC = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "LC", tokenVec[tokenIndex - 1].word);
        pNode node_DefList = DefList();
        pNode node_StmtList = StmtList();
        if (tokens[tokenIndex] == RC) {
            tokenIndex++;
            pNode token_RC = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "RC", tokenVec[tokenIndex - 1].word);
            return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Compst", 4, token_LC, node_DefList, node_StmtList, token_RC);
        } else { // 语法错误 匹配失败
            synError = 1;
            errornum = tokenIndex;
        }
    } else { // 语法错误 匹配失败
        synError = 1;
        errornum = tokenIndex;
    }
}
/**
 * @note
 * StmtList => Stmt StmtList
 *           | empty
 * @return pNode 
 */
pNode StmtList() {
    if (tokens[tokenIndex] == RETURN || tokens[tokenIndex] == LP || tokens[tokenIndex] == NOT || tokens[tokenIndex] == INT || tokens[tokenIndex] == ID) {
        pNode node_Stmt = Stmt();
        pNode node_StmtList = StmtList();
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "StmtList", 2, node_Stmt, node_StmtList);
    }
    return nullptr;
}
/**
 * @note
 * Stmt => Exp SEMI 
 *       | RETURN Exp Stmt
 * @return pNode 
 */
pNode Stmt() { 
    if (tokens[tokenIndex] == RETURN) {
        tokenIndex++;
        pNode token_RETURN = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "RETURN", tokenVec[tokenIndex - 1].word);
        pNode node_Exp = Exp();
        if (tokens[tokenIndex] == SEMI) {
            tokenIndex++;
            pNode token_SEMI = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "SEMI", tokenVec[tokenIndex - 1].word);
            return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Stmt", 3, token_RETURN, node_Exp, token_SEMI);
        } else { // 语法错误 匹配失败
            synError = 1;
            errornum = tokenIndex;
        }
    } else {
        pNode node_Exp = Exp();
        if (tokens[tokenIndex] == SEMI) {
            tokenIndex++;
            pNode token_SEMI = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "SEMI", tokenVec[tokenIndex - 1].word);
            return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Stmt", 2, node_Exp, token_SEMI);
        } else { // 语法错误 匹配失败
            synError = 1;
            errornum = tokenIndex;
        }
    }
}
/**
 * @note
 * DefList => Def DefList 
 *          | empty
 * @return pNode 
 */
pNode DefList() { 
    if (tokens[tokenIndex] == TYPE) {
        pNode node_Def = Def();
        pNode node_DefList = DefList();
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "DefList", 2, node_Def, node_DefList);
    }
    return nullptr;
}
/**
 * @note
 * Def => Specifier DecList SEMI
 * @return pNode 
 */
pNode Def() { 
    pNode node_Specifier = Specifier();
    pNode node_DecList = DecList();
    if (tokens[tokenIndex] == SEMI) {
        tokenIndex++;
        pNode token_SEMI = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "SEMI", tokenVec[tokenIndex - 1].word);
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Def", 3, node_Specifier, node_DecList, token_SEMI);
    } else { // 语法错误 匹配失败
        synError = 1;
        errornum = tokenIndex;
    }
}
/**
 * @note
 * DecList => Dec;
 * @return pNode 
 */
pNode DecList() { 
    pNode node_Dec = Dec();
    return newNode(line_num_record[tokenIndex], NOT_A_TOKEN, "DecList", 1, node_Dec);
}
/**
 * @note
 * Dec => VarDec ASSIGNOP Exp
 *      | VarDec
 * @return pNode 
 */
pNode Dec() { 
    pNode node_VarDec = VarDec();
    if (tokens[tokenIndex] == ASSIGNOP) {
        tokenIndex++;
        pNode token_ASSIGNOP = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "ASSIGNOP", tokenVec[tokenIndex - 1].word);
        pNode node_Exp = Exp();
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Dec", 3, node_VarDec, token_ASSIGNOP, node_Exp);
    }
    return newNode(line_num_record[tokenIndex], NOT_A_TOKEN, "Dec", 1, node_VarDec);
}
/**
 * @note
 * Exp-> LP Exp RP
 *     | NOP Exp 
 *     | INT
 *     | ID
 * @return pNode 
 */
pNode Exp() { 
    if (tokens[tokenIndex] == LP) {
        tokenIndex++;
        pNode token_LP = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "LP", tokenVec[tokenIndex - 1].word);
        pNode node_Exp = Exp();
        if (tokens[tokenIndex] == RP) {
            tokenIndex++;
            pNode token_RP = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "RP", tokenVec[tokenIndex - 1].word);
            return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Exp", 3, token_LP, node_Exp, token_RP);
        } else {
            synError = 1;
            errornum = tokenIndex;
        }
    } else if (tokens[tokenIndex] == NOT) {
        tokenIndex++;
        pNode token_NOT = newTokenNode(line_num_record[tokenIndex- 1], OTHER_TOKEN, "NOT", tokenVec[tokenIndex - 1].word);
        pNode node_Exp = Exp();
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Exp", 2, token_NOT, node_Exp);
    } else if (tokens[tokenIndex] == INT) {
        tokenIndex++;
        pNode token_INT = newTokenNode(line_num_record[tokenIndex- 1], INT_TOKEN, "INT", tokenVec[tokenIndex - 1].word);
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Exp", 1, token_INT);
    } else if (tokens[tokenIndex] == ID) {
        tokenIndex++;
        pNode token_ID = newTokenNode(line_num_record[tokenIndex- 1], ID_TOKEN, "ID", tokenVec[tokenIndex - 1].word);
        return newNode(line_num_record[tokenIndex- 1], NOT_A_TOKEN, "Exp", 1, token_ID);
    } else {
        synError = 1;
        errornum = tokenIndex;
    }
}

//****************************************************************
int main() {
    text[0] = getchar();
    while (text[len] != EOF) {
        text[++len] = getchar();
    }
    fun();
    if (example) {
        for (vector<TokenInfo>::iterator iter = tokenVec.begin(); iter != tokenVec.end(); iter++) {
            line_num_record[tokenSum] = (*iter).line;
            if ((*iter).type == "INT") tokens[tokenSum] = INT;
            if ((*iter).type == "FLOAT") tokens[tokenSum] = FLOAT;
            if ((*iter).type == "ID") tokens[tokenSum] = ID;
            if ((*iter).type == "SEMI") tokens[tokenSum] = SEMI;
            if ((*iter).type == "ASSIGNOP") tokens[tokenSum] = ASSIGNOP;
            if ((*iter).type == "RELOP") tokens[tokenSum] = RELOP;
            if ((*iter).type == "PLUS") tokens[tokenSum] = PLUS;
            if ((*iter).type == "MINUS") tokens[tokenSum] = MINUS;
            if ((*iter).type == "STAR") tokens[tokenSum] = STAR;
            if ((*iter).type == "DIV") tokens[tokenSum] = DIV;
            if ((*iter).type == "AND") tokens[tokenSum] = AND;
            if ((*iter).type == "OR") tokens[tokenSum] = OR;
            if ((*iter).type == "DOT") tokens[tokenSum] = DOT;
            if ((*iter).type == "NOT") tokens[tokenSum] = NOT;
            if ((*iter).type == "LP") tokens[tokenSum] = LP;
            if ((*iter).type == "RP") tokens[tokenSum] = RP;
            if ((*iter).type == "LB") tokens[tokenSum] = LB;
            if ((*iter).type == "RB") tokens[tokenSum] = RB;
            if ((*iter).type == "LC") tokens[tokenSum] = LC;
            if ((*iter).type == "RC") tokens[tokenSum] = RC;
            if ((*iter).type == "STRUCT") tokens[tokenSum] = STRUCT;
            if ((*iter).type == "RETURN") tokens[tokenSum] = RETURN;
            if ((*iter).type == "IF") tokens[tokenSum] = IF;
            if ((*iter).type == "ELSE") tokens[tokenSum] = ELSE;
            if ((*iter).type == "WHILE") tokens[tokenSum] = WHILE;
            if ((*iter).type == "TYPE") tokens[tokenSum] = TYPE;
            if ((*iter).type == "COMMA") tokens[tokenSum] = COMMA;
            tokenSum++;
        }
    }
    pNode root;
    root = Program();
    if (synError == 1 || tokenIndex != tokenSum) { // 存在语法错误或者未处理到句子末尾
        if (errornum == 1 || line_num_record[errornum] == line_num_record[errornum - 1])
            cout << "Error type (Syntactical) at line " << line_num_record[errornum] << "." << endl;
        else
            cout << "Error type (Syntactical) at line " << line_num_record[errornum - 1] << "." << endl;
    } else {
        cout << "Syntactical Correct." << endl;
        printAST(root, 0);
    }
}
