#include "Lexer.h"
#include <cctype>

// Lexical Analyzer implementation

Lexer::Lexer(string input) 
{
    src = input;
    pos = 0;
    line = 1;
}

char Lexer::peek() 
{
    if (pos >= src.size()) return '\0';
    return src[pos];
}

char Lexer::advance() 
{
    if (pos >= src.size()) return '\0';
    char c = src[pos];
    pos++;
    return c;
}

bool Lexer::match(char expected) 
{
    if (peek() != expected) return false;
    pos++;
    return true;
}

vector<Token> Lexer::tokenize() 
{
    vector<Token> tokens;

    while (pos < src.size()) 
    {
        char c = peek();

        // skip whitespace
        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
        if (c == '\n') { line++; advance(); continue; }

        // DFA for identifiers/keywords
        // q0 --letter/--> q1, q1 --letter/digit/_--> q1
        if (isalpha(c) || c == '_') 
        {
            int start = pos;
            while (isalnum(peek()) || peek() == '_') advance();
            string text = src.substr(start, pos - start);
            if (text == "int") tokens.push_back({TokenType::INT_KW, text, line});
            else if (text == "if") tokens.push_back({TokenType::IF_KW, text, line});
            else if (text == "else") tokens.push_back({TokenType::ELSE_KW, text, line});
            else if (text == "while") tokens.push_back({TokenType::WHILE_KW, text, line});
            else if (text == "print") tokens.push_back({TokenType::PRINT_KW, text, line});
            else tokens.push_back({TokenType::IDENTIFIER, text, line});
            continue;
        }

        // DFA for number literals
        // q0 --digit--> q1, q1 --digit--> q1
        if (isdigit(c)) 
        {
            int start = pos;
            while (isdigit(peek())) advance();
            tokens.push_back({TokenType::NUMBER, src.substr(start, pos - start), line});
            continue;
        }

        // operators and delimiters
        char ch = advance();
        if (ch == '+') tokens.push_back({TokenType::PLUS, "+", line});
        else if (ch == '-') tokens.push_back({TokenType::MINUS, "-", line});
        else if (ch == '*') tokens.push_back({TokenType::STAR, "*", line});
        else if (ch == '/') tokens.push_back({TokenType::SLASH, "/", line});
        else if (ch == '(') tokens.push_back({TokenType::LPAREN, "(", line});
        else if (ch == ')') tokens.push_back({TokenType::RPAREN, ")", line});
        else if (ch == '{') tokens.push_back({TokenType::LBRACE, "{", line});
        else if (ch == '}') tokens.push_back({TokenType::RBRACE, "}", line});
        else if (ch == ';') tokens.push_back({TokenType::SEMICOLON, ";", line});
        else if (ch == '=') 
        {
            // DFA: q0 --'='--> q1 --'='--> ACCEPT(==), q1 --other--> ACCEPT(=)
            if (match('=')) tokens.push_back({TokenType::EQ, "==", line});
            else tokens.push_back({TokenType::ASSIGN, "=", line});
        }
        else if (ch == '!') 
        {
            if (match('=')) tokens.push_back({TokenType::NEQ, "!=", line});
            else errors.push_back("Line " + to_string(line) + ": unexpected '!'");
        }
        else if (ch == '<') 
        {
            if (match('=')) tokens.push_back({TokenType::LE, "<=", line});
            else tokens.push_back({TokenType::LT, "<", line});
        }
        else if (ch == '>') 
        {
            if (match('=')) tokens.push_back({TokenType::GE, ">=", line});
            else tokens.push_back({TokenType::GT, ">", line});
        }
        else 
        {
            errors.push_back("Line " + to_string(line) + ": unrecognized character '" + string(1, ch) + "'");
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

vector<string> Lexer::getErrors() 
{
    return errors;
}