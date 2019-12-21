#include "symbol.h"

using namespace MiniC;

Symbol::Symbol(string name, string type, int scope, const vector<string>& parameters, bool isFunction)
        : name(move(name)), type(move(type)), scope(scope), parameters(parameters), isFunction(isFunction) {}

Symbol::Symbol(const Token &name, const Token &type, int scope)
        : name(name.getValue()), type(type.getValue()), scope(scope), parameters() {}

Symbol::Symbol(const Token &name, const Token &type, vector<string> parameters, int scope)
        : name(name.getValue()), type(type.getValue()), scope(scope), parameters(move(parameters)), isFunction(true) {}

Symbol::Symbol(const Token &name, const Token &type, vector<Token> lengths, int scope)
        : name(name.getValue()),
          type(type.getValue() + string(lengths.size(), '*')),
          scope(scope),
          lengths(move(lengths)),
          isArray(true) {}
