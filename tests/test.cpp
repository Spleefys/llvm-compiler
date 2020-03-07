#include "../extra.h"
#include "../lexer.h"
#include "../node.h"
#include "../parser.h"
#include "../printer.h"
#include "gtest/gtest.h"

extern FILE *yyin;

using namespace std;

extern NBlock *programBlock;
extern int yyparse();
extern int yylex();

class CTest : public ::testing::Test {};

/*TEST(CTest, LexerTest) {
  ASSERT_TRUE((yyin = fopen("tests/test.php", "r")) != NULL);
  int lxm = yylex();
  GTEST_ASSERT_EQ(token_to_str(lxm),
                  "\x1B[0;32mType: PR_BEGIN, Value: '<?php', Loc: <1, 1>\n");
  lxm = yylex();
  GTEST_ASSERT_EQ(token_to_str(lxm),
                  "\x1B[0;32mType: VARID, Value: '$a', Loc: <2, 3>\n");
  fclose(yyin);
}*/

TEST(CTest, ParserTest) {
  ASSERT_TRUE((yyin = fopen("tests/test.php", "r")) != NULL);
  yyparse();
  ostringstream ast;
  if (programBlock) {
    Printer visitor(ast);
    visitor.VisitBlock(*programBlock);
    GTEST_ASSERT_EQ(ast.str(),
                    "Visit Block\nVisit Assignment (Visit VarIdentifier a = "
                    "Visit Integer 4)\nEnd Of Assignment\nEnd Of Block\n");
  }
  fclose(yyin);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}