#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "token.h"
#include "ASTNode.h"
using namespace std;

// Parser and Interpreter
// Context-Free Grammar (Production Rules):
//   program     - declaration*
//   declaration - intDecl | statement
//   intDecl     - "int" IDENTIFIER "=" expression ";"
//   statement   - printStmt | ifStmt | whileStmt | block | assignment
//   printStmt   - "print" "(" expression ")" ";"
//   ifStmt      - "if" "(" expression ")" "{" declaration* "}" ("else" "{" declaration* "}")?
//   whileStmt   - "while" "(" expression ")" "{" declaration* "}"
//   block       - "{" declaration* "}"
//   assignment  - IDENTIFIER "=" expression ";"
//   expression  - equality
//   equality    - comparison (("==" | "!=") comparison)*
//   comparison  - term (("<" | ">" | "<=" | ">=") term)*
//   term        - factor (("+" | "-") factor)*
//   factor      - unary (("*" | "/") unary)*
//   unary       - "-" unary | primary
//   primary     - NUMBER | IDENTIFIER | "(" expression ")"

// used for dataflow analysis output
struct DataFlowInfo
{
    int line;
    string statement;
    vector<string> defined;     // variables written
    vector<string> used;        // variables read
};

// used for memory management output
struct MemEntry 
{
    string name;
    int address;
    int size;
};

class ParserInterpreter 
{
private:
    vector<Token> tokens;
    int current;
    unordered_map<string, int> variables;
    vector<string> errors;
    vector<int> outputs;
    vector<ASTNode*> astRoots;
    vector<string> asmCode;
    int regCounter;
    int labelCounter;
    bool makeAST;
    bool genAsm;

    vector<DataFlowInfo> dataFlow;
    DataFlowInfo currentDF;

    int nextAddress;

    // token navigation
    Token peek();
    Token previous();
    bool isAtEnd();
    Token advance();
    bool check(TokenType type);
    bool match(vector<TokenType> types);
    Token consume(TokenType type, string message);
    void synchronize();

    // assembly helpers
    void emit(string instruction);
    string newLabel();

    // dataflow tracking
    void trackDef(string var);
    void trackUse(string var);
    void startStatement(int ln, string desc);
    void endStatement();

    // expression parsing (recursive descent)
    pair<int, ASTNode*> expression();
    pair<int, ASTNode*> equality();
    pair<int, ASTNode*> comparison();
    pair<int, ASTNode*> term();
    pair<int, ASTNode*> factor();
    pair<int, ASTNode*> unary();
    pair<int, ASTNode*> primary();

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
    bool isTopLevel;
    vector<MemEntry> memoryMap;

    // constructor for top level
    ParserInterpreter(vector<Token> t, bool ast, bool asm_gen);

    // constructor for nested blocks (shares variables)
    ParserInterpreter(vector<Token> t, unordered_map<string, int> vars, bool ast, bool asm_gen);

    void parseProgram();
    int evaluateExpressionOnly();

    vector<string> getErrors();
    vector<int> getOutputs();
    unordered_map<string, int> getVariables();
    vector<ASTNode*> getAST();
    vector<string> getAssembly();
    vector<DataFlowInfo> getDataFlow();
    vector<MemEntry> getMemoryMap();

    ~ParserInterpreter();
};

#endif