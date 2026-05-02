// parser.cpp - Parser and Interpreter
// Uses recursive descent to parse and interpret at the same time

#include "parser.h"

// constructors
ParserInterpreter::ParserInterpreter(vector<Token> t) 
{
    tokens = t;
    current = 0;
}

ParserInterpreter::ParserInterpreter(vector<Token> t, unordered_map<string, int> vars) 
{
    tokens = t;
    current = 0;
    variables = vars;
}

// token navigation
Token ParserInterpreter::peek() { return tokens[current]; }
Token ParserInterpreter::previous() { return tokens[current - 1]; }
bool ParserInterpreter::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }

Token ParserInterpreter::advance() 
{
    if (!isAtEnd()) current++;
    return previous();
}

bool ParserInterpreter::check(TokenType type) 
{
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool ParserInterpreter::match(vector<TokenType> types) 
{
    for (int i = 0; i < types.size(); i++) 
    {
        if (check(types[i])) 
        {
            advance();
            return true;
        }
    }
    return false;
}

Token ParserInterpreter::consume(TokenType type, string message) 
{
    if (check(type)) return advance();
    errors.push_back("Line " + to_string(peek().line) + ": " + message);
    Token bad;
    bad.type = TokenType::INVALID;
    bad.lexeme = "";
    bad.line = peek().line;
    return bad;
}

void ParserInterpreter::synchronize() 
{
    while (!isAtEnd()) 
    {
        if (previous().type == TokenType::SEMICOLON) return;
        if (peek().type == TokenType::INT_KW || peek().type == TokenType::IF_KW ||
            peek().type == TokenType::WHILE_KW || peek().type == TokenType::PRINT_KW ||
            peek().type == TokenType::ELSE_KW) 
        {
            return;
        }
        advance();
    }
}

// Expression parsing - recursive descent
// Each function handles one level of precedence

int ParserInterpreter::expression() { return equality(); }

int ParserInterpreter::equality() 
{
    int left = comparison();
    while (match({TokenType::EQ, TokenType::NEQ})) 
    {
        Token op = previous();
        int right = comparison();
        if (op.type == TokenType::EQ) left = (left == right);
        else left = (left != right);
    }
    return left;
}

int ParserInterpreter::comparison() 
{
    int left = term();
    while (match({TokenType::LT, TokenType::GT, TokenType::LE, TokenType::GE})) 
    {
        Token op = previous();
        int right = term();
        if (op.type == TokenType::LT) left = (left < right);
        else if (op.type == TokenType::GT) left = (left > right);
        else if (op.type == TokenType::LE) left = (left <= right);
        else left = (left >= right);
    }
    return left;
}

int ParserInterpreter::term() 
{
    int left = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) 
    {
        Token op = previous();
        int right = factor();
        if (op.type == TokenType::PLUS) left = left + right;
        else left = left - right;
    }
    return left;
}

int ParserInterpreter::factor() 
{
    int left = unary();
    while (match({TokenType::STAR, TokenType::SLASH})) 
    {
        Token op = previous();
        int right = unary();
        if (op.type == TokenType::STAR) 
        {
            left = left * right;
        } 
        else 
        {
            if (right == 0) 
            {
                errors.push_back("Line " + to_string(op.line) + ": division by zero");
                left = 0;
            } 
            else 
            {
                left = left / right;
            }
        }
    }
    return left;
}

int ParserInterpreter::unary() 
{
    if (match({TokenType::MINUS})) 
    {
        return -unary();
    }
    return primary();
}

int ParserInterpreter::primary() 
{
    if (match({TokenType::NUMBER})) 
    {
        return stoi(previous().lexeme);
    }

    if (match({TokenType::IDENTIFIER})) 
    {
        string name = previous().lexeme;
        if (variables.find(name) == variables.end()) 
        {
            errors.push_back("Line " + to_string(previous().line) + ": undeclared variable '" + name + "'");
            return 0;
        }
        return variables[name];
    }

    if (match({TokenType::LPAREN})) 
    {
        int value = expression();
        consume(TokenType::RPAREN, "expected ')' after expression");
        return value;
    }

    errors.push_back("Line " + to_string(peek().line) + ": expected expression");
    return 0;
}

// Statement parsing

void ParserInterpreter::declaration() 
{
    if (match({TokenType::INT_KW})) 
    {
        intDeclaration();
        return;
    }
    statement();
}

void ParserInterpreter::intDeclaration() 
{
    Token name = consume(TokenType::IDENTIFIER, "expected variable name after 'int'");
    consume(TokenType::ASSIGN, "expected '=' in declaration");
    int value = expression();
    consume(TokenType::SEMICOLON, "expected ';' after declaration");

    if (name.type != TokenType::INVALID) 
    {
        variables[name.lexeme] = value;
    }
}

void ParserInterpreter::statement() 
{
    if (match({TokenType::PRINT_KW})) { printStatement(); return; }
    if (match({TokenType::IF_KW})) { ifStatement(); return; }
    if (match({TokenType::WHILE_KW})) { whileStatement(); return; }
    if (match({TokenType::LBRACE})) { block(); return; }
    if (check(TokenType::IDENTIFIER)) { assignmentStatement(); return; }

    errors.push_back("Line " + to_string(peek().line) + ": invalid statement");
    advance();
}

void ParserInterpreter::assignmentStatement() 
{
    Token name = advance();
    consume(TokenType::ASSIGN, "expected '=' in assignment");
    int value = expression();
    consume(TokenType::SEMICOLON, "expected ';' after assignment");

    if (variables.find(name.lexeme) == variables.end()) 
    {
        errors.push_back("Line " + to_string(name.line) + ": undeclared variable '" + name.lexeme + "'");
    } 
    else 
    {
        variables[name.lexeme] = value;
    }
}

void ParserInterpreter::printStatement() 
{
    consume(TokenType::LPAREN, "expected '(' after 'print'");
    int value = expression();
    consume(TokenType::RPAREN, "expected ')' after print expression");
    consume(TokenType::SEMICOLON, "expected ';' after print statement");
    outputs.push_back(value);
}

// Block handling for if/while

vector<Token> ParserInterpreter::captureBlockTokens() 
{
    int braceCount = 1;
    int start = current;
    while (!isAtEnd() && braceCount > 0) 
    {
        if (tokens[current].type == TokenType::LBRACE) braceCount++;
        if (tokens[current].type == TokenType::RBRACE) braceCount--;
        current++;
    }
    int end = current - 1;
    vector<Token> blockTokens;
    for (int i = start; i < end; i++) 
    {
        blockTokens.push_back(tokens[i]);
    }
    blockTokens.push_back({TokenType::END_OF_FILE, "", tokens[end].line});
    return blockTokens;
}

void ParserInterpreter::executeBlockTokens(vector<Token> blockTokens) 
{
    ParserInterpreter nested(blockTokens, variables);
    nested.parseProgram();

    // copy variables back
    variables = nested.getVariables();

    // copy outputs
    vector<int> nestedOut = nested.getOutputs();
    for (int i = 0; i < nestedOut.size(); i++) 
    {
        outputs.push_back(nestedOut[i]);
    }

    // copy errors
    vector<string> nestedErr = nested.getErrors();
    for (int i = 0; i < nestedErr.size(); i++) 
    {
        errors.push_back(nestedErr[i]);
    }
}

void ParserInterpreter::ifStatement() 
{
    consume(TokenType::LPAREN, "expected '(' after 'if'");
    int condition = expression();
    consume(TokenType::RPAREN, "expected ')' after if condition");
    consume(TokenType::LBRACE, "expected '{' after if condition");
    vector<Token> ifBlock = captureBlockTokens();

    vector<Token> elseBlock;
    bool hasElse = false;
    if (match({TokenType::ELSE_KW})) 
    {
        consume(TokenType::LBRACE, "expected '{' after 'else'");
        elseBlock = captureBlockTokens();
        hasElse = true;
    }

    if (condition) 
    {
        executeBlockTokens(ifBlock);
    } 
    else if (hasElse) 
    {
        executeBlockTokens(elseBlock);
    }
}

void ParserInterpreter::whileStatement() 
{
    consume(TokenType::LPAREN, "expected '(' after 'while'");
    int condStart = current;
    int condValue = expression();
    int condEnd = current;
    consume(TokenType::RPAREN, "expected ')' after while condition");
    consume(TokenType::LBRACE, "expected '{' after while condition");
    vector<Token> loopBlock = captureBlockTokens();

    int counter = 0;
    while (condValue) 
    {
        executeBlockTokens(loopBlock);

        // re-evaluate condition with updated variables
        vector<Token> condTokens;
        for (int i = condStart; i < condEnd; i++) 
        {
            condTokens.push_back(tokens[i]);
        }
        condTokens.push_back({TokenType::END_OF_FILE, "", tokens[condEnd - 1].line});

        ParserInterpreter condParser(condTokens, variables);
        condValue = condParser.evaluateExpressionOnly();

        counter++;
        if (counter > 10000) 
        {
            errors.push_back("Stopped: possible infinite loop");
            break;
        }
    }
}

void ParserInterpreter::block() 
{
    while (!check(TokenType::RBRACE) && !isAtEnd()) 
    {
        declaration();
    }
    consume(TokenType::RBRACE, "expected '}' after block");
}

// Public methods

void ParserInterpreter::parseProgram() 
{
    while (!isAtEnd()) 
    {
        declaration();
    }
}

int ParserInterpreter::evaluateExpressionOnly() 
{
    current = 0;
    return expression();
}

vector<string> ParserInterpreter::getErrors() { return errors; }
vector<int> ParserInterpreter::getOutputs() { return outputs; }
unordered_map<string, int> ParserInterpreter::getVariables() { return variables; }
vector<Token> ParserInterpreter::getTokens() { return tokens; }