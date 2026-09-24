CC = gcc
CFLAGS = -Wall
LDLIBS = -lfl

LEXER_SRC = lexer/lexer.l
PARSER_SRC = parser/parser.y

LEX_OUT = lex.yy.c
PARSER_OUT_C = parser.tab.c
PARSER_OUT_H = parser.tab.h

TARGET = compilador

.PHONY: all run test clean

all: $(TARGET)

$(PARSER_OUT_C) $(PARSER_OUT_H): $(PARSER_SRC)
	bison -d $(PARSER_SRC)

$(LEX_OUT): $(LEXER_SRC) $(PARSER_OUT_H)
	flex $(LEXER_SRC)

$(TARGET): $(LEX_OUT) $(PARSER_OUT_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(LEX_OUT) $(PARSER_OUT_C) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	chmod +x tests/testes.sh
	cd tests && bash ./testes.sh

clean:
	rm -f $(LEX_OUT) $(PARSER_OUT_C) $(PARSER_OUT_H) $(TARGET)
