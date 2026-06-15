#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

struct TokenInfo {
    string name;
    string lexeme;
    int line;
};

extern vector<TokenInfo> gTokenStream;

enum class NodeType {
    PROGRAM, FUNCTION, BLOCK, VAR_DECL, ASSIGN, IF, WHILE, FOR, RETURN, CALL, EXPR, IO
};

class ASTNode {
public:
    NodeType type;
    int line;
    ASTNode(NodeType t, int l) : type(t), line(l) {}
    virtual ~ASTNode() = default;
};

class ExprNode : public ASTNode {
public:
    string op;
    shared_ptr<ExprNode> left;
    shared_ptr<ExprNode> right;
    string value;
    string exprType; // int, float, bool, id
    vector<shared_ptr<ExprNode>> args;

    ExprNode(string o, shared_ptr<ExprNode> l, shared_ptr<ExprNode> r, int ln);
    ExprNode(string val, string t, int ln); // Literal or ID
    ExprNode(string funcName, vector<shared_ptr<ExprNode>> a, int ln);
};

class StmtNode : public ASTNode {
public:
    StmtNode(NodeType t, int l) : ASTNode(t, l) {}
};

class VarDeclNode : public StmtNode {
public:
    string varType;
    string id;
    VarDeclNode(string t, string i, int ln);
};

class AssignNode : public StmtNode {
public:
    string id;
    shared_ptr<ExprNode> expr;
    AssignNode(string i, shared_ptr<ExprNode> e, int ln);
};

class BlockNode : public StmtNode {
public:
    vector<shared_ptr<StmtNode>> statements;
    BlockNode(int ln);
};

class IfNode : public StmtNode {
public:
    shared_ptr<ExprNode> condition;
    shared_ptr<BlockNode> thenBlock;
    shared_ptr<BlockNode> elseBlock;
    IfNode(shared_ptr<ExprNode> c, shared_ptr<BlockNode> t, shared_ptr<BlockNode> e, int ln);
};

class WhileNode : public StmtNode {
public:
    shared_ptr<ExprNode> condition;
    shared_ptr<BlockNode> body;
    WhileNode(shared_ptr<ExprNode> c, shared_ptr<BlockNode> b, int ln);
};

class ForNode : public StmtNode {
public:
    shared_ptr<AssignNode> init;
    shared_ptr<ExprNode> condition;
    shared_ptr<AssignNode> update;
    shared_ptr<BlockNode> body;
    ForNode(shared_ptr<AssignNode> i, shared_ptr<ExprNode> c, shared_ptr<AssignNode> u, shared_ptr<BlockNode> b, int ln);
};

class IONode : public StmtNode {
public:
    bool isInput;
    string id;
    shared_ptr<ExprNode> expr;
    IONode(bool input, string i, shared_ptr<ExprNode> e, int ln);
};

class ExprStmtNode : public StmtNode {
public:
    shared_ptr<ExprNode> expr;
    ExprStmtNode(shared_ptr<ExprNode> e, int ln);
};

class ReturnNode : public StmtNode {
public:
    shared_ptr<ExprNode> expr;
    ReturnNode(shared_ptr<ExprNode> e, int ln);
};

class FunctionNode : public ASTNode {
public:
    string returnType;
    string id;
    vector<pair<string, string>> params; // type, name
    shared_ptr<BlockNode> body;
    FunctionNode(string rt, string i, vector<pair<string, string>> p, shared_ptr<BlockNode> b, int ln);
};

class ProgramNode : public ASTNode {
public:
    vector<shared_ptr<ASTNode>> declarations;
    ProgramNode(int ln);
};

void printAST(const shared_ptr<ProgramNode>& root);

#endif
