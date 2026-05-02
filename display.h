#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>
#include "token.h"
using namespace std;

void printHeader(string title);
void printSourceCode(string source);
void printTokens(vector<Token> tokens);
void printDFATables();
void printFormalDFA();
void printInputTapeSimulation();
void printComplementLanguage();
void printDerivations();
void printCNFandGNF();
void printCFG();

#endif