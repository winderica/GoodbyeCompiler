#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <vector>

#include "lexer.h"
#include "token.h"
#include "parser.hpp"

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
        void addToken(const Token &cmd);

        void increaseLocationX(int x);

        void increaseLocationY(int y);

        MiniC::location &getLocation();

    private:
        Lexer lexer;
        Parser parser;
        vector<Token> tokens;
        MiniC::location loc;
    };

}

#endif // _INTERPRETER_H
