#include "analyzer.h"
#include "interpreter.h"

using namespace MiniC;

SemanticAnalyzer::SemanticAnalyzer(Interpreter &driver) : driver(driver) {}

runtime_error SemanticAnalyzer::semanticError(const string &message, const Token &token) {
    return runtime_error(Interpreter::makeError(message, token.getLocation().value()));
}

optional<Token> SemanticAnalyzer::findToken(const Token &root, const string &name) {
    if (root == name) {
        return root;
    }
    for (const auto &child: root.getChildren()) {
        auto res = findToken(child, name);
        if (res != nullopt) {
            return res;
        }
    }
    return nullopt;
}

void SemanticAnalyzer::insertVariable(const Token &name, const Token &type) {
    if (tables.back().insertSymbol(Symbol(name, type, tables.size()))) {
        driver.addError("Symbol " + name.getValue() + " has already been declared.", name);
    }
}

void SemanticAnalyzer::insertArray(const Token &type, const vector<Token> &items) {
    auto name = items[0];
    auto lengths = vector<Token>(items.begin() + 1, items.end());
    if (tables.back().insertSymbol(Symbol(name, type, lengths, tables.size()))) {
        driver.addError("Symbol " + name.getValue() + " has already been declared.", name);
    }
}

void SemanticAnalyzer::insertFunction(const Token &name, const Token &type, const vector<Token> &parameters) {
    vector<string> paramTypes;
    for (const auto &param: parameters) {
        paramTypes.push_back(param.getChildren(0).getValue());
    }
    if (tables.back().insertSymbol(Symbol(name, type, paramTypes, tables.size()))) {
        driver.addError("Symbol " + name.getValue() + " has already been declared.", name);
    }
}

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
                    insertVariable(var, type);
                } else if (var == "IndexExpression") {
                    lType += string(var.getChildren().size() - 1, '*');
                    insertArray(type, var.getChildren());
                    checkType(var);
                }
                if (decDef == "VariableDefinition") {
                    auto rValue = decDef.getChildren(2);
                    auto rType = checkType(rValue);
                    if (lType != rType) {
                        throw semanticError("Type " + rType + " is not assignable to type " + lType + ".", var);
                    }
                    if (var == "IndexExpression") {
                        // TODO: put into data section
                    } else {
                        auto left = generateTAC(decDef.getChildren(0));
                        auto right = generateTAC(decDef.getChildren(2));
                        quadruples.emplace_back("=", right, "", left);
                    }
                }
            } catch (runtime_error &e) {
                driver.addError(e.what());
            }
        }
    } else if (root == "FunctionDefinition") {
        currentReturnType.clear();
        quadruples.emplace_back("start", "", "", children[1].getValue());
        auto parameters = children[2].getChildren();
        insertFunction(children[1], children[0], parameters);
        tables.emplace_back();
        for (const auto &param: parameters) {
            insertVariable(param.getChildren(1), param.getChildren(0));
        }
        auto endLabel = generateLabel();
        currentReturnLabel = endLabel;
        for (const auto &child: children[3].getChildren()) {
            analyze(child);
            generateTAC(child);
        }
        quadruples.emplace_back("label", "", "", endLabel);
        quadruples.emplace_back("end", "", "", children[1].getValue());
        currentReturnLabel = "";
        auto returnType = children[0].getValue();
        if (currentReturnType != returnType) {
            // Return type is checked after function rather than in function, in case there are no return statements in it.
            driver.addError(
                    "Return type of function " + children[1].getValue() + " is not consistent: " +
                    returnType + " required, " + (currentReturnType.empty() ? "void" : currentReturnType) + " given.",
                    root
            );
        }
        tables.pop_back();
    } else if (root == "BlockStatement") {
        tables.emplace_back();
        for (const auto &child: children) {
            analyze(child);
            generateTAC(child);
        }
        tables.pop_back();
    } else if (root == "Program") {
        tables.emplace_back();
        for (const auto &child: children) {
            analyze(child);
        }
        tables.pop_back();
    } else if (root == "ReturnStatement" && currentReturnType.empty() && !children.empty()) {
        try {
            currentReturnType = checkType(children[0]);
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
            if (root == "ContinueStatement" && !loopDepth) {
                driver.addError("Continue statement should be in loops.", root);
            } else if (root == "BreakStatement" && !loopDepth && !switchDepth) {
                driver.addError("Break statement should be in loops or switch-cases.", root);
            }
            if (root == "IfStatement") {
                auto label = generateLabel();
                analyze(children[0]); // cond
                quadruples.emplace_back("jz", generateTAC(children[0]), "", label); // cond
                analyze(children[1]); // body
                if (children.size() == 3) {
                    auto endLabel = generateLabel();
                    quadruples.emplace_back("goto", "", "", endLabel);
                    quadruples.emplace_back("label", "", "", label);
                    analyze(children[2]); // else body
                    quadruples.emplace_back("label", "", "", endLabel);
                } else {
                    quadruples.emplace_back("label", "", "", label);
                }
            } else if (root == "WhileStatement") {
                auto startLabel = generateLabel();
                auto endLabel = generateLabel();
                currentContinueLabels.push_back(startLabel);
                currentBreakLabels.push_back(endLabel);
                quadruples.emplace_back("label", "", "", startLabel);
                analyze(children[0]); // cond
                quadruples.emplace_back("jz", generateTAC(children[0]), "", endLabel); // cond
                loopDepth++;
                analyze(children[1]); // body
                loopDepth--;
                quadruples.emplace_back("goto", "", "", startLabel);
                quadruples.emplace_back("label", "", "", endLabel);
                currentContinueLabels.pop_back();
                currentBreakLabels.pop_back();
            } else if (root == "DoWhileStatement") {
                auto labelIndex = quadruples.size();
                quadruples.emplace_back("label", "", "", "");
                loopDepth++;
                analyze(children[0]); // body
                loopDepth--;
                auto startLabel = generateLabel();
                auto endLabel = generateLabel();
                currentContinueLabels.push_back(startLabel);
                currentBreakLabels.push_back(endLabel);
                analyze(children[1]); // cond
                quadruples.emplace_back("jnz", generateTAC(children[1]), "", startLabel); // cond
                quadruples[labelIndex].res = startLabel;
                quadruples.emplace_back("label", "", "", endLabel);
                currentContinueLabels.pop_back();
                currentBreakLabels.pop_back();
            } else if (root == "ForStatement") {
                analyze(children[0]); // init
                generateTAC(children[0]); // init
                auto startLabel = generateLabel();
                auto endLabel = generateLabel();
                currentContinueLabels.push_back(startLabel);
                currentBreakLabels.push_back(endLabel);
                quadruples.emplace_back("label", "", "", startLabel);
                analyze(children[1]); // cond
                quadruples.emplace_back("jz", generateTAC(children[1]), "", endLabel); // cond
                loopDepth++;
                analyze(children[3]); // body
                loopDepth--;
                analyze(children[2]); // step
                generateTAC(children[2]); // step
                quadruples.emplace_back("goto", "", "", startLabel);
                quadruples.emplace_back("label", "", "", endLabel);
                currentContinueLabels.pop_back();
                currentBreakLabels.pop_back();
            } else if (root == "SwitchStatement") {
                switchDepth++;
                // TODO: switch-case
                for (const auto &child: children) {
                    analyze(child);
                }
                switchDepth--;
            } else {
                for (const auto &child: children) {
                    analyze(child);
                }
            }
        }
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

string SemanticAnalyzer::generateTAC(const Token &root) {
    auto children = root.getChildren();
    if (root == "ContinueStatement") {
        // TODO: user defined label
        quadruples.emplace_back("goto", "", "", currentContinueLabels.back());
    } else if (root == "BreakStatement") {
        quadruples.emplace_back("goto", "", "", currentBreakLabels.back());
    } else if (root == "ReturnStatement") {
        quadruples.emplace_back("ret", "", "", children.empty() ? "" : generateTAC(children[0]));
        quadruples.emplace_back("goto", "", "", currentReturnLabel);
    } else if (root == "AssignmentExpression") {
        auto left = generateTAC(children[0]);
        auto right = generateTAC(children[2]);
        quadruples.emplace_back("=", right, "", left);
    } else if (root == "CompoundAssignmentExpression") {
        auto left = generateTAC(children[0]);
        auto right = generateTAC(children[2]);
        auto op = children[1].getValue();
        quadruples.emplace_back(op.substr(0, op.length() - 1), right, left, left);
    } else if (root == "BinaryExpression" || root == "RelationalExpression") {
        auto alias = generateAlias();
        auto left = generateTAC(children[0]);
        auto right = generateTAC(children[2]);
        auto op = children[1].getValue();
        quadruples.emplace_back(op, left, right, alias);
        return alias;
    } else if (root == "ParenthesisExpression") {
        return generateTAC(children[0]);
    } else if (root == "PrefixExpression") {
        auto alias = generateAlias();
        quadruples.emplace_back(children[0].getValue(), generateTAC(children[1]), "", alias);
        return alias;
    } else if (root == "PostfixExpression") {
        auto alias = generateAlias();
        quadruples.emplace_back(children[1].getValue(), generateTAC(children[0]), "", alias);
        return alias;
    } else if (root == "TernaryExpression") {
        auto alias = generateAlias();
        auto falseLabel = generateLabel();
        auto endLabel = generateLabel();
        quadruples.emplace_back("jz", generateTAC(children[0]), "", falseLabel);
        quadruples.emplace_back("=", generateTAC(children[2]), "", alias);
        quadruples.emplace_back("goto", "", "", endLabel);
        quadruples.emplace_back("label", "", "", falseLabel);
        quadruples.emplace_back("=", generateTAC(children[4]), "", alias);
        quadruples.emplace_back("label", "", "", endLabel);
        return alias;
    } else if (root == "CallExpression") {
        auto alias = generateAlias();
        auto args = children[1].getChildren();
        for (auto arg = args.rbegin(); arg != args.rend(); arg++) {
            quadruples.emplace_back("push", generateTAC(*arg), "", "");
        }
        quadruples.emplace_back("call", children[0].getValue(), "", alias);
        for (const auto &arg: args) {
            quadruples.emplace_back("pop", "", "", "");
        }
        return alias;
    } else if (root == "IndexExpression") {
        auto tokens = root.getChildren();
        auto arr = tokens[0];
        vector<string> lengths;
        for (auto table = tables.rbegin(); table != tables.rend(); table++) {
            auto symbol = table->findSymbol(arr.getValue());
            if (symbol != nullopt) {
                for (const auto &length: symbol->lengths) {
                    lengths.push_back(generateTAC(length));
                }
            }
        }
        auto pointer = generateAlias();
        quadruples.emplace_back("=", generateTAC(arr), "", pointer);
        for (auto i = 0; i < lengths.size(); i++) {
            auto alias = generateAlias();
            quadruples.emplace_back("=", generateTAC(tokens[i + 1]), "", alias);
            for (auto j = i + 1; j < lengths.size(); j++) {
                quadruples.emplace_back("*", lengths[j], alias, alias);
            }
            quadruples.emplace_back("+", alias, pointer, pointer);
        }
        auto res = generateAlias();
        quadruples.emplace_back("ptr", pointer, "", res);
        return res;
    } else if (root == "Identifier") {
        for (auto table = tables.rbegin(); table != tables.rend(); table++) {
            auto symbol = table->findSymbol(root.getValue());
            if (symbol != nullopt) {
                return "@" + symbol.value().name + "$" + to_string(symbol.value().scope);
            }
        }
        return root.getValue();
    } else if (root == "IntegerLiteral" || root == "FloatLiteral" || root == "CharLiteral") {
        return root.getValue(); // TODO: array literal
    } else if (root == "Index") {
        if (!children.empty()) {
            return generateTAC(children[0]);
        }
    }
    return "";
}

string SemanticAnalyzer::generateAlias() {
    return "@T" + to_string(currentAlias++);
}

string SemanticAnalyzer::generateLabel() {
    return "#L" + to_string(currentLabel++);
}

vector<Quadruple> SemanticAnalyzer::getQuadruples() const {
    return quadruples;
}