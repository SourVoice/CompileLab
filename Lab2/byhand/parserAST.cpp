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
        cout << "Program (" << number[flag] << ")" << endl;
    }
    ExtDeflist(ceng);
}
void ExtDeflist(int ceng) { // ExtDeflist-> ExtDef ExtDeflist|e  注意走e的问题
    if (def[flag] == 'a') {

        if (a == 1) {
            ceng++;
            t(ceng);
            cout << "ExtDefList (" << number[flag] << ")" << endl;
        }
        ExtDef(ceng);
        ExtDeflist(ceng);
    }
}
void ExtDef(int ceng) { // ExtDef->  Specifier FunDec Compst
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "ExtDef (" << number[flag] << ")" << endl;
    }
    Specifier(ceng);
    FunDec(ceng);
    Compst(ceng);
}
void Specifier(int ceng) { // Specifier->a
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Specifier (" << number[flag] << ")" << endl;
    }
    if (def[flag] == 'a') {
        if (a == 1) {
            t(ceng + 1);
            cout << "TYPE: ";
            int k = 0;
            vector<Node>::iterator iter;
            for (iter = stack.begin(); k < flag; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        flag++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void VarDec(int ceng) { // VarDec->C
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "VarDec (" << number[flag] << ")" << endl;
    }
    if (def[flag] == 'C') {
        if (a == 1) {
            t(ceng + 1);
            cout << "ID: ";
            int k = 0;
            vector<Node>::iterator iter;
            for (iter = stack.begin(); k < flag; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        flag++; // 匹配成功
    } else {    // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void FunDec(int ceng) { // FunDec->C O Q|C
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "FunDec (" << number[flag] << ")" << endl;
    }
    if (def[flag] == 'C') {
        if (a == 1) {
            t(ceng + 1);
            cout << "ID: ";
            int k = 0;
            vector<Node>::iterator iter;
            for (iter = stack.begin(); k < flag; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        flag++; // 匹配成功
        if (def[flag] == 'O') {
            if (a == 1) {
                t(ceng + 1);
                cout << "LP" << endl;
            }
            flag++;
            if (def[flag] == 'Q') {
                if (a == 1) {
                    t(ceng + 1);
                    cout << "RP" << endl;
                }
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
void Compst(int ceng) { // Compst-> T DefList StmtList U
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Compst (" << number[flag] << ")" << endl;
    }
    if (def[flag] == 'T') {
        if (a == 1) {
            t(ceng + 1);
            cout << "LC" << endl;
        }
        flag++;
        DefList(ceng);
        StmtList(ceng);
        if (def[flag] == 'U') {
            if (a == 1) {
                t(ceng + 1);
                cout << "RC" << endl;
            }
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
void StmtList(int ceng) { // StmtList->Stmt StmtList|e    注意e
    if (def[flag] == 'W' || def[flag] == 'O' || def[flag] == 'N' || def[flag] == 'A' || def[flag] == 'C') {
        if (a == 1) {
            ceng++;
            t(ceng);
            cout << "StmtList (" << number[flag] << ")" << endl;
        }
        Stmt(ceng);
        StmtList(ceng);
    }
}
void Stmt(int ceng) { // Stmt->Exp D| W Exp D
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Stmt (" << number[flag] << ")" << endl;
    }
    if (def[flag] == 'W') {
        if (a == 1) {
            t(ceng + 1);
            cout << "RETURN" << endl;
        }
        flag++;
        Exp(ceng);
        if (def[flag] == 'D') {
            if (a == 1) {
                t(ceng + 1);
                cout << "SEMI" << endl;
            }
            flag++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = flag;
        }
    } else {
        Exp(ceng);
        if (def[flag] == 'D') {
            if (a == 1) {
                t(ceng + 1);
                cout << "SEMI" << endl;
            }
            flag++;
        } else { // 语法错误 匹配失败
            error = 1;
            errornum = flag;
        }
    }
}
void DefList(int ceng) { // DefList-> Def DefList |e 注意e
    if (def[flag] == 'a') {
        if (a == 1) {
            ceng++;
            t(ceng);
            cout << "DefList (" << number[flag] << ")" << endl;
        }
        Def(ceng);
        DefList(ceng);
    }
}
void Def(int ceng) { // Def-> Specifier DecList D
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Def (" << number[flag] << ")" << endl;
    }
    Specifier(ceng);
    DecList(ceng);
    if (def[flag] == 'D') {
        if (a == 1) {
            t(ceng + 1);
            cout << "SEMI" << endl;
        }
        flag++;
    } else { // 语法错误 匹配失败
        error = 1;
        errornum = flag;
    }
}
void DecList(int ceng) { // DecList->Dec();
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "DecList (" << number[flag] << ")" << endl;
    }
    Dec(ceng);
}
void Dec(int ceng) { // Dec->VarDec E Exp |VaeDec
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Dec (" << number[flag] << ")" << endl;
    }
    VarDec(ceng);
    if (def[flag] == 'E') {
        if (a == 1) {
            t(ceng + 1);
            cout << "ASSIGNOP" << endl;
        }
        flag++;
        Exp(ceng);
    }

}
void Exp(int ceng) { // Exp-> O Exp Q|N Exp | A | C
    if (a == 1) {
        ceng++;
        t(ceng);
        cout << "Exp (" << number[flag] << ")" << endl;
    }
    if (def[flag] == 'O') {
        if (a == 1) {
            t(ceng + 1);
            cout << "LP" << endl;
        }
        flag++;
        Exp(ceng);
        if (def[flag] == 'Q') {
            if (a == 1) {
                t(ceng + 1);
                cout << "RP" << endl;
            }
            flag++;
        } else {
            error = 1;
            errornum = flag;
        }
    } else if (def[flag] == 'N') {
        if (a == 1) {
            t(ceng + 1);
            cout << "NOT" << endl;
        }
        flag++;
        Exp(ceng);
    } else if (def[flag] == 'A') {
        if (a == 1) {
            t(ceng + 1);
            cout << "INT: ";
            int k = 0;
            vector<Node>::iterator iter;
            for (iter = stack.begin(); k < flag; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
        flag++;
    } else if (def[flag] == 'C') {
        if (a == 1) {
            t(ceng + 1);
            cout << "ID: ";
            int k = 0;
            vector<Node>::iterator iter;
            for (iter = stack.begin(); k < flag; iter++, k++)
                ;
            cout << (*iter).word << endl;
        }
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
            if ((*iter).type == "INT") { // 注意八进制和16进制向十进制的转换
                int ab = 0, flag1 = 0, length = 0;
                length = (*iter).word.length();

                if (((*iter).word[0] == '0' && (*iter).word[1] == 'x') || ((*iter).word[0] == '0' && (*iter).word[1] == 'X')) // 该数为16进制
                {
                    flag1 = 16;
                    ab = 2;
                } else if ((*iter).word[0] == '0') // 该数为八进制
                {
                    flag1 = 8;
                    ab = 0;
                } else {
                    flag1 = 10;
                }

                int ans = 0;
                for (int j = ab; j < length; j++) {
                    char t = (*iter).word[j];
                    if (t >= '0' && t <= '9')
                        ans = ans * flag1 + (t - '0');
                    else if (t >= 'A' && t <= 'F')
                        ans = ans * flag1 + (t - 'A' + 10);
                    else
                        ans = ans * flag1 + (t - 'a' + 10);
                }

                (*iter).word = to_string(ans);
                def[sum] = 'A';
            }
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
    Program(0);
    if (errornum == 2) {
        a = 1;
        flag = 0;
        Program(0);
    } else {

        if (error == 1 || flag != sum) { // 存在语法错误或者未处理到句子末尾
            if (errornum == 1 || number[errornum] == number[errornum - 1])
                cout << "Error type (Syntactical) at line " << number[errornum] << "." << endl;
            else
                cout << "Error type (Syntactical) at line " << number[errornum - 1] << "." << endl;
        } else {
            a = 1;
            flag = 0;
            Program(0);
        }
    }
}
