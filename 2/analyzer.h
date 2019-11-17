#ifndef _ANALYZER_H
#define _ANALYZER_H

#include <unordered_map>
#include "token.h"

namespace MiniC {

    class Symbol {
    public:
        Symbol(string name, string type, int scope);

        Symbol(const Token &name, const Token &type, int scope);

        string name;
        string type;
        int scope;
    };

    class SymbolTable {
    public:
        void insertSymbol(const Symbol &symbol, const location &loc);

        optional<Symbol> findSymbol(const string &name);

    private:
        unordered_map<string, Symbol> table;
    };

    class SemanticAnalyzer {
    public:
        void analyze(const Token &root);

    private:
        string checkType(const Token &token);

        void insert(const Token &name, const Token &type);

        static optional<Token> find(const Token &root, const string &name);

        vector<SymbolTable> tables;
    };
}

#endif // _ANALYZER_H
