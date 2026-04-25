// DFA-based scanner
// Regex rules:
//   identifier -> [a-zA-Z_][a-zA-Z0-9_]*
//   number     -> [0-9]+
//   Multi-char operators checked before single-char (== before =)

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"
using namespace std;

class Lexer 
{
private:
    string src;
    int pos;
    int line;
    vector<string> errors;

    char peek();
    char advance();
    bool match(char expected);

public:
    Lexer(string input);
    vector<Token> tokenize();
    vector<string> getErrors();
};

#endif