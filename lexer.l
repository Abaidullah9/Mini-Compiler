%{
#include "ast.h"
#include "parser.tab.h"
#include <string.h>
#include <iostream>
#include <vector>

int line_num = 1;
int lex_errors = 0;
std::vector<TokenInfo> gTokenStream;

static void logToken(const char* name) {
    gTokenStream.push_back({name, yytext, line_num});
}

void lex_error(const char* msg) {
    std::cerr << "[Lexical Error] Line " << line_num << ": " << msg << " '" << yytext << "'\n";
    lex_errors++;
}
%}

%option noyywrap

%%

"//".* { /* Single line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Block comment */ }

[ \t\r]+                { /* whitespace */ }
\n                      { line_num++; }

"int"                   { logToken("T_INT"); return T_INT; }
"float"                 { logToken("T_FLOAT"); return T_FLOAT; }
"bool"                  { logToken("T_BOOL"); return T_BOOL; }
"void"                  { logToken("T_VOID"); return T_VOID; }
"if"                    { logToken("T_IF"); return T_IF; }
"else"                  { logToken("T_ELSE"); return T_ELSE; }
"while"                 { logToken("T_WHILE"); return T_WHILE; }
"for"                   { logToken("T_FOR"); return T_FOR; }
"return"                { logToken("T_RETURN"); return T_RETURN; }
"input"                 { logToken("T_INPUT"); return T_INPUT; }
"output"                { logToken("T_OUTPUT"); return T_OUTPUT; }

"true"|"false"          { yylval.str = strdup(yytext); logToken("T_BOOL_LITERAL"); return T_BOOL_LITERAL; }
[0-9]+\.[0-9]+          { yylval.str = strdup(yytext); logToken("T_FLOAT_LITERAL"); return T_FLOAT_LITERAL; }
[0-9]+                  { yylval.str = strdup(yytext); logToken("T_INT_LITERAL"); return T_INT_LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); logToken("T_ID"); return T_ID; }

"=="                    { yylval.str = strdup(yytext); logToken("T_EQ"); return T_EQ; }
"!="                    { yylval.str = strdup(yytext); logToken("T_REL_OP"); return T_REL_OP; }
"<"|">"|"<="|">="       { yylval.str = strdup(yytext); logToken("T_REL_OP"); return T_REL_OP; }
"&&"                    { yylval.str = strdup(yytext); logToken("T_AND"); return T_AND; }
"||"                    { yylval.str = strdup(yytext); logToken("T_OR"); return T_OR; }
"++"                    { yylval.str = strdup(yytext); logToken("T_INC"); return T_INC; }
"--"                    { yylval.str = strdup(yytext); logToken("T_DEC"); return T_DEC; }
"!"                     { yylval.str = strdup(yytext); logToken("T_NOT"); return T_NOT; }
"+"|"-"                 { yylval.str = strdup(yytext); logToken("T_ADD_OP"); return T_ADD_OP; }
"*"|"/"                 { yylval.str = strdup(yytext); logToken("T_MUL_OP"); return T_MUL_OP; }
"+="                    { logToken("T_PLUS_ASSIGN"); return T_PLUS_ASSIGN; }
"-="                    { logToken("T_MINUS_ASSIGN"); return T_MINUS_ASSIGN; }
"*="                    { logToken("T_MUL_ASSIGN"); return T_MUL_ASSIGN; }
"/="                    { logToken("T_DIV_ASSIGN"); return T_DIV_ASSIGN; }
"="                     { logToken("T_ASSIGN"); return T_ASSIGN; }

";"                     { logToken("T_SEMI"); return T_SEMI; }
","                     { logToken("T_COMMA"); return T_COMMA; }
"("                     { logToken("T_LPAREN"); return T_LPAREN; }
")"                     { logToken("T_RPAREN"); return T_RPAREN; }
"{"                     { logToken("T_LBRACE"); return T_LBRACE; }
"}"                     { logToken("T_RBRACE"); return T_RBRACE; }

[0-9]+[a-zA-Z_]+        { lex_error("Invalid identifier format"); }
.                       { lex_error("Illegal character"); }

%%
