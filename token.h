#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

enum class TokenType 
{
    INT_KW, IF_KW, ELSE_KW, WHILE_KW, PRINT_KW, IDENTIFIER, NUMBER, PLUS, MINUS, 
    STAR, SLASH, ASSIGN, EQ, NEQ, LT, GT, LE, GE, 
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, END_OF_FILE, INVALID
};

struct Token 
{
    TokenType type;
    string lexeme;
    int line;
};

string tokenTypeToString(TokenType t);
string getCategory(TokenType t);

#endif