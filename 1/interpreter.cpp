#include "interpreter.h"
#include "token.h"

#include <sstream>

using namespace MiniC;

Interpreter::Interpreter() : tokens(), lexer(*this), parser(lexer, *this), loc(location()) {}

int Interpreter::parse() {
    loc.initialize();
    return parser.parse();
}

void Interpreter::clear() {
    loc.initialize();
    tokens.clear();
}

string Interpreter::toString() const {
    stringstream s;
    string divider = string(100, '=');
    s << divider << endl << "AST: " << endl << divider << endl;
    for (const auto &token : tokens) {
        s << token.toString() << endl;
    }
    return s.str();
}

void Interpreter::switchInputStream(istream *is) {
    loc.initialize();
    lexer.switch_streams(is, nullptr);
    tokens.clear();
}

void Interpreter::addToken(const Token &cmd) {
    tokens.push_back(cmd);
}

void Interpreter::increaseLocationX(int x) {
    loc.columns(x);
    loc.step();
}

void Interpreter::increaseLocationY(int y) {
    loc.lines(y);
    loc.step();
}

MiniC::location &Interpreter::getLocation() {
    return loc;
}
