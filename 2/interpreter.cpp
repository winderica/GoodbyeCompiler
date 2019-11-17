#include "interpreter.h"

using namespace MiniC;

Interpreter::Interpreter() : tokens(), lexer(*this), parser(lexer, *this), analyzer(), loc(location()) {}

int Interpreter::parse() {
    loc.initialize();
    return parser.parse();
}

void Interpreter::clear() {
    loc.initialize();
    tokens.clear();
}

void Interpreter::analyze() {
    for (const auto &token: tokens) {
        analyzer.analyze(token);
    }
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

void Interpreter::addToken(const Token &token) {
    tokens.push_back(token);
}

void Interpreter::increaseLocationX(int x) {
    loc.step();
    loc.columns(x);
}

void Interpreter::increaseLocationY(int y) {
    loc.lines(y);
    loc.step();
}

location &Interpreter::getLocation() {
    return loc;
}
