#include "display.h"
#include <iostream>
#include <sstream>
#include <cctype>
using namespace std;

void printHeader(string title)
{
    cout << endl;
    cout << "  " << title << endl;
    cout << endl;
}

void printSourceCode(string source)
{
    cout << "[SOURCE CODE]" << endl;
    istringstream ss(source);
    string line;
    int num = 0;
    while (getline(ss, line))
    {
        num++;
        if (line.empty()) continue;
        bool blank = true;
        for (int i = 0; i < line.size(); i++)
            if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r')
            {
                blank = false;
                break;
            }
        if (blank) continue;
        cout << "  " << num << " | " << line << endl;
    }
    cout << endl;
}

void printTokens(vector<Token> tokens)
{
    cout << "[LEXICAL TOKENS]" << endl;
    cout << "  Line  Type            Lexeme       Category" << endl;

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == TokenType::END_OF_FILE) break;
        string typeStr = tokenTypeToString(tokens[i].type);
        string cat = getCategory(tokens[i].type);
        while (typeStr.size() < 14) typeStr += " ";
        cout << "  " << tokens[i].line << "     " << typeStr << "  \"" << tokens[i].lexeme << "\"";
        int lexLen = tokens[i].lexeme.size() + 2;
        for (int j = lexLen; j < 12; j++) cout << " ";
        cout << cat << endl;
    }
    cout << endl;
}

// DFA transition tables
void printDFATables()
{
    cout << "[DFA TRANSITION TABLES]" << endl;

    cout << endl << "  DFA 1: Identifier   regex: [a-zA-Z_][a-zA-Z0-9_]*" << endl;
    cout << "  State     Input               Next State" << endl;
    cout << "  q0        letter or _          q1" << endl;
    cout << "  q0        digit                REJECT" << endl;
    cout << "  q1        letter/digit/_       q1" << endl;
    cout << "  q1        other                ACCEPT" << endl;

    cout << endl << "  DFA 2: Integer Literal   regex: [0-9]+" << endl;
    cout << "  State     Input               Next State" << endl;
    cout << "  q0        digit (0-9)          q1" << endl;
    cout << "  q0        other                REJECT" << endl;
    cout << "  q1        digit                q1" << endl;
    cout << "  q1        other                ACCEPT" << endl;

    cout << endl << "  DFA 3: Operators   regex: =|==|<|<=|>|>=|!=" << endl;
    cout << "  State     Input               Next State" << endl;
    cout << "  q0        =                    q1" << endl;
    cout << "  q0        <                    q2" << endl;
    cout << "  q0        >                    q3" << endl;
    cout << "  q0        !                    q4" << endl;
    cout << "  q1        =                    ACCEPT (==)" << endl;
    cout << "  q1        other                ACCEPT (=)" << endl;
    cout << "  q2        =                    ACCEPT (<=)" << endl;
    cout << "  q2        other                ACCEPT (<)" << endl;
    cout << "  q3        =                    ACCEPT (>=)" << endl;
    cout << "  q3        other                ACCEPT (>)" << endl;
    cout << "  q4        =                    ACCEPT (!=)" << endl;
    cout << "  q4        other                REJECT" << endl;
    cout << endl;
}

// formal 5-tuple DFA definitions
void printFormalDFA()
{
    cout << "[FORMAL DFA DESCRIPTIONS (5-TUPLE)]" << endl;

    cout << endl << "  DFA 1: Identifier" << endl;
    cout << "  M = (Q, Sigma, delta, q0, F)" << endl;
    cout << "    Q     = {q0, q1, q_dead}" << endl;
    cout << "    Sigma = {letter, digit, underscore, other}" << endl;
    cout << "    q0    = q0" << endl;
    cout << "    F     = {q1}" << endl;
    cout << "    delta: q0--letter/_ -> q1, q1--letter/digit/_ -> q1, q1--other -> ACCEPT" << endl;

    cout << endl << "  DFA 2: Integer Literal" << endl;
    cout << "  M = (Q, Sigma, delta, q0, F)" << endl;
    cout << "    Q     = {q0, q1, q_dead}" << endl;
    cout << "    Sigma = {digit, other}" << endl;
    cout << "    q0    = q0" << endl;
    cout << "    F     = {q1}" << endl;
    cout << "    delta: q0--digit -> q1, q1--digit -> q1, q1--other -> ACCEPT" << endl;

    cout << endl << "  DFA 3: Operators" << endl;
    cout << "  M = (Q, Sigma, delta, q0, F)" << endl;
    cout << "    Q     = {q0, q1, q2, q3, q4}" << endl;
    cout << "    Sigma = {=, <, >, !, other}" << endl;
    cout << "    q0    = q0" << endl;
    cout << "    F     = {q_accept}" << endl;
    cout << "    delta: q0--= -> q1, q0--< -> q2, q0--> -> q3, q0--! -> q4" << endl;
    cout << "           q1--= -> ACCEPT(==), q1--other -> ACCEPT(=)" << endl;
    cout << "           q2--= -> ACCEPT(<=), q2--other -> ACCEPT(<)" << endl;
    cout << "           q3--= -> ACCEPT(>=), q3--other -> ACCEPT(>)" << endl;
    cout << "           q4--= -> ACCEPT(!=), q4--other -> REJECT" << endl;
    cout << endl;
}

// input tape simulation
void printInputTapeSimulation()
{
    cout << "[INPUT TAPE SIMULATION]" << endl;

    cout << endl << "  Processing \"count1\" on Identifier DFA:" << endl;
    cout << "  Step  State   Char   Action" << endl;
    cout << "  1     q0      c      letter -> q1" << endl;
    cout << "  2     q1      o      letter -> q1" << endl;
    cout << "  3     q1      u      letter -> q1" << endl;
    cout << "  4     q1      n      letter -> q1" << endl;
    cout << "  5     q1      t      letter -> q1" << endl;
    cout << "  6     q1      1      digit  -> q1" << endl;
    cout << "  7     q1      EOF    ACCEPT (valid identifier)" << endl;

    cout << endl << "  Processing \"42\" on Number DFA:" << endl;
    cout << "  Step  State   Char   Action" << endl;
    cout << "  1     q0      4      digit -> q1" << endl;
    cout << "  2     q1      2      digit -> q1" << endl;
    cout << "  3     q1      EOF    ACCEPT (valid number)" << endl;

    cout << endl << "  Processing \"3abc\" on Number DFA:" << endl;
    cout << "  Step  State   Char   Action" << endl;
    cout << "  1     q0      3      digit  -> q1" << endl;
    cout << "  2     q1      a      letter -> ACCEPT at '3', then 'abc' starts new token" << endl;
    cout << endl;
}

// complement of regular languages
void printComplementLanguage()
{
    cout << "[COMPLEMENT OF REGULAR LANGUAGES]" << endl;
    cout << endl;
    cout << "  Complement: swap accepting and non-accepting states in the DFA." << endl;
    cout << endl;
    cout << "  Identifier language L: accepts \"x\", \"count\", \"_temp\"" << endl;
    cout << "  Complement L':         accepts \"123\", \"3abc\", \"+\", \"\"" << endl;
    cout << "                         (everything that is NOT a valid identifier)" << endl;
    cout << endl;
    cout << "  Number language L:     accepts \"0\", \"42\", \"999\"" << endl;
    cout << "  Complement L':         accepts \"abc\", \"12x\", \"\"" << endl;
    cout << "                         (everything that is NOT a valid number)" << endl;
    cout << endl;
}

// left and right derivation
void printDerivations()
{
    cout << "[LEFT AND RIGHT DERIVATION]" << endl;
    cout << endl << "  Expression: (x + 3) * y" << endl;

    cout << endl << "  Leftmost Derivation:" << endl;
    cout << "  expr => term => factor * unary => ( expr ) * unary" << endl;
    cout << "       => ( term + factor ) * unary => ( primary + factor ) * unary" << endl;
    cout << "       => ( x + primary ) * unary => ( x + 3 ) * primary" << endl;
    cout << "       => ( x + 3 ) * y" << endl;

    cout << endl << "  Rightmost Derivation:" << endl;
    cout << "  expr => term => factor * unary => factor * primary" << endl;
    cout << "       => factor * y => ( expr ) * y => ( term + factor ) * y" << endl;
    cout << "       => ( term + primary ) * y => ( term + 3 ) * y" << endl;
    cout << "       => ( primary + 3 ) * y => ( x + 3 ) * y" << endl;

    cout << endl << "  Parse tree:" << endl;
    cout << "            *" << endl;
    cout << "           / \\" << endl;
    cout << "          +   y" << endl;
    cout << "         / \\" << endl;
    cout << "        x   3" << endl;
    cout << endl;
}

// CNF and GNF
void printCNFandGNF()
{
    cout << "[CHOMSKY NORMAL FORM (CNF)]" << endl;
    cout << endl;
    cout << "  Original:  E -> E + T | T,  T -> T * F | F,  F -> id | num" << endl;
    cout << endl;
    cout << "  CNF (A -> BC or A -> a):" << endl;
    cout << "    E  -> E A1 | T        A1 -> P T" << endl;
    cout << "    T  -> T A2 | F        A2 -> M F" << endl;
    cout << "    F  -> L A3 | id | num A3 -> E R" << endl;
    cout << "    P  -> +    M  -> *    L  -> (    R  -> )" << endl;

    cout << endl << "[GREIBACH NORMAL FORM (GNF)]" << endl;
    cout << endl;
    cout << "  GNF (A -> a B1 B2 ..., starts with terminal):" << endl;
    cout << "    E  -> id term' expr' | num term' expr'" << endl;
    cout << "    T  -> id term' | num term'" << endl;
    cout << "    F  -> id | num" << endl;
    cout << "    expr' -> + T expr' | epsilon" << endl;
    cout << "    term' -> * F term' | epsilon" << endl;
    cout << endl;
}

// context-free grammar
void printCFG()
{
    cout << "[CONTEXT-FREE GRAMMAR]" << endl;
    cout << endl;
    cout << "  program     -> declaration*" << endl;
    cout << "  declaration -> intDecl | statement" << endl;
    cout << "  intDecl     -> \"int\" ID \"=\" expression \";\"" << endl;
    cout << "  statement   -> printStmt | ifStmt | whileStmt | assignment" << endl;
    cout << "  printStmt   -> \"print\" \"(\" expression \")\" \";\"" << endl;
    cout << "  ifStmt      -> \"if\" \"(\" expr \")\" \"{\" decl* \"}\" (\"else\" \"{\" decl* \"}\")?" << endl;
    cout << "  whileStmt   -> \"while\" \"(\" expr \")\" \"{\" decl* \"}\"" << endl;
    cout << "  assignment  -> ID \"=\" expression \";\"" << endl;
    cout << "  expression  -> equality" << endl;
    cout << "  equality    -> comparison ((\"==\" | \"!=\") comparison)*" << endl;
    cout << "  comparison  -> term ((\"<\" | \">\" | \"<=\" | \">=\") term)*" << endl;
    cout << "  term        -> factor ((\"+\" | \"-\") factor)*" << endl;
    cout << "  factor      -> unary ((\"*\" | \"/\") unary)*" << endl;
    cout << "  unary       -> \"-\" unary | primary" << endl;
    cout << "  primary     -> NUMBER | IDENTIFIER | \"(\" expression \")\"" << endl;
    cout << endl;
}