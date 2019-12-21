all:
	make transpile
	make compile

compile:
	g++ -std=c++2a -pipe -g main.cpp lexer.cpp parser.cpp interpreter.cpp analyzer.cpp token.cpp symbol.cpp symbolTable.cpp quadruple.cpp mips.cpp -o compiler

transpile:
	node lexerGenerator.js
	flex -o lexer.cpp lexer.l
	bison -v -o parser.cpp parser.y

clean:
	rm -rf lexer.cpp parser.cpp parser.hpp parser.output location.hh position.hh stack.hh compiler
