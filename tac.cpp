#include "tac.h"

string TACGenerator::newTemp() { return "t" + to_string(tempCount++); }
string TACGenerator::newLabel() { return "L" + to_string(labelCount++); }

void TACGenerator::emit(string op, string arg1, string arg2, string result) {
    instructions.push_back({op, arg1, arg2, result});
}

string TACGenerator::genExpr(shared_ptr<ExprNode> expr) {
    if (!expr) return "";
    if (expr->op == "val") return expr->value;
    if (expr->op == "call") {
        for (const auto& arg : expr->args) {
            emit("PARAM", genExpr(arg), "", "");
        }
        string res = newTemp();
        emit("CALL", expr->value, to_string(expr->args.size()), res);
        return res;
    }

    if (expr->op == "!") {
        string arg = genExpr(expr->right);
        string res = newTemp();
        emit("NOT", arg, "", res);
        return res;
    }

    if (expr->op == "++" || expr->op == "--") {
        string target = genExpr(expr->right);
        string res = newTemp();
        string op = expr->op == "++" ? "+" : "-";
        emit(op, target, "1", res);
        emit("=", res, "", target);
        return res;
    }

    string arg1 = genExpr(expr->left);
    string arg2 = genExpr(expr->right);
    string res = newTemp();
    emit(expr->op, arg1, arg2, res);
    return res;
}

void TACGenerator::genStmt(shared_ptr<StmtNode> stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case NodeType::ASSIGN: {
            auto a = dynamic_pointer_cast<AssignNode>(stmt);
            string res = genExpr(a->expr);
            emit("=", res, "", a->id);
            break;
        }
        case NodeType::IF: {
            auto i = dynamic_pointer_cast<IfNode>(stmt);
            string cond = genExpr(i->condition);
            string lElse = newLabel();
            string lEnd = newLabel();
            emit("ifFalse", cond, "goto", lElse);
            genBlock(i->thenBlock);
            emit("goto", "", "", lEnd);
            emit("LABEL", "", "", lElse);
            if (i->elseBlock) genBlock(i->elseBlock);
            emit("LABEL", "", "", lEnd);
            break;
        }
        case NodeType::WHILE: {
            auto w = dynamic_pointer_cast<WhileNode>(stmt);
            string lStart = newLabel();
            string lEnd = newLabel();
            emit("LABEL", "", "", lStart);
            string cond = genExpr(w->condition);
            emit("ifFalse", cond, "goto", lEnd);
            genBlock(w->body);
            emit("goto", "", "", lStart);
            emit("LABEL", "", "", lEnd);
            break;
        }
        case NodeType::FOR: {
            auto f = dynamic_pointer_cast<ForNode>(stmt);
            genStmt(f->init);
            string lStart = newLabel();
            string lEnd = newLabel();
            emit("LABEL", "", "", lStart);
            string cond = genExpr(f->condition);
            emit("ifFalse", cond, "goto", lEnd);
            genBlock(f->body);
            genStmt(f->update);
            emit("goto", "", "", lStart);
            emit("LABEL", "", "", lEnd);
            break;
        }
        case NodeType::IO: {
            auto io = dynamic_pointer_cast<IONode>(stmt);
            if (io->isInput) emit("INPUT", "", "", io->id);
            else emit("OUTPUT", genExpr(io->expr), "", "");
            break;
        }
        case NodeType::RETURN: {
            auto r = dynamic_pointer_cast<ReturnNode>(stmt);
            emit("RETURN", genExpr(r->expr), "", "");
            break;
        }
        case NodeType::EXPR: {
            auto e = dynamic_pointer_cast<ExprStmtNode>(stmt);
            if (e) genExpr(e->expr);
            break;
        }
        default: break;
    }
}

void TACGenerator::genBlock(shared_ptr<BlockNode> block) {
    for (auto s : block->statements) genStmt(s);
}

void TACGenerator::gen(shared_ptr<ProgramNode> root) {
    for (auto d : root->declarations) {
        if (d->type == NodeType::FUNCTION) {
            auto f = dynamic_pointer_cast<FunctionNode>(d);
            emit("FUNC", "", "", f->id);
            genBlock(f->body);
            emit("ENDFUNC", "", "", f->id);
        } else {
            genStmt(dynamic_pointer_cast<StmtNode>(d));
        }
    }
}

void TACGenerator::printTAC() {
    cout << "\n--- THREE ADDRESS CODE ---\n";
    for (auto t : instructions) {
        if (t.op == "LABEL" || t.op == "FUNC" || t.op == "ENDFUNC") cout << t.op << " " << t.result << ":\n";
        else if (t.op == "=") cout << "\t" << t.result << " = " << t.arg1 << "\n";
        else if (t.op == "ifFalse") cout << "\t" << t.op << " " << t.arg1 << " " << t.arg2 << " " << t.result << "\n";
        else if (t.op == "goto") cout << "\t" << t.op << " " << t.result << "\n";
        else if (t.op == "INPUT" || t.op == "OUTPUT") cout << "\t" << t.op << " " << (t.op=="INPUT"?t.result:t.arg1) << "\n";
        else if (t.op == "PARAM") cout << "\tPARAM " << t.arg1 << "\n";
        else if (t.op == "CALL") cout << "\t" << t.result << " = CALL " << t.arg1 << ", " << t.arg2 << "\n";
        else if (t.op == "RETURN") cout << "\tRETURN " << t.arg1 << "\n";
        else if (t.op == "NOT" || t.op == "&&" || t.op == "||" || t.op == "++" || t.op == "--")
            cout << "\t" << t.result << " = " << t.op << " " << t.arg1 << (t.arg2.empty() ? "" : " " + t.arg2) << "\n";
        else cout << "\t" << t.result << " = " << t.arg1 << " " << t.op << " " << t.arg2 << "\n";
    }
}
