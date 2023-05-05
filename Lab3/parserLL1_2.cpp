/**
 * @file parserAST.cpp
 * @author 高本涵 (you@domain.com)

 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <unordered_map>
#include <vector>
using namespace std;
enum tokentype {
    // 终结符
    INT = 0, FLOAT, ID, SEMI, COMMA, ASSIGNOP, RELOP, PLUS, MINUS, STAR, DIV, AND, OR, DOT, NOT, TYPE,
    LP, RP, LB, RB, LC, RC,
    STRUCT, RETURN, IF, ELSE, WHILE,
    LOWER_THAN_ELSE,
    END
};
enum nonterminal {
    // 非终结符
    Program = 100,
    Exp,
    E,
    Args,
    A,
};
typedef pair<int, int> pii;
vector<string> keywords = {"else", "for", "if", "return", "struct", "while", "return"};
vector<string> Type = {"int", "void", "char", "double", "short", "float"};
map<pii, vector<int>> predict_table{
    {{Exp, LP},         {LP, Exp, RP}},
    {{Exp, MINUS},      {MINUS, Exp}},
    {{Exp, ID},         {ID, E}},
    {{Exp, INT},        {INT}},
    {{Exp, FLOAT},      {FLOAT}},
    {{Exp, NOT},        {Exp}},

    {{E, LP},           {LP, Args, RP}},
    {{E, RP},           {}},
    {{E, COMMA},        {}},
    {{E, END},          {}},

    {{Args, LP},        {Exp, A}},
    {{Args, MINUS},     {Exp, A}},
    {{Args, ID},        {Exp, A}},
    {{Args, INT},       {Exp, A}},
    {{Args, FLOAT},     {Exp, A}},
    {{Args, NOT},       {Exp, A}},

    {{A, RP},           {}},
    {{A, COMMA},        {COMMA, Args}},
};
//*********************************************
int line_num_record[200]; // 记录所在行数
int tokens[200];          // 记录词法类型
int tokenSum = 0;         // 记录词法数量
int synError = 0;         // 报错标志
int errornum = 0;         // 错误词法
int tokenindex = 0;       // 从0走到sum 读完所有词法
stack<int> symbolStack;   // 符号栈
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
void fun() {
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

void LL1() {
    symbolStack.push(END);
    symbolStack.push(Exp);
    while (1) {
        auto token = tokens[tokenindex];
        auto top_elem = symbolStack.top();
        if (top_elem == END && token == END) break;
        if (top_elem >= 0 && top_elem < 100) { // 终结符
            if (top_elem == token) {
                symbolStack.pop();
                tokenindex++;
            }
            else {
                synError = 1;
                break;
            }
        } else {
            pii input = {top_elem, token};
            if (!predict_table.count(input)) {
                synError = true;
                break;
            }
            symbolStack.pop();
            auto rules = predict_table[{top_elem, token}];
            if(rules.empty()) continue;
            auto it = rules.crbegin();
            while (it != rules.crend()) {
                symbolStack.push(*it++);
            }
        }
    }
}

//****************************************************************
int main(int argc, char *argv[]) {
    if (argc == 1) {
        text[0] = getchar();
        while (text[len] != EOF) { text[++len] = getchar(); }
    } else {
        ifstream infile;
        infile.open(argv[1]);
        if (!infile) { cerr << "error open"; exit(1); }
        else { cout << "open file " << argv[1] << endl; }
        string filecontent;
        infile.unsetf(ios::skipws);
        filecontent.assign(std::istreambuf_iterator<char>(infile),
                           std::istreambuf_iterator<char>());
        strcpy(text, filecontent.c_str());
    }
    fun();
    if (example) {
        for (vector<TokenInfo>::iterator iter = tokenVec.begin(); iter != tokenVec.end(); iter++) {
            line_num_record[tokenSum] = (*iter).line;
            if ((*iter).type == "INT")      tokens[tokenSum] = INT;
            if ((*iter).type == "FLOAT")    tokens[tokenSum] = FLOAT;
            if ((*iter).type == "ID")       tokens[tokenSum] = ID;
            if ((*iter).type == "MINUS")    tokens[tokenSum] = MINUS;
            if ((*iter).type == "LP")       tokens[tokenSum] = LP;
            if ((*iter).type == "RP")       tokens[tokenSum] = RP;
            if ((*iter).type == "NOT")      tokens[tokenSum] = NOT;
            if ((*iter).type == "COMMA")    tokens[tokenSum] = COMMA;
            tokenSum++;
        }
    }
    LL1();
    // TODO: 退出条件判断
    if (synError == 1 && tokenindex != tokenSum) { // 存在语法错误或者未处理到句子末尾
            cout << "Error type (Syntactical) at line " << line_num_record[tokenindex] << "." << endl;
    } else {
        cout << "Syntactical Correct." << endl;
    }
}
