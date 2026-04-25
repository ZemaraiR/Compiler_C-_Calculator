#include "Parser.h"

// parser.cpp - Parser and Interpreter implementation

// Constructors and destructor
ParserInterpreter::ParserInterpreter(vector<Token> t, bool ast, bool asm_gen) 
{
    tokens = t; current = 0; makeAST = ast; genAsm = asm_gen;
    regCounter = 0; labelCounter = 0; nextAddress = 0x1000;
    isTopLevel = true;
}

ParserInterpreter::ParserInterpreter(vector<Token> t, unordered_map<string, int> vars, bool ast, bool asm_gen) 
{
    tokens = t; current = 0; variables = vars; makeAST = ast; genAsm = asm_gen;
    regCounter = 0; labelCounter = 0; nextAddress = 0x1000;
    isTopLevel = false;
}

ParserInterpreter::~ParserInterpreter() 
{
    for (int i = 0; i < astRoots.size(); i++)
    {
        delete astRoots[i];
    }
}

// Token navigation helpers
Token ParserInterpreter::peek() 
{ 
    return tokens[current];
}
Token ParserInterpreter::previous()
{ 
    return tokens[current - 1]; 
}
bool ParserInterpreter::isAtEnd() 
{ 
    return peek().type == TokenType::END_OF_FILE; 
}

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
            peek().type == TokenType::ELSE_KW) return;
        advance();
    }
}

// Assembly and dataflow helpers
void ParserInterpreter::emit(string instruction) 
{
    if (genAsm) asmCode.push_back(instruction);
}

string ParserInterpreter::newLabel() 
{
    labelCounter++;
    return "L" + to_string(labelCounter);
}

void ParserInterpreter::trackDef(string var) 
{ 
    currentDF.defined.push_back(var); 
}
void ParserInterpreter::trackUse(string var) 
{ 
    currentDF.used.push_back(var); 
}

void ParserInterpreter::startStatement(int ln, string desc) 
{
    currentDF.line = ln;
    currentDF.statement = desc;
    currentDF.defined.clear();
    currentDF.used.clear();
}

void ParserInterpreter::endStatement() 
{
    if (currentDF.statement != "") 
    {
        dataFlow.push_back(currentDF);
    }
    currentDF.statement = "";
}

// Expression parsing (recursive descent)
pair<int, ASTNode*> ParserInterpreter::expression() 
{ 
    return equality(); 
}

pair<int, ASTNode*> ParserInterpreter::equality() 
{
    pair<int, ASTNode*> result = comparison();
    int left = result.first;
    ASTNode* node = result.second;

    while (match({TokenType::EQ, TokenType::NEQ})) 
    {
        Token op = previous();
        pair<int, ASTNode*> rr = comparison();
        int right = rr.first;
        ASTNode* rNode = rr.second;

        string asmOp = (op.type == TokenType::EQ) ? "CMP_EQ" : "CMP_NEQ";
        emit("  " + asmOp + " R" + to_string(regCounter-2) + ", R" + to_string(regCounter-2) + ", R" + to_string(regCounter-1));
        regCounter--;

        if (op.type == TokenType::EQ) left = (left == right);
        else left = (left != right);

        if (makeAST) 
        {
            ASTNode* newNode = new ASTNode(op.lexeme);
            newNode->addChild(node); newNode->addChild(rNode);
            node = newNode;
        }
    }
    return make_pair(left, node);
}

pair<int, ASTNode*> ParserInterpreter::comparison() 
{
    pair<int, ASTNode*> result = term();
    int left = result.first;
    ASTNode* node = result.second;

    while (match({TokenType::LT, TokenType::GT, TokenType::LE, TokenType::GE})) 
    {
        Token op = previous();
        pair<int, ASTNode*> rr = term();
        int right = rr.first;
        ASTNode* rNode = rr.second;

        string asmOp = "CMP";
        if (op.type == TokenType::LT) asmOp = "CMP_LT";
        else if (op.type == TokenType::GT) asmOp = "CMP_GT";
        else if (op.type == TokenType::LE) asmOp = "CMP_LE";
        else asmOp = "CMP_GE";

        emit("  " + asmOp + " R" + to_string(regCounter-2) + ", R" + to_string(regCounter-2) + ", R" + to_string(regCounter-1));
        regCounter--;

        if (op.type == TokenType::LT) left = (left < right);
        else if (op.type == TokenType::GT) left = (left > right);
        else if (op.type == TokenType::LE) left = (left <= right);
        else left = (left >= right);

        if (makeAST) 
        {
            ASTNode* newNode = new ASTNode(op.lexeme);
            newNode->addChild(node); newNode->addChild(rNode);
            node = newNode;
        }
    }
    return make_pair(left, node);
}

pair<int, ASTNode*> ParserInterpreter::term() 
{
    pair<int, ASTNode*> result = factor();
    int left = result.first;
    ASTNode* node = result.second;

    while (match({TokenType::PLUS, TokenType::MINUS})) 
    {
        Token op = previous();
        pair<int, ASTNode*> rr = factor();
        int right = rr.first;
        ASTNode* rNode = rr.second;

        string asmOp = (op.type == TokenType::PLUS) ? "ADD" : "SUB";
        emit("  " + asmOp + " R" + to_string(regCounter-2) + ", R" + to_string(regCounter-2) + ", R" + to_string(regCounter-1));
        regCounter--;

        if (op.type == TokenType::PLUS) left = left + right;
        else left = left - right;

        if (makeAST) 
        {
            ASTNode* newNode = new ASTNode(op.lexeme);
            newNode->addChild(node); newNode->addChild(rNode);
            node = newNode;
        }
    }
    return make_pair(left, node);
}

pair<int, ASTNode*> ParserInterpreter::factor() 
{
    pair<int, ASTNode*> result = unary();
    int left = result.first;
    ASTNode* node = result.second;

    while (match({TokenType::STAR, TokenType::SLASH})) 
    {
        Token op = previous();
        pair<int, ASTNode*> rr = unary();
        int right = rr.first;
        ASTNode* rNode = rr.second;

        string asmOp = (op.type == TokenType::STAR) ? "MUL" : "DIV";
        emit("  " + asmOp + " R" + to_string(regCounter-2) + ", R" + to_string(regCounter-2) + ", R" + to_string(regCounter-1));
        regCounter--;

        if (op.type == TokenType::STAR) 
        {
            left = left * right;
        } else 
        {
            if (right == 0) 
            {
                errors.push_back("Line " + to_string(op.line) + ": division by zero");
                left = 0;
            } else 
            {
                left = left / right;
            }
        }

        if (makeAST) 
        {
            ASTNode* newNode = new ASTNode(op.lexeme);
            newNode->addChild(node); newNode->addChild(rNode);
            node = newNode;
        }
    }
    return make_pair(left, node);
}

pair<int, ASTNode*> ParserInterpreter::unary() 
{
    if (match({TokenType::MINUS})) 
    {
        pair<int, ASTNode*> result = unary();
        emit("  NEG R" + to_string(regCounter-1) + ", R" + to_string(regCounter-1));
        ASTNode* node = nullptr;
        if (makeAST) { node = new ASTNode("NEG"); node->addChild(result.second); }
        return make_pair(-result.first, node);
    }
    return primary();
}

pair<int, ASTNode*> ParserInterpreter::primary() 
{
    if (match({TokenType::NUMBER})) 
    {
        int val = stoi(previous().lexeme);
        emit("  LOAD R" + to_string(regCounter) + ", #" + previous().lexeme);
        regCounter++;
        ASTNode* node = nullptr;
        if (makeAST) node = new ASTNode(previous().lexeme);
        return make_pair(val, node);
    }

    if (match({TokenType::IDENTIFIER})) 
    {
        string name = previous().lexeme;
        trackUse(name);
        emit("  LOAD R" + to_string(regCounter) + ", [" + name + "]");
        regCounter++;
        if (variables.find(name) == variables.end()) 
        {
            errors.push_back("Line " + to_string(previous().line) + ": undeclared variable '" + name + "'");
            return make_pair(0, makeAST ? new ASTNode(name + "?") : nullptr);
        }
        return make_pair(variables[name], makeAST ? new ASTNode(name) : nullptr);
    }

    if (match({TokenType::LPAREN})) 
    {
        pair<int, ASTNode*> result = expression();
        consume(TokenType::RPAREN, "expected ')' after expression");
        return result;
    }

    errors.push_back("Line " + to_string(peek().line) + ": expected expression");
    return make_pair(0, nullptr);
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

    startStatement(name.line, "int " + name.lexeme + " = ...");
    regCounter = 0;

    pair<int, ASTNode*> result = expression();
    int value = result.first;
    ASTNode* exprNode = result.second;
    consume(TokenType::SEMICOLON, "expected ';' after declaration");

    if (name.type != TokenType::INVALID) 
    {
        variables[name.lexeme] = value;
        trackDef(name.lexeme);
        emit("  STORE R0, [" + name.lexeme + "]");

        // memory allocation
        bool found = false;
        for (int i = 0; i < memoryMap.size(); i++) 
        {
            if (memoryMap[i].name == name.lexeme) 
            { 
                found = true; break; 
            }
        }
        if (!found) 
        {
            MemEntry me;
            me.name = name.lexeme;
            me.address = nextAddress;
            me.size = 4;
            memoryMap.push_back(me);
            nextAddress += 4;
        }

        if (makeAST) 
        {
            ASTNode* decl = new ASTNode("DECLARE");
            decl->addChild(new ASTNode("int " + name.lexeme));
            decl->addChild(new ASTNode("="));
            if (exprNode != nullptr) decl->addChild(exprNode);
            else decl->addChild(new ASTNode(to_string(value)));
            astRoots.push_back(decl);
        }
    }
    endStatement();
}

void ParserInterpreter::statement() 
{
    if (match({TokenType::PRINT_KW})) 
    { 
        printStatement(); 
        return; 
    }
    if (match({TokenType::IF_KW})) 
    { 
        ifStatement(); 
        return; 
    }
    if (match({TokenType::WHILE_KW})) 
    { 
        whileStatement(); 
        return; 
    }
    if (match({TokenType::LBRACE})) 
    { 
        block(); 
        return; 
    }
    if (check(TokenType::IDENTIFIER)) 
    { 
        assignmentStatement(); 
        return; 
    }
    errors.push_back("Line " + to_string(peek().line) + ": invalid statement");
    advance();
}

void ParserInterpreter::assignmentStatement() 
{
    Token name = advance();
    consume(TokenType::ASSIGN, "expected '=' in assignment");

    startStatement(name.line, name.lexeme + " = ...");
    regCounter = 0;

    pair<int, ASTNode*> result = expression();
    int value = result.first;
    ASTNode* exprNode = result.second;
    consume(TokenType::SEMICOLON, "expected ';' after assignment");

    trackDef(name.lexeme);

    if (variables.find(name.lexeme) == variables.end()) 
    {
        errors.push_back("Line " + to_string(name.line) + ": undeclared variable '" + name.lexeme + "'");
    } else 
    {
        variables[name.lexeme] = value;
    }
    emit("  STORE R0, [" + name.lexeme + "]");

    if (makeAST) 
    {
        ASTNode* n = new ASTNode("ASSIGN");
        n->addChild(new ASTNode(name.lexeme));
        if (exprNode != nullptr) n->addChild(exprNode);
        else n->addChild(new ASTNode(to_string(value)));
        astRoots.push_back(n);
    }
    endStatement();
}

void ParserInterpreter::printStatement() 
{
    consume(TokenType::LPAREN, "expected '(' after 'print'");

    startStatement(previous().line, "print(...)");
    regCounter = 0;

    pair<int, ASTNode*> result = expression();
    int value = result.first;
    ASTNode* exprNode = result.second;
    consume(TokenType::RPAREN, "expected ')' after print expression");
    consume(TokenType::SEMICOLON, "expected ';' after print statement");
    outputs.push_back(value);

    emit("  OUT R0");

    if (makeAST) 
    {
        ASTNode* n = new ASTNode("PRINT");
        if (exprNode != nullptr) n->addChild(exprNode);
        else n->addChild(new ASTNode(to_string(value)));
        astRoots.push_back(n);
    }
    endStatement();
}

// Block handling (for if/while)
vector<Token> ParserInterpreter::captureBlockTokens() 
{
    int braceCount = 1;
    int start = current;
    while (!isAtEnd() && braceCount > 0) 
    {
        if (tokens[current].type == TokenType::LBRACE)
        {
            braceCount++;
        }
        if (tokens[current].type == TokenType::RBRACE)
        {
            braceCount--;
        }
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
    ParserInterpreter nested(blockTokens, variables, false, genAsm);
    nested.memoryMap = memoryMap;
    nested.nextAddress = nextAddress;
    nested.labelCounter = labelCounter;
    nested.parseProgram();
    variables = nested.getVariables();
    memoryMap = nested.memoryMap;
    nextAddress = nested.nextAddress;
    labelCounter = nested.labelCounter;
    vector<int> nestedOut = nested.getOutputs();
    for (int i = 0; i < nestedOut.size(); i++)
    {
        outputs.push_back(nestedOut[i]);
    }
    vector<string> nestedErr = nested.getErrors();
    for (int i = 0; i < nestedErr.size(); i++)
    {
        errors.push_back(nestedErr[i]);
    }
    vector<string> nestedAsm = nested.getAssembly();
    for (int i = 0; i < nestedAsm.size(); i++)
    {
        asmCode.push_back(nestedAsm[i]);
    }
    vector<DataFlowInfo> nestedDF = nested.getDataFlow();
    for (int i = 0; i < nestedDF.size(); i++)
    {
        dataFlow.push_back(nestedDF[i]);
    }
}

void ParserInterpreter::ifStatement() 
{
    consume(TokenType::LPAREN, "expected '(' after 'if'");
    regCounter = 0;
    pair<int, ASTNode*> condResult = expression();
    int condition = condResult.first;
    ASTNode* condNode = condResult.second;
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

    string elseLabel = newLabel();
    string endLabel = newLabel();

    emit("  CMP R0, #0");
    emit("  JMP_EQ " + elseLabel);
    emit("  ; -- then branch --");

    if (makeAST) {
        ASTNode* n = new ASTNode("IF");
        ASTNode* condWrap = new ASTNode("COND");
        if (condNode != nullptr) condWrap->addChild(condNode);
        n->addChild(condWrap);
        if (condition) n->addChild(new ASTNode("-> THEN (taken)"));
        else n->addChild(new ASTNode("-> THEN (skipped)"));
        if (hasElse) {
            if (!condition) n->addChild(new ASTNode("-> ELSE (taken)"));
            else n->addChild(new ASTNode("-> ELSE (skipped)"));
        }
        astRoots.push_back(n);
    }

    if (condition) executeBlockTokens(ifBlock);

    emit("  JMP " + endLabel);
    emit(elseLabel + ":");
    emit("  ; -- else branch --");

    if (!condition && hasElse) executeBlockTokens(elseBlock);

    emit(endLabel + ":");
}

void ParserInterpreter::whileStatement() 
{
    consume(TokenType::LPAREN, "expected '(' after 'while'");
    int condStart = current;
    regCounter = 0;
    pair<int, ASTNode*> condResult = expression();
    int condValue = condResult.first;
    ASTNode* condNode = condResult.second;
    int condEnd = current;
    consume(TokenType::RPAREN, "expected ')' after while condition");
    consume(TokenType::LBRACE, "expected '{' after while condition");
    vector<Token> loopBlock = captureBlockTokens();

    string loopStart = newLabel();
    string loopEnd = newLabel();

    emit(loopStart + ":");
    emit("  ; -- evaluate condition --");
    emit("  CMP R0, #0");
    emit("  JMP_EQ " + loopEnd);
    emit("  ; -- loop body --");

    if (makeAST) {
        ASTNode* n = new ASTNode("WHILE");
        ASTNode* condWrap = new ASTNode("COND");
        if (condNode != nullptr) condWrap->addChild(condNode);
        n->addChild(condWrap);
        n->addChild(new ASTNode("BODY {...}"));
        astRoots.push_back(n);
    }

    // first iteration: emit assembly and track dataflow
    // subsequent iterations: only interpret (no duplicate asm/dataflow)
    bool firstIteration = true;
    int counter = 0;
    while (condValue) 
    {
        if (firstIteration) 
        {
            executeBlockTokens(loopBlock);
            firstIteration = false;
        } else 
        {
            // run without assembly gen to avoid duplicating code
            ParserInterpreter bodyParser(loopBlock, variables, false, false);
            bodyParser.parseProgram();
            variables = bodyParser.getVariables();
            vector<int> nestedOut = bodyParser.getOutputs();
            for (int i = 0; i < nestedOut.size(); i++) outputs.push_back(nestedOut[i]);
            vector<string> nestedErr = bodyParser.getErrors();
            for (int i = 0; i < nestedErr.size(); i++) errors.push_back(nestedErr[i]);
        }
        vector<Token> condTokens;
        for (int i = condStart; i < condEnd; i++) condTokens.push_back(tokens[i]);
        condTokens.push_back({TokenType::END_OF_FILE, "", tokens[condEnd - 1].line});
        ParserInterpreter condParser(condTokens, variables, false, false);
        condValue = condParser.evaluateExpressionOnly();
        counter++;
        if (counter > 10000) { errors.push_back("Stopped: possible infinite loop"); break; }
    }

    emit("  JMP " + loopStart);
    emit(loopEnd + ":");
}

void ParserInterpreter::block() 
{
    while (!check(TokenType::RBRACE) && !isAtEnd()) declaration();
    consume(TokenType::RBRACE, "expected '}' after block");
}

// Public methods
void ParserInterpreter::parseProgram() 
{
    if (genAsm && isTopLevel) emit("  ; --- program start ---");
    while (!isAtEnd()) 
    {
        try { declaration(); } catch (...) 
        {
            errors.push_back("Line " + to_string(peek().line) + ": parser error");
            synchronize();
        }
    }
    if (genAsm && isTopLevel) emit("  HALT");
}

int ParserInterpreter::evaluateExpressionOnly() 
{
    current = 0;
    return expression().first;
}

vector<string> ParserInterpreter::getErrors() 
{ 
    return errors; 
}
vector<int> ParserInterpreter::getOutputs() 
{ 
    return outputs; 
}
unordered_map<string, int> ParserInterpreter::getVariables() 
{ 
    return variables; 
}
vector<ASTNode*> ParserInterpreter::getAST() 
{ 
    return astRoots; 
}
vector<string> ParserInterpreter::getAssembly() 
{ 
    return asmCode; 
}
vector<DataFlowInfo> ParserInterpreter::getDataFlow() 
{ 
    return dataFlow; 
}
vector<MemEntry> ParserInterpreter::getMemoryMap() 
{ 
    return memoryMap; 
}
