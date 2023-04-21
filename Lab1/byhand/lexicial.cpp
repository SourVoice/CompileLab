/**
 * @file lexicial.cpp
 * @author your name (you@domain.com)
 * @brief 手工构造的词法分析
 * @details 浮点数部分未经过调试 
 * @version 0.1
 * @date 2023-04-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

struct Info {
    int line;
    string type = "whatever";
    string token;
};
bool lexError = false;
vector<Info> vec_Info = {};
vector<char> symbols = {';', ',', '(', ')', '+', '-', '[', ']', '{', '}'};
vector<string> multi_symbols = {"==", "!=", "<=", ">="};

vector<string> keywords = {"char", "else", "float", "for", "if", "int", "return", "struct", "while"};

bool store_symbols(int line, string &symbol) {
    if (symbol == ";") {
        vec_Info.push_back({line, "SEMI", symbol});
    } else if (symbol == ",") {
        vec_Info.push_back({line, "COMMA", symbol});
    } else if (symbol == "(") {
        vec_Info.push_back({line, "LP", symbol});
    } else if (symbol == ")") {
        vec_Info.push_back({line, "RP", symbol});
    } else if (symbol == "+") {
        vec_Info.push_back({line, "PLUS", symbol});
    } else if (symbol == "-") {
        vec_Info.push_back({line, "MINUS", symbol});
    } else if (symbol == "[") {
        vec_Info.push_back({line, "LB", symbol});
    } else if (symbol == "]") {
        vec_Info.push_back({line, "RB", symbol});
    } else if (symbol == "{") {
        vec_Info.push_back({line, "LC", symbol});
    } else if (symbol == "}") {
        vec_Info.push_back({line, "RC", symbol});
    } else {
        cerr << symbol << " is not in the symbols vector.\n";
        return false;
    }
    return true;
}

bool sotre_keyword(int line, string &keyword) {
    if (keyword == "if") {
        vec_Info.push_back({line, "IF", keyword});
    } else if (keyword == "else") {
        vec_Info.push_back({line, "ELSE", keyword});
    } else if (keyword == "while") {
        vec_Info.push_back({line, "WHILE", keyword});
    } else if (keyword == "struct") {
        vec_Info.push_back({line, "STRUCT", keyword});
    } else if (keyword == "return") {
        vec_Info.push_back({line, "RETURN", keyword});
    } else if (keyword == "int") {
        vec_Info.push_back({line, "TYPE", keyword});
    } else if (keyword == "float") {
        vec_Info.push_back({line, "TYPE", keyword});
    } else {
        cerr << keyword << " is not in the keywords vector.\n";
        return false;
    }
    return true;
}

void lex(char *text) {
    int line = 0;
    char *yypos = text;
    string word;
    while (*yypos != EOF && *yypos != '\0') {
        char ch = *yypos;
        if (ch == ' ' || ch == '\t' || ch == '\n') // 跳过空格,
        {
            if (ch == '\n') {
                line++;
            }
            yypos++;
        } else if (ch == '/') {        // 注释 ( // 型)
            if (*(yypos + 1) == '/') { // 识别为 //
                while (*yypos != '\n')
                    yypos++;
            } else if (*(yypos + 1) == '*') {
                yypos++;
                while (*(yypos + 1) != '*') {
                    yypos++;
                    if (*(yypos + 1) == '\n')
                        line++;
                }
            } else { // 一半的注释
                cerr << "error";
            }
        } else if (ch == '*') { // 识别为 */
            if (*(yypos + 1) == '/') {
                yypos++;
            } else { // 识别为 "*"
                word.push_back(ch);
                vec_Info.push_back({line, "STAR", word});
            }
        } else if (find(symbols.begin(), symbols.end(), ch) != symbols.end()) { // 识别单个符号
            word.push_back(ch);
            store_symbols(line, word);
            yypos += word.size();
        } else if (ch == '=') { //
            if (*(yypos + 1) == '=') {
                word = "==";
                vec_Info.push_back({line, "ASSIGNOP", word});
                yypos += word.size();
            } else {
                word = "=";
                vec_Info.push_back({line, "ASSIGNOP", word});
                yypos += word.size();
            }
        } else if (ch == '!') {
            if (*(yypos + 1) == '=') {
                word = "!=";
                vec_Info.push_back({line, "RELOP", word});
                yypos += word.size();
            } else {
                word = "!";
                vec_Info.push_back({line, word});
                yypos += word.size();
            }
        } else if (ch == '>') {
            if (*(yypos + 1) == '=') {
                word = ">=";
                vec_Info.push_back({line, "RELOP", word});
                yypos += word.size();
            } else {
                word = ">";
                vec_Info.push_back({line, "RELOP", word});
                yypos += word.size();
            }
        } else if (ch == '<') {
            if (*(yypos + 1) == '=') {
                word = "<=";
                vec_Info.push_back({line, "RELOP", word});
                yypos += word.size();
            } else {
                word = "<";
                vec_Info.push_back({line, "RELOP", word});
                yypos += word.size();
            }
        } else if (ch == '&') {
            if (*(yypos + 1) == '&') {
                word = "&&";
                vec_Info.push_back({line, "AND", word});
                yypos += word.size();
            } else {
                word = "&";
                vec_Info.push_back({line, "AND BIT", word});
                yypos += word.size();
            }
        } else if (ch == '|') {
            if (*(yypos + 1) == '|') {
                word = "||";
                vec_Info.push_back({line, "OR", word});
                yypos += word.size();
            } else {
                word = "|";
                vec_Info.push_back({line, "OR BIT", word});
                yypos += word.size();
            }
        } else if (isalpha(ch) || ch == '_') // 标识符 || 关键字
        {
            auto start = yypos;
            while (isdigit(ch) || isalpha(ch) || ch == '_') {
                ch = *++yypos;
            }
            while (start < yypos) {
                word.push_back(*start++);
            }
            if (find(keywords.begin(), keywords.end(), word) != keywords.end()) { // 关键字
                sotre_keyword(line, word);
            } else { // 标识符
                vec_Info.push_back({line, "ID", word});
            }
        } else if (ch == '.') { // 匹配数字
            auto start = yypos;
            if (isdigit(*(yypos + 1))) { // .后面至少一个数字
                ++yypos;
                while (isdigit(*yypos)) {
                    yypos++;
                }
                if (*yypos == 'e') {
                    yypos++;
                    ch = *yypos;
                    if (ch == '+' || ch == '-') {
                        yypos++;
                        while (isdigit(*yypos)) {
                            yypos++;
                        }
                    } else if (isdigit(ch)) {
                        yypos++;
                        while (isdigit(*yypos)) {
                            yypos++;
                        }
                    }
                } else {
                    while (isdigit(*yypos)) {
                        yypos++;
                    }
                }
                // 存储
                while (start < yypos) {
                    word.push_back(*start++);
                }
                vec_Info.push_back({line, "FLOAT", word});
            } else {
                lexError = true;
                cerr << "非法字符";
            }
        } else if (ch == '0') { // 匹配十六进制
            auto start = yypos;
            if (*(yypos + 1) == 'x' || *(yypos + 1) == 'X') { // !使用isHex()进行判断
                yypos++;
                auto isHex = [](const char &x) -> bool {
                    return (x >= 'a' || x <= 'f') || (x >= 'A' || x <= 'F') || isdigit(x);
                };
                if (isHex(*yypos)) // 至少一个合法, {HEX}+
                {
                    while (isHex(*yypos)) {
                        yypos++;
                    }
                    while (start < yypos) {
                        word.push_back(*start);
                    }
                    vec_Info.push_back({line, "INT HEX", word});

                } else if (*(yypos + 1) == '.') {
                    if (isdigit(*(yypos + 1))) { // .后面至少一个数字
                        ++yypos;
                        while (isdigit(*yypos)) {
                            yypos++;
                        }
                        if (*yypos == 'e') {
                            yypos++;
                            ch = *yypos;
                            if (ch == '+' || ch == '-') {
                                yypos++;
                                while (isdigit(*yypos)) {
                                    yypos++;
                                }
                            } else if (isdigit(ch)) {
                                yypos++;
                                while (isdigit(*yypos)) {
                                    yypos++;
                                }
                            }
                        } else {
                            while (isdigit(*yypos)) {
                                yypos++;
                            }
                        }
                        // 存储
                        while (start < yypos) {
                            word.push_back(*start++);
                        }
                        vec_Info.push_back({line, word});
                    } else {
                        cerr << "非法字符";
                    }
                }

                else { // 只有 "0x"
                    cerr << "error";
                }
            } else if (*(yypos + 1) == '.') {
                if (isdigit(*(yypos + 1))) {
                    ++yypos;
                    while (isdigit(*yypos)) {
                        yypos++;
                    }
                    if (*yypos == 'e') {
                        yypos++;
                        ch = *yypos;
                        if (ch == '+' || ch == '-') {
                            yypos++;
                            while (isdigit(*yypos)) {
                                yypos++;
                            }
                        } else if (isdigit(ch)) {
                            yypos++;
                            while (isdigit(*yypos)) {
                                yypos++;
                            }
                        }
                    } else {
                        while (isdigit(*yypos)) {
                            yypos++;
                        }
                    }
                    // 存储
                    while (start < yypos) {
                        word.push_back(*start++);
                    }
                    vec_Info.push_back({line, word});
                } else {
                    cerr << "非法字符";
                }

            } else {
                word = "0";
                vec_Info.push_back({line, "INT", word});
                yypos++;
            }
        } else if (isdigit(ch)) { // 判断整形数, 如果有 ".", 进行上面"."状态的判断
            auto start = yypos;
            while (isdigit(*yypos)) {
                yypos++;
            }
            if (*yypos == '.') {

                if (isdigit(*(yypos + 1))) {
                    ++yypos;
                    while (isdigit(*yypos)) {
                        yypos++;
                    }
                    if (*yypos == 'e') {
                        yypos++;
                        ch = *yypos;
                        if (ch == '+' || ch == '-') {
                            yypos++;
                            while (isdigit(*yypos)) {
                                yypos++;
                            }
                        } else if (isdigit(ch)) {
                            yypos++;
                            while (isdigit(*yypos)) {
                                yypos++;
                            }
                        }
                    } else {
                        while (isdigit(*yypos)) {
                            yypos++;
                        }
                    }
                    // 存储
                    while (start < yypos) {
                        word.push_back(*start++);
                    }
                    vec_Info.push_back({line, "FLOAT", word});
                } else {
                    cerr << "非法字符";
                }

            } else {
                while (start < yypos) {
                    word.push_back(*start++);
                }
                vec_Info.push_back({line, "INT", word});
            }
        } else { // 非法符号
            lexError = true;
            word.push_back(ch);
            {
                printf("Error type (Lexical) at line %d: Mysterious character \"%s\".\n", line, word.c_str());
                lexError = 1;
            }
            yypos++;
        }
        word.clear();
    }
}

int main(int argc, char *argv[]) {
    ifstream infile;
    if (argc > 1) {
        infile.open(argv[1]);
        if (!infile) {
            cerr << "error open";
            exit(1);
        } else {
            cout << "open file " << argv[1] << endl;
        }
    }
    std::string contents;
    infile.unsetf(ios::skipws);
    contents.assign(std::istreambuf_iterator<char>(infile),
                    std::istreambuf_iterator<char>());
    int len = contents.length() + 1;
    char *cstr = new char[len];
    strcpy(cstr, contents.c_str());
    // for (int i = 0; context[i] != EOF; i++) { //     infile >> context[i]; // } // for (int i = 0; i < len; i++) { //     cout << cstr[i]; // } // cout << endl;
    lex(cstr);
    if (!lexError) {
        for (vector<Info>::iterator iter = vec_Info.begin(); iter != vec_Info.end(); iter++)
            cout << "line" << (*iter).line << ":(" << (*iter).type << ", " << (*iter).token << ')' << endl;
    }
}
