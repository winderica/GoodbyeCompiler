#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "lexer.h"
#include "analyzer.h"

namespace MiniC {
    class Interpreter {
    public:
        Interpreter();

        int parse();

        void clear();

        void analyze();

        string toString() const;

        void switchInputStream(istream *is);

        friend class Parser;

        friend class Lexer;

    private:
        void addToken(const Token &token);

        void increaseLocationX(int x);

        void increaseLocationY(int y);

        location &getLocation();

        Lexer lexer;
        Parser parser;
        SemanticAnalyzer analyzer;
        vector<Token> tokens;
        location loc;
    };
}

#endif // _INTERPRETER_H
