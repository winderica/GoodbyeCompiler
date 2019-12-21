#include "mips.h"

using namespace MiniC;

string MIPS::replaceIllegal(const string &str) {
    string res = str;
    replace(res.begin(), res.end(), '@', '_');
    replace(res.begin(), res.end(), '$', '_');
    replace(res.begin(), res.end(), '#', '_');
    return res;
}

string MIPS::getOffset(const string &identifier) {
    auto offset = offsets[identifier];
    if (offset.second) { // global or arr
        return replaceIllegal(identifier);
    } else { // local
        return to_string(offset.first * 4) + "($fp)";
    }
}

bool MIPS::isLiteral(const string &str) {
    return str[0] != '@';
}

string MIPS::load(const string &from, const string &to) {
    if (isLiteral(from)) {
        return "li " + to + ", " + from;
    } else if (offsets[from].second == 2) {
        return "la " + to + ", " + getOffset(from);
    }
    return "lw " + to + ", " + getOffset(from);
}

string MIPS::save(const string &res, const string &from) {
    if (!isLiteral(res) && res.find('$') == string::npos) {
        code << "sub $sp, $sp, 4" << endl;
        localOffset--;
        offsets[res] = {localOffset, 0};
    }
    return "sw " + from + ", " + getOffset(res);
}

void MIPS::generateData(const vector<Quadruple> &quadruples) {
    data << ".data" << endl;
    for (const auto &quadruple: quadruples) {
        auto op = quadruple.op;
        auto arg1 = quadruple.arg1;
        auto arg2 = quadruple.arg2;
        auto res = quadruple.res;
        if (op == "glb") {
            offsets[res] = {dataOffset, 1};
            dataOffset++;
            data << replaceIllegal(res) << ": .word 0" << endl;
        } else if (op == "arr") {
            offsets[res] = {dataOffset, 2};
            dataOffset += stoi(arg1);
            if (arg2[0] == '"') {
                data << replaceIllegal(res) << ": .ascii " << arg2 << endl;
            }
        }
    }
}

void MIPS::generateText(const vector<Quadruple> &quadruples) {
    code << ".text" << endl
         << ".globl main" << endl;
    bool returned = false;
    for (const auto &quadruple: quadruples) {
        auto op = quadruple.op;
        auto arg1 = quadruple.arg1;
        auto arg2 = quadruple.arg2;
        auto res = quadruple.res;
        if (instructions.find(op) != instructions.end()) {
            code << load(arg1, "$t1") << endl
                 << load(arg2, "$t2") << endl
                 << instructions.at(op) << " $t0, $t1, $t2" << endl
                 << save(res, "$t0") << endl;
        } else if (op == "~") {
            code << load(arg1, "$t1") << endl
                 << "not $t0, $t1" << endl
                 << save(res, "$t0") << endl;
        } else if (op == "=") {
            code << load(arg1, "$t0") << endl
                 << save(res, "$t0") << endl;
        } else if (op == "jz") {
            code << load(arg1, "$t0") << endl
                 << "beq $t0, $zero, " << replaceIllegal(res) << endl;
        } else if (op == "jnz") {
            code << load(arg1, "$t0") << endl
                 << "bne $t0, $zero, " << replaceIllegal(res) << endl;
        } else if (op == "start") {
            localOffset = 0;
            argOffset = 1;
            returned = false;
            code << res << ":" << endl
                 << "sub $sp, $sp, 8" << endl
                 << "sw $ra, 4($sp)" << endl
                 << "sw $fp, 0($sp)" << endl
                 << "move $fp, $sp" << endl;
        } else if (op == "label") {
            code << replaceIllegal(res) << ":" << endl;
        } else if (op == "goto") {
            code << "j " << replaceIllegal(res) << endl;
        } else if (op == "addr") {
            code << "la $t0, " << getOffset(arg1) << endl
                 << save(res, "$t0") << endl;
        } else if (op == "ptr") {
            code << "lw $t1, " << getOffset(arg1) << endl
                 << "lw $t0, 0($t1)" << endl
                 << save(res, "$t0") << endl;
        } else if (op == "ret") {
            returned = true;
            if (!res.empty()) {
                code << load(res, "$v0") << endl;
            }
            code << "move $sp, $fp" << endl
                 << "lw $fp, 0($sp)" << endl
                 << "lw $ra, 4($sp)" << endl
                 << "addi $sp, $sp, 8" << endl
                 << "jr $ra" << endl;
        } else if (op == "end" && !returned) {
            code << "move $sp, $fp" << endl
                 << "lw $fp, 0($sp)" << endl
                 << "lw $ra, 4($sp)" << endl
                 << "addi $sp, $sp, 8" << endl
                 << "jr $ra" << endl;
        } else if (op == "push") {
            code << load(arg1, "$t1") << endl
                 << "sub $sp, $sp, 4" << endl
                 << "sw $t1, 0($sp)" << endl;
        } else if (op == "pop") {
            code << "addi $sp, $sp, 4" << endl;
        } else if (op == "call") {
            if (arg1 == "print_int") {
                code << "li $v0, 1" << endl
                     << "lw $a0, 0($sp)" << endl
                     << "syscall" << endl
                     << "li $v0, 11" << endl
                     << "li $a0, 10" << endl
                     << "syscall" << endl;
            } else if (arg1 == "read_int") {
                code << "li $v0, 5" << endl
                     << "syscall" << endl
                     << save(res, "$v0") << endl;
            } else if (arg1 == "print_string") {
                code << "li $v0, 4" << endl
                     << "lw $a0, 0($sp)" << endl
                     << "syscall" << endl;
            } else {
                code << "jal " << arg1 << endl
                     << save(res, "$v0") << endl;
            }
        } else if (op == "loc") {
            code << "sub $sp, $sp, 4" << endl;
            localOffset--;
            offsets[res] = {localOffset, 0};
        } else if (op == "arg") {
            argOffset++;
            offsets[res] = {argOffset, 0};
        }
    }
}

string MIPS::compile(const vector<Quadruple> &quadruples) {
    generateData(quadruples);
    generateText(quadruples);
    return data.str() + code.str();
}