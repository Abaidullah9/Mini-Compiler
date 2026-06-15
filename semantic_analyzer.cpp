#include "semantic_analyzer.h"

namespace {
bool isNumericType(const string& type) {
    return type == "int" || type == "float";
}

bool canPromote(const string& from, const string& to) {
    return from == to || (from == "int" && to == "float");
}
}

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& st) : symTable(st) {}

void SemanticAnalyzer::reportError(string msg, int line) {
    cerr << "[Semantic Error] Line " << line << ": " << msg << endl;
    errorCount++;
}

string SemanticAnalyzer::checkExpr(shared_ptr<ExprNode> expr) {
    if (!expr) return "void";
    if (expr->op == "val") {
        if (expr->exprType == "id") {
            SymbolInfo* sym = symTable.lookup(expr->value);
            if (!sym) { reportError("Undeclared variable '" + expr->value + "'", expr->line); return "error"; }
            return sym->type;
        }
        return expr->exprType; // int, float, bool
    }

    if (expr->op == "call") {
        SymbolInfo* sym = symTable.lookup(expr->value);
        if (!sym || sym->kind != "func") {
            reportError("Call to undeclared function '" + expr->value + "'", expr->line);
            return "error";
        }

        if (sym->paramTypes.size() != expr->args.size()) {
            reportError("Parameter count mismatch in call to '" + expr->value + "'", expr->line);
        } else {
            for (size_t i = 0; i < expr->args.size(); ++i) {
                string actualType = checkExpr(expr->args[i]);
                if (actualType == "error") continue;
                if (!canPromote(actualType, sym->paramTypes[i])) {
                    reportError("Parameter type mismatch in call to '" + expr->value + "'", expr->args[i]->line);
                }
            }
        }
        return sym->type;
    }

    if (expr->op == "!") {
        string operandType = checkExpr(expr->right);
        if (operandType != "bool" && operandType != "error") reportError("Logical not requires a boolean operand", expr->line);
        return "bool";
    }

    if (expr->op == "++" || expr->op == "--") {
        string operandType = checkExpr(expr->right);
        if (!isNumericType(operandType) && operandType != "error") reportError("Increment/decrement requires a numeric operand", expr->line);
        return operandType == "error" ? "error" : operandType;
    }
    
    string lType = checkExpr(expr->left);
    string rType = checkExpr(expr->right);
    
    if (lType == "error" || rType == "error") return "error";
    if (expr->op == "&&" || expr->op == "||") {
        if (lType != "bool" || rType != "bool") reportError("Logical operators require boolean operands", expr->line);
        return "bool";
    }
    if (expr->op == "==" || expr->op == "!=" || expr->op == "<" || expr->op == ">" || expr->op == "<=" || expr->op == ">=") return "bool";
    if (isNumericType(lType) && isNumericType(rType)) return (lType == "float" || rType == "float") ? "float" : "int";
    return "error";
}

void SemanticAnalyzer::checkStmt(shared_ptr<StmtNode> stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case NodeType::VAR_DECL: {
            auto d = dynamic_pointer_cast<VarDeclNode>(stmt);
            if (d->varType == "void") reportError("Variable cannot be void", d->line);
            if (!symTable.insert(d->id, d->varType, "var", d->line))
                reportError("Redeclaration of '" + d->id + "'", d->line);
            break;
        }
        case NodeType::ASSIGN: {
            auto a = dynamic_pointer_cast<AssignNode>(stmt);
            SymbolInfo* sym = symTable.lookup(a->id);
            if (!sym) reportError("Undeclared variable '" + a->id + "'", a->line);
            else {
                string rType = checkExpr(a->expr);
                if (sym->type != rType && rType != "error" && !(sym->type=="float" && rType=="int"))
                    reportError("Type mismatch in assignment to '" + a->id + "'", a->line);
            }
            break;
        }
        case NodeType::IF: {
            auto i = dynamic_pointer_cast<IfNode>(stmt);
            string condType = checkExpr(i->condition);
            if (condType != "bool" && condType != "error") reportError("Condition must be boolean", i->line);
            checkBlock(i->thenBlock);
            if (i->elseBlock) checkBlock(i->elseBlock);
            break;
        }
        case NodeType::WHILE: {
            auto w = dynamic_pointer_cast<WhileNode>(stmt);
            if (checkExpr(w->condition) != "bool") reportError("Condition must be boolean", w->line);
            checkBlock(w->body);
            break;
        }
        case NodeType::FOR: {
            auto f = dynamic_pointer_cast<ForNode>(stmt);
            checkStmt(f->init);
            string condType = checkExpr(f->condition);
            if (condType != "bool" && condType != "error") reportError("For loop condition must be boolean", f->line);
            checkStmt(f->update);
            checkBlock(f->body);
            break;
        }
        case NodeType::RETURN: {
            auto r = dynamic_pointer_cast<ReturnNode>(stmt);
            string retType = checkExpr(r->expr);
            if (retType != currentFuncType && !(currentFuncType=="float" && retType=="int"))
                reportError("Return type mismatch", r->line);
            break;
        }
        case NodeType::BLOCK: {
            checkBlock(dynamic_pointer_cast<BlockNode>(stmt));
            break;
        }
        case NodeType::EXPR: {
            auto e = dynamic_pointer_cast<ExprStmtNode>(stmt);
            if (e) checkExpr(e->expr);
            break;
        }
        default: break;
    }
}

void SemanticAnalyzer::checkBlock(shared_ptr<BlockNode> block) {
    if (!block) return;
    symTable.enterScope();
    for (auto s : block->statements) checkStmt(s);
    symTable.exitScope();
}

bool SemanticAnalyzer::analyze(shared_ptr<ProgramNode> root) {
    for (auto node : root->declarations) {
        if (node->type == NodeType::FUNCTION) {
            auto f = dynamic_pointer_cast<FunctionNode>(node);
            vector<string> pTypes;
            for (auto p : f->params) pTypes.push_back(p.first);
            if (!symTable.insert(f->id, f->returnType, "func", f->line, pTypes))
                reportError("Redeclaration of function '" + f->id + "'", f->line);
            
            symTable.enterScope();
            currentFuncType = f->returnType;
            for (auto p : f->params) symTable.insert(p.second, p.first, "var", f->line);
            checkBlock(f->body);
            symTable.exitScope();
        } else {
            checkStmt(dynamic_pointer_cast<StmtNode>(node));
        }
    }
    return errorCount == 0;
}
