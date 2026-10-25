CC = gcc
CFLAGS = -Wall -I. -Isrc
LDLIBS = -lfl

LEXER_SRC = lexer/lexer.l
PARSER_SRC = parser/parser.y
SRC_FILES = src/main.c src/ast.c src/tabela_simbolos.c
SRC_HEADERS = src/ast.h src/tabela_simbolos.h

LEX_OUT = lex.yy.c
PARSER_OUT_C = parser.tab.c
PARSER_OUT_H = parser.tab.h

TARGET = compilador
UNIT_TESTS = tests/test_ast tests/test_tabela_simbolos

.PHONY: all run test clean

all: $(TARGET)

$(PARSER_OUT_C) $(PARSER_OUT_H): $(PARSER_SRC)
	bison -d $(PARSER_SRC)

$(LEX_OUT): $(LEXER_SRC) $(PARSER_OUT_H)
	flex $(LEXER_SRC)

$(TARGET): $(LEX_OUT) $(PARSER_OUT_C) $(SRC_FILES) $(SRC_HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(LEX_OUT) $(PARSER_OUT_C) $(SRC_FILES) $(LDLIBS)

tests/test_ast: tests/test_ast.c tests/teste_util.h src/ast.c src/ast.h
	$(CC) $(CFLAGS) -o $@ tests/test_ast.c src/ast.c

tests/test_tabela_simbolos: tests/test_tabela_simbolos.c tests/teste_util.h src/tabela_simbolos.c src/tabela_simbolos.h
	$(CC) $(CFLAGS) -o $@ tests/test_tabela_simbolos.c src/tabela_simbolos.c

run: $(TARGET)
	./$(TARGET)

test: $(TARGET) $(UNIT_TESTS)
	./tests/test_ast
	./tests/test_tabela_simbolos
	chmod +x tests/testes.sh
	cd tests && bash ./testes.sh

clean:
	rm -f $(LEX_OUT) $(PARSER_OUT_C) $(PARSER_OUT_H) $(TARGET) $(UNIT_TESTS)