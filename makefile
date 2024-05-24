CC=g++
LEX=flex
CFLAGS=-Wall -g -std=c++11 `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/
YACC=bison
LFLAGS= --nounput -o
YFLAGS=-d -t -v -Wcounterexamples -o
VALGRIND_OPTS=--leak-check=full --show-leak-kinds=all --track-origins=yes

SOURCE = minic
OUT_EXEC=main
TEST_DIR = ./tests

all: main

main: lex.yy.o y.tab.o ast.o semantic_analysis.o cosineSimilarity.o main.o
	g++ lex.yy.o y.tab.o ast.o semantic_analysis.o cosineSimilarity.o main.o `llvm-config-15 --cxxflags --ldflags --libs core` -o main

y.tab.c y.tab.h: $(SOURCE).y
	$(YACC) $(YFLAGS) y.tab.c $<

lex.yy.o: lex.yy.c y.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c -o $@

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c y.tab.c -o $@

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c -o $@

semantic_analysis.o: semantic_analysis.c semantic_analysis.h
	$(CC) $(CFLAGS) -c semantic_analysis.c -o $@

cosineSimilarity.o: cosineSimilarity.c cosineSimilarity.h
	$(CC) $(CFLAGS) -c cosineSimilarity.c -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $@

lex.yy.c: $(SOURCE).l y.tab.h
	$(LEX) $(LFLAGS) $@ $<

parser_test: $(OUT_EXEC)
	./$(OUT_EXEC) < $(PARSER_TEST_DIR)/p1.c

comparison_test: $(OUT_EXEC)
	./$(OUT_EXEC) $(TEST_DIR)/p5.c $(TEST_DIR)/p6.c

clean:
	rm -f vgcore* lex.yy.c y.tab.c lex.yy.o y.tab.o ast.o semantic_analysis.o cosineSimilarity.o main.o $(OUT_EXEC) y.tab.h y.output

.PHONY: comparison_test parser_test semantic_test builder_test parser_tests semantic_tests builder_tests valgrind_test clean run