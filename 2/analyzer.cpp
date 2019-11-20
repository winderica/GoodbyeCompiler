#include <iomanip>
#include "analyzer.h"
#include "color.h"
#include "interpreter.h"

using namespace MiniC;

Symbol::Symbol(string name, string type, int scope)
        : name(move(name)), type(move(type)), scope(scope), parameters(), isFunction(false) {}

Symbol::Symbol(const Token &name, const Token &type, int scope)
        : name(name.getValue()), type(type.getValue()), scope(scope), parameters(), isFunction(false) {}

Symbol::Symbol(const Token &name, const Token &type, vector<string> parameters, int scope)
        : name(name.getValue()), type(type.getValue()), scope(scope), parameters(move(parameters)), isFunction(true) {}

int SymbolTable::insertSymbol(const Symbol &symbol) {
    if (table.find(symbol.name) != table.end()) {
        return 1;
    }
    stringstream ss;
    ss << BLUE << setw(10) << symbol.name
       << YELLOW << setw(3) << symbol.scope
       << GREEN << setw(7) << symbol.type
       << MAGENTA << (symbol.isFunction ? " function" : "");
    for (const auto &param: symbol.parameters) {
        ss << CYAN << setw(7) << param;
    }
    ss << RESET_COLOR << endl;
    cout << ss.str();
    table.insert({symbol.name, symbol});
    return 0;
}

optional<Symbol> SymbolTable::findSymbol(const string &name) {
    try {
        return table.at(name);
    } catch (...) {
        return nullopt;
    }
}

runtime_error SemanticAnalyzer::semanticError(const string &message, const Token &token) {
    return runtime_error(Interpreter::makeError(message, token.getLocation().value()));
}

optional<Token> SemanticAnalyzer::find(const Token &root, const string &name) {
    if (root == name) {
        return root;
    }
    for (const auto &child: root.getChildren()) {
        auto res = find(child, name);
        if (res != nullopt) {
            return res;
        }
    }
    return nullopt;
}

void SemanticAnalyzer::insert(const Token &name, const Token &type) {
    if (tables.back().insertSymbol(Symbol(name, type, tables.size()))) {
        driver.addError("Symbol " + name.getValue() + " has already been declared.", name);
    }
}

void SemanticAnalyzer::insert(const Token &name, const string &type) {
    if (tables.back().insertSymbol(Symbol(name.getValue(), type, tables.size()))) {
        driver.addError("Symbol " + name.getValue() + " has already been declared.", name);
    }
}

void SemanticAnalyzer::insert(const Token &name, const Token &type, const vector<Token> &parameters) {
    vector<string> paramTypes;
    for (const auto &param: parameters) {
        paramTypes.push_back(param.getChildren(0).getValue());
    }
    if (tables.back().insertSymbol(Symbol(name, type, paramTypes, tables.size()))) {
        driver.addError("Symbol " + name.getValue() + " has already been declared.", name);
    }
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
        throw semanticError("Variable " + token.getValue() + " is accessed before declaration.", token);
    } else if (token == "BinaryExpression" || token == "RelationalExpression") {
        auto lValue = token.getChildren(0);
        auto op = token.getChildren(1).getValue();
        auto rValue = token.getChildren(2);
        auto lType = checkType(lValue);
        auto rType = checkType(rValue);
        if (lType != rType) {
            throw semanticError("Left value and right value around " + op + " are not of the same type.", lValue);
        }
        return token == "RelationalExpression" ? "int" /*TODO: bool*/ : lType;
    } else if (token == "AssignmentExpression" || token == "CompoundAssignmentExpression") {
        auto lValue = token.getChildren(0);
        auto rValue = token.getChildren(2);
        auto lType = checkType(lValue);
        auto rType = checkType(rValue);
        if (lType != rType) {
            throw semanticError("Type " + rType + " is not assignable to type " + lType + ".", lValue);
        }
        return lType;
    } else if (token == "ParenthesisExpression") {
        return checkType(token.getChildren(0));
    } else if (token == "PrefixExpression") {
        auto type = checkType(token.getChildren(1));
        auto op = token.getChildren(0).getValue();
        if ((op == "++" || op == "--") && type != "int") {
            throw semanticError("Operand type of prefix increment/decrement expression must be integer.", token);
        }
        return type;
    } else if (token == "PostfixExpression") {
        auto type = checkType(token.getChildren(0));
        if (type != "int") {
            throw semanticError("Operand type of postfix increment/decrement expression must be integer.", token);
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
            throw semanticError(
                    "True value and false value of conditional expression should be of the same type.",
                    yValue
            );
        }
        return yType;
    } else if (token == "CallExpression") {
        auto function = token.getChildren(0);
        auto args = token.getChildren(1).getChildren();
        for (auto table = tables.rbegin(); table != tables.rend(); table++) {
            auto symbol = table->findSymbol(function.getValue());
            if (symbol != nullopt) {
                if (!symbol->isFunction) {
                    throw semanticError(function.getValue() + " is not callable.", function);
                }
                auto params = symbol.value().parameters;
                auto paramsCount = params.size();
                auto argsCount = args.size();
                if (paramsCount != argsCount) {
                    throw semanticError(
                            "Function " + function.getValue() + " requires "
                            + to_string(paramsCount) + " parameters, but "
                            + to_string(argsCount) + " is given.",
                            function
                    );
                }
                for (int i = 0; i < argsCount; i++) {
                    auto type = checkType(args[i]);
                    if (type != params[i]) {
                        throw semanticError(
                                "Function " + function.getValue() + " requires "
                                + params[i] + " as parameter, but "
                                + type + " is given.",
                                args[i]
                        );
                    }
                }
                return symbol.value().type;
            }
        }
        throw semanticError("Function " + function.getValue() + " is accessed before declaration.", token);
    } else if (token == "IndexExpression") {
        auto tokens = token.getChildren();
        auto arr = tokens[0];
        auto type = checkType(arr);
        for (auto index = tokens.begin() + 1; index != tokens.end(); index++) {
            if (checkType(index->getChildren(0)) == "int") {
                if (type.back() != '*') {
                    throw semanticError(type + " can not be indexed.", *index);
                }
                type.pop_back();
            } else {
                throw semanticError("Array index should be integer.", *index);
            }
        }
        return type;
    } else if (token == "IntegerLiteral") {
        return "int";
    } else if (token == "FloatLiteral") {
        return "float";
    } else if (token == "CharLiteral") {
        return "char";
    } else if (token == "ArrayLiteral") {
        string type;
        for (const auto &item: token.getChildren()) {
            auto newType = checkType(item);
            if (!type.empty() && newType != type) {
                throw semanticError("Item in array literal should be of the same type.", item);
            }
            type = newType;
        }
        return type + "*";
    } else {
        return "";
    }
}

SemanticAnalyzer::SemanticAnalyzer(Interpreter &driver) : driver(driver) {}

void SemanticAnalyzer::analyze(const Token &root) {
    auto value = root.getValue();
    auto children = root.getChildren();
    if (root == "GlobalVariableDeclaration" || root == "LocalVariableDeclaration") {
        auto type = children[0];
        for (const auto &decDef: children[1].getChildren()) {
            auto var = decDef.getChildren(0);
            try {
                auto lType = type.getValue();
                if (var == "Identifier") {
                    insert(var, type);
                } else if (var == "IndexExpression") {
                    lType += string(var.getChildren().size() - 1, '*');
                    insert(var.getChildren(0), lType);
                    checkType(var);
                }
                if (decDef == "VariableDefinition") {
                    auto rValue = decDef.getChildren(2);
                    auto rType = checkType(rValue);
                    if (lType != rType) {
                        throw semanticError("Type " + rType + " is not assignable to type " + lType + ".", rValue);
                    }
                }
            } catch (runtime_error &e) {
                driver.addError(e.what());
            }
        }
    } else if (root == "FunctionDefinition") {
        currentReturn.clear();
        auto parameters = children[2].getChildren();
        insert(children[1], children[0], parameters);
        tables.emplace_back();
        for (const auto &param: parameters) {
            insert(param.getChildren(1), param.getChildren(0));
        }
        for (const auto &child: children[3].getChildren()) {
            analyze(child);
        }
        auto returnType = children[0].getValue();
        if (currentReturn != returnType) {
            // Return type is checked after function rather than in function, in case there are no return statements in it.
            driver.addError(
                    "Return type of function " + children[1].getValue() + " is not consistent: " +
                    returnType + " required, " + (currentReturn.empty() ? "void" : currentReturn) + " given.",
                    root
            );
        }
        tables.pop_back();
    } else if (root == "BlockStatement" || root == "Program") {
        tables.emplace_back();
        for (const auto &child: children) {
            analyze(child);
        }
        tables.pop_back();
    } else if (root == "ReturnStatement" && currentReturn.empty() && !children.empty()) {
        try {
            currentReturn = checkType(children[0]);
        } catch (runtime_error &e) {
            driver.addError(e.what());
        }
    } else {
        optional<string> res = nullopt;
        try {
            res = checkType(root);
        } catch (runtime_error &e) {
            driver.addError(e.what());
        }
        if (res != nullopt && res.value().empty()) {
            if (root == "WhileStatement" || root == "DoWhileStatement" || root == "ForStatement") {
                loopDepth++;
            } else if (root == "SwitchStatement") {
                switchDepth++;
            } else if (root == "ContinueStatement" && !loopDepth) {
                driver.addError("Continue statement should be in loops.", root);
            } else if (root == "BreakStatement" && !loopDepth && !switchDepth) {
                driver.addError("Break statement should be in loops or switch-cases.", root);
            }
            for (const auto &child: children) {
                analyze(child);
            }
            if (root == "SwitchStatement") {
                switchDepth--;
            } else if (root == "WhileStatement" || root == "DoWhileStatement" || root == "ForStatement") {
                loopDepth--;
            }
        }
    }
}
