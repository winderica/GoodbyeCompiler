#ifndef _ANALYZER_H
#define _ANALYZER_H

#include <unordered_map>
#include "token.h"

namespace MiniC {
    class Interpreter;

    class Symbol {
    public:
        Symbol(string name, string type, int scope);

        Symbol(const Token &name, const Token &type, int scope);

        Symbol(const Token &name, const Token &type, vector<string> parameters, int scope);

        string name;
        string type;
        vector<string> parameters;
        int scope;
        bool isFunction;
    };

    class SymbolTable {
    public:
        int insertSymbol(const Symbol &symbol);

        optional<Symbol> findSymbol(const string &name);

    private:
        unordered_map<string, Symbol> table;
    };

    class SemanticAnalyzer {
    public:
        explicit SemanticAnalyzer(Interpreter &driver);

        void analyze(const Token &root);

    private:
        static runtime_error semanticError(const string &message, const Token &token);

        string checkType(const Token &token);

        void insert(const Token &name, const Token &type);

        void insert(const Token &name, const string &type);

        void insert(const Token &name, const Token &type, const vector<Token> &arguments);

        static optional<Token> find(const Token &root, const string &name);

        vector<SymbolTable> tables;

        int loopDepth = 0;

        int switchDepth = 0;

        string currentReturn;

        Interpreter &driver;
    };
}

#endif // _ANALYZER_H
