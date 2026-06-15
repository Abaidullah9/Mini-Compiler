#ifndef TAC_H
#define TAC_H

#include "ast.h"
#include <string>
#include <vector>

struct TAC { string op, arg1, arg2, result; };

class TACGenerator {
private:
    int tempCount = 0;
    int labelCount = 0;
    
public:
    vector<TAC> instructions;
    string newTemp();
    string newLabel();
    void emit(string op, string arg1, string arg2, string result);
    string genExpr(shared_ptr<ExprNode> expr);
    void genStmt(shared_ptr<StmtNode> stmt);
    void genBlock(shared_ptr<BlockNode> block);
    void gen(shared_ptr<ProgramNode> root);
    void printTAC();
};

#endif
