#include "analyzer.h"
#include "color.h"

using namespace MiniC;

Symbol::Symbol(string name, string type, int scope)
        : name(move(name)), type(move(type)), scope(scope) {}

Symbol::Symbol(const Token &name, const Token &type, int scope)
        : name(name.getValue()), type(type.getValue()), scope(scope) {}

void SymbolTable::insertSymbol(const Symbol &symbol, const location &loc) {
    if (table.find(symbol.name) != table.end()) {
        cout << RED << "Error: Symbol " << symbol.name << " has already been declared." << endl
             << YELLOW << "Location: " << loc << RESET_COLOR << endl;
    } else {
        table.insert({symbol.name, symbol});
    }
}

optional<Symbol> SymbolTable::findSymbol(const string &name) {
    try {
        return table.at(name);
    } catch (...) {
        return nullopt;
    }
}

optional<Token> SemanticAnalyzer::find(const Token &root, const string &name) {
    if (root == name) {
        return root;
    }
    for (const auto &child: root.getChildren()) {
        auto res = find(child, name);
        if (!res) {
            return res;
        }
    }
    return nullopt;
}

void SemanticAnalyzer::insert(const MiniC::Token &name, const MiniC::Token &type) {
    tables.back().insertSymbol(Symbol(name, type, tables.size()), name.getLocation().value());
}

string SemanticAnalyzer::checkType(const Token &token) {
    stringstream ss;
    if (token == "Identifier") {
        for (auto table = tables.rbegin(); table != tables.rend(); table++) {
            auto symbol = table->findSymbol(token.getValue());
            if (symbol != nullopt) {
                return symbol.value().type;
            }
        }
        ss << RED << "Error: Symbol " << token.getValue() << " is accessed before declaration." << endl
           << YELLOW << "Location: " << token.getLocation().value() << RESET_COLOR << endl;
        throw runtime_error(ss.str());
    } else if (token == "BinaryExpression" || token == "RelationalExpression") {
        auto lValue = token.getChildren(0);
        auto rValue = token.getChildren(2);
        auto lType = checkType(lValue);
        auto rType = checkType(rValue);
        if (lType != rType) {
            ss << RED << "Error: Left value and right value around " << token.getChildren(1).getValue() << " are not of the same type." << endl
                 << YELLOW << "Location: " << lValue.getLocation().value() << RESET_COLOR << endl;
            throw runtime_error(ss.str());
        }
        return lType;
    } else if (token == "AssignmentExpression" || token == "CompoundAssignmentExpression") {
        auto lValue = token.getChildren(0);
        auto rValue = token.getChildren(2);
        auto lType = checkType(lValue);
        auto rType = checkType(rValue);
        if (lType != rType) {
            ss << RED << "Error: Type " << rType << " is not assignable to type " << lType << endl
                 << YELLOW << "Location: " << lValue.getLocation().value().begin << RESET_COLOR << endl;
            throw runtime_error(ss.str());
        }
        return lType;
    } else if (token == "ParenthesisExpression") {
        return checkType(token.getChildren(0));
    } else if (token == "PrefixExpression") {
        auto type = checkType(token.getChildren(1));
        auto op = token.getChildren(0).getValue();
        if ((op == "++" || op == "--") && type != "int") {
            ss << RED << "Error: Operand type of prefix increment/decrement expression must be integer." << endl
               << YELLOW << "Location: " << token.getLocation().value() << RESET_COLOR << endl;
            throw runtime_error(ss.str());
        }
        return type;
    } else if (token == "PostfixExpression") {
        auto type = checkType(token.getChildren(0));
        if (type != "int") {
            ss << RED << "Error: Operand type of postfix increment/decrement expression must be integer." << endl
               << YELLOW << "Location: " << token.getLocation().value() << RESET_COLOR << endl;
            throw runtime_error(ss.str());
        }
        return type;
    } else if (token == "TernaryExpression") {
        auto cValue = token.getChildren(0);
        auto yValue = token.getChildren(2);
        auto nValue = token.getChildren(4);
        auto cType = checkType(cValue);
        auto yType = checkType(yValue);
        auto nType = checkType(nValue);
        if (yType != nType) {
            ss << RED << "Error: True value and false value around : are not of the same type." << endl
               << YELLOW << "Location: " << yValue.getLocation().value() << RESET_COLOR << endl;
            throw runtime_error(ss.str());
        }
        return yType;
    } else if (token == "IntegerLiteral") {
        return "int";
    } else if (token == "FloatLiteral") {
        return "float";
    } else if (token == "CharLiteral") {
        return "char";
    } else {
        return "";
    }
}
// TODO: control flow checking, array index && function call checking
void SemanticAnalyzer::analyze(const Token &root) {
    auto value = root.getValue();
    auto children = root.getChildren();
    if (root == "GlobalVariableDeclaration" || root == "LocalVariableDeclaration") {
        for (const auto &vars: children[1].getChildren()) {
            insert(find(vars.getChildren(0), "Identifier").value(), children[0]);
        }
    } else if (root == "FunctionDefinition") {
        insert(children[1], children[0]);
        tables.emplace_back();
        for (const auto &params: children[2].getChildren()) {
            insert(params.getChildren(1), params.getChildren(0));
        }
        for (const auto &child: children[3].getChildren()) {
            analyze(child);
        }
        tables.pop_back();
    } else if (root == "BlockStatement" || root == "Program") {
        tables.emplace_back();
        for (const auto &child: children) {
            analyze(child);
        }
        tables.pop_back();
    } else {
        optional<string> res = nullopt;
        try {
            res = checkType(root);
        } catch (runtime_error &e) {
            cout << e.what();
        }
        if (res != nullopt && res.value().empty()) {
            for (const auto &child: children) {
                analyze(child);
            }
        }
    }
}