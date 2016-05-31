CC=gcc
LEX=flex
YACC=bison
PROGRAM=scm
OBJS=main.o scanner.o parser.o scheme.o
LIBS=

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

scanner.c: scanner.l parser.h
	$(LEX) -I -o scanner.c $<

parser.c parser.h: parser.y
	$(YACC) -o parser.c -d $<

scheme.c: scheme.h
main.c: scheme.h parser.h

main.o: main.c
scanner.o: scanner.c
parser.o: parser.c
scheme.o: scheme.c


.PHONY: clean
clean:
	rm -rf $(PROGRAM)
	rm -rf $(OBJS)
	rm -f scanner.c parser.c parser.h
