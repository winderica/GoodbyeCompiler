%skeleton "lalr1.cc"
%require "3.4.2"
%defines
%define api.namespace { MiniC }
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include "token.h"

    namespace MiniC {
        class Lexer;
        class Interpreter;
    }
}

%code top {
    #include "interpreter.h"
    #include "color.h"
    using namespace MiniC;

    static Parser::symbol_type yylex(Lexer &lexer, Interpreter &driver) {
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

%type <Token> Program GlobalDeclarationDefinitionList GlobalDeclarationDefinition Type VariableList AssignmentExpression BlockStatement ParameterList Parameter Statement StatementList SwitchBodyStatement CaseStatementList CaseStatement DefaultStatement LocalDeclarationDefinition Expression CallExpression ArgumentList IndexExpression Index Assignable Identifier Literal ArrayLiteral ArrayItemList ArrayItem

%token END 0 "EOF"
%token PUNCTUATOR_PARENTHESIS_LEFT PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_BRACE_LEFT PUNCTUATOR_BRACE_RIGHT PUNCTUATOR_BRACKET_LEFT PUNCTUATOR_BRACKET_RIGHT PUNCTUATOR_SEMICOLON PUNCTUATOR_COMMA OPERATOR_QUESTION PUNCTUATOR_COLON
%token KEYWORD_FOR KEYWORD_IF KEYWORD_ELSE KEYWORD_WHILE KEYWORD_DO KEYWORD_RETURN KEYWORD_BREAK KEYWORD_CONTINUE KEYWORD_SWITCH KEYWORD_DEFAULT KEYWORD_CASE
%token <string> IDENTIFIER KEYWORD_TYPE LITERAL_FLOAT LITERAL_INTEGER LITERAL_CHAR LITERAL_STRING
%token <string> OPERATOR_SHIFT_RIGHT OPERATOR_SHIFT_LEFT OPERATOR_BITWISE_AND OPERATOR_BITWISE_OR OPERATOR_BITWISE_XOR OPERATOR_BITWISE_NOT
%token <string> OPERATOR_LOGICAL_NOT OPERATOR_LOGICAL_AND OPERATOR_LOGICAL_OR
%token <string> OPERATOR_COMPOUND_ASSIGNMENT OPERATOR_RELATIONAL OPERATOR_ASSIGNMENT
%token <string> OPERATOR_PLUS OPERATOR_MINUS OPERATOR_MULTIPLY OPERATOR_DIVIDE OPERATOR_MODULUS
%token <string> OPERATOR_INCREMENT OPERATOR_DECREMENT

%left OPERATOR_ASSIGNMENT OPERATOR_COMPOUND_ASSIGNMENT
%left OPERATOR_QUESTION PUNCTUATOR_COLON
%left OPERATOR_LOGICAL_OR
%left OPERATOR_LOGICAL_AND
%left OPERATOR_BITWISE_OR
%left OPERATOR_BITWISE_XOR
%left OPERATOR_BITWISE_AND
%left OPERATOR_RELATIONAL
%left OPERATOR_SHIFT_RIGHT OPERATOR_SHIFT_LEFT
%left OPERATOR_PLUS OPERATOR_MINUS
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
        $$ = $1;
        driver.addToken($$);
    };
GlobalDeclarationDefinitionList:
    GlobalDeclarationDefinitionList GlobalDeclarationDefinition {
        $$ = $1;
        $$.addChild($2);
    } | GlobalDeclarationDefinition {
        $$ = Token("Program");
        $$.addChild($1);
    };
GlobalDeclarationDefinition:
    Type VariableList PUNCTUATOR_SEMICOLON {
        $$ = Token("GlobalVariableDeclaration");
        $$.addChild($1);
        $$.addChild($2);
    } | Type Assignable PUNCTUATOR_PARENTHESIS_LEFT ParameterList PUNCTUATOR_PARENTHESIS_RIGHT BlockStatement {
        $$ = Token("FunctionDefinition");
        $$.addChild($1);
        $$.addChild($2);
        $$.addChild($4);
        $$.addChild($6);
    } | Type Assignable PUNCTUATOR_PARENTHESIS_LEFT ParameterList PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_SEMICOLON {
        $$ = Token("FunctionDeclaration");
        $$.addChild($1);
        $$.addChild($2);
        $$.addChild($4);
    } | error PUNCTUATOR_SEMICOLON {
        $$ = Token("ERROR_STATEMENT");
        yyerrok;
    };
Type:
    KEYWORD_TYPE {
        $$ = Token("Type", $1, @1);
    };
Assignable:
    Identifier {
        $$ = $1;
    } | IndexExpression {
        $$ = $1;
    };
Identifier:
    IDENTIFIER {
        $$ = Token("Identifier", $1, @1);
    };
VariableList:
    Assignable {
        $$ = Token("VariableList");
        Token declaration("VariableDeclaration");
        declaration.addChild($1);
        $$.addChild(declaration);
    } | AssignmentExpression {
        $$ = Token("VariableList");
        $1.setKind("VariableDefinition");
        $$.addChild($1);
    } | VariableList PUNCTUATOR_COMMA Assignable {
        $$ = $1;
        Token declaration("VariableDeclaration");
        declaration.addChild($3);
        $$.addChild(declaration);
    } | VariableList PUNCTUATOR_COMMA AssignmentExpression {
        $$ = $1;
        $3.setKind("VariableDefinition");
        $$.addChild($3);
    };
AssignmentExpression:
    Assignable OPERATOR_ASSIGNMENT Expression {
        $$ = Token("AssignmentExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Assignable OPERATOR_ASSIGNMENT ArrayLiteral {
        $$ = Token("AssignmentExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    };
ParameterList:
    ParameterList PUNCTUATOR_COMMA Parameter {
        $$ = $1;
        $$.addChild($3);
    } | Parameter {
        $$ = Token("ParameterList");
        $$.addChild($1);
    } | {
        $$ = Token("ParameterList");
    };
Parameter:
    Type Assignable {
        $$ = Token("ParameterDeclaration");
        $$.addChild($1);
        $$.addChild($2);
    };
BlockStatement:
    PUNCTUATOR_BRACE_LEFT StatementList PUNCTUATOR_BRACE_RIGHT {
        $$ = $2;
    } | PUNCTUATOR_BRACE_LEFT PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("BlockStatement");
    } | error PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("ERROR_STATEMENT");
        yyerrok;
    };
StatementList:
    StatementList Statement {
        $$ = $1;
        $$.addChild($2);
    } | Statement {
        $$ = Token("BlockStatement");
        $$.addChild($1);
    };
Statement:
    LocalDeclarationDefinition
      | BlockStatement
      | Expression PUNCTUATOR_SEMICOLON
      | KEYWORD_RETURN Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("ReturnStatement");
        $$.addChild($2);
    } | KEYWORD_CONTINUE Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("ContinueStatement");
        $$.addChild($2);
    } | KEYWORD_BREAK Expression PUNCTUATOR_SEMICOLON {
        $$ = Token("BreakStatement");
        $$.addChild($2);
    } | KEYWORD_RETURN PUNCTUATOR_SEMICOLON {
        $$ = Token("ReturnStatement");
    } | KEYWORD_CONTINUE PUNCTUATOR_SEMICOLON {
        $$ = Token("ContinueStatement");
    } | KEYWORD_BREAK PUNCTUATOR_SEMICOLON {
        $$ = Token("BreakStatement");
    } | KEYWORD_IF PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement %prec IF_END {
        $$ = Token("IfStatement");
        $$.addChild($3);
        $$.addChild($5);
    } | KEYWORD_IF PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement KEYWORD_ELSE Statement {
        $$ = Token("IfStatement");
        $$.addChild($3);
        $$.addChild($5);
        Token elseStatement("ElseStatement");
        elseStatement.addChild($7);
        $$.addChild(elseStatement);
    } | KEYWORD_WHILE PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement {
        $$ = Token("WhileStatement");
        $$.addChild($3);
        $$.addChild($5);
    } | KEYWORD_DO Statement KEYWORD_WHILE PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_SEMICOLON {
        $$ = Token("DoWhileStatement");
        $$.addChild($2);
        $$.addChild($5);
    } | KEYWORD_FOR PUNCTUATOR_PARENTHESIS_LEFT LocalDeclarationDefinition Expression PUNCTUATOR_SEMICOLON Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement {
        $$ = Token("ForStatement");
        $$.addChild($3);
        $$.addChild($4);
        $$.addChild($6);
        $$.addChild($8);
    } | KEYWORD_FOR PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_SEMICOLON Expression PUNCTUATOR_SEMICOLON Expression PUNCTUATOR_PARENTHESIS_RIGHT Statement {
        $$ = Token("ForStatement");
        $$.addChild($3);
        $$.addChild($5);
        $$.addChild($7);
        $$.addChild($9);
    } | KEYWORD_SWITCH PUNCTUATOR_PARENTHESIS_LEFT Expression PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_BRACE_LEFT SwitchBodyStatement PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("SwitchStatement");
        $$.addChild($3);
        $$.addChild($6);
    } | error PUNCTUATOR_SEMICOLON {
        $$ = Token("ERROR_STATEMENT");
        yyerrok;
    };
SwitchBodyStatement:
    CaseStatementList DefaultStatement {
        $$ = $1;
        $$.addChild($2);
    } | CaseStatementList
      | DefaultStatement {
        $$ = Token("BlockStatement");
        $$.addChild($1);
    } | {
        $$ = Token("BlockStatement");
    };
CaseStatementList:
    CaseStatementList CaseStatement {
        $$ = $1;
        $$.addChild($2);
    } | CaseStatement {
        $$ = Token("SwitchBodyStatement");
        $$.addChild($1);
    };
CaseStatement:
    KEYWORD_CASE Expression PUNCTUATOR_COLON StatementList {
        $$ = Token("CaseStatement");
        $$.addChild($2);
        $$.addChild($4);
    } | KEYWORD_CASE Expression PUNCTUATOR_COLON {
        $$ = Token("CaseStatement");
        $$.addChild($2);
    };
DefaultStatement:
    KEYWORD_DEFAULT PUNCTUATOR_COLON StatementList {
        $$ = Token("DefaultStatement");
        $$.addChild($3);
    } | KEYWORD_DEFAULT PUNCTUATOR_COLON {
        $$ = Token("DefaultStatement");
    };
LocalDeclarationDefinition:
    Type VariableList PUNCTUATOR_SEMICOLON {
        $$ = Token("LocalVariableDeclaration");
        $$.addChild($1);
        $$.addChild($2);
    };
Expression:
    Assignable
      | Literal
      | CallExpression
      | AssignmentExpression
      | Assignable OPERATOR_COMPOUND_ASSIGNMENT Expression {
        $$ = Token("CompoundAssignmentExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", $2));
        $$.addChild($3);
    } | Expression OPERATOR_QUESTION Expression PUNCTUATOR_COLON Expression {
        $$ = Token("TernaryExpression");
        $$.addChild($1);
        $$.addChild(Token("Operator", "?"));
        $$.addChild($3);
        $$.addChild(Token("Operator", ":"));
        $$.addChild($5);
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
        $$ = Token("RelationalExpression");
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
    };
Literal:
    LITERAL_INTEGER {
        $$ = Token("IntegerLiteral", $1, @1);
    } | LITERAL_FLOAT {
        $$ = Token("FloatLiteral", $1, @1);
    } | LITERAL_CHAR {
        $$ = Token("CharLiteral", $1, @1);
    } | LITERAL_STRING {
        $$ = Token("StringLiteral", $1, @1);
    };
ArrayLiteral:
    PUNCTUATOR_BRACE_LEFT ArrayItemList PUNCTUATOR_BRACE_RIGHT {
        $$ = $2;
    };
ArrayItemList:
    ArrayItemList PUNCTUATOR_COMMA ArrayItem {
        $$ = $1;
        $$.addChild($3);
    } | ArrayItem {
        $$ = Token("ArrayLiteral");
        $$.addChild($1);
    } | {
        $$ = Token("ArrayLiteral");
    };
ArrayItem:
    Expression | ArrayLiteral;
CallExpression:
    CallExpression PUNCTUATOR_PARENTHESIS_LEFT ArgumentList PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = $1;
        $$.addChild($3);
    } | Assignable PUNCTUATOR_PARENTHESIS_LEFT ArgumentList PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("CallExpression");
        $$.addChild($1);
        $$.addChild($3);
    };
ArgumentList:
    ArgumentList PUNCTUATOR_COMMA Expression {
        $$ = $1;
        $$.addChild($3);
    } | Expression {
        $$ = Token("ArgumentList");
        $$.addChild($1);
    } | {
        $$ = Token("ArgumentList");
    };
IndexExpression:
    IndexExpression PUNCTUATOR_BRACKET_LEFT Index PUNCTUATOR_BRACKET_RIGHT {
        $$ = $1;
        $$.addChild($3);
    } | Identifier PUNCTUATOR_BRACKET_LEFT Index PUNCTUATOR_BRACKET_RIGHT {
        $$ = Token("IndexExpression");
        $$.addChild($1);
        $$.addChild($3);
    };
Index:
    Expression {
        $$ = Token("Index");
        $$.addChild($1);
    } | {
        $$ = Token("Index");
    };
%%

void Parser::error(const location &loc, const string &message) {
     cout << RED << "Error: " << message << endl << YELLOW << "Location: " << loc << RESET_COLOR << endl;
}
