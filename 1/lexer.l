%{
    #include "interpreter.h"
    #include "color.h"
    #define YY_USER_ACTION driver.increaseLocationX(yyleng);
%}
%option nodefault
%option noyywrap
%option c++
%option yyclass="Lexer"
%option prefix="MiniC_"

%x COMMENT_BLOCK

%%
%{
    auto loc = driver.getLocation();
%}
"/*" {
    BEGIN(COMMENT_BLOCK);
    // cout << "COMMENT_BLOCK" << endl;
}
<COMMENT_BLOCK>"*/" {
    BEGIN(INITIAL);
    // cout << "COMMENT_BLOCK_END" << endl;
}
<COMMENT_BLOCK>\n {
    driver.increaseLocationY(1);
}
<COMMENT_BLOCK>. {
}
<COMMENT_BLOCK><<EOF>> {
    // cout << "Unterminated comment" << endl;
}
\/\/.*\n {
    driver.increaseLocationY(1);
    // cout << "COMMENT_INLINE" << endl;
}
[0-9]+[lLuU]? {
    // cout << "LITERAL_INTEGER" << " " << yytext << endl;
    return MiniC::Parser::make_LITERAL_INTEGER(yytext, loc);
}
([0-9]*\.[0-9]+)|([0-9]+\.)[lLuU]? {
    // cout << "LITERAL_FLOAT" << " " << yytext << endl;
    return MiniC::Parser::make_LITERAL_FLOAT(yytext, loc);
}
'([^'\\\n]|\\.){0,1}' {
    // cout << "LITERAL_CHAR" << " " << yytext << endl;
    return MiniC::Parser::make_LITERAL_CHAR(yytext, loc);
}
\"(\\.|[^"\\])*\" {
    // cout << "LITERAL_STRING" << " " << yytext << endl;
    return MiniC::Parser::make_LITERAL_STRING(yytext, loc);
}
"int"|"float"|"long"|"char"|"double"|"void"|"short" {
    // cout << "KEYWORD_TYPE" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_TYPE(yytext, loc);
}
"return" {
    // cout << "KEYWORD_RETURN" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_RETURN(loc);
}
"break" {
    // cout << "KEYWORD_BREAK" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_BREAK(loc);
}
"continue" {
    // cout << "KEYWORD_CONTINUE" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_CONTINUE(loc);
}
"for" {
    // cout << "KEYWORD_FOR" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_FOR(loc);
}
"if" {
    // cout << "KEYWORD_IF" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_IF(loc);
}
"else" {
    // cout << "KEYWORD_ELSE" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_ELSE(loc);
}
"while" {
    // cout << "KEYWORD_WHILE" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_WHILE(loc);
}
"do" {
    // cout << "KEYWORD_DO" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_DO(loc);
}
"switch" {
    // cout << "KEYWORD_SWITCH" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_SWITCH(loc);
}
"case" {
    // cout << "KEYWORD_CASE" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_CASE(loc);
}
"default" {
    // cout << "KEYWORD_DEFAULT" << " " << yytext << endl;
    return MiniC::Parser::make_KEYWORD_DEFAULT(loc);
}
[A-Za-z_][A-Za-z0-9_]* {
    // cout << "IDENTIFIER" << " " << yytext << endl;
    return MiniC::Parser::make_IDENTIFIER(yytext, loc);
}
"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=" {
    // cout << "OPERATOR_COMPOUND_ASSIGNMENT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_COMPOUND_ASSIGNMENT(yytext, loc);
}
"++" {
    // cout << "OPERATOR_INCREMENT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_INCREMENT(yytext, loc);
}
"--" {
    // cout << "OPERATOR_DECREMENT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_DECREMENT(yytext, loc);
}
">>" {
    // cout << "OPERATOR_SHIFT_RIGHT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_SHIFT_RIGHT(yytext, loc);
}
"<<" {
    // cout << "OPERATOR_SHIFT_LEFT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_SHIFT_LEFT(yytext, loc);
}
">"|"<"|">="|"<="|"=="|"!=" {
    // cout << "OPERATOR_RELATIONAL" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_RELATIONAL(yytext, loc);
}
"=" {
    // cout << "OPERATOR_ASSIGNMENT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_ASSIGNMENT(yytext, loc);
}
"+" {
    // cout << "OPERATOR_PLUS" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_PLUS(yytext, loc);
}
"-" {
    // cout << "OPERATOR_MINUS" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_MINUS(yytext, loc);
}
"*" {
    // cout << "OPERATOR_MULTIPLY" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_MULTIPLY(yytext, loc);
}
"/" {
    // cout << "OPERATOR_DIVIDE" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_DIVIDE(yytext, loc);
}
"%" {
    // cout << "OPERATOR_MODULUS" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_MODULUS(yytext, loc);
}
"&&" {
    // cout << "OPERATOR_LOGICAL_AND" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_LOGICAL_AND(yytext, loc);
}
"&" {
    // cout << "OPERATOR_BITWISE_AND" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_BITWISE_AND(yytext, loc);
}
"||" {
    // cout << "OPERATOR_LOGICAL_OR" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_LOGICAL_OR(yytext, loc);
}
"|" {
    // cout << "OPERATOR_BITWISE_OR" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_BITWISE_OR(yytext, loc);
}
"^" {
    // cout << "OPERATOR_BITWISE_XOR" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_BITWISE_XOR(yytext, loc);
}
"~" {
    // cout << "OPERATOR_BITWISE_NOT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_BITWISE_NOT(yytext, loc);
}
"!" {
    // cout << "OPERATOR_LOGICAL_NOT" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_LOGICAL_NOT(yytext, loc);
}
"?" {
    // cout << "OPERATOR_QUESTION" << " " << yytext << endl;
    return MiniC::Parser::make_OPERATOR_QUESTION(loc);
}
":" {
    // cout << "PUNCTUATOR_COLON" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_COLON(loc);
}
"(" {
    // cout << "PUNCTUATOR_PARENTHESIS_LEFT" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_PARENTHESIS_LEFT(loc);
}
")" {
    // cout << "PUNCTUATOR_PARENTHESIS_RIGHT" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_PARENTHESIS_RIGHT(loc);
}
"{" {
    // cout << "PUNCTUATOR_BRACE_LEFT" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_BRACE_LEFT(loc);
}
"}" {
    // cout << "PUNCTUATOR_BRACE_RIGHT" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_BRACE_RIGHT(loc);
}
"[" {
    // cout << "PUNCTUATOR_BRACKET_LEFT" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_BRACKET_LEFT(loc);
}
"]" {
    // cout << "PUNCTUATOR_BRACKET_RIGHT" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_BRACKET_RIGHT(loc);
}
";" {
    // cout << "PUNCTUATOR_SEMICOLON" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_SEMICOLON(loc);
}
"," {
    // cout << "PUNCTUATOR_COMMA" << " " << yytext << endl;
    return MiniC::Parser::make_PUNCTUATOR_COMMA(loc);
}
\n|(\r\n) {
    // cout << "lineFeed" << endl;
    driver.increaseLocationY(1);
}
[ \t\v\f] {
    // cout << "whiteSpace" << endl;
}
<<EOF>> {
    return MiniC::Parser::make_END(loc);
}
. {
    cout << RED << "Error: unexpected " << yytext << endl << YELLOW << "Location: " << loc << RESET_COLOR << endl;
}
