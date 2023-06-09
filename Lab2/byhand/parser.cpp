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
int tokenSum = 0;              // 记录词法数量
int error = 0;            // 报错标志
int errornum = 0;         // 错误词法
int index = 0;             // 从0走到sum 读完所有词法
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
struct Node {
    int line = 0;
    string type;
    string word;
};

vector<Node> tokenVec;
int line = 1;
char text[1000] = "";
char ch = ' ';
int len = 0;
int i = 0;
int example = 1;
int eline = 0;
string word;
Node temp;

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
void Program();
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
void Program() { // Program->ExtDeflist;
    ExtDeflist();
}
void ExtDeflist() { // ExtDeflist-> ExtDef ExtDeflist|e  注意走e的问题
    if (tokens[index] == TYPE) {
        ExtDef();
        ExtDeflist();
    }
}
void ExtDef() { // ExtDef->  Specifier FunDec Compst
    Specifier();
    FunDec();
    Compst();
}
void Specifier() { // Specifier->a
    if (tokens[index] == TYPE) {
        index++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void VarDec() { // VarDec->C
    if (tokens[index] == ID) {
        index++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void FunDec() { // FunDec->C O Q|C
    if (tokens[index] == ID) {
        index++; // 匹配成功
        if (tokens[index] == LP) {
            index++;
            if (tokens[index] == RP) {
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
void Compst() { // Compst-> T DefList StmtList U
    if (tokens[index] == LC) {
        index++;
        DefList();
        StmtList();
        if (tokens[index] == RC) {
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
void StmtList() { // StmtList->Stmt StmtList|e    注意e
    if (tokens[index] == RETURN || tokens[index] == LP || tokens[index] == NOT || tokens[index] == 'A' || tokens[index] == ID) {
        Stmt();
        StmtList();
    }
}
void Stmt() { // Stmt->Exp D| W Exp D
    if (tokens[index] == RETURN) {
        index++;
        Exp();
        if (tokens[index] == SEMI) {
            index++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = index;
        }
    } else {
        Exp();
        if (tokens[index] == SEMI) {
            index++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = index;
        }
    }
}
void DefList() { // DefList-> Def DefList |e 注意e
    if (tokens[index] == TYPE) {
        Def();
        DefList();
    }
}
void Def() { // Def-> Specifier DecList D
    Specifier();
    DecList();
    if (tokens[index] == SEMI) {
        index++;
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = index;
    }
}
void DecList() { // DecList->Dec();
    Dec();
}
void Dec() { // Dec->VarDec E Exp |VaeDec
    VarDec();
    if (tokens[index] == ASSIGNOP) {
        index++;
        Exp();
    }
}
void Exp() { // Exp-> O Exp Q|N Exp | A | C
    if (tokens[index] == LP) {
        index++;
        Exp();
        if (tokens[index] == RP) {
            index++;
        } else {
            error = 1;
            errornum = index;
        }
    } else if (tokens[index] == NOT) {
        index++;
        Exp();
    } else if (tokens[index] == INT) {
        index++;
    } else if (tokens[index] == ID) {
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
        for (vector<Node>::iterator iter = tokenVec.begin(); iter != tokenVec.end(); iter++) {
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
    Program();
    if (errornum == 2) {
        cout << "Syntactical Correct." << endl;
    } else {

        if (error == 1 || index != tokenSum) { // 存在语法错误或者未处理到句子末尾
            if (errornum == 1 || line_num_record[errornum] == line_num_record[errornum - 1])
                cout << "Error type (Syntactical) at line " << line_num_record[errornum] << "." << endl;
            else
                cout << "Error type (Syntactical) at line " << line_num_record[errornum - 1] << "." << endl;
        } else
            cout << "Syntactical Correct." << endl;
    }
}
