CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wno-deprecated-register
BISON = bison
FLEX = flex

SRCS = main.cpp ast.cpp symbol_table.cpp semantic_analyzer.cpp tac.cpp optimizer.cpp codegen.cpp parser.tab.c lex.yy.c
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)
EXEC = mini_compiler

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

parser.tab.o: parser.tab.c parser.tab.h
	$(CXX) $(CXXFLAGS) -x c++ -c $< -o $@

lex.yy.o: lex.yy.c parser.tab.h
	$(CXX) $(CXXFLAGS) -x c++ -c $< -o $@

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

lex.yy.c: lexer.l parser.tab.h
	$(FLEX) lexer.l

clean:
	rm -f *.o parser.tab.c parser.tab.h lex.yy.c $(EXEC)
