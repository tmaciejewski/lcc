all:
	make lcc
	make machine

lcc: parser.c parser.h lcc.c lexer.c lexer.h prettyprinter.c prettyprinter.h compiler.c compiler.h
    gcc -g -o lcc -Wall -O2 -ansi -pedantic lexer.c parser.c lcc.c prettyprinter.c compiler.c -std=c99

machine: machine.c machine.h
	gcc machine.c -o machine -O2 -Wall -ansi -pedantic
