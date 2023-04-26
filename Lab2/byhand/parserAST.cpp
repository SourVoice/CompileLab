/**
 * @file parserAST.cpp
 * @author 杨钧硕 (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../Node.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
enum tokentype {
    INT,
    FLOAT,
    ID,
    SEMI,
    COMMA,
    ASSIGNOP,
    RELOP,
    PLUS,
    MINUS,
    STAR,
    DIV,
    AND,
    OR,
    DOT,
    NOT,
    TYPE,
    LP,
    RP,
    LB,
    RB,
    LC,
    RC,
    STRUCT,
    RETURN,
    IF,
    ELSE,
    WHILE,
    LOWER_THAN_ELSE
};
vector<string> keywords = {"else", "for", "if", "return", "struct", "while", "return"};
vector<string> Type = {"int", "void", "char", "double", "short", "float"};
//*********************************************
int line_num_record[200]; // 记录所在行数
int tokens[200];          // 记录词法类型
int sum = 0;              // 记录词法数量
int error = 0;            // 报错标志
int errornum = 0;         // 错误词法
int index = 0;             // 从0走到sum 读完所有词法
int a = 0;
//********************************************
void Program(); // 语法分析器的函数声明
void ExtDeflist();
void ExtDef();
void Specifier();
void VarDec();
void FunDec();
void Compst();
void StmtList();
void Stmt();
void DefList();
void Def();
void DecList();
void Dec();
void Exp();
//************************************************
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
        if (ch == 'e' || ch == ASSIGNOP)
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

void fun() // 状态图
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
void Program(int ceng);
void ExtDeflist(int ceng);
void ExtDef(int ceng);
void Specifier(int ceng);
void VarDec(int ceng);
void FunDec(int ceng);
void Compst(int ceng);
void StmtList(int ceng);
void Stmt(int ceng);
void DefList(int ceng);
void Def(int ceng);
void DecList(int ceng);
void Dec(int ceng);
void Exp(int ceng);
void t(int ceng) {
    for (int j = 0; j < ceng; j++) {
        cout << "    ";
    }
}

void Program(int ceng) { // Program->ExtDeflist;
    if (a == 1) {
        t(ceng);
        cout << "Program (" << line_num_record[index] << ")" << endl;
    }
    ExtDeflist(ceng);
}
void ExtDeflist(int ceng) { // ExtDeflist-> ExtDef ExtDeflist|e  注意走e的问题
    if (tokens[index] == TYPE) {

        if (a == 1) {
            ceng++;
            t(ceng);
            cout << "ExtDefList (" << line_num_record[index] << ")" << endl;
        }
        ExtDef(ceng);
        ExtDeflist(ceng);
    }
}
void ExtDef(int ceng) { // ExtDef->  Specifier FunDec Compst
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "ExtDef (" << line_num_record[index] << ")" << endl;
    }
    Specifier(ceng);
    FunDec(ceng);
    Compst(ceng);
}
void Specifier(int ceng) { // Specifier->a
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Specifier (" << line_num_record[index] << ")" << endl;
    }
    if (tokens[index] == TYPE) {
        if (a == 1) {
            t(ceng + 1);
            cout << "TYPE: ";
            int k = 0;
            vector<TokenInfo>::iterator iter;
            for (iter = tokenVec.begin(); k < index; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        index++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void VarDec(int ceng) { // VarDec->C
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "VarDec (" << line_num_record[index] << ")" << endl;
    }
    if (tokens[index] == ID) {
        if (a == 1) {
            t(ceng + 1);
            cout << "ID: ";
            int k = 0;
            vector<TokenInfo>::iterator iter;
            for (iter = tokenVec.begin(); k < index; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        index++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void FunDec(int ceng) { // FunDec->C O Q|C
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "FunDec (" << line_num_record[index] << ")" << endl;
    }
    if (tokens[index] == ID) {
        if (a == 1) {
            t(ceng + 1);
            cout << "ID: ";
            int k = 0;
            vector<TokenInfo>::iterator iter;
            for (iter = tokenVec.begin(); k < index; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        index++; // 匹配成功
        if (tokens[index] == LP) {
            if (a == 1) {
                t(ceng + 1);
                cout << "LP" << endl;
            }
            index++;
            if (tokens[index] == RP) {
                if (a == 1) {
                    t(ceng + 1);
                    cout << "RP" << endl;
                }
                index++;
            } else { // 语法错误 匹配失败
                error = 1;
                errornum = index;
            }
        }

    } else { // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void Compst(int ceng) { // Compst-> T DefList StmtList U
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Compst (" << line_num_record[index] << ")" << endl;
    }
    if (tokens[index] == LC) {
        if (a == 1) {
            t(ceng + 1);
            cout << "LC" << endl;
        }
        index++;
        DefList(ceng);
        StmtList(ceng);
        if (tokens[index] == RC) {
            if (a == 1) {
                t(ceng + 1);
                cout << "RC" << endl;
            }
            index++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = index;
        }
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void StmtList(int ceng) { // StmtList->Stmt StmtList|e    注意e
    if (tokens[index] == RETURN || tokens[index] == LP || tokens[index] == NOT || tokens[index] == INT || tokens[index] == ID) {
        if (a == 1) {
            ceng++;
            t(ceng);
            cout << "StmtList (" << line_num_record[index] << ")" << endl;
        }
        Stmt(ceng);
        StmtList(ceng);
    }
}
void Stmt(int ceng) { // Stmt->Exp D| W Exp D
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Stmt (" << line_num_record[index] << ")" << endl;
    }
    if (tokens[index] == RETURN) {
        if (a == 1) {
            t(ceng + 1);
            cout << "RETURN" << endl;
        }
        index++;
        Exp(ceng);
        if (tokens[index] == SEMI) {
            if (a == 1) {
                t(ceng + 1);
                cout << "SEMI" << endl;
            }
            index++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = index;
        }
    } else {
        Exp(ceng);
        if (tokens[index] == SEMI) {
            if (a == 1) {
                t(ceng + 1);
                cout << "SEMI" << endl;
            }
            index++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = index;
        }
    }
}
void DefList(int ceng) { // DefList-> Def DefList |e 注意e
    if (tokens[index] == TYPE) {
        if (a == 1) {
            ceng++;
            t(ceng);
            cout << "DefList (" << line_num_record[index] << ")" << endl;
        }
        Def(ceng);
        DefList(ceng);
    }
}
void Def(int ceng) { // Def-> Specifier DecList D
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Def (" << line_num_record[index] << ")" << endl;
    }
    Specifier(ceng);
    DecList(ceng);
    if (tokens[index] == SEMI) {
        if (a == 1) {
            t(ceng + 1);
            cout << "SEMI" << endl;
        }
        index++;
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void DecList(int ceng) { // DecList->Dec();
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "DecList (" << line_num_record[index] << ")" << endl;
    }
    Dec(ceng);
}
void Dec(int ceng) { // Dec->VarDec E Exp |VaeDec
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Dec (" << line_num_record[index] << ")" << endl;
    }
    VarDec(ceng);
    if (tokens[index] == ASSIGNOP) {
        if (a == 1) {
            t(ceng + 1);
            cout << "ASSIGNOP" << endl;
        }
        index++;
        Exp(ceng);
    }
}
void Exp(int ceng) { // Exp-> O Exp Q|N Exp | A | C
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Exp (" << line_num_record[index] << ")" << endl;
    }
    if (tokens[index] == LP) {
        if (a == 1) {
            t(ceng + 1);
            cout << "LP" << endl;
        }
        index++;
        Exp(ceng);
        if (tokens[index] == RP) {
            if (a == 1) {
                t(ceng + 1);
                cout << "RP" << endl;
            }
            index++;
        } else {
            error = 1;
            errornum = index;
        }
    } else if (tokens[index] == NOT) {
        if (a == 1) {
            t(ceng + 1);
            cout << "NOT" << endl;
        }
        index++;
        Exp(ceng);
    } else if (tokens[index] == INT) {
        if (a == 1) {
            t(ceng + 1);
            cout << "INT: ";
            int k = 0;
            vector<TokenInfo>::iterator iter;
            for (iter = tokenVec.begin(); k < index; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        index++;
    } else if (tokens[index] == ID) {
        if (a == 1) {
            t(ceng + 1);
            cout << "ID: ";
            int k = 0;
            vector<TokenInfo>::iterator iter;
            for (iter = tokenVec.begin(); k < index; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        index++;
    } else {
        error = 1;
        errornum = index;
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
            line_num_record[sum] = (*iter).line;
            if ((*iter).type == "INT") {
                auto content = (*iter).word;
                (*iter).word = to_string(stoi(content)); // 注意八进制和16进制向十进制的转换
                tokens[sum] = INT;
            }
            if ((*iter).type == "FLOAT") tokens[sum] = FLOAT;
            if ((*iter).type == "ID") tokens[sum] = ID;
            if ((*iter).type == "SEMI") tokens[sum] = SEMI;
            if ((*iter).type == "ASSIGNOP") tokens[sum] = ASSIGNOP;
            if ((*iter).type == "RELOP") tokens[sum] = RELOP;
            if ((*iter).type == "PLUS") tokens[sum] = PLUS;
            if ((*iter).type == "MINUS") tokens[sum] = MINUS;
            if ((*iter).type == "STAR") tokens[sum] = STAR;
            if ((*iter).type == "DIV") tokens[sum] = DIV;
            if ((*iter).type == "AND") tokens[sum] = AND;
            if ((*iter).type == "OR") tokens[sum] = OR;
            if ((*iter).type == "DOT") tokens[sum] = DOT;
            if ((*iter).type == "NOT") tokens[sum] = NOT;
            if ((*iter).type == "LP") tokens[sum] = LP;
            if ((*iter).type == "RP") tokens[sum] = RP;
            if ((*iter).type == "LB") tokens[sum] = LB;
            if ((*iter).type == "RB") tokens[sum] = RB;
            if ((*iter).type == "LC") tokens[sum] = LC;
            if ((*iter).type == "RC") tokens[sum] = RC;
            if ((*iter).type == "STRUCT") tokens[sum] = tokentype::STRUCT;
            if ((*iter).type == "RETURN") tokens[sum] = RETURN;
            if ((*iter).type == "IF") tokens[sum] = tokentype::IF;
            if ((*iter).type == "ELSE") tokens[sum] = tokentype::ELSE;
            if ((*iter).type == "WHILE") tokens[sum] = tokentype::WHILE;
            if ((*iter).type == "TYPE") tokens[sum] = TYPE;
            if ((*iter).type == "COMMA") tokens[sum] = COMMA;
            sum++;
        }
    }
    Program(0);
    if (errornum == 2) {
        a = 1;
        index = 0;
        Program(0);
    } else {
        if (error == 1 || index != sum) { // 存在语法错误或者未处理到句子末尾
            if (errornum == 1 || line_num_record[errornum] == line_num_record[errornum - 1])
                cout << "Error type (Syntactical) at line " << line_num_record[errornum] << "." << endl;
            else
                cout << "Error type (Syntactical) at line " << line_num_record[errornum - 1] << "." << endl;
        } else {
            a = 1;
            index = 0;
            Program(0);
        }
    }
}
