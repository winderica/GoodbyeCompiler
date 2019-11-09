#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "lexer.h"

namespace MiniC {
    class Interpreter {
    public:
        Interpreter();

        int parse();

        void clear();

        string toString() const;

        void switchInputStream(istream *is);

        friend class Parser;

        friend class Lexer;

    private:
        void addToken(const Token &token);

        void increaseLocationX(int x);

        void increaseLocationY(int y);

        MiniC::location &getLocation();

        Lexer lexer;
        Parser parser;
        vector<Token> tokens;
        MiniC::location loc;
    };
}

#endif // _INTERPRETER_H
