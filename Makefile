all: phpcomp

OBJS = lexer.o parser.o main.o printer.o extra.o codegen.o helper.o

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11 -g
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	$(RM) -rf parser.cpp parser.h lexer.cpp lexer.h tests/test.o tests/test $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.h: parser.cpp

lexer.cpp: lexer.l parser.h
	flex -o $@ $^

lexer.h: lexer.cpp

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

phpcomp: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

TOBJS = lexer.o parser.o tests/test.o printer.o extra.o codegen.o helper.o

tests/test.o: tests/test.cpp
	g++ -c $(CPPFLAGS) -o $@ $< -lgtest

test: $(TOBJS)
	g++ -o tests/test $(TOBJS) $(LIBS) $(LDFLAGS) -lgtest

runtest: tests/test
	./tests/test

exmpl: phpcomp
	./phpcomp --l exmp.php

exmpp: phpcomp
	./phpcomp --p exmp.php

exmpi: phpcomp
	./phpcomp --i exmp.php