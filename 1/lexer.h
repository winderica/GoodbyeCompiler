#ifndef _LEXER_H
#define _LEXER_H


/**
 * Generated Flex class name is yyFlexLexer by default. If we want to use more flex-generated
 * classes we should name them differently. See scanner.l prefix option.
 *
 * Unfortunately the implementation relies on this trick with redefining class name
 * with a preprocessor macro. See GNU Flex manual, "Generating C++ Scanners" section
 */
#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer MiniC_FlexLexer

#include <FlexLexer.h>

#endif

// Scanner method signature is defined by this macro. Original yylex() returns int.
// Sinice Bison 3 uses symbol_type, we must change returned type. We also rename it
// to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL MiniC::Parser::symbol_type MiniC::Lexer::get_next_token()

#include "parser.hpp"

namespace MiniC {

// Forward declare interpreter to avoid include. Header is added inimplementation file.
    class Interpreter;

    class Lexer : public yyFlexLexer {
    public:
        explicit Lexer(Interpreter &driver) : driver(driver) {}

        ~Lexer() override = default;

        virtual MiniC::Parser::symbol_type get_next_token();

    private:
        Interpreter &driver;
    };

}

#endif