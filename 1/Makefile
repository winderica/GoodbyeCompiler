all:
	node lexerGenerator.js
	flex -o lexer.cpp lexer.l
	bison -v -o parser.cpp parser.y
	g++ -pipe -g main.cpp lexer.cpp parser.cpp interpreter.cpp token.cpp -o compiler

clean:
	rm -rf lexer.cpp parser.cpp parser.hpp parser.output location.hh position.hh stack.hh compiler
