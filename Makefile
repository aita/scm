CC=gcc
LEX=flex
PROGRAM=myscheme
OBJS=main.o lex.yy.o
LIBS=

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

lex.yy.c: scanner.l
	$(LEX) $<

main.o: main.c
lex.yy.o: lex.yy.c

.PHONY: clean
clean:
	rm -rf $(PROGRAM)
	rm -rf $(OBJS)
	rm lex.yy.c
