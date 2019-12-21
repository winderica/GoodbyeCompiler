#ifndef _SYMBOL_H
#define _SYMBOL_H

#include "token.h"

namespace MiniC {

    class Symbol {
    public:
        Symbol(string name, string type, int scope, const vector<string> &parameters = {}, bool isFunction = false);

        Symbol(const Token &name, const Token &type, int scope);

        Symbol(const Token &name, const Token &type, vector<string> parameters, int scope);

        Symbol(const Token &name, const Token &type, vector<Token> lengths, int scope);

        string name;
        string type;
        vector<string> parameters;
        vector<Token> lengths;
        int scope;
        bool isFunction = false;
        bool isArray = false;
    };

}

#endif //_SYMBOL_H
