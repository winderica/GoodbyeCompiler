#include "token.h"
#include "color.h"

using namespace MiniC;

Token::Token(string kind, string value) : kind(move(kind)), value(move(value)), children(), loc(nullopt) {}

Token::Token(string kind) : kind(move(kind)), value(), children(), loc(nullopt) {}

Token::Token(string kind, string value, location loc) : kind(move(kind)), value(move(value)), children(), loc(loc) {}

Token::Token(string kind, location loc) : kind(move(kind)), value(), children(), loc(loc) {}

Token::Token() = default;

void Token::addChild(const Token &child) {
    children.push_back(child);
}

string Token::getKind() const {
    return kind;
}

void Token::setKind(const string &newKind) {
    kind = newKind;
}

string Token::getValue() const {
    return value;
}

vector<Token> Token::getChildren() const {
    return children;
}

Token Token::getChildren(int index) const {
    return children.at(index);
}

optional<location> Token::getLocation() const {
    if (loc == nullopt) {
        for (const auto& child : children) {
            auto childLoc = child.getLocation();
            if (childLoc != nullopt) {
                return childLoc;
            }
        }
        return nullopt;
    }
    return loc;
}

string Token::toString(const string &baseIndent) const {
    stringstream s;
    s << "kind: [[" << (kind.find("ERROR") == string::npos ? BLUE : RED) << kind << RESET_COLOR << "]]";
    if (!value.empty()) {
        s << ", value: [[";
        if (kind == "Type") {
            s << MAGENTA;
        } else if (kind == "Identifier") {
            s << CYAN;
        } else if (kind == "Operator") {
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

bool Token::operator==(const string &str) const {
    return kind == str;
}

bool Token::operator!=(const string &str) const {
    return kind != str;
}
