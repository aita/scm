CC=gcc
LEX=flex
YACC=bison
TARGET=scm
OBJS=main.o scanner.o parser.o scheme.o syntax.o print.o string.o math.o
LIBS=
CFLAGS+=-I.

all: $(TARGET)

debug: CFLAGS+=-g
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

scanner.c: scanner.l parser.h
	$(LEX) -I -o scanner.c $<

parser.c parser.h: parser.y
	$(YACC) -o parser.c -d $<

main.o: main.c scheme.h parser.h
scanner.o: scanner.c scheme.h
parser.o: parser.c parser.h scheme.h
scheme.o: scheme.c scheme.h
syntax.o: syntax.c scheme.h
print.o: print.c scheme.h
string.o: string.c scheme.h
math.o: math.c scheme.h

.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -rf $(OBJS)
	rm -f scanner.c parser.c parser.h
