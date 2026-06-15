#include "ast.h"

namespace {
string branchPrefix(const string& prefix, bool last) {
    return prefix + (last ? "+-- " : "|-- ");
}

string childPrefix(const string& prefix, bool last) {
    return prefix + (last ? "    " : "|   ");
}

void printExprNode(const shared_ptr<ExprNode>& expr, const string& prefix, bool last);
void printStmtNode(const shared_ptr<StmtNode>& stmt, const string& prefix, bool last);
void printASTNode(const shared_ptr<ASTNode>& node, const string& prefix, bool last);

void printExprNode(const shared_ptr<ExprNode>& expr, const string& prefix, bool last) {
    if (!expr) return;
    if (expr->op == "val") {
        cout << branchPrefix(prefix, last) << expr->exprType << ": " << expr->value << "\n";
        return;
    }

    if (expr->op == "call") {
        cout << branchPrefix(prefix, last) << "call: " << expr->value << "\n";
        for (size_t i = 0; i < expr->args.size(); ++i) {
            printExprNode(expr->args[i], childPrefix(prefix, last), i + 1 == expr->args.size());
        }
        return;
    }

    cout << branchPrefix(prefix, last) << "expr: " << expr->op << "\n";
    if (expr->left) printExprNode(expr->left, childPrefix(prefix, last), expr->right == nullptr);
    if (expr->right) printExprNode(expr->right, childPrefix(prefix, last), true);
}

void printStmtNode(const shared_ptr<StmtNode>& stmt, const string& prefix, bool last) {
    if (!stmt) return;
    switch (stmt->type) {
        case NodeType::VAR_DECL: {
            auto node = dynamic_pointer_cast<VarDeclNode>(stmt);
            cout << branchPrefix(prefix, last) << "var_decl: " << node->varType << " " << node->id << "\n";
            break;
        }
        case NodeType::ASSIGN: {
            auto node = dynamic_pointer_cast<AssignNode>(stmt);
            cout << branchPrefix(prefix, last) << "assign: " << node->id << "\n";
            printExprNode(node->expr, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::IF: {
            auto node = dynamic_pointer_cast<IfNode>(stmt);
            cout << branchPrefix(prefix, last) << "if\n";
            printExprNode(node->condition, childPrefix(prefix, last), false);
            printASTNode(node->thenBlock, childPrefix(prefix, last), node->elseBlock == nullptr);
            if (node->elseBlock) printASTNode(node->elseBlock, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::WHILE: {
            auto node = dynamic_pointer_cast<WhileNode>(stmt);
            cout << branchPrefix(prefix, last) << "while\n";
            printExprNode(node->condition, childPrefix(prefix, last), false);
            printASTNode(node->body, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::FOR: {
            auto node = dynamic_pointer_cast<ForNode>(stmt);
            cout << branchPrefix(prefix, last) << "for\n";
            printASTNode(node->init, childPrefix(prefix, last), false);
            printExprNode(node->condition, childPrefix(prefix, last), false);
            printASTNode(node->update, childPrefix(prefix, last), false);
            printASTNode(node->body, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::IO: {
            auto node = dynamic_pointer_cast<IONode>(stmt);
            cout << branchPrefix(prefix, last) << (node->isInput ? "input" : "output") << "\n";
            if (node->isInput) cout << childPrefix(prefix, last) << "+-- id: " << node->id << "\n";
            else printExprNode(node->expr, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::RETURN: {
            auto node = dynamic_pointer_cast<ReturnNode>(stmt);
            cout << branchPrefix(prefix, last) << "return\n";
            printExprNode(node->expr, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::EXPR: {
            auto node = dynamic_pointer_cast<ExprStmtNode>(stmt);
            cout << branchPrefix(prefix, last) << "expr_stmt\n";
            printExprNode(node->expr, childPrefix(prefix, last), true);
            break;
        }
        case NodeType::BLOCK: {
            auto node = dynamic_pointer_cast<BlockNode>(stmt);
            cout << branchPrefix(prefix, last) << "block\n";
            for (size_t i = 0; i < node->statements.size(); ++i) {
                printStmtNode(node->statements[i], childPrefix(prefix, last), i + 1 == node->statements.size());
            }
            break;
        }
        default:
            cout << branchPrefix(prefix, last) << "stmt\n";
            break;
    }
}

void printASTNode(const shared_ptr<ASTNode>& node, const string& prefix, bool last) {
    if (!node) return;
    if (node->type == NodeType::BLOCK) {
        printStmtNode(dynamic_pointer_cast<StmtNode>(node), prefix, last);
        return;
    }

    auto stmt = dynamic_pointer_cast<StmtNode>(node);
    if (stmt) {
        printStmtNode(stmt, prefix, last);
        return;
    }

    auto func = dynamic_pointer_cast<FunctionNode>(node);
    if (func) {
        cout << branchPrefix(prefix, last) << "function: " << func->returnType << " " << func->id << "\n";
        for (size_t i = 0; i < func->params.size(); ++i) {
            cout << childPrefix(prefix, last) << branchPrefix("", i + 1 == func->params.size())
                 << "param: " << func->params[i].first << " " << func->params[i].second << "\n";
        }
        printASTNode(func->body, childPrefix(prefix, last), true);
    }
}
}

ExprNode::ExprNode(string o, shared_ptr<ExprNode> l, shared_ptr<ExprNode> r, int ln) 
    : ASTNode(NodeType::EXPR, ln), op(o), left(l), right(r), exprType("op") {}
ExprNode::ExprNode(string val, string t, int ln) 
    : ASTNode(NodeType::EXPR, ln), op("val"), value(val), exprType(t) {}
ExprNode::ExprNode(string funcName, vector<shared_ptr<ExprNode>> a, int ln)
    : ASTNode(NodeType::EXPR, ln), op("call"), value(funcName), exprType("call"), args(a) {}

VarDeclNode::VarDeclNode(string t, string i, int ln) : StmtNode(NodeType::VAR_DECL, ln), varType(t), id(i) {}
AssignNode::AssignNode(string i, shared_ptr<ExprNode> e, int ln) : StmtNode(NodeType::ASSIGN, ln), id(i), expr(e) {}
BlockNode::BlockNode(int ln) : StmtNode(NodeType::BLOCK, ln) {}

IfNode::IfNode(shared_ptr<ExprNode> c, shared_ptr<BlockNode> t, shared_ptr<BlockNode> e, int ln) 
    : StmtNode(NodeType::IF, ln), condition(c), thenBlock(t), elseBlock(e) {}

WhileNode::WhileNode(shared_ptr<ExprNode> c, shared_ptr<BlockNode> b, int ln) 
    : StmtNode(NodeType::WHILE, ln), condition(c), body(b) {}

ForNode::ForNode(shared_ptr<AssignNode> i, shared_ptr<ExprNode> c, shared_ptr<AssignNode> u, shared_ptr<BlockNode> b, int ln) 
    : StmtNode(NodeType::FOR, ln), init(i), condition(c), update(u), body(b) {}

IONode::IONode(bool input, string i, shared_ptr<ExprNode> e, int ln) 
    : StmtNode(NodeType::IO, ln), isInput(input), id(i), expr(e) {}

ExprStmtNode::ExprStmtNode(shared_ptr<ExprNode> e, int ln)
    : StmtNode(NodeType::EXPR, ln), expr(e) {}

ReturnNode::ReturnNode(shared_ptr<ExprNode> e, int ln) : StmtNode(NodeType::RETURN, ln), expr(e) {}

FunctionNode::FunctionNode(string rt, string i, vector<pair<string, string>> p, shared_ptr<BlockNode> b, int ln) 
    : ASTNode(NodeType::FUNCTION, ln), returnType(rt), id(i), params(p), body(b) {}

ProgramNode::ProgramNode(int ln) : ASTNode(NodeType::PROGRAM, ln) {}

void printAST(const shared_ptr<ProgramNode>& root) {
    if (!root) {
        cout << "<no AST available>\n";
        return;
    }

    cout << "PROGRAM\n";
    for (size_t i = 0; i < root->declarations.size(); ++i) {
        printASTNode(root->declarations[i], "", i + 1 == root->declarations.size());
    }
}
