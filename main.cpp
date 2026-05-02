// main.cpp - Main driver for Mini C++ Compiler

// How to compile:
//   g++ -std=c++17 -c token.cpp -o token.o
//   g++ -std=c++17 -c lexer.cpp -o lexer.o
//   g++ -std=c++17 -c parser.cpp -o parser.o
//   g++ -std=c++17 -c ASTNode.cpp -o ASTNode.o
//   g++ -std=c++17 -c display.cpp -o display.o
//   g++ -std=c++17 -c main.cpp -o main.o
//   g++ -o mini_compiler token.o lexer.o parser.o ASTNode.o display.o main.o

// Limitations:
//   - Only supports int type (no float, string, char, bool)
//   - No functions or procedures
//   - No arrays or pointers
//   - No for loops (only while)
//   - No comments in source code
//   - No string literals
//   - Maximum 10000 loop iterations (safety limit)

// main.cpp - Mini C++ Compiler, CMPE 152

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "display.h"
using namespace std;

// walks tokens and prints a simple AST
void printAST(vector<Token> tokens)
{
    cout << "[ABSTRACT SYNTAX TREE]" << endl;

    int i = 0;
    while (i < tokens.size() && tokens[i].type != TokenType::END_OF_FILE)
    {
        if (tokens[i].type == TokenType::INT_KW)
        {
            string var = tokens[i + 1].lexeme;
            string expr = "";
            int j = i + 3;
            while (j < tokens.size() && tokens[j].type != TokenType::SEMICOLON)
            {
                expr += tokens[j].lexeme + " ";
                j++;
            }
            cout << "  DECLARE int " << var << " = " << expr << endl;
            i = j + 1;
        }
        else if (tokens[i].type == TokenType::PRINT_KW)
        {
            string expr = "";
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::RPAREN)
            {
                expr += tokens[j].lexeme + " ";
                j++;
            }
            cout << "  PRINT( " << expr << ")" << endl;
            i = j + 2;
        }
        else if (tokens[i].type == TokenType::IF_KW)
        {
            string cond = "";
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::RPAREN)
            {
                cond += tokens[j].lexeme + " ";
                j++;
            }
            cout << "  IF( " << cond << ")" << endl;
            cout << "    THEN { ... }" << endl;

            int braces = 0;
            while (j < tokens.size())
            {
                if (tokens[j].type == TokenType::LBRACE) braces++;
                if (tokens[j].type == TokenType::RBRACE)
                {
                    braces--;
                    if (braces == 0) break;
                }
                j++;
            }
            j++;
            if (j < tokens.size() && tokens[j].type == TokenType::ELSE_KW)
            {
                cout << "    ELSE { ... }" << endl;
                braces = 0;
                j++;
                while (j < tokens.size())
                {
                    if (tokens[j].type == TokenType::LBRACE) braces++;
                    if (tokens[j].type == TokenType::RBRACE)
                    {
                        braces--;
                        if (braces == 0) break;
                    }
                    j++;
                }
                j++;
            }
            i = j;
        }
        else if (tokens[i].type == TokenType::WHILE_KW)
        {
            string cond = "";
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::RPAREN)
            {
                cond += tokens[j].lexeme + " ";
                j++;
            }
            cout << "  WHILE( " << cond << ")" << endl;
            cout << "    BODY { ... }" << endl;

            int braces = 0;
            while (j < tokens.size())
            {
                if (tokens[j].type == TokenType::LBRACE) braces++;
                if (tokens[j].type == TokenType::RBRACE)
                {
                    braces--;
                    if (braces == 0) break;
                }
                j++;
            }
            i = j + 1;
        }
        else if (tokens[i].type == TokenType::IDENTIFIER &&
                 i + 1 < tokens.size() && tokens[i + 1].type == TokenType::ASSIGN)
        {
            string var = tokens[i].lexeme;
            string expr = "";
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::SEMICOLON)
            {
                expr += tokens[j].lexeme + " ";
                j++;
            }
            cout << "  ASSIGN " << var << " = " << expr << endl;
            i = j + 1;
        }
        else
        {
            i++;
        }
    }
    cout << endl;
}

// generates stack-based pseudo assembly from tokens
void printAssembly(vector<Token> tokens)
{
    cout << "[GENERATED ASSEMBLY CODE]" << endl;
    cout << "  ; program start" << endl;

    int label = 0;
    int i = 0;

    while (i < tokens.size() && tokens[i].type != TokenType::END_OF_FILE)
    {
        if ((tokens[i].type == TokenType::INT_KW) ||
            (tokens[i].type == TokenType::IDENTIFIER &&
             i + 1 < tokens.size() && tokens[i + 1].type == TokenType::ASSIGN))
        {
            string var;
            int start;
            if (tokens[i].type == TokenType::INT_KW)
            {
                var = tokens[i + 1].lexeme;
                start = i + 3;
            }
            else
            {
                var = tokens[i].lexeme;
                start = i + 2;
            }

            int j = start;
            while (j < tokens.size() && tokens[j].type != TokenType::SEMICOLON)
            {
                if (tokens[j].type == TokenType::NUMBER)
                    cout << "  PUSH #" << tokens[j].lexeme << endl;
                else if (tokens[j].type == TokenType::IDENTIFIER)
                    cout << "  PUSH [" << tokens[j].lexeme << "]" << endl;
                else if (tokens[j].type == TokenType::PLUS) cout << "  ADD" << endl;
                else if (tokens[j].type == TokenType::MINUS) cout << "  SUB" << endl;
                else if (tokens[j].type == TokenType::STAR) cout << "  MUL" << endl;
                else if (tokens[j].type == TokenType::SLASH) cout << "  DIV" << endl;
                j++;
            }
            cout << "  POP [" << var << "]" << endl;
            i = j + 1;
        }
        else if (tokens[i].type == TokenType::PRINT_KW)
        {
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::RPAREN)
            {
                if (tokens[j].type == TokenType::NUMBER)
                    cout << "  PUSH #" << tokens[j].lexeme << endl;
                else if (tokens[j].type == TokenType::IDENTIFIER)
                    cout << "  PUSH [" << tokens[j].lexeme << "]" << endl;
                else if (tokens[j].type == TokenType::PLUS) cout << "  ADD" << endl;
                else if (tokens[j].type == TokenType::MINUS) cout << "  SUB" << endl;
                else if (tokens[j].type == TokenType::STAR) cout << "  MUL" << endl;
                j++;
            }
            cout << "  OUT" << endl;
            i = j + 2;
        }
        else if (tokens[i].type == TokenType::IF_KW)
        {
            label++;
            int elseL = label;
            int endL = label + 1;
            label++;

            cout << "  CMP_EQ 0" << endl;
            cout << "  JMP_IF L" << elseL << endl;
            cout << "  ; then branch" << endl;
            cout << "  JMP L" << endL << endl;
            cout << "  L" << elseL << ": ; else branch" << endl;
            cout << "  L" << endL << ":" << endl;

            int braces = 0;
            int j = i;
            while (j < tokens.size())
            {
                if (tokens[j].type == TokenType::LBRACE) braces++;
                if (tokens[j].type == TokenType::RBRACE)
                {
                    braces--;
                    if (braces == 0) break;
                }
                j++;
            }
            j++;
            if (j < tokens.size() && tokens[j].type == TokenType::ELSE_KW)
            {
                j++;
                braces = 0;
                while (j < tokens.size())
                {
                    if (tokens[j].type == TokenType::LBRACE) braces++;
                    if (tokens[j].type == TokenType::RBRACE)
                    {
                        braces--;
                        if (braces == 0) break;
                    }
                    j++;
                }
                j++;
            }
            i = j;
        }
        else if (tokens[i].type == TokenType::WHILE_KW)
        {
            label++;
            int startL = label;
            int endL = label + 1;
            label++;

            cout << "  L" << startL << ": ; loop start" << endl;
            cout << "  CMP_EQ 0" << endl;
            cout << "  JMP_IF L" << endL << endl;
            cout << "  ; loop body" << endl;
            cout << "  JMP L" << startL << endl;
            cout << "  L" << endL << ": ; loop end" << endl;

            int braces = 0;
            int j = i;
            while (j < tokens.size())
            {
                if (tokens[j].type == TokenType::LBRACE) braces++;
                if (tokens[j].type == TokenType::RBRACE)
                {
                    braces--;
                    if (braces == 0) break;
                }
                j++;
            }
            i = j + 1;
        }
        else
        {
            i++;
        }
    }
    cout << "  HALT" << endl;
    cout << endl;
}

// scans tokens for DEF/USE per statement
void printDataflow(vector<Token> tokens)
{
    cout << "[DATAFLOW ANALYSIS]" << endl;
    cout << "  Line  Statement         DEF               USE" << endl;

    int i = 0;
    while (i < tokens.size() && tokens[i].type != TokenType::END_OF_FILE)
    {
        string def = "", use = "", desc = "";
        int line = tokens[i].line;

        if (tokens[i].type == TokenType::INT_KW && i + 1 < tokens.size())
        {
            def = tokens[i + 1].lexeme;
            desc = "int " + def + " = ...";
            int j = i + 3;
            while (j < tokens.size() && tokens[j].type != TokenType::SEMICOLON)
            {
                if (tokens[j].type == TokenType::IDENTIFIER)
                {
                    if (!use.empty()) use += ", ";
                    use += tokens[j].lexeme;
                }
                j++;
            }
            i = j + 1;
        }
        else if (tokens[i].type == TokenType::IDENTIFIER &&
                 i + 1 < tokens.size() && tokens[i + 1].type == TokenType::ASSIGN)
        {
            def = tokens[i].lexeme;
            desc = def + " = ...";
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::SEMICOLON)
            {
                if (tokens[j].type == TokenType::IDENTIFIER)
                {
                    if (!use.empty()) use += ", ";
                    use += tokens[j].lexeme;
                }
                j++;
            }
            i = j + 1;
        }
        else if (tokens[i].type == TokenType::PRINT_KW)
        {
            desc = "print(...)";
            int j = i + 2;
            while (j < tokens.size() && tokens[j].type != TokenType::RPAREN)
            {
                if (tokens[j].type == TokenType::IDENTIFIER)
                {
                    if (!use.empty()) use += ", ";
                    use += tokens[j].lexeme;
                }
                j++;
            }
            i = j + 2;
        }
        else
        {
            i++;
            continue;
        }

        if (def.empty()) def = "-";
        if (use.empty()) use = "-";
        while (desc.size() < 18) desc += " ";
        while (def.size() < 18) def += " ";
        cout << "  " << line << "     " << desc << def << use << endl;
    }
    cout << endl;
}

// shows where each variable sits in memory
void printMemory(unordered_map<string, int> vars)
{
    if (vars.size() == 0) return;
    cout << "[MEMORY MANAGEMENT]" << endl;
    cout << "  Each int = 4 bytes, stack-based allocation" << endl;
    cout << "  Address    Variable    Value" << endl;

    int addr = 0x1000;
    for (auto it = vars.begin(); it != vars.end(); it++)
    {
        string name = it->first;
        while (name.size() < 12) name += " ";
        cout << "  0x" << hex << addr << dec << "   " << name << it->second << endl;
        addr += 4;
    }
    cout << "  Total: " << vars.size() * 4 << " bytes (" << vars.size() << " variables)" << endl;
    cout << endl;
}

// test programs
string testProgram1()
{
    return "int x = 5;\n"
           "int y = 2;\n"
           "int z = (x + 3) * y;\n"
           "print(z);\n";
}

string testProgram2()
{
    return "int x = 3;\n"
           "if (x > 5) {\n"
           "    print(x);\n"
           "} else {\n"
           "    print(0);\n"
           "}\n"
           "print(x + 10);\n";
}

string testProgram3()
{
    return "int x = 0;\n"
           "int sum = 0;\n"
           "while (x < 5) {\n"
           "    sum = sum + x;\n"
           "    x = x + 1;\n"
           "}\n"
           "print(sum);\n"
           "print(x);\n";
}

// run a test case
void runProgram(string source, string title)
{
    printHeader(title);
    printSourceCode(source);

    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    vector<string> lexErrors = lexer.getErrors();
    printTokens(tokens);

    if (lexErrors.size() > 0)
    {
        cout << "[LEXICAL ERRORS]" << endl;
        for (int i = 0; i < lexErrors.size(); i++)
            cout << "  " << lexErrors[i] << endl;
        return;
    }

    ParserInterpreter parser(tokens);
    parser.parseProgram();
    vector<string> parseErrors = parser.getErrors();

    if (parseErrors.size() > 0)
    {
        cout << "[ERRORS]" << endl;
        for (int i = 0; i < parseErrors.size(); i++)
            cout << "  " << parseErrors[i] << endl;
    }

    printAST(tokens);

    unordered_map<string, int> vars = parser.getVariables();
    if (vars.size() > 0)
    {
        cout << "[SYMBOL TABLE]" << endl;
        for (auto it = vars.begin(); it != vars.end(); it++)
        {
            string name = it->first;
            while (name.size() < 12) name += " ";
            cout << "  " << name << "int    " << it->second << endl;
        }
        cout << endl;
    }

    vector<int> outs = parser.getOutputs();
    cout << "[PROGRAM OUTPUT]" << endl;
    if (outs.size() == 0)
    {
        cout << "  (no output)" << endl;
    }
    else
    {
        for (int i = 0; i < outs.size(); i++)
            cout << "  >>> " << outs[i] << endl;
    }
    cout << endl;

    printAssembly(tokens);
    printDataflow(tokens);
    printMemory(vars);
}

int main()
{
    printHeader("MINI C++ COMPILER - CMPE 152");
    cout << "  Phases: Lexical Analysis -> Parsing -> AST -> Code Gen -> Interpretation" << endl;
    cout << "  Grammar: Context-Free Grammar (recursive descent)" << endl;
    cout << "  Lexer:   DFA-based scanner" << endl;
    cout << endl;

    printDFATables();
    printFormalDFA();
    printInputTapeSimulation();
    printComplementLanguage();
    printCFG();
    printDerivations();
    printCNFandGNF();

    runProgram(testProgram1(), "TEST 1 - Arithmetic");
    runProgram(testProgram2(), "TEST 2 - If/Else");
    runProgram(testProgram3(), "TEST 3 - While Loop");

    cout << "  Done. 3 test cases executed." << endl;
    cout << endl;

    return 0;
}