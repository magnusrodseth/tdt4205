LEX=flex
YACC=bison
YFLAGS+=--defines=src/y.tab.h -o y.tab.c
CFLAGS+=-std=c99 -Wall -g -Isrc -Iinclude -D_POSIX_C_SOURCE=200809L -DYYSTYPE="node_t *"

src/vslc: src/vslc.o src/parser.o src/scanner.o src/tree.o src/graphviz_output.o src/symbols.o src/symbol_table.o src/generator.o
src/y.tab.h: src/parser.c
src/scanner.c: src/y.tab.h src/scanner.l
clean:
	-rm -f src/parser.c src/scanner.c src/*.tab.* src/*.o
purge: clean
	-rm -f src/vslc
