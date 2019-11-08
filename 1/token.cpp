#include "token.h"
#include "color.h"

#include <sstream>

using namespace MiniC;

Token::Token(string name, string value) : name(move(name)), value(move(value)), children() {}

Token::Token(string name) : name(move(name)), value(), children() {}

Token::Token() : name(), value(), children() {}

Token::~Token() = default;

void Token::addChild(const Token &child) {
    children.push_back(child);
}

string Token::toString(const string &baseIndent) const {
    stringstream s;
    s << "name: [[" << (name.find("ERROR") == string::npos ? BLUE : RED) << name << RESET_COLOR << "]]";
    if (!value.empty()) {
        s << ", value: [[" << GREEN << value << RESET_COLOR << "]]";
    }
    if (!children.empty()) {
        string tab = "    ";
        s << ", child: [" << endl;
        for (const auto &child: children) {
            s << tab << baseIndent << child.toString(baseIndent + tab) << endl;
        }
        s << baseIndent << "]";
    }
    return s.str();
}

string Token::getName() const {
    return name;
}