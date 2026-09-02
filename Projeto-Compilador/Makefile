CC = gcc
CFLAGS = -Wall
LDLIBS = -lfl

LEXER_SRC = lexer/lexer.l
PARSER_SRC = parser/parser.y

LEX_OUT = lex.yy.c
PARSER_OUT_C = parser.tab.c
PARSER_OUT_H = parser.tab.h

TARGET = compilador

.PHONY: all run clean

all: $(TARGET)

$(LEX_OUT): $(LEXER_SRC)
	flex $(LEXER_SRC)

$(PARSER_OUT_C) $(PARSER_OUT_H): $(PARSER_SRC)
	bison -d $(PARSER_SRC)

$(TARGET): $(LEX_OUT) $(PARSER_OUT_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(LEX_OUT) $(PARSER_OUT_C) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(LEX_OUT) $(PARSER_OUT_C) $(PARSER_OUT_H) $(TARGET)