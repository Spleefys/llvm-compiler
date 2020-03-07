%code requires {
  #include<string>
  #include<vector>
  #include "node.h"
}

%{
#include "node.h"
#include "extra.h"
NBlock *programBlock;

extern int yylineno;
extern int yycolumn;
extern int yyleng;
extern int yylex();
void yyerror(const char *message);
%}

%output  "parser.cpp"
%defines "parser.h"

%union {
  Node *node;
  NBlock *block;
  NIdentifier *ident;
  NArray *array;
  std::string *string;
  int token;
}

%token-table
%token<token> PR_BEGIN PR_END
%token<string> VARID FUNID INTEGER STRING
%token<token> NEQ EQ LT GT LE GE
%token<token> L_PARENTHESIS R_PARENTHESIS L_BRACKETS R_BRACKETS L_BRACE R_BRACE
%token<token> ADD SUB MUL DIV
%token<token> ASSIGN COMMA SEMI
%token<token> IF THEN ELSE WHILE BREAK FUNCTION RETURN

%left ADD SUB
%left MUL DIV

%start program

%type <block> program stmts
%type <array> array

%type <node> ident assignment
%type <node> literals expr
%type <node> methodcall
%type <node> stmt
%type <node> arrid index
%type <node> math factor
%type <node> ifstmt loop cond
%type <node> return

%%

program : PR_BEGIN PR_END { programBlock = new NBlock(); }
        | PR_BEGIN stmts PR_END { programBlock = $2; }
        ;
  
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($1); }
      | stmts stmt { $1->statements.push_back($2); }
      ;

stmt  : assignment SEMI
      | methodcall SEMI
      | ifstmt
      | loop
      | return SEMI
      ;

ifstmt : IF L_PARENTHESIS cond R_PARENTHESIS stmt { $$ = new NIf(*$3, *$5); }
       | IF L_PARENTHESIS cond R_PARENTHESIS L_BRACE stmts R_BRACE { $$ = new NIf(*$3, *$6); }
       | IF L_PARENTHESIS cond R_PARENTHESIS L_BRACE stmts R_BRACE ELSE L_BRACE stmts R_BRACE { $$ = new NIf(*$3, *$6, $10); }
       ;

loop  : WHILE L_PARENTHESIS cond R_PARENTHESIS L_BRACE stmts R_BRACE { $$ = new NLoop(*$3, *$6); }
      ;

methodcall : ident factor { $$ = new NMethodCall(*$1, *$2);  }
            ;

return : RETURN factor{ $$ = new NReturn($2); }
        ;

expr  : factor
      | L_BRACKETS array R_BRACKETS { $$ = $2; }
      ;

assignment : ident ASSIGN expr { $$ = new NAssignment(*$1, *$3); }
            ;

factor : ident
       | literals
       | math
       | arrid
       ;

array : factor { $$ = new NArray(); $$->data.push_back($1); } 
      | array COMMA factor { $1->data.push_back($3); }
      ;

cond  : factor LT factor { $$ = new NCond(*$1, *$3, "<"); }
      | factor GT factor { $$ = new NCond(*$1, *$3, ">"); } 
      | factor LE factor { $$ = new NCond(*$1, *$3, "<="); }
      | factor GE factor { $$ = new NCond(*$1, *$3, ">="); }
      | factor EQ factor { $$ = new NCond(*$1, *$3, "=="); }
      | factor NEQ factor { $$ = new NCond(*$1, *$3, "!="); }
      ;

math: factor ADD factor { $$ = new NMath(*$1, *$3, "+"); }
    | factor SUB factor { $$ = new NMath(*$1, *$3, "-"); }
    | factor MUL factor { $$ = new NMath(*$1, *$3, "*"); }
    | factor DIV factor { $$ = new NMath(*$1, *$3, "/"); }
    | L_PARENTHESIS factor R_PARENTHESIS {$$ = $2;};
    ;

arrid : ident L_BRACKETS index R_BRACKETS { $$ = new NArrid(*$1, *$3); };

literals  : INTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
          | STRING { $$ = new NString(*$1); delete $1; }
          ;

index : literals | ident;

ident : VARID { $$ = new NIdentifier(*$1); delete $1; }
      | FUNID { $$ = new NIdentifier(*$1); delete $1; }
      ;

%%

void yyerror(const char *s) { printf("\033[0;31mparser_error: %s, loc<%d:%d>\n", s, yylineno, yycolumn); }

const char* get_token_name(int t) { return yytname[YYTRANSLATE(t)]; }