#include "extra.h"

extern int yylineno;
extern char *yytext;
extern int yyleng;
extern int yycolumn;
extern const char *yytname;
extern const char *get_token_name(int t);

std::string token_to_str(int lxm) {
  return std::string (GREEN)+"Type: "+std::string(get_token_name(lxm))+", Value: '"+yytext+
          "', Loc: <"+std::to_string(yylineno)+", "+std::to_string(yycolumn - yyleng)+">\n";
}