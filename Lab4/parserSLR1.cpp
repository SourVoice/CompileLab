/**
 * @file parserSLR.cpp
 * @author gbh (you@domain.com)
 * @brief
 * (1) StmtList' => StmtList
 * (2) StmtList => Stmt StmtList
 * (3) StmtList => empty
 * (4) Stmt => Exp SEMI
 * (5) Stmt => IF LP Exp RP Stme
 * (6) Exp => MINUS Exp
 * (7) Exp => ID
 * 
 * @version 0.1
 * @date 2023-05-19
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
    INT = 0, FLOAT, ID, SEMI, COMMA, ASSIGNOP, RELOP, PLUS,
    MINUS, STAR, DIV, AND, OR, DOT, NOT, TYPE,
    LP, RP, LB, RB, LC, RC,
    STRUCT, RETURN, IF, ELSE, WHILE, LOWER_THAN_ELSE,
    END /* 代表# */
};
enum nonterminal {
    // 非终结符
    StmtList = 100,
    Stmt,
    Exp,
};
typedef pair<int, int> pii;
typedef pair<int, vector<int>> piv;
vector<string> keywords = {"else", "for", "if", "return", "struct", "while", "return"};
vector<string> Type = {"int", "void", "char", "double", "short", "float"};

enum action { s0 = 200, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, acc, };

enum reduction_ { r0 = 300, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, };

map<pii, vector<int>> action_table = {
    {{s0, IF}, {s4}},
    {{s0, MINUS}, {s5}},
    {{s0, ID}, {s6}},
    {{s0, END}, {r3}},

    {{s1, END}, {acc}},

    {{s2, IF}, {s4}},
    {{s2, MINUS}, {s5}},
    {{s2, ID}, {s6}},
    {{s2, END}, {r3}},

    {{s3, SEMI}, {s8}},

    {{s4, LP}, {s9}},

    {{s5, MINUS}, {s5}},
    {{s5, ID}, {s6}},

    {{s6, SEMI}, {r7}},
    {{s6, IF}, {r7}},
    {{s6, LP}, {r7}},
    {{s6, RP}, {r7}},
    {{s6, MINUS}, {r7}},
    {{s6, ID}, {r7}},
    {{s6, END}, {r7}},

    {{s7, SEMI}, {r2}},
    {{s7, IF}, {r2}},
    {{s7, LP}, {r2}},
    {{s7, RP}, {r2}},
    {{s7, MINUS}, {r2}},
    {{s7, ID}, {r2}},
    {{s7, END}, {r2}},

    {{s8, SEMI}, {r4}},
    {{s8, IF}, {r4}},
    {{s8, LP}, {r4}},
    {{s8, RP}, {r4}},
    {{s8, MINUS}, {r4}},
    {{s8, ID}, {r4}},
    {{s8, END}, {r4}},

    {{s9, MINUS}, {s5}},
    {{s9, ID}, {s6}},

    {{s10, SEMI}, {r6}},
    {{s10, IF}, {r6}},
    {{s10, LP}, {r6}},
    {{s10, RP}, {r6}},
    {{s10, MINUS}, {r6}},
    {{s10, ID}, {r6}},
    {{s10, END}, {r6}},

    {{s11, RP}, {s12}},

    {{s12, IF}, {s4}},
    {{s12, MINUS}, {s5}},
    {{s12, ID}, {s6}},

    {{s13, SEMI}, {r5}},
    {{s13, IF}, {r5}},
    {{s13, LP}, {r5}},
    {{s13, RP}, {r5}},
    {{s13, MINUS}, {r5}},
    {{s13, ID}, {r5}},
    {{s13, END}, {r5}},
};

map<pii, vector<int>> goto_table = {
    {{s0, StmtList}, {s1}},
    {{s0, Stmt}, {s2}},
    {{s0, Exp}, {s3}},

    {{s2, StmtList}, {s7}},
    {{s2, Stmt}, {s2}},
    {{s2, Exp}, {s3}},

    {{s5, Exp}, {s10}},

    {{s9, Exp}, {s11}},

    {{s12, Stmt}, {s13}},
    {{s12, Exp}, {s3}},
};

map<int, piv> reduction = {
    {{r2}, {{StmtList}, {Stmt, StmtList}}},
    {{r3}, {{StmtList}, {}}},

    {{r4}, {{Stmt}, {Exp, SEMI}}},
    {{r5}, {{Stmt}, {IF, LP, Exp, RP, Stmt}}},

    {{r6}, {{Exp}, {MINUS, Exp}}},
    {{r7}, {{Exp}, {ID}}},
};

//*********************************************
int line_num_record[200]; // 记录所在行数
int tokens[200];          // 记录词法类型
int tokenSum = 0;         // 记录词法数量
int synError = 0;         // 报错标志
int errornum = 0;         // 错误词法
int tokenindex = 0;       // 从0走到sum 读完所有词法
stack<int> symbolStack;   // 符号栈
stack<int> stateStack;    // 状态栈

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

void SLR1() {
    stateStack.push(s0);
    symbolStack.push(END);

    while (1) {
        auto state_top = stateStack.top();
        int input_symbol = tokens[tokenindex];
        if (input_symbol < 0 || input_symbol > 28) {
            synError = 1;
            break;
        }

        // 查表得到下一次的状态
        auto iter = action_table.find(make_pair(state_top, input_symbol));
        // 没有查找到, 走到空
        if (iter == action_table.end()) {
            synError = 1;
            break;
        }
        int next_state = iter->second.front();
        // 规约成功
        if (next_state == acc) {
            break;
        }
        // 入栈action
        else if (next_state >= s0 && next_state <= s13) {
            stateStack.push(next_state);
            symbolStack.push(input_symbol);
            tokenindex++;
        }
        // 规约
        else if (next_state >= r0 && next_state <= r10) {
            vector<int> pop_token; 
            auto reduction_value = reduction[next_state];
            for (size_t i = 0; i < reduction_value.second.size(); i++) {
                auto symbol = symbolStack.top();
                pop_token.push_back(symbol);
                symbolStack.pop();
                stateStack.pop();
            }
            // 判断弹出内容是否合法
            reverse(begin(pop_token), end(pop_token)); // 弹出的与预测表相反, 需要反转
            if (pop_token != reduction_value.second) {
                synError = 1;
                break;
            }
            symbolStack.push(reduction_value.first);
            auto s3 = stateStack.top();
            auto it_ = goto_table.find({s3, reduction_value.first});
            if (it_ == goto_table.end()) {
                synError = 1;
                break;
            } else {
                stateStack.push(it_->second.front());
            }
        }
    }
}

//****************************************************************
int main(int argc, char *argv[]) {
    if (argc == 1) {
        text[0] = getchar();
        while (text[len] != EOF) {
            text[++len] = getchar();
        }
    } else {
        ifstream infile;
        infile.open(argv[1]);
        if (!infile) {
            cerr << "error open";
            exit(1);
        } else {
            cout << "open file " << argv[1] << endl;
        }
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
            if ((*iter).type == "ID") tokens[tokenSum] = ID;
            if ((*iter).type == "SEMI") tokens[tokenSum] = SEMI;
            if ((*iter).type == "MINUS") tokens[tokenSum] = MINUS;
            if ((*iter).type == "LP") tokens[tokenSum] = LP;
            if ((*iter).type == "RP") tokens[tokenSum] = RP;
            if ((*iter).type == "IF") tokens[tokenSum] = IF;
            tokenSum++;
        }
        tokens[tokenSum++] = END;
    }
    SLR1();
    stateStack.pop();
    if (synError != 1 && tokenindex == tokenSum - 1 && symbolStack.top() == StmtList && stateStack.top() == s0) {
        cout << "Syntactical Correct." << endl;
    }
    // 特判
    else
        cout << "Error type (Syntactical) at line " << 1 << "." << endl;
}
