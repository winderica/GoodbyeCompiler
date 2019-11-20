#include "interpreter.h"
#include "color.h"

using namespace MiniC;

Interpreter::Interpreter() : tokens(), lexer(*this), parser(lexer, *this), analyzer(*this), loc(location()) {}

int Interpreter::parse() {
    errors.str("");
    loc.initialize();
    auto res = parser.parse();
    string divider = string(100, '=');
    cout << divider << endl << "Errors: " << endl << divider << endl << errors.str();
    return res;
}

void Interpreter::clear() {
    loc.initialize();
    tokens.clear();
}

void Interpreter::analyze() {
    errors.str("");
    string divider = string(100, '=');
    cout << divider << endl << "Symbol table: " << endl << divider << endl;
    for (const auto &token: tokens) {
        analyzer.analyze(token);
        cout << divider << endl << "Errors: " << endl << divider << endl << errors.str();
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

string Interpreter::makeError(const string &message, const location &location) {
    stringstream ss;
    ss << RED << "Error: " << message << endl
       << YELLOW << "Location: " << location << RESET_COLOR << endl;
    return ss.str();
}

void Interpreter::addError(const string &message, const Token &token) {
    addError(message, token.getLocation().value());
}

void Interpreter::addError(const string &message, const location &location) {
    errors << RED << "Error: " << message << endl
           << YELLOW << "Location: " << location << RESET_COLOR << endl;
}

void Interpreter::addError(const string &message) {
    errors << message;
}