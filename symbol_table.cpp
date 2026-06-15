#include "symbol_table.h"
#include <iomanip>

SymbolTable::SymbolTable() { enterScope(); } // Global scope

void SymbolTable::enterScope() {
    stack.push_back(unordered_map<string, SymbolInfo>());
}

void SymbolTable::exitScope() {
    if (!stack.empty()) stack.pop_back();
}

bool SymbolTable::insert(string name, string type, string kind, int line, vector<string> params) {
    if (stack.back().find(name) != stack.back().end()) return false;
    
    SymbolInfo sym = {name, type, kind, (int)stack.size()-1, line, memCounter, params};
    if (kind == "var") memCounter += 4;
    stack.back()[name] = sym;
    history.push_back(sym);
    return true;
}

SymbolInfo* SymbolTable::lookup(string name) {
    for (int i = stack.size() - 1; i >= 0; --i) {
        if (stack[i].find(name) != stack[i].end()) return &stack[i][name];
    }
    return nullptr;
}

SymbolInfo* SymbolTable::lookupCurrentScope(string name) {
    if (stack.back().find(name) != stack.back().end()) return &stack.back()[name];
    return nullptr;
}

void SymbolTable::printTable() {
    cout << "\n[STAGE 3] SYMBOL TABLE\n";
    cout << left << setw(22) << "Name" << setw(14) << "Type" << setw(14) << "Kind"
         << setw(14) << "Scope Level" << setw(16) << "Memory Location" << "Params\n";
    cout << string(86, '-') << "\n";
    for (const auto& val : history) {
        cout << left << setw(22) << val.name << setw(14) << val.type << setw(14) << val.kind
             << setw(14) << val.scopeLevel << setw(16) << val.memoryLoc;
        if (!val.paramTypes.empty()) {
            for (size_t i = 0; i < val.paramTypes.size(); ++i) {
                cout << val.paramTypes[i];
                if (i + 1 < val.paramTypes.size()) cout << ", ";
            }
        } else {
            cout << "-";
        }
        cout << "\n";
    }
    cout << string(86, '-') << "\n";
}
