#ifndef _LEXER_H
#define _LEXER_H

#include "parser.hpp"

#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer MiniC_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL MiniC::Parser::symbol_type MiniC::Lexer::get_next_token()

namespace MiniC {
    class Lexer : public yyFlexLexer {
    public:
        explicit Lexer(Interpreter &driver) : driver(driver) {}

        ~Lexer() override = default;

        virtual MiniC::Parser::symbol_type get_next_token();

    private:
        Interpreter &driver;
    };
}

#endif // _LEXER_H