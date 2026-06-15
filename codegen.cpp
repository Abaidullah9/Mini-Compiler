#include "codegen.h"
#include <iostream>

void CodeGenerator::generateAssembly(const vector<TAC>& tacList) {
    cout << "\n--- PSEUDO ASSEMBLY CODE ---\n";
    for (const auto& t : tacList) {
        if (t.op == "FUNC") cout << t.result << ":\n";
        else if (t.op == "LABEL") cout << t.result << ":\n";
        else if (t.op == "=") {
            cout << "\tMOV R1, " << t.arg1 << "\n";
            cout << "\tMOV " << t.result << ", R1\n";
        }
        else if (t.op == "+" || t.op == "-" || t.op == "*" || t.op == "/") {
            cout << "\tMOV R1, " << t.arg1 << "\n";
            cout << "\tMOV R2, " << t.arg2 << "\n";
            string instr = (t.op=="+")?"ADD":(t.op=="-")?"SUB":(t.op=="*")?"MUL":"DIV";
            cout << "\t" << instr << " R1, R2\n";
            cout << "\tMOV " << t.result << ", R1\n";
        }
        else if (t.op == "<" || t.op == ">" || t.op == "<=" || t.op == ">=" || t.op == "==" || t.op == "!=") {
            cout << "\tMOV R1, " << t.arg1 << "\n";
            cout << "\tMOV R2, " << t.arg2 << "\n";
            cout << "\tCMP R1, R2\n";
            string setInstr = (t.op == "<") ? "SETL" : (t.op == ">") ? "SETG" : (t.op == "<=") ? "SETLE" : (t.op == ">=") ? "SETGE" : (t.op == "==") ? "SETE" : "SETNE";
            cout << "\t" << setInstr << " " << t.result << "\n";
        }
        else if (t.op == "NOT") {
            cout << "\tCMP " << t.arg1 << ", 0\n";
            cout << "\tSETE " << t.result << "\n";
        }
        else if (t.op == "&&" || t.op == "||") {
            cout << "\tMOV R1, " << t.arg1 << "\n";
            cout << "\tMOV R2, " << t.arg2 << "\n";
            cout << "\t" << (t.op == "&&" ? "AND" : "OR") << " R1, R2\n";
            cout << "\tMOV " << t.result << ", R1\n";
        }
        else if (t.op == "++" || t.op == "--") {
            cout << "\tMOV R1, " << t.arg1 << "\n";
            cout << "\t" << (t.op == "++" ? "INC" : "DEC") << " R1\n";
            cout << "\tMOV " << t.result << ", R1\n";
        }
        else if (t.op == "PARAM") cout << "\tPUSH " << t.arg1 << "\n";
        else if (t.op == "CALL") {
            cout << "\tCALL " << t.arg1 << ", " << t.arg2 << "\n";
            cout << "\tMOV " << t.result << ", R0\n";
        }
        else if (t.op == "ifFalse") {
            cout << "\tCMP " << t.arg1 << ", 0\n";
            cout << "\tJE " << t.result << "\n";
        }
        else if (t.op == "goto") cout << "\tJMP " << t.result << "\n";
        else if (t.op == "OUTPUT") cout << "\tPRINT " << t.arg1 << "\n";
        else if (t.op == "RETURN") {
            cout << "\tMOV R0, " << t.arg1 << "\n";
            cout << "\tRET\n";
        }
    }
}
