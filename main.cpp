#include "codegen.h"
#include "extra.h"
#include "node.h"
#include "parser.h"
#include "printer.h"

using namespace std;

extern NBlock *programBlock;
extern int yyparse();

extern int yylex();
extern FILE *yyin;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Error!\n");
    return -1;
  }

  if ((yyin = fopen(argv[1], "r")) == NULL) {
    printf("\nCannot open file %s.\n", argv[1]);
    return -1;
  }

  /*int lxm;
  while (lxm = yylex()) cout << token_to_str(lxm);*/

  yyparse();
  ostringstream ast;
  /*if (programBlock) {
    Printer visitor(ast);
    visitor.VisitBlock(*programBlock);
    cout << ast.str();
  }*/
  CodeGenContext context;
  context.generateCode(programBlock);
  context.runCode();

  fclose(yyin);
  return 0;
}
