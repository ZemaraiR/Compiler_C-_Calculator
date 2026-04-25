#include "token.h"

// token.cpp - Token helper function implementations

string tokenTypeToString(TokenType t) 
{
    if (t == TokenType::INT_KW) return "INT_KW";
    if (t == TokenType::IF_KW) return "IF_KW";
    if (t == TokenType::ELSE_KW) return "ELSE_KW";
    if (t == TokenType::WHILE_KW) return "WHILE_KW";
    if (t == TokenType::PRINT_KW) return "PRINT_KW";
    if (t == TokenType::IDENTIFIER) return "IDENTIFIER";
    if (t == TokenType::NUMBER) return "NUMBER";
    if (t == TokenType::PLUS) return "PLUS";
    if (t == TokenType::MINUS) return "MINUS";
    if (t == TokenType::STAR) return "STAR";
    if (t == TokenType::SLASH) return "SLASH";
    if (t == TokenType::ASSIGN) return "ASSIGN";
    if (t == TokenType::EQ) return "EQ";
    if (t == TokenType::NEQ) return "NEQ";
    if (t == TokenType::LT) return "LT";
    if (t == TokenType::GT) return "GT";
    if (t == TokenType::LE) return "LE";
    if (t == TokenType::GE) return "GE";
    if (t == TokenType::LPAREN) return "LPAREN";
    if (t == TokenType::RPAREN) return "RPAREN";
    if (t == TokenType::LBRACE) return "LBRACE";
    if (t == TokenType::RBRACE) return "RBRACE";
    if (t == TokenType::SEMICOLON) return "SEMICOLON";
    if (t == TokenType::END_OF_FILE) return "EOF";
    return "INVALID";
}

string getCategory(TokenType t) 
{
    if (t == TokenType::INT_KW || t == TokenType::IF_KW || t == TokenType::ELSE_KW ||
        t == TokenType::WHILE_KW || t == TokenType::PRINT_KW)
        return "Keyword";
    if (t == TokenType::IDENTIFIER) return "Identifier";
    if (t == TokenType::NUMBER) return "Integer Literal";
    if (t == TokenType::PLUS || t == TokenType::MINUS || t == TokenType::STAR || t == TokenType::SLASH)
        return "Arithmetic Op";
    if (t == TokenType::ASSIGN) return "Assignment Op";
    if (t == TokenType::EQ || t == TokenType::NEQ || t == TokenType::LT ||
        t == TokenType::GT || t == TokenType::LE || t == TokenType::GE)
        return "Relational Op";
    if (t == TokenType::LPAREN || t == TokenType::RPAREN || t == TokenType::LBRACE ||
        t == TokenType::RBRACE || t == TokenType::SEMICOLON)
        return "Delimiter";
    return "Other";
}