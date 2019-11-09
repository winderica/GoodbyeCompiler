#include <sstream>
#include "token.h"
#include "color.h"

using namespace MiniC;

Token::Token(string name, string value) : name(move(name)), value(move(value)), children() {}

Token::Token(string name) : name(move(name)), value(), children() {}

Token::Token() : name(), value(), children() {}

Token::~Token() = default;

void Token::addChild(const Token &child) {
    children.push_back(child);
}

string Token::getName() const {
    return name;
}

string Token::getValue() const {
    return value;
}

string Token::toString(const string &baseIndent) const {
    stringstream s;
    s << "name: [[" << (name.find("ERROR") == string::npos ? BLUE : RED) << name << RESET_COLOR << "]]";
    if (!value.empty()) {
        s << ", value: [[";
        if (name == "Type") {
            s << MAGENTA;
        } else if (name == "Identifier") {
            s << CYAN;
        } else if (name == "Operator") {
            s << BOLD_YELLOW;
        } else {
            s << BOLD_GREEN;
        }
        s << value << RESET_COLOR << "]]";
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
