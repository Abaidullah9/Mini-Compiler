#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

using namespace std;

struct SymbolInfo {
    string name;
    string type;
    string kind; // "var", "func"
    int scopeLevel;
    int line;
    int memoryLoc;
    vector<string> paramTypes; // For functions
};

class SymbolTable {
private:
    vector<unordered_map<string, SymbolInfo>> stack;
    vector<SymbolInfo> history;
    int memCounter = 0;

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    bool insert(string name, string type, string kind, int line, vector<string> params = {});
    SymbolInfo* lookup(string name);
    SymbolInfo* lookupCurrentScope(string name);
    void printTable();
};

#endif
