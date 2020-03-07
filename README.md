###PHP-like compiler on llvm

####How to run
For run this, you need to install:
1. GNU Flex
2. GNU Bison
3. LLVM-6.0
4. Clang(version 6.0)
5. Make

And then you can:
* make - for compile all project
* make exmpl - for run only lexer
* make exmpp - for run only parser and u can see AST-tree
* make exmpi - for run LLVM IR
* ./phpcomp (FLAG) (FILE) - another way for launch this
* make test - for compile simple test for parser
* make runtest - run this test
* make clean - clean builds files of project

####About
This is simple PHP-like compiler, and you can compile only variable declaration(only integer, char and array) and assignment for this, simple math, if(without else), while, return, print("print" - for integer, "echo" - for constant string, "echoc" - for char).
Almost everything a compiler can do writed in exmp.php.
The codegen is not the best, as it is made for educational purposes.
