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
vector<string> Keyword = {"scanf", "printf", "for", "do", "main", "abs", "sqrt"};
vector<string> Return = {"return"};
vector<string> IF = {"if"};
vector<string> ELSE = {"else"};
vector<string> WHILE = {"while"};
vector<string> STRUCT = {"struct"};
vector<string> Type = {"int", "void", "char", "double", "short", "float"};
//*********************************************
int number[200];  // 记录所在行数
char def[200];    // 记录词法类型
int sum = 0;      // 记录词法数量
int error = 0;    // 报错标志
int errornum = 0; // 错误词法
int flag = 0;     // 从0走到sum 读完所有词法
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

vector<Node> stack;
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
    stack.push_back(temp);
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

void push1() // 十进制
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
void push0() // 八进制 十六进制
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
            push1();
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
                push1();
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
                push1();
            else
                push("PLUS");
        }
        if (ch == '.') {
            word += ch;
            ch = text[++i];
            if (ch < '0' || ch > '9') {
                push("DOT");
            } else
                push1();
        }
        if (ch >= '0' && ch <= '9') {
            if (ch >= '1' && ch <= '9') {
                word += ch;
                ch = text[++i];
                push1();
            } else if (ch == '0') {
                word += ch;
                ch = text[++i];
                push0();
            }
        }
        if (isalnum(ch) || ch == '_') {
            while (isalnum(ch) || ch == '_') {
                word += ch;
                ch = text[++i];
            }
            if (find(Keyword.begin(), Keyword.end(), word) != Keyword.end()) {
                push("ID");

            } else if (find(Return.begin(), Return.end(), word) != Return.end())
                push("RETURN");
            else if (find(IF.begin(), IF.end(), word) != IF.end())
                push("IF");
            else if (find(ELSE.begin(), ELSE.end(), word) != ELSE.end())
                push("ELSE");
            else if (find(WHILE.begin(), WHILE.end(), word) != WHILE.end())
                push("WHILE");
            else if (find(STRUCT.begin(), STRUCT.end(), word) != STRUCT.end())
                push("STRUCT");
            else if (find(Type.begin(), Type.end(), word) != Type.end())
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
    if (def[flag] == 'a') {
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
    if (def[flag] == 'a') {
        flag++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void VarDec() { // VarDec->C
    if (def[flag] == 'C') {
        flag++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void FunDec() { // FunDec->C O Q|C
    if (def[flag] == 'C') {
        flag++; // 匹配成功
        if (def[flag] == 'O') {
            flag++;
            if (def[flag] == 'Q') {
                flag++;
            } else { // 语法错误 匹配失败
                error = 1;
                errornum = flag;
            }
        }
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void Compst() { // Compst-> T DefList StmtList U
    if (def[flag] == 'T') {
        flag++;
        DefList();
        StmtList();
        if (def[flag] == 'U') {
            flag++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = flag;
        }
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void StmtList() { // StmtList->Stmt StmtList|e    注意e
    if (def[flag] == 'W' || def[flag] == 'O' || def[flag] == 'N' || def[flag] == 'A' || def[flag] == 'C') {
        Stmt();
        StmtList();
    }
}
void Stmt() { // Stmt->Exp D| W Exp D
    if (def[flag] == 'W') {
        flag++;
        Exp();
        if (def[flag] == 'D') {
            flag++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = flag;
        }
    } else {
        Exp();
        if (def[flag] == 'D') {
            flag++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = flag;
        }
    }
}
void DefList() { // DefList-> Def DefList |e 注意e
    if (def[flag] == 'a') {
        Def();
        DefList();
    }
}
void Def() { // Def-> Specifier DecList D
    Specifier();
    DecList();
    if (def[flag] == 'D') {
        flag++;
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void DecList() { // DecList->Dec();
    Dec();
}
void Dec() { // Dec->VarDec E Exp |VaeDec
    VarDec();
    if (def[flag] == 'E') {
        flag++;
        Exp();
    }
}
void Exp() { // Exp-> O Exp Q|N Exp | A | C
    if (def[flag] == 'O') {
        flag++;
        Exp();
        if (def[flag] == 'Q') {
            flag++;
        } else {
            error = 1;
            errornum = flag;
        }
    } else if (def[flag] == 'N') {
        flag++;
        Exp();
    } else if (def[flag] == 'A') {
        flag++;
    } else if (def[flag] == 'C') {
        flag++;
    } else {
        error = 1;
        errornum = flag;
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
        for (vector<Node>::iterator iter = stack.begin(); iter != stack.end(); iter++) {
            number[sum] = (*iter).line;
            if ((*iter).type == "INT") def[sum] = 'A';
            if ((*iter).type == "FLOAT") def[sum] = 'B';
            if ((*iter).type == "ID") def[sum] = 'C';
            if ((*iter).type == "SEMI") def[sum] = 'D';
            if ((*iter).type == "ASSIGNOP") def[sum] = 'E';
            if ((*iter).type == "RELOP") def[sum] = 'F';
            if ((*iter).type == "PLUS") def[sum] = 'G';
            if ((*iter).type == "MINUS") def[sum] = 'H';
            if ((*iter).type == "STAR") def[sum] = 'I';
            if ((*iter).type == "DIV") def[sum] = 'J';
            if ((*iter).type == "AND") def[sum] = 'K';
            if ((*iter).type == "OR") def[sum] = 'L';
            if ((*iter).type == "DOT") def[sum] = 'M';
            if ((*iter).type == "NOT") def[sum] = 'N';
            if ((*iter).type == "LP") def[sum] = 'O';
            if ((*iter).type == "RP") def[sum] = 'Q';
            if ((*iter).type == "LB") def[sum] = 'R';
            if ((*iter).type == "RB") def[sum] = 'S';
            if ((*iter).type == "LC") def[sum] = 'T';
            if ((*iter).type == "RC") def[sum] = 'U';
            if ((*iter).type == "STRUCT") def[sum] = 'V';
            if ((*iter).type == "RETURN") def[sum] = 'W';
            if ((*iter).type == "IF") def[sum] = 'X';
            if ((*iter).type == "ELSE") def[sum] = 'Y';
            if ((*iter).type == "WHILE") def[sum] = 'Z';
            if ((*iter).type == "TYPE") def[sum] = 'a';
            if ((*iter).type == "COMMA") def[sum] = 'b';
            sum++;
        }
    }
    Program();
    if (errornum == 2) {
        cout << "Syntactical Correct." << endl;
    } else {

        if (error == 1 || flag != sum) { // 存在语法错误或者未处理到句子末尾
            if (errornum == 1 || number[errornum] == number[errornum - 1])
                cout << "Error type (Syntactical) at line " << number[errornum] << "." << endl;
            else
                cout << "Error type (Syntactical) at line " << number[errornum - 1] << "." << endl;
        } else
            cout << "Syntactical Correct." << endl;
    }
}
