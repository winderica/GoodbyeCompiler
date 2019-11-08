%skeleton "lalr1.cc"
%require "3.4.2"
%defines
%define api.namespace { MiniC }
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <string>
    #include <iostream>
    #include "token.h"

    using namespace std;

    namespace MiniC {
        class Lexer;
        class Interpreter;
    }
}

%code top {
    #include "lexer.h"
    #include "parser.hpp"
    #include "interpreter.h"
    #include "location.hh"
    #include "color.h"
    using namespace MiniC;

    static Parser::symbol_type yylex(MiniC::Lexer &lexer, MiniC::Interpreter &driver) {
        return lexer.get_next_token();
    }
}

%lex-param { Lexer &lexer }
%lex-param { Interpreter &driver }
%parse-param { Lexer &lexer }
%parse-param { Interpreter &driver }
%locations
%define parse.trace
%define parse.error verbose

%type <Token> Program GlobalDeclarationDefinitionList GlobalDeclarationDefinition Type VariableDeclarationDefinitionList FunctionDefinition VariableDefinition BlockStatement ParameterDelarationList VariableDeclaration ParameterDeclaration Statement StatementList LocalDeclarationDefinition Expression Arguments Identifier

%token END 0 "EOF"
%token PUNCTUATOR_PARENTHESIS_LEFT PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_BRACE_LEFT PUNCTUATOR_BRACE_RIGHT PUNCTUATOR_BRACKET_LEFT PUNCTUATOR_BRACKET_RIGHT PUNCTUATOR_SEMICOLON PUNCTUATOR_COMMA
%token <string> IDENTIFIER KEYWORD_TYPE LITERAL_FLOAT LITERAL_INTEGER LITERAL_CHAR LITERAL_STRING
%token <string> OPERATOR_SHIFT_RIGHT OPERATOR_SHIFT_LEFT OPERATOR_BITWISE_AND OPERATOR_BITWISE_OR OPERATOR_BITWISE_XOR OPERATOR_BITWISE_NOT
%token <string> OPERATOR_LOGICAL_NOT OPERATOR_LOGICAL_AND OPERATOR_LOGICAL_OR
%token <string> OPERATOR_COMPOUND_ASSIGNMENT OPERATOR_RELATIONAL OPERATOR_ASSIGNMENT
%token <string> OPERATOR_PLUS OPERATOR_MINUS OPERATOR_MULTIPLY OPERATOR_DIVIDE OPERATOR_MODULUS
%token <string> OPERATOR_INCREMENT OPERATOR_DECREMENT
%token KEYWORD_FOR KEYWORD_IF KEYWORD_ELSE KEYWORD_WHILE KEYWORD_RETURN KEYWORD_BREAK KEYWORD_CONTINUE

%left OPERATOR_ASSIGNMENT OPERATOR_COMPOUND_ASSIGNMENT
%left OPERATOR_LOGICAL_OR
%left OPERATOR_LOGICAL_AND
%left OPERATOR_BITWISE_OR
%left OPERATOR_BITWISE_XOR
%left OPERATOR_BITWISE_AND
%left OPERATOR_RELATIONAL
%left OPERATOR_PLUS OPERATOR_MINUS
%left OPERATOR_SHIFT_RIGHT OPERATOR_SHIFT_LEFT
%left OPERATOR_MULTIPLY OPERATOR_DIVIDE OPERATOR_MODULUS

%right OPERATOR_LOGICAL_NOT OPERATOR_BITWISE_NOT

%nonassoc IF_END
%nonassoc KEYWORD_ELSE
%nonassoc OPERATOR_UNARY_MINUS OPERATOR_UNARY_PLUS
%nonassoc OPERATOR_INCREMENT OPERATOR_DECREMENT

%start Program

%%
Program:
    GlobalDeclarationDefinitionList {
        driver.clear();
        $$ = Token("Program");
        $$.addChild($1);
        driver.addToken($$);
        cout << driver.toString() << endl;
    };
GlobalDeclarationDefinitionList:
    GlobalDeclarationDefinitionList GlobalDeclarationDefinition {
        $$ = $1;
        $$.addChild($2);
    } | GlobalDeclarationDefinition {
        $$ = Token("GlobalDeclarationDefinitionList");
        $$.addChild($1);
    };
GlobalDeclarationDefinition:
    Type VariableDeclarationDefinitionList PUNCTUATOR_SEMICOLON {
        $$ = Token("GlobalDeclarationDefinition");
        $$.addChild($1);
        $$.addChild($2);
    } | Type FunctionDefinition BlockStatement {
        $$ = Token("GlobalDeclarationDefinition");
        $$.addChild($1);
        $$.addChild($2);
        $$.addChild($3);
    } | error PUNCTUATOR_SEMICOLON {
        $$ = Token("ERROR_STATEMENT");
        yyerrok;
    };
Type:
    KEYWORD_TYPE {
        const string& value = $1;
        $$ = Token("Type", value);
    };
Identifier:
    IDENTIFIER {
        $$ = Token("Identifier", $1);
    };
VariableDeclarationDefinitionList:
    VariableDeclaration {
        $$ = Token("VariableDeclarationDefinitionList");
        $$.addChild($1);
    } | VariableDefinition {
        $$ = Token("VariableDeclarationDefinitionList");
        $$.addChild($1);
    } | VariableDeclarationDefinitionList PUNCTUATOR_COMMA VariableDeclaration {
        $$ = $1;
        $$.addChild($3);
    } | VariableDeclarationDefinitionList PUNCTUATOR_COMMA VariableDefinition {
        $$ = $1;
        $$.addChild($3);
    };
VariableDeclaration:
    Identifier {
        $$ = Token("VariableDeclaration");
        $$.addChild($1);
    };
VariableDefinition:
    Identifier OPERATOR_ASSIGNMENT Expression {
        $$ = Token("VariableDefinition");
        $$.addChild($1);
        $$.addChild($3);
    };
FunctionDefinition:
    Identifier PUNCTUATOR_PARENTHESIS_LEFT ParameterDelarationList PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("FunctionDeclaration");
        $$.addChild($1);
        $$.addChild($3);
    } | Identifier PUNCTUATOR_PARENTHESIS_LEFT PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("FunctionDeclaration");
        $$.addChild($1);
    };
ParameterDelarationList:
    ParameterDeclaration {
        $$ = Token("ParameterDelarationList");
        $$.addChild($1);
    } | ParameterDelarationList PUNCTUATOR_COMMA ParameterDeclaration {
        $$ = $1;
        $$.addChild($3);
    };
ParameterDeclaration:
    Type VariableDeclaration {
        $$ = Token("ParameterDeclaration");
        $$.addChild($1);
        $$.addChild($2);
    };
BlockStatement:
    PUNCTUATOR_BRACE_LEFT StatementList PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("BlockStatement");
        $$.addChild($2);
    } | PUNCTUATOR_BRACE_LEFT PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("BlockStatement");
    };
StatementList:
    Statement {
        $$ = Token("StatementList");
        $$.addChild($1);
    } | StatementList Statement {
        $$ = $1;
        $$.addChild($2);
    };
Statement:
    Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("ExpressionStatement");
        $$.addChild($1);
    } | KEYWORD_RETURN Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("ReturnStatement");
        $$.addChild($2);
    } | KEYWORD_CONTINUE Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("ContinueStatement");
        $$.addChild($2);
    } | KEYWORD_BREAK Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("BreakStatement");
        $$.addChild($2);
    } | KEYWORD_IF PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement %prec IF_END {
        $$ = Token("IfStatement");
        $$.addChild($3);
        $$.addChild($5);
    } | KEYWORD_IF PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement KEYWORD_ELSE Statement {
        $$ = Token("IfStatement");
        $$.addChild($3);
        $$.addChild($5);
        $$.addChild($7);
    } | KEYWORD_WHILE PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement {
        $$ = Token("WhileStatement");
        $$.addChild($3);
        $$.addChild($5);
    } | KEYWORD_FOR PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_SEMICOLON Expression PUNCTUATOR_SEMICOLON Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement {
        $$ = Token("ForStatement");
        $$.addChild($3);
        $$.addChild($5);
        $$.addChild($7);
        $$.addChild($9);
    } | LocalDeclarationDefinition {
        $$ = Token("DeclarationDefinitionStatement");
        $$.addChild($1);
    } | error PUNCTUATOR_SEMICOLON {
        $$ = Token("ERROR_STATEMENT");
        yyerrok;
    } | BlockStatement;

LocalDeclarationDefinition:
    Type VariableDeclarationDefinitionList PUNCTUATOR_SEMICOLON {
        $$ = Token("LocalDeclarationDefinition");
        $$.addChild($1);
        $$.addChild($2);
    };
Expression:
    Identifier OPERATOR_ASSIGNMENT Expression {
        $$ = Token("AssignmentExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Identifier OPERATOR_COMPOUND_ASSIGNMENT Expression {
        $$ = Token("CompoundAssignmentExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_LOGICAL_AND Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_LOGICAL_OR Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_BITWISE_AND Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_BITWISE_OR Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_BITWISE_XOR Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_SHIFT_RIGHT Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_SHIFT_LEFT Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_RELATIONAL Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_PLUS Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_MINUS Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_MULTIPLY Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_DIVIDE Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_MODULUS Expression {
        $$ = Token("BinaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("ParenthesisExpression");
        $$.addChild($2);
    } | OPERATOR_PLUS Expression %prec OPERATOR_UNARY_PLUS {
        $$ = Token("PrefixExpression");
        $$.addChild(Token("Operator", $1));
        $$.addChild($2);
    } | OPERATOR_MINUS Expression %prec OPERATOR_UNARY_MINUS {
        $$ = Token("PrefixExpression");
        $$.addChild(Token("Operator", $1));
        $$.addChild($2);
    } | OPERATOR_LOGICAL_NOT Expression {
        $$ = Token("PrefixExpression");
        $$.addChild(Token("Operator", $1));
        $$.addChild($2);
    } | OPERATOR_BITWISE_NOT Expression {
        $$ = Token("PrefixExpression");
        $$.addChild(Token("Operator", $1));
        $$.addChild($2);
    } | OPERATOR_INCREMENT Expression {
        $$ = Token("PrefixExpression");
        $$.addChild(Token("Operator", $1));
        $$.addChild($2);
    } | OPERATOR_DECREMENT Expression {
        $$ = Token("PrefixExpression");
        $$.addChild(Token("Operator", $1));
        $$.addChild($2);
    } | Expression OPERATOR_INCREMENT {
        $$ = Token("PostfixExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
    } | Expression OPERATOR_DECREMENT {
        $$ = Token("PostfixExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
    } | Identifier PUNCTUATOR_PARENTHESIS_LEFT Arguments PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("CallExpression");
        $$.addChild($1);
        $$.addChild($3);
    } | Identifier {
        $$ = Token("Expression");
        $$.addChild($1);
    } | LITERAL_INTEGER {
        $$ = Token("IntegerLiteral", $1);
    } | LITERAL_FLOAT {
        $$ = Token("FloatLiteral", $1);
    } | LITERAL_CHAR {
        $$ = Token("CharLiteral", $1);
    } | LITERAL_STRING{
        $$ = Token("StringLiteral", $1);
    };
Arguments:
    Arguments PUNCTUATOR_COMMA Expression {
        $$ = $1;
        $1.addChild($3);
    } | Expression {
        $$ = Token("Arguments");
        $$.addChild($1);
    } | {
        $$ = Token("Arguments");
    };
%%

void Parser::error(const location &loc , const string &message) {
     cout << RED << "Error: " << message << endl << YELLOW << "Location: " << loc << RESET_COLOR << endl;
}
