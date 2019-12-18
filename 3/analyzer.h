#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "symbolTable.h"
#include "quadruple.h"

namespace MiniC {
    class Interpreter;

    class SemanticAnalyzer {
    public:
        explicit SemanticAnalyzer(Interpreter &driver);

        void analyze(const Token &root);

        [[nodiscard]] vector<Quadruple> getQuadruples() const;

    private:
        static runtime_error semanticError(const string &message, const Token &token);

        string checkType(const Token &token);

        void insertVariable(const Token &name, const Token &type);

        void insertArray(const Token &type, const vector<Token> &items);

        void insertFunction(const Token &name, const Token &type, const vector<Token> &parameters);

        string generateAlias();

        string generateLabel();

        string generateTAC(const Token &root);

        static optional<Token> findToken(const Token &root, const string &name);

        vector<SymbolTable> tables;

        vector<Quadruple> quadruples;

        int currentAlias = 0;

        int currentLabel = 0;

        vector<string> currentBreakLabels;

        vector<string> currentContinueLabels;

        string currentReturnLabel;

        int loopDepth = 0;

        int switchDepth = 0;

        string currentReturnType;

        Interpreter &driver;
    };
}

#endif // _ANALYZER_H
