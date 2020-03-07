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
  if (argc < 3) {
    printf("Usage: make [exmpl | exmpp | exmpi]\n");
    printf("Modes ::= [ --l | --p | --i ]\n");
    return -1;
  }

  if ((yyin = fopen(argv[2], "r")) == NULL) {
    printf("\nCannot open file %s.\n", argv[1]);
    return -1;
  }
  if (strcmp(argv[1], "--l") == 0) {
    int lxm;
    while (lxm = yylex()) cout << token_to_str(lxm);
    fclose(yyin);
    return 0;
  } else if (strcmp(argv[1], "--p") == 0) {
    yyparse();
    ostringstream ast;
    if (programBlock) {
      Printer visitor(ast);
      visitor.VisitBlock(*programBlock);
      cout << ast.str();
    }
    fclose(yyin);
    return 0;
  } else if (strcmp(argv[1], "--i") == 0) {
    yyparse();
    CodeGenContext context;
    context.generateCode(programBlock);
    context.runCode();
    fclose(yyin);
    return 0;
  }
  return 0;
}
