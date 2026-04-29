// display.h - Display and output functions
// Part of Mini C++ Compiler - CMPE 152

#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>
#include "token.h"
using namespace std;

// basic formatting helpers
void printLine();
void printHeader(string title);

// source code display
void printSourceCode(string source);

// token table display
void printTokens(vector<Token> tokens);

// DFA transition tables
void printDFATables();

// formal DFA descriptions (5-tuple)
void printFormalDFA();

// input tape simulation
void simulateInputTape(string input, string dfaName);
void printInputTapeSimulation();

// complement of regular languages
void printComplementLanguage();

// left/right derivation trees
void printDerivations();

// Chomsky Normal Form and Greibach Normal Form
void printCNFandGNF();

// context-free grammar display
void printCFG();

#endif