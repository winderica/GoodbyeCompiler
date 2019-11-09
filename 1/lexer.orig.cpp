// HEAD
#include "interpreter.h"
#include "color.h"

#define YY_USER_ACTION driver.increaseLocationX(yyleng);

// BODY
int test() {
    /// %{
    auto loc = driver.getLocation();
    /// %}
    /// "/*"
    BEGIN(COMMENT_BLOCK);
    // cout << "COMMENT_BLOCK" << endl;
    ///
    /// <COMMENT_BLOCK>"*/"
    BEGIN(INITIAL);
    // cout << "COMMENT_BLOCK_END" << endl;
    ///
    /// <COMMENT_BLOCK>\n
    ///
    /// <COMMENT_BLOCK>.
    ///
    /// <COMMENT_BLOCK><<EOF>>
    // cout << "Unterminated comment" << endl;
    ///
    /// \/\/.*\n
    // cout << "COMMENT_INLINE" << endl;
    ///
    /// [0-9]+[lLuU]?
    // cout << "LITERAL_INTEGER" << endl;
    // return MiniC::Parser::make_LITERAL_INTEGER(yytext, loc);
    ///
    /// ([0-9]*\.[0-9]+)|([0-9]+\.)[lLuU]?
    // cout << "LITERAL_FLOAT" << endl;
    // return MiniC::Parser::make_LITERAL_FLOAT(yytext, loc);
    ///
    /// '([^'\\\n]|\\.){0,1}'
    // cout << "LITERAL_CHAR" << endl;
    // return MiniC::Parser::make_LITERAL_CHAR(yytext, loc);
    ///
    /// \"(\\.|[^"\\])*\"
    // cout << "LITERAL_STRING" << endl;
    // return MiniC::Parser::make_LITERAL_STRING(yytext, loc);
    ///
    /// "int"|"float"|"long"|"char"|"double"|"void"|"short"
    // cout << "KEYWORD_TYPE" << endl;
    // return MiniC::Parser::make_KEYWORD_TYPE(yytext, loc);
    ///
    /// "return"
    // cout << "KEYWORD_RETURN" << endl;
    // return MiniC::Parser::make_KEYWORD_RETURN(loc);
    ///
    /// "break"
    // cout << "KEYWORD_BREAK" << endl;
    // return MiniC::Parser::make_KEYWORD_BREAK(loc);
    ///
    /// "continue"
    // cout << "KEYWORD_CONTINUE" << endl;
    // return MiniC::Parser::make_KEYWORD_CONTINUE(loc);
    ///
    /// "for"
    // cout << "KEYWORD_FOR" << endl;
    // return MiniC::Parser::make_KEYWORD_FOR(loc);
    ///
    /// "if"
    // cout << "KEYWORD_IF" << endl;
    // return MiniC::Parser::make_KEYWORD_IF(loc);
    ///
    /// "else"
    // cout << "KEYWORD_ELSE" << endl;
    // return MiniC::Parser::make_KEYWORD_ELSE(loc);
    ///
    /// "while"
    // cout << "KEYWORD_WHILE" << endl;
    // return MiniC::Parser::make_KEYWORD_WHILE(loc);
    ///
    /// [A-Za-z_][A-Za-z0-9_]*
    // cout << "IDENTIFIER" << endl;
    // return MiniC::Parser::make_IDENTIFIER(yytext, loc);
    ///
    /// "+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>="
    // cout << "OPERATOR_COMPOUND_ASSIGNMENT" << endl;
    // return MiniC::Parser::make_OPERATOR_COMPOUND_ASSIGNMENT(yytext, loc);
    ///
    /// "++"
    // cout << "OPERATOR_INCREMENT" << endl;
    // return MiniC::Parser::make_OPERATOR_INCREMENT(yytext, loc);
    ///
    /// "--"
    // cout << "OPERATOR_DECREMENT" << endl;
    // return MiniC::Parser::make_OPERATOR_DECREMENT(yytext, loc);
    ///
    /// ">>"
    // cout << "OPERATOR_SHIFT_RIGHT" << endl;
    // return MiniC::Parser::make_OPERATOR_SHIFT_RIGHT(yytext, loc);
    ///
    /// "<<"
    // cout << "OPERATOR_SHIFT_LEFT" << endl;
    // return MiniC::Parser::make_OPERATOR_SHIFT_LEFT(yytext, loc);
    ///
    /// ">"|"<"|">="|"<="|"=="|"!="
    // cout << "OPERATOR_RELATIONAL" << endl;
    // return MiniC::Parser::make_OPERATOR_RELATIONAL(yytext, loc);
    ///
    /// "="
    // cout << "OPERATOR_ASSIGNMENT" << endl;
    // return MiniC::Parser::make_OPERATOR_ASSIGNMENT(yytext, loc);
    ///
    /// "+"
    // cout << "OPERATOR_PLUS" << endl;
    // return MiniC::Parser::make_OPERATOR_PLUS(yytext, loc);
    ///
    /// "-"
    // cout << "OPERATOR_MINUS" << endl;
    // return MiniC::Parser::make_OPERATOR_MINUS(yytext, loc);
    ///
    /// "*"
    // cout << "OPERATOR_MULTIPLY" << endl;
    // return MiniC::Parser::make_OPERATOR_MULTIPLY(yytext, loc);
    ///
    /// "/"
    // cout << "OPERATOR_DIVIDE" << endl;
    // return MiniC::Parser::make_OPERATOR_DIVIDE(yytext, loc);
    ///
    /// "%"
    // cout << "OPERATOR_MODULUS" << endl;
    // return MiniC::Parser::make_OPERATOR_MODULUS(yytext, loc);
    ///
    /// "&&"
    // cout << "OPERATOR_LOGICAL_AND" << endl;
    // return MiniC::Parser::make_OPERATOR_LOGICAL_AND(yytext, loc);
    ///
    /// "&"
    // cout << "OPERATOR_BITWISE_AND" << endl;
    // return MiniC::Parser::make_OPERATOR_BITWISE_AND(yytext, loc);
    ///
    /// "||"
    // cout << "OPERATOR_LOGICAL_OR" << endl;
    // return MiniC::Parser::make_OPERATOR_LOGICAL_OR(yytext, loc);
    ///
    /// "|"
    // cout << "OPERATOR_BITWISE_OR" << endl;
    // return MiniC::Parser::make_OPERATOR_BITWISE_OR(yytext, loc);
    ///
    /// "^"
    // cout << "OPERATOR_BITWISE_XOR" << endl;
    // return MiniC::Parser::make_OPERATOR_BITWISE_XOR(yytext, loc);
    ///
    /// "~"
    // cout << "OPERATOR_BITWISE_NOT" << endl;
    // return MiniC::Parser::make_OPERATOR_BITWISE_NOT(yytext, loc);
    ///
    /// "!"
    // cout << "OPERATOR_LOGICAL_NOT" << endl;
    // return MiniC::Parser::make_OPERATOR_LOGICAL_NOT(yytext, loc);
    ///
    /// "("
    // cout << "PUNCTUATOR_PARENTHESIS_LEFT" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_PARENTHESIS_LEFT(loc);
    ///
    /// ")"
    // cout << "PUNCTUATOR_PARENTHESIS_RIGHT" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_PARENTHESIS_RIGHT(loc);
    ///
    /// "{"
    // cout << "PUNCTUATOR_BRACE_LEFT" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_BRACE_LEFT(loc);
    ///
    /// "}"
    // cout << "PUNCTUATOR_BRACE_RIGHT" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_BRACE_RIGHT(loc);
    ///
    /// "["
    // cout << "PUNCTUATOR_BRACKET_LEFT" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_BRACKET_LEFT(loc);
    ///
    /// "]"
    // cout << "PUNCTUATOR_BRACKET_RIGHT" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_BRACKET_RIGHT(loc);
    ///
    /// ";"
    // cout << "PUNCTUATOR_SEMICOLON" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_SEMICOLON(loc);
    ///
    /// ","
    // cout << "PUNCTUATOR_COMMA" << endl;
    // return MiniC::Parser::make_PUNCTUATOR_COMMA(loc);
    ///
    /// \n|(\r\n)
    // cout << "lineFeed" << endl;
    driver.increaseLocationY(1);
    ///
    /// [ \t\v\f]
    // cout << "whiteSpace" << endl;
    ///
    /// <<EOF>>
    // return MiniC::Parser::make_END(loc);
    ///
    /// .
    cout << RED << "Error: unexpected " << yytext << endl << YELLOW << "Location: " << loc << RESET_COLOR << endl;
    ///
} // END BODY
