#include <iomanip>
#include "quadruple.h"
#include "symbolTable.h"
#include "color.h"

using namespace MiniC;

Quadruple::Quadruple(string op, string arg1, string arg2, string res)
        : op(move(op)), arg1(move(arg1)), arg2(move(arg2)), res(move(res)) {}

string Quadruple::toString() {
    stringstream ss;
    ss << "(" << BLUE << setw(5) << op << RESET_COLOR << ", "
       << CYAN << setw(5) << (arg1.empty() ? "__" : arg1) << RESET_COLOR << ", "
       << CYAN << setw(5) << (arg2.empty() ? "__" : arg2) << RESET_COLOR << ", "
       << MAGENTA << setw(5) << (res.empty() ? "__" : res) << RESET_COLOR << ")";
    return ss.str();
}