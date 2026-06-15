#include "optimizer.h"
#include <iostream>
#include <queue>

namespace {
bool isNumber(const string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit(static_cast<unsigned char>(c)) && c != '.') return false;
    return true;
}

bool isBoolLiteral(const string& s) {
    return s == "0" || s == "1" || s == "true" || s == "false";
}

int boolValue(const string& s) {
    return (s == "1" || s == "true") ? 1 : 0;
}

int findLabelIndex(const vector<TAC>& tacList, const string& label) {
    for (size_t i = 0; i < tacList.size(); ++i) {
        if ((tacList[i].op == "LABEL" || tacList[i].op == "FUNC") && tacList[i].result == label) return static_cast<int>(i);
    }
    return -1;
}

bool isTerminator(const TAC& t) {
    return t.op == "goto" || t.op == "RETURN" || t.op == "ENDFUNC";
}
}

void Optimizer::optimize(vector<TAC>& tacList) {
    map<string, string> constants;
    for (auto& t : tacList) {
        if (constants.count(t.arg1)) t.arg1 = constants[t.arg1];
        if (constants.count(t.arg2)) t.arg2 = constants[t.arg2];

        if (t.op == "+" || t.op == "-" || t.op == "*" || t.op == "/") {
            if (isNumber(t.arg1) && isNumber(t.arg2)) {
                int v1 = stoi(t.arg1), v2 = stoi(t.arg2), res = 0;
                if (t.op == "+") res = v1 + v2;
                if (t.op == "-") res = v1 - v2;
                if (t.op == "*") res = v1 * v2;
                if (t.op == "/") res = (v2 != 0) ? v1 / v2 : 0;
                t.op = "=";
                t.arg1 = to_string(res);
                t.arg2 = "";
            } else if (t.op == "+" && t.arg2 == "0") {
                t.op = "=";
                t.arg2 = "";
            } else if (t.op == "*" && t.arg2 == "1") {
                t.op = "=";
                t.arg2 = "";
            }
        } else if ((t.op == "<" || t.op == ">" || t.op == "<=" || t.op == ">=" || t.op == "==" || t.op == "!=") && isNumber(t.arg1) && isNumber(t.arg2)) {
            int v1 = stoi(t.arg1), v2 = stoi(t.arg2), res = 0;
            if (t.op == "<") res = v1 < v2;
            if (t.op == ">") res = v1 > v2;
            if (t.op == "<=") res = v1 <= v2;
            if (t.op == ">=") res = v1 >= v2;
            if (t.op == "==") res = v1 == v2;
            if (t.op == "!=") res = v1 != v2;
            t.op = "=";
            t.arg1 = to_string(res);
            t.arg2 = "";
        } else if ((t.op == "&&" || t.op == "||") && isBoolLiteral(t.arg1) && isBoolLiteral(t.arg2)) {
            int v1 = boolValue(t.arg1), v2 = boolValue(t.arg2), res = 0;
            if (t.op == "&&") res = v1 && v2;
            if (t.op == "||") res = v1 || v2;
            t.op = "=";
            t.arg1 = to_string(res);
            t.arg2 = "";
        }

        if (t.op == "=" && isNumber(t.arg1)) constants[t.result] = t.arg1;
        else if (t.op == "=") constants.erase(t.result);
    }

    vector<bool> reachable(tacList.size(), false);
    queue<int> work;
    bool hasFunction = false;
    for (size_t i = 0; i < tacList.size(); ++i) {
        if (tacList[i].op == "FUNC") {
            reachable[i] = true;
            work.push(static_cast<int>(i));
            hasFunction = true;
        }
    }
    if (!hasFunction && !tacList.empty()) {
        reachable[0] = true;
        work.push(0);
    }

    auto mark = [&](int index) {
        if (index >= 0 && index < static_cast<int>(tacList.size()) && !reachable[index]) {
            reachable[index] = true;
            work.push(index);
        }
    };

    while (!work.empty()) {
        int index = work.front();
        work.pop();
        const TAC& t = tacList[index];

        if (t.op == "goto") {
            mark(findLabelIndex(tacList, t.result));
            continue;
        }

        if (t.op == "ifFalse") {
            if (isNumber(t.arg1)) {
                if (t.arg1 == "0") mark(findLabelIndex(tacList, t.result));
                else mark(index + 1);
            } else {
                mark(index + 1);
                mark(findLabelIndex(tacList, t.result));
            }
            continue;
        }

        if (isTerminator(t)) continue;
        mark(index + 1);
    }

    vector<TAC> filtered;
    filtered.reserve(tacList.size());
    for (size_t i = 0; i < tacList.size(); ++i) {
        const TAC& t = tacList[i];
        if (reachable[i] || t.op == "LABEL" || t.op == "FUNC" || t.op == "ENDFUNC") {
            filtered.push_back(t);
        }
    }

    tacList.swap(filtered);
}
