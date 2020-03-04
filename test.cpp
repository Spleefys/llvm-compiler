#include "gtest/gtest.h"
#include "extra.h"
#include "lexer.h"
#include "parser.h"

extern FILE *yyin;
char *LFILE;

class CTest : public ::testing::Test {
};


TEST(CTest, LexerTest1)
{ 
  ASSERT_TRUE((yyin = fopen(LFILE, "r")) != NULL);
  int lxm = yylex();

  GTEST_ASSERT_EQ(token_to_str(lxm), 
                  "\x1B[0;32mType: PR_BEGIN, Value: '<?php', Loc: <1, 1>\n");
  fclose(yyin);
}

TEST(CTest, LexerTest2)
{
  ASSERT_TRUE((yyin = fopen(LFILE, "r")) != NULL);
  int lxm = yylex();

  GTEST_ASSERT_EQ(token_to_str(lxm), 
                  "\x1B[0;32mType: IF, Value: 'if', Loc: <3, 1>\n");
  
  fclose(yyin);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
  
  if (argc < 2) {
    printf("Ops!:(\n");
    return -1;
  }
  LFILE = argv[1];

	return RUN_ALL_TESTS();
}