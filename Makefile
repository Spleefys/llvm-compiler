all: main

OBJS = lexer.o parser.o main.o printer.o extra.o codegen.o helper.o

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11 -g
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	$(RM) -rf parser.cpp parser.h main lexer.cpp lexer.h $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.h: parser.cpp

lexer.cpp: lexer.l parser.h
	flex -o $@ $^

lexer.h: lexer.cpp

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

main: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

exmp: main
	./main exmp.php