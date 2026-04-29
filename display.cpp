// display.cpp - Display and output function implementations
// Part of Mini C++ Compiler - CMPE 152

#include "display.h"
#include <iostream>
#include <sstream>
#include <cctype>
using namespace std;

void printLine() {
    cout << "------------------------------------------------------------" << endl;
}

void printHeader(string title) {
    cout << endl;
    cout << "============================================================" << endl;
    cout << "  " << title << endl;
    cout << "============================================================" << endl;
}

void printSourceCode(string source) {
    cout << endl << "[SOURCE CODE]" << endl;
    printLine();
    istringstream ss(source);
    string line;
    int num = 0;
    while (getline(ss, line)) {
        num++;
        if (line.empty()) continue;
        bool blank = true;
        for (int i = 0; i < line.size(); i++) {
            if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r') { blank = false; break; }
        }
        if (blank) continue;
        cout << "  " << num << "  | " << line << endl;
    }
}

void printTokens(vector<Token> tokens) {
    cout << endl << "[LEXICAL TOKENS]" << endl;
    printLine();
    cout << "  Line   Token Type      Lexeme       Category" << endl;
    cout << "  ------------------------------------------------" << endl;

    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::END_OF_FILE) break;
        string typeStr = tokenTypeToString(tokens[i].type);
        string cat = getCategory(tokens[i].type);
        while (typeStr.size() < 14) typeStr += " ";
        cout << "  " << tokens[i].line << "      " << typeStr << "  \"" << tokens[i].lexeme << "\"";
        int lexLen = tokens[i].lexeme.size() + 2;
        for (int j = lexLen; j < 12; j++) cout << " ";
        cout << cat << endl;
    }

    int kwCount = 0, idCount = 0, numCount = 0, arithCount = 0;
    int relCount = 0, assignCount = 0, delimCount = 0;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::END_OF_FILE) break;
        string cat = getCategory(tokens[i].type);
        if (cat == "Keyword") kwCount++;
        else if (cat == "Identifier") idCount++;
        else if (cat == "Integer Literal") numCount++;
        else if (cat == "Arithmetic Op") arithCount++;
        else if (cat == "Relational Op") relCount++;
        else if (cat == "Assignment Op") assignCount++;
        else if (cat == "Delimiter") delimCount++;
    }
    cout << endl;
    cout << "  Token count: Keywords=" << kwCount << " Identifiers=" << idCount
         << " Literals=" << numCount << " ArithOps=" << arithCount
         << " RelOps=" << relCount << " AssignOps=" << assignCount
         << " Delimiters=" << delimCount << endl;
}

// =============================================
// Formal DFA Descriptions (5-tuple)
// =============================================

void printFormalDFA() {
    cout << endl << "[FORMAL DFA DESCRIPTIONS (5-TUPLE)]" << endl;
    printLine();

    cout << endl;
    cout << "  DFA 1: Identifier recognizer" << endl;
    cout << "  Regex: [a-zA-Z_][a-zA-Z0-9_]*" << endl;
    cout << endl;
    cout << "  M = (Q, Sigma, delta, q0, F) where:" << endl;
    cout << "    Q     = {q0, q1, q_dead}" << endl;
    cout << "    Sigma = {letter, digit, underscore, other}" << endl;
    cout << "    q0    = q0  (start state)" << endl;
    cout << "    F     = {q1} (accepting states)" << endl;
    cout << "    delta:" << endl;
    cout << "      delta(q0, letter)     = q1" << endl;
    cout << "      delta(q0, underscore) = q1" << endl;
    cout << "      delta(q0, digit)      = q_dead" << endl;
    cout << "      delta(q1, letter)     = q1" << endl;
    cout << "      delta(q1, digit)      = q1" << endl;
    cout << "      delta(q1, underscore) = q1" << endl;
    cout << "      delta(q1, other)      = ACCEPT" << endl;
    cout << "      delta(q_dead, *)      = q_dead" << endl;

    cout << endl;
    cout << "  DFA 2: Integer literal recognizer" << endl;
    cout << "  Regex: [0-9]+" << endl;
    cout << endl;
    cout << "  M = (Q, Sigma, delta, q0, F) where:" << endl;
    cout << "    Q     = {q0, q1, q_dead}" << endl;
    cout << "    Sigma = {digit, other}" << endl;
    cout << "    q0    = q0  (start state)" << endl;
    cout << "    F     = {q1} (accepting states)" << endl;
    cout << "    delta:" << endl;
    cout << "      delta(q0, digit) = q1" << endl;
    cout << "      delta(q0, other) = q_dead" << endl;
    cout << "      delta(q1, digit) = q1" << endl;
    cout << "      delta(q1, other) = ACCEPT" << endl;
    cout << "      delta(q_dead, *) = q_dead" << endl;

    cout << endl;
    cout << "  DFA 3: Operator recognizer" << endl;
    cout << "  Regex: =|==|!=|<|<=|>|>=" << endl;
    cout << endl;
    cout << "  M = (Q, Sigma, delta, q0, F) where:" << endl;
    cout << "    Q     = {q0, q1, q2, q3, q4, q_accept, q_reject}" << endl;
    cout << "    Sigma = {=, <, >, !, other}" << endl;
    cout << "    q0    = q0  (start state)" << endl;
    cout << "    F     = {q_accept}" << endl;
    cout << "    delta:" << endl;
    cout << "      delta(q0, '=') = q1      delta(q0, '<') = q2" << endl;
    cout << "      delta(q0, '>') = q3      delta(q0, '!') = q4" << endl;
    cout << "      delta(q1, '=') = ACCEPT(==)   delta(q1, other) = ACCEPT(=)" << endl;
    cout << "      delta(q2, '=') = ACCEPT(<=)   delta(q2, other) = ACCEPT(<)" << endl;
    cout << "      delta(q3, '=') = ACCEPT(>=)   delta(q3, other) = ACCEPT(>)" << endl;
    cout << "      delta(q4, '=') = ACCEPT(!=)   delta(q4, other) = REJECT" << endl;
}

// =============================================
// DFA Transition Tables
// =============================================

void printDFATables() {
    cout << endl << "[DFA TRANSITION TABLES]" << endl;
    printLine();

    cout << endl;
    cout << "  DFA 1: Identifier   regex: [a-zA-Z_][a-zA-Z0-9_]*" << endl;
    cout << endl;
    cout << "  State     Input               Next State" << endl;
    cout << "  ----------------------------------------" << endl;
    cout << "  q0        letter or _          q1" << endl;
    cout << "  q0        digit                REJECT" << endl;
    cout << "  q1        letter/digit/_       q1" << endl;
    cout << "  q1        other                ACCEPT" << endl;

    cout << endl;
    cout << "  DFA 2: Integer Literal   regex: [0-9]+" << endl;
    cout << endl;
    cout << "  State     Input               Next State" << endl;
    cout << "  ----------------------------------------" << endl;
    cout << "  q0        digit (0-9)          q1" << endl;
    cout << "  q0        other                REJECT" << endl;
    cout << "  q1        digit                q1" << endl;
    cout << "  q1        other                ACCEPT" << endl;

    cout << endl;
    cout << "  DFA 3: Operators   regex: =|==|<|<=|>|>=|!=" << endl;
    cout << endl;
    cout << "  State     Input               Next State" << endl;
    cout << "  ----------------------------------------" << endl;
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
}

// =============================================
// Input Tape Simulation
// =============================================

void simulateInputTape(string input, string dfaName) {
    cout << endl;
    cout << "  Simulating " << dfaName << " on input: \"" << input << "\"" << endl;
    cout << endl;
    cout << "  Step  State    Read    Remaining          Action" << endl;
    cout << "  -------------------------------------------------------" << endl;

    if (dfaName == "Identifier DFA") {
        string state = "q0";
        for (int i = 0; i <= input.size(); i++) {
            string remaining = (i < input.size()) ? input.substr(i) : "(empty)";
            string readChar = (i < input.size()) ? string(1, input[i]) : "EOF";

            // pad for alignment
            string stepStr = to_string(i + 1);
            while (stepStr.size() < 4) stepStr += " ";
            while (state.size() < 8) state += " ";
            string readPad = readChar;
            while (readPad.size() < 7) readPad += " ";
            while (remaining.size() < 18) remaining += " ";

            if (i == input.size()) {
                // check if we accept
                string trimState = state;
                while (trimState.back() == ' ') trimState.pop_back();
                if (trimState == "q1") {
                    cout << "  " << stepStr << trimState;
                    while (trimState.size() < 8) { cout << " "; trimState += " "; }
                    cout << " " << readPad << " " << remaining << " ACCEPT" << endl;
                } else {
                    cout << "  " << stepStr << trimState;
                    while (trimState.size() < 8) { cout << " "; trimState += " "; }
                    cout << " " << readPad << " " << remaining << " REJECT" << endl;
                }
                break;
            }

            char c = input[i];
            string action = "";
            string nextState = "";

            string trimState = state;
            while (trimState.back() == ' ') trimState.pop_back();

            if (trimState == "q0") {
                if (isalpha(c) || c == '_') { nextState = "q1"; action = "letter/_ -> q1"; }
                else { nextState = "q_dead"; action = "not letter -> DEAD"; }
            } else if (trimState == "q1") {
                if (isalnum(c) || c == '_') { nextState = "q1"; action = "letter/digit/_ -> q1"; }
                else { nextState = "q1"; action = "other -> ACCEPT (stay)"; }
            } else {
                nextState = "q_dead"; action = "DEAD state";
            }

            cout << "  " << stepStr << trimState;
            int pad = 8 - trimState.size();
            for (int p = 0; p < pad; p++) cout << " ";
            cout << " " << readPad << " " << remaining << " " << action << endl;

            state = nextState;
            while (state.size() < 8) state += " ";
        }
    }
    else if (dfaName == "Number DFA") {
        string state = "q0";
        for (int i = 0; i <= input.size(); i++) {
            string remaining = (i < input.size()) ? input.substr(i) : "(empty)";
            string readChar = (i < input.size()) ? string(1, input[i]) : "EOF";

            string stepStr = to_string(i + 1);
            while (stepStr.size() < 4) stepStr += " ";
            string readPad = readChar;
            while (readPad.size() < 7) readPad += " ";
            while (remaining.size() < 18) remaining += " ";

            if (i == input.size()) {
                string trimState = state;
                while (trimState.back() == ' ') trimState.pop_back();
                string result = (trimState == "q1") ? "ACCEPT" : "REJECT";
                cout << "  " << stepStr << trimState;
                int pad = 8 - trimState.size();
                for (int p = 0; p < pad; p++) cout << " ";
                cout << " " << readPad << " " << remaining << " " << result << endl;
                break;
            }

            char c = input[i];
            string action = "";
            string nextState = "";
            string trimState = state;
            while (trimState.back() == ' ') trimState.pop_back();

            if (trimState == "q0") {
                if (isdigit(c)) { nextState = "q1"; action = "digit -> q1"; }
                else { nextState = "q_dead"; action = "not digit -> DEAD"; }
            } else if (trimState == "q1") {
                if (isdigit(c)) { nextState = "q1"; action = "digit -> q1"; }
                else { nextState = "q_dead"; action = "other -> ACCEPT"; }
            } else {
                nextState = "q_dead"; action = "DEAD state";
            }

            cout << "  " << stepStr << trimState;
            int pad = 8 - trimState.size();
            for (int p = 0; p < pad; p++) cout << " ";
            cout << " " << readPad << " " << remaining << " " << action << endl;

            state = nextState;
            while (state.size() < 8) state += " ";
        }
    }
}

void printInputTapeSimulation() {
    cout << endl << "[INPUT TAPE SIMULATION - DFA PROCESSING]" << endl;
    printLine();
    cout << "  Demonstrating how the DFA processes input strings" << endl;
    cout << "  character by character on the input tape." << endl;

    simulateInputTape("count1", "Identifier DFA");
    simulateInputTape("42", "Number DFA");
    simulateInputTape("3abc", "Number DFA");
}

// =============================================
// Complement of Regular Language
// =============================================

void printComplementLanguage() {
    cout << endl << "[COMPLEMENT OF REGULAR LANGUAGES]" << endl;
    printLine();

    cout << endl;
    cout << "  The complement of a regular language L over alphabet Sigma is:" << endl;
    cout << "    L' = Sigma* - L  (all strings NOT in L)" << endl;
    cout << endl;
    cout << "  To build the complement DFA, we swap accepting and non-accepting states." << endl;

    cout << endl;
    cout << "  Example: Complement of Identifier Language" << endl;
    cout << "  Original L = {strings matching [a-zA-Z_][a-zA-Z0-9_]*}" << endl;
    cout << "  L accepts:  \"x\", \"count\", \"_temp\", \"var1\"" << endl;
    cout << "  L rejects:  \"123\", \"3abc\", \"+\", \"\"" << endl;
    cout << endl;
    cout << "  Complement L':" << endl;
    cout << "  Original DFA states: q0(start), q1(accept), q_dead(reject)" << endl;
    cout << "  Complement DFA:      q0(start), q1(REJECT), q_dead(ACCEPT)" << endl;
    cout << "  L' accepts: \"123\", \"3abc\", \"+\", \"\"  (strings that are NOT identifiers)" << endl;
    cout << "  L' rejects: \"x\", \"count\", \"_temp\"    (strings that ARE identifiers)" << endl;
    cout << endl;
    cout << "  This works because regular languages are closed under complementation," << endl;
    cout << "  and DFAs can be complemented by flipping accept/reject states." << endl;

    cout << endl;
    cout << "  Example: Complement of Number Language" << endl;
    cout << "  Original L = {strings matching [0-9]+}" << endl;
    cout << "  L accepts:  \"0\", \"42\", \"999\"" << endl;
    cout << "  L rejects:  \"abc\", \"12x\", \"\"" << endl;
    cout << endl;
    cout << "  Complement L': accepts everything that is NOT a pure integer literal." << endl;
    cout << "  L' accepts: \"abc\", \"12x\", \"\", \"hello123\"" << endl;
    cout << "  L' rejects: \"0\", \"42\", \"999\"" << endl;
}

// =============================================
// Left/Right Derivation
// =============================================

void printDerivations() {
    cout << endl << "[PARSE TREES - LEFT AND RIGHT DERIVATION]" << endl;
    printLine();

    cout << endl;
    cout << "  Example expression: (x + 3) * y" << endl;
    cout << "  Using our CFG production rules." << endl;

    cout << endl;
    cout << "  --- Leftmost Derivation ---" << endl;
    cout << "  (Always expand the leftmost non-terminal first)" << endl;
    cout << endl;
    cout << "  expression" << endl;
    cout << "  => equality" << endl;
    cout << "  => comparison" << endl;
    cout << "  => term" << endl;
    cout << "  => factor" << endl;
    cout << "  => factor * unary" << endl;
    cout << "  => unary * unary" << endl;
    cout << "  => primary * unary" << endl;
    cout << "  => ( expression ) * unary" << endl;
    cout << "  => ( equality ) * unary" << endl;
    cout << "  => ( comparison ) * unary" << endl;
    cout << "  => ( term ) * unary" << endl;
    cout << "  => ( term + factor ) * unary" << endl;
    cout << "  => ( factor + factor ) * unary" << endl;
    cout << "  => ( unary + factor ) * unary" << endl;
    cout << "  => ( primary + factor ) * unary" << endl;
    cout << "  => ( IDENTIFIER(x) + factor ) * unary" << endl;
    cout << "  => ( IDENTIFIER(x) + unary ) * unary" << endl;
    cout << "  => ( IDENTIFIER(x) + primary ) * unary" << endl;
    cout << "  => ( IDENTIFIER(x) + NUMBER(3) ) * unary" << endl;
    cout << "  => ( IDENTIFIER(x) + NUMBER(3) ) * primary" << endl;
    cout << "  => ( IDENTIFIER(x) + NUMBER(3) ) * IDENTIFIER(y)" << endl;

    cout << endl;
    cout << "  --- Rightmost Derivation ---" << endl;
    cout << "  (Always expand the rightmost non-terminal first)" << endl;
    cout << endl;
    cout << "  expression" << endl;
    cout << "  => equality" << endl;
    cout << "  => comparison" << endl;
    cout << "  => term" << endl;
    cout << "  => factor" << endl;
    cout << "  => factor * unary" << endl;
    cout << "  => factor * primary" << endl;
    cout << "  => factor * IDENTIFIER(y)" << endl;
    cout << "  => unary * IDENTIFIER(y)" << endl;
    cout << "  => primary * IDENTIFIER(y)" << endl;
    cout << "  => ( expression ) * IDENTIFIER(y)" << endl;
    cout << "  => ( equality ) * IDENTIFIER(y)" << endl;
    cout << "  => ( comparison ) * IDENTIFIER(y)" << endl;
    cout << "  => ( term ) * IDENTIFIER(y)" << endl;
    cout << "  => ( term + factor ) * IDENTIFIER(y)" << endl;
    cout << "  => ( term + unary ) * IDENTIFIER(y)" << endl;
    cout << "  => ( term + primary ) * IDENTIFIER(y)" << endl;
    cout << "  => ( term + NUMBER(3) ) * IDENTIFIER(y)" << endl;
    cout << "  => ( factor + NUMBER(3) ) * IDENTIFIER(y)" << endl;
    cout << "  => ( unary + NUMBER(3) ) * IDENTIFIER(y)" << endl;
    cout << "  => ( primary + NUMBER(3) ) * IDENTIFIER(y)" << endl;
    cout << "  => ( IDENTIFIER(x) + NUMBER(3) ) * IDENTIFIER(y)" << endl;

    cout << endl;
    cout << "  Both derivations produce the same parse tree:" << endl;
    cout << endl;
    cout << "            *" << endl;
    cout << "           / \\" << endl;
    cout << "          +   y" << endl;
    cout << "         / \\" << endl;
    cout << "        x   3" << endl;
    cout << endl;
    cout << "  This grammar is unambiguous: left and right derivations" << endl;
    cout << "  produce the same tree. The recursive descent parser" << endl;
    cout << "  naturally follows leftmost derivation (top-down)." << endl;
}

// =============================================
// Chomsky Normal Form (CNF) and
// Greibach Normal Form (GNF)
// =============================================

void printCNFandGNF() {
    cout << endl << "[CHOMSKY NORMAL FORM (CNF)]" << endl;
    printLine();

    cout << endl;
    cout << "  CNF rules: every production is either A -> BC or A -> a" << endl;
    cout << "  (two non-terminals or one terminal, no epsilon except S -> epsilon)" << endl;
    cout << endl;
    cout << "  Converting a subset of our CFG to CNF:" << endl;
    cout << endl;
    cout << "  Original productions (simplified for expressions):" << endl;
    cout << "    E  -> E + T | T" << endl;
    cout << "    T  -> T * F | F" << endl;
    cout << "    F  -> ( E ) | id | num" << endl;
    cout << endl;
    cout << "  Step 1: Eliminate mixed terminals/non-terminals" << endl;
    cout << "    Introduce: P -> +    M -> *    L -> (    R -> )" << endl;
    cout << endl;
    cout << "    E  -> E P T | T         (replace + with P)" << endl;
    cout << "    T  -> T M F | F         (replace * with M)" << endl;
    cout << "    F  -> L E R | id | num  (replace ( ) with L R)" << endl;
    cout << endl;
    cout << "  Step 2: Break productions with >2 non-terminals" << endl;
    cout << "    Introduce: A1 -> P T    A2 -> M F    A3 -> E R" << endl;
    cout << endl;
    cout << "  CNF Result:" << endl;
    cout << "    E  -> E A1 | T          (A -> BC form)" << endl;
    cout << "    A1 -> P T               (A -> BC form)" << endl;
    cout << "    T  -> T A2 | F          (A -> BC form)" << endl;
    cout << "    A2 -> M F               (A -> BC form)" << endl;
    cout << "    F  -> L A3 | id | num   (A -> BC or A -> a)" << endl;
    cout << "    A3 -> E R               (A -> BC form)" << endl;
    cout << "    P  -> +                 (A -> a form)" << endl;
    cout << "    M  -> *                 (A -> a form)" << endl;
    cout << "    L  -> (                 (A -> a form)" << endl;
    cout << "    R  -> )                 (A -> a form)" << endl;
    cout << endl;
    cout << "  All productions are now in CNF (A -> BC or A -> a)." << endl;
    cout << "  CNF is useful for the CYK parsing algorithm (O(n^3))." << endl;

    cout << endl;
    cout << "============================================================" << endl;
    cout << endl << "[GREIBACH NORMAL FORM (GNF)]" << endl;
    printLine();

    cout << endl;
    cout << "  GNF rules: every production starts with a terminal" << endl;
    cout << "  followed by zero or more non-terminals: A -> a B1 B2 ..." << endl;
    cout << endl;
    cout << "  Converting the same grammar to GNF:" << endl;
    cout << endl;
    cout << "  Starting from CNF, we substitute to make each" << endl;
    cout << "  production start with a terminal symbol." << endl;
    cout << endl;
    cout << "  Step 1: Remove left recursion from E -> E A1" << endl;
    cout << "    E  -> T | T A1 | T A1 E'    (where E' handles repetition)" << endl;
    cout << "    E' -> A1 | A1 E'" << endl;
    cout << endl;
    cout << "  Step 2: Remove left recursion from T -> T A2" << endl;
    cout << "    T  -> F | F A2 | F A2 T'" << endl;
    cout << "    T' -> A2 | A2 T'" << endl;
    cout << endl;
    cout << "  Step 3: Substitute until each production starts with a terminal" << endl;
    cout << endl;
    cout << "  GNF Result:" << endl;
    cout << "    E  -> ( A3         | id        | num" << endl;
    cout << "         | ( A3 A1     | id A1     | num A1" << endl;
    cout << "         | ( A3 A1 E'  | id A1 E'  | num A1 E'" << endl;
    cout << "    E' -> + T          | + T E'" << endl;
    cout << "    T  -> ( A3         | id        | num" << endl;
    cout << "         | ( A3 A2     | id A2     | num A2" << endl;
    cout << "         | ( A3 A2 T'  | id A2 T'  | num A2 T'" << endl;
    cout << "    T' -> * F          | * F T'" << endl;
    cout << "    A1 -> + T" << endl;
    cout << "    A2 -> * F" << endl;
    cout << "    A3 -> ( A3 R      | id R      | num R" << endl;
    cout << "         (substituting E inside F -> ( E ) )" << endl;
    cout << endl;
    cout << "  Every production now starts with a terminal (id, num, (, +, *)." << endl;
    cout << "  GNF is useful for building LL(1) parsers and for proving" << endl;
    cout << "  that every CFL can be recognized by a PDA." << endl;
}

// =============================================
// Context-Free Grammar display
// =============================================

void printCFG() {
    cout << endl << "[CONTEXT-FREE GRAMMAR - PRODUCTION RULES]" << endl;
    printLine();

    cout << endl;
    cout << "  Our language is a Context-Free Language (CFL) because it has" << endl;
    cout << "  nested structures (blocks inside blocks, expressions in parens)" << endl;
    cout << "  that cannot be described by regular expressions alone." << endl;
    cout << endl;
    cout << "  Production rules:" << endl;
    cout << endl;
    cout << "  program     -> declaration*" << endl;
    cout << "  declaration -> intDecl | statement" << endl;
    cout << "  intDecl     -> \"int\" IDENTIFIER \"=\" expression \";\"" << endl;
    cout << "  statement   -> printStmt | ifStmt | whileStmt | block | assignment" << endl;
    cout << "  printStmt   -> \"print\" \"(\" expression \")\" \";\"" << endl;
    cout << "  ifStmt      -> \"if\" \"(\" expression \")\" \"{\" declaration* \"}\"" << endl;
    cout << "                  ( \"else\" \"{\" declaration* \"}\" )?" << endl;
    cout << "  whileStmt   -> \"while\" \"(\" expression \")\" \"{\" declaration* \"}\"" << endl;
    cout << "  block       -> \"{\" declaration* \"}\"" << endl;
    cout << "  assignment  -> IDENTIFIER \"=\" expression \";\"" << endl;
    cout << "  expression  -> equality" << endl;
    cout << "  equality    -> comparison ( (\"==\" | \"!=\") comparison )*" << endl;
    cout << "  comparison  -> term ( (\"<\" | \">\" | \"<=\" | \">=\") term )*" << endl;
    cout << "  term        -> factor ( (\"+\" | \"-\") factor )*" << endl;
    cout << "  factor      -> unary ( (\"*\" | \"/\") unary )*" << endl;
    cout << "  unary       -> \"-\" unary | primary" << endl;
    cout << "  primary     -> NUMBER | IDENTIFIER | \"(\" expression \")\"" << endl;
    cout << endl;
    cout << "  This grammar is parsed using recursive descent (top-down)." << endl;
    cout << "  Each non-terminal is a function in the parser." << endl;
}