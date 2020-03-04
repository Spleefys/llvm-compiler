%{
#include "extra.h"
#include "parser.h"

#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)

/* location */
extern int yylineno;
int yycolumn = 1;
void add_column(int yyleng);

/* error message */
void lexer_error(const char *message);
%}

%option noyywrap nodefault yylineno
%option outfile="lexer.cpp" header-file="lexer.h"

digit      [0-9]
letter     [a-zA-Z]
ws         [ \t]
vid        ("$")("_"|{letter})("_"|{letter}|{digit})*
fid        ("_"|{letter})("_"|{letter}|{digit})*
sign       [-+]
comment    "//".*
string     \"[^\"\n]*\"
integer    (0|([1-9]{digit}*))

%%

[\n]        { yycolumn = 1; }
{ws}+       { add_column(yyleng); }
{comment}   ;
";"         { add_column(yyleng); return TOKEN(SEMI); }
"+"         { add_column(yyleng); return TOKEN(ADD); }
"-"         { add_column(yyleng); return TOKEN(SUB); }
"*"         { add_column(yyleng); return TOKEN(MUL); }
"/"         { add_column(yyleng); return TOKEN(DIV); }
"!="        { add_column(yyleng); return TOKEN(NEQ); }
"=="        { add_column(yyleng); return TOKEN(EQ); }
"<"         { add_column(yyleng); return TOKEN(LT); }
">"         { add_column(yyleng); return TOKEN(GT); }
"<="        { add_column(yyleng); return TOKEN(LE); }
">="        { add_column(yyleng); return TOKEN(GE); }
"("         { add_column(yyleng); return TOKEN(L_PARENTHESIS); }
")"         { add_column(yyleng); return TOKEN(R_PARENTHESIS); }
"["         { add_column(yyleng); return TOKEN(L_BRACKETS); }
"]"         { add_column(yyleng); return TOKEN(R_BRACKETS); }
"{"         { add_column(yyleng); return TOKEN(L_BRACE); }
"}"         { add_column(yyleng); return TOKEN(R_BRACE); }
"="         { add_column(yyleng); return TOKEN(ASSIGN); }
","         { add_column(yyleng); return TOKEN(COMMA); }
"if"        { add_column(yyleng); return TOKEN(IF); }
"then"      { add_column(yyleng); return TOKEN(THEN); }
"else"      { add_column(yyleng); return TOKEN(ELSE); }
"while"     { add_column(yyleng); return TOKEN(WHILE); }
"break"     { add_column(yyleng); return TOKEN(BREAK); }
"function"  { add_column(yyleng); return TOKEN(FUNCTION); }
"return"    { add_column(yyleng); return TOKEN(RETURN); }
{fid}        { add_column(yyleng); SAVE_TOKEN; return FUNID; }
{vid}        { add_column(yyleng); SAVE_TOKEN; return VARID; }
{integer}   { add_column(yyleng); SAVE_TOKEN; return INTEGER; }
{string}    { add_column(yyleng); SAVE_TOKEN; return STRING; }

"<?php"     { add_column(yyleng); return TOKEN(PR_BEGIN); }
"?>"        { add_column(yyleng); return TOKEN(PR_END); }

.           { lexer_error("unknown literal"); yyterminate(); }

%%

void lexer_error(const char* message){
  printf("\033[0;31mlexer_error: %s, loc<%d:%d>\n", message, yylineno, yycolumn);
}

void add_column(int yyleng) {
  yycolumn += yyleng;
}