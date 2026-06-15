#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h"
#include "symbol_table.h"

class SemanticAnalyzer {
private:
    SymbolTable& symTable;
    string currentFuncType = "";
    int errorCount = 0;

    void reportError(string msg, int line);
    string checkExpr(shared_ptr<ExprNode> expr);
    void checkBlock(shared_ptr<BlockNode> block);
    void checkStmt(shared_ptr<StmtNode> stmt);
    void checkFunc(shared_ptr<FunctionNode> func);

public:
    SemanticAnalyzer(SymbolTable& st);
    bool analyze(shared_ptr<ProgramNode> root);
};

#endif
