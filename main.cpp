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

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "token.h"
#include "ASTNode.h"
#include "lexer.h"
#include "parser.h"
#include "display.h"
using namespace std;

// Test programs
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


// Run a single test case
void runProgram(string source, string title) 
{
    printHeader(title);
    printSourceCode(source);

    // lexical analysis
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();
    vector<string> lexErrors = lexer.getErrors();
    printTokens(tokens);

    if (lexErrors.size() > 0) 
    {
        cout << endl << "[LEXICAL ERRORS]" << endl;
        printLine();
        for (int i = 0; i < lexErrors.size(); i++)
            cout << "  ERROR: " << lexErrors[i] << endl;
        return;
    }

    // parse, interpret, and generate assembly
    ParserInterpreter parser(tokens, true, true);
    parser.parseProgram();
    vector<string> parseErrors = parser.getErrors();

    if (parseErrors.size() > 0) 
    {
        cout << endl << "[SYNTAX/SEMANTIC ERRORS]" << endl;
        printLine();
        for (int i = 0; i < parseErrors.size(); i++)
            cout << "  ERROR: " << parseErrors[i] << endl;
    }

    // AST
    vector<ASTNode*> ast = parser.getAST();
    if (ast.size() > 0) 
    {
        cout << endl << "[ABSTRACT SYNTAX TREE]" << endl;
        printLine();
        for (int i = 0; i < ast.size(); i++)
            ast[i]->print("  ", i == ast.size() - 1);
    }

    // symbol table
    unordered_map<string, int> vars = parser.getVariables();
    if (vars.size() > 0) 
    {
        cout << endl << "[SYMBOL TABLE]" << endl;
        printLine();
        cout << "  Variable        Type      Value" << endl;
        cout << "  --------------------------------" << endl;
        for (auto it = vars.begin(); it != vars.end(); it++) 
        {
            string name = it->first;
            int val = it->second;
            while (name.size() < 16) name += " ";
            cout << "  " << name << "int       " << val << endl;
        }
    }

    // program output
    vector<int> outs = parser.getOutputs();
    cout << endl << "[PROGRAM OUTPUT]" << endl;
    printLine();
    if (outs.size() == 0) {
        cout << "  (no output)" << endl;
    } else {
        for (int i = 0; i < outs.size(); i++)
            cout << "  >>> " << outs[i] << endl;
    }

    // assembly code (Requirement 2h)
    vector<string> asmCode = parser.getAssembly();
    if (asmCode.size() > 0) {
        cout << endl << "[GENERATED ASSEMBLY CODE]" << endl;
        printLine();
        for (int i = 0; i < asmCode.size(); i++)
            cout << "  " << asmCode[i] << endl;
    }

    // dataflow analysis (Requirement 2i)
    vector<DataFlowInfo> dfData = parser.getDataFlow();
    if (dfData.size() > 0) {
        cout << endl << "[DATAFLOW ANALYSIS]" << endl;
        printLine();
        cout << "  Line  Statement         DEF (written)     USE (read)" << endl;
        cout << "  -------------------------------------------------------" << endl;
        for (int i = 0; i < dfData.size(); i++) {
            string lineStr = to_string(dfData[i].line);
            while (lineStr.size() < 4) lineStr += " ";
            string stmtStr = dfData[i].statement;
            while (stmtStr.size() < 18) stmtStr += " ";

            string defStr = "";
            for (int j = 0; j < dfData[i].defined.size(); j++) {
                if (j > 0) defStr += ", ";
                defStr += dfData[i].defined[j];
            }
            if (defStr.empty()) defStr = "-";
            while (defStr.size() < 18) defStr += " ";

            string useStr = "";
            for (int j = 0; j < dfData[i].used.size(); j++) {
                if (j > 0) useStr += ", ";
                useStr += dfData[i].used[j];
            }
            if (useStr.empty()) useStr = "-";

            cout << "  " << lineStr << stmtStr << defStr << useStr << endl;
        }

        // compute live variables at each point
        cout << endl;
        cout << "  Live Variable Analysis (backward):" << endl;
        cout << "  A variable is 'live' if it may be used before being redefined." << endl;
        cout << endl;

        // simple backward analysis
        unordered_set<string> liveSet;
        vector<pair<int, unordered_set<string>>> liveAtPoint;
        for (int i = dfData.size() - 1; i >= 0; i--) {
            // add used variables
            for (int j = 0; j < dfData[i].used.size(); j++) {
                liveSet.insert(dfData[i].used[j]);
            }
            liveAtPoint.push_back(make_pair(dfData[i].line, liveSet));
            // remove defined variables (killed)
            for (int j = 0; j < dfData[i].defined.size(); j++) {
                bool alsoUsed = false;
                for (int k = 0; k < dfData[i].used.size(); k++) {
                    if (dfData[i].used[k] == dfData[i].defined[j]) alsoUsed = true;
                }
                if (!alsoUsed) liveSet.erase(dfData[i].defined[j]);
            }
        }

        for (int i = liveAtPoint.size() - 1; i >= 0; i--) {
            cout << "  After line " << liveAtPoint[i].first << ": live = {";
            bool first = true;
            for (auto it = liveAtPoint[i].second.begin(); it != liveAtPoint[i].second.end(); it++) {
                if (!first) cout << ", ";
                cout << *it;
                first = false;
            }
            cout << "}" << endl;
        }
    }

    // memory management (Requirement 2j)
    vector<MemEntry> memMap = parser.getMemoryMap();
    if (memMap.size() > 0) {
        cout << endl << "[MEMORY MANAGEMENT - STACK LAYOUT]" << endl;
        printLine();
        cout << "  Memory model: stack-based allocation for local variables" << endl;
        cout << "  Each int variable uses 4 bytes on the stack." << endl;
        cout << endl;
        cout << "  Address     Size    Variable    Final Value" << endl;
        cout << "  -------------------------------------------" << endl;

        int totalSize = 0;
        for (int i = 0; i < memMap.size(); i++) {
            cout << "  0x" << hex << memMap[i].address << dec
                 << "     " << memMap[i].size << " B     ";
            string name = memMap[i].name;
            while (name.size() < 12) name += " ";
            cout << name;
            if (vars.find(memMap[i].name) != vars.end()) {
                cout << vars[memMap[i].name];
            }
            cout << endl;
            totalSize += memMap[i].size;
        }

        cout << endl;
        cout << "  Total stack usage: " << totalSize << " bytes (" << memMap.size() << " variables)" << endl;
        cout << "  Stack grows downward from 0x" << hex << (0x1000 + totalSize - 4) << dec
             << " to 0x1000" << endl;
        cout << endl;
        cout << "  Memory lifecycle:" << endl;
        cout << "    1. Variables allocated on stack when declared (PUSH)" << endl;
        cout << "    2. Accessed via stack offset during execution (LOAD/STORE)" << endl;
        cout << "    3. Deallocated when scope ends (POP / stack pointer restore)" << endl;
    }
}

// =============================================
// Main
// =============================================

int main() {
    printHeader("MINI C++ COMPILER - CMPE 152");

    cout << endl;
    cout << "  Compiler phases: Lexical Analysis -> Parsing -> AST -> Code Gen -> Interpretation" << endl;
    cout << "  Grammar type:    Context-Free Grammar (recursive descent)" << endl;
    cout << "  Lexer type:      DFA-based scanner" << endl;

    // Requirement 2d: Regular expressions and DFA tables
    printDFATables();

    // Requirement 2e-ii: Formal DFA descriptions
    printFormalDFA();

    // Requirement 2e-iii: Input tape simulation
    printInputTapeSimulation();

    // Requirement 2e-iv: Complement of regular language
    printComplementLanguage();

    // Requirement 2f: Context-free grammar
    printCFG();

    // Requirement 2c-iii: Left/right derivation
    printDerivations();

    // Requirement 2g: CNF and GNF
    printCNFandGNF();

    // --- Test cases ---
    // Each test case shows: tokens, AST, symbol table, output,
    // assembly code (2h), dataflow analysis (2i), memory management (2j)

    runProgram(testProgram1(), "TEST 1 - Arithmetic Expressions");
    runProgram(testProgram2(), "TEST 2 - If/Else Conditional");
    runProgram(testProgram3(), "TEST 3 - While Loop");

    cout << endl;
    cout << "============================================================" << endl;
    cout << "  Done. 3 test cases executed successfully." << endl;
    cout << "============================================================" << endl;
    cout << endl;

    return 0;
}