// parser.h - Parser and Interpreter
// Context-Free Grammar:
//   program     -> declaration*
//   declaration -> intDecl | statement
//   intDecl     -> "int" IDENTIFIER "=" expression ";"
//   statement   -> printStmt | ifStmt | whileStmt | block | assignment
//   printStmt   -> "print" "(" expression ")" ";"
//   ifStmt      -> "if" "(" expression ")" "{" declaration* "}" ("else" "{" declaration* "}")?
//   whileStmt   -> "while" "(" expression ")" "{" declaration* "}"
//   block       -> "{" declaration* "}"
//   assignment  -> IDENTIFIER "=" expression ";"
//   expression  -> equality
//   equality    -> comparison (("==" | "!=") comparison)*
//   comparison  -> term (("<" | ">" | "<=" | ">=") term)*
//   term        -> factor (("+" | "-") factor)*
//   factor      -> unary (("*" | "/") unary)*
//   unary       -> "-" unary | primary
//   primary     -> NUMBER | IDENTIFIER | "(" expression ")"

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "token.h"
using namespace std;

class ParserInterpreter 
{
private:
    vector<Token> tokens;
    int current;
    unordered_map<string, int> variables;
    vector<string> errors;
    vector<int> outputs;

    // token navigation
    Token peek();
    Token previous();
    bool isAtEnd();
    Token advance();
    bool check(TokenType type);
    bool match(vector<TokenType> types);
    Token consume(TokenType type, string message);
    void synchronize();

    // expression parsing
    int expression();
    int equality();
    int comparison();
    int term();
    int factor();
    int unary();
    int primary();

    // statement parsing
    void declaration();
    void intDeclaration();
    void statement();
    void assignmentStatement();
    void printStatement();
    vector<Token> captureBlockTokens();
    void executeBlockTokens(vector<Token> blockTokens);
    void ifStatement();
    void whileStatement();
    void block();

public:
    ParserInterpreter(vector<Token> t);
    ParserInterpreter(vector<Token> t, unordered_map<string, int> vars);

    void parseProgram();
    int evaluateExpressionOnly();

    vector<string> getErrors();
    vector<int> getOutputs();
    unordered_map<string, int> getVariables();

    // expose tokens for post-processing
    vector<Token> getTokens();
};

#endif