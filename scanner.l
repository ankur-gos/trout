/* $Id: lexer.l,v 1.6 2016-10-20 13:48:57-07 - - $ */

%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }

#define RETURN(SYMBOL) { \
    yylval = new astree (SYMBOL, lexer::lloc, yytext); \
    return SYMBOL; \
}

%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
NOTIDENT        ({DIGIT}({LETTER}|{DIGIT})*)
CHARCON         ('([^\\'\n]|\\[\\'"0nt])')
STRINGCON       (\"([^\\"\n]|\\[\\"0nt])*\")

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"char"          {  RETURN (TOK_CHAR); }
"else"          {  RETURN (TOK_ELSE); }
"if"            {  RETURN (TOK_IF); }
"int"           {  RETURN (TOK_INT); }
"new"           {  RETURN (TOK_NEW); }
"null"          {  RETURN (TOK_NULL); }
"return"        {  RETURN (TOK_RETURN); }
"string"        {  RETURN (TOK_STRING); }
"struct"        {  RETURN (TOK_STRUCT); }
"void"          {  RETURN (TOK_VOID); }
"while"         {  RETURN (TOK_WHILE); }

{NUMBER}        {  RETURN (TOK_INTCON); }
{IDENT}         {  RETURN (TOK_IDENT); }
{CHARCON}       {  RETURN (TOK_CHARCON); }
{STRINGCON}     {  RETURN (TOK_STRINGCON); }

"=="            {  RETURN (TOK_EQ); }
"!="            {  RETURN (TOK_NE); }
"<="            {  RETURN (TOK_LE); }
">="            {  RETURN (TOK_GE); }
"<"             {  RETURN (TOK_LT); }
">"             {  RETURN (TOK_GT); }
"!"             {  RETURN ('!'); }
"%"             {  RETURN ('%'); }
","             {  RETURN (','); }
"."             {  RETURN ('.'); }
"[]"            {  RETURN (TOK_ARRAY); }
"["             {  RETURN ('['); }
"]"             {  RETURN (']'); }
"{"             {  RETURN ('{'); }
"}"             {  RETURN ('}'); }
"="             {  RETURN ('='); }
"+"             {  RETURN ('+'); }
"-"             {  RETURN ('-'); }
"*"             {  RETURN ('*'); }
"/"             {  RETURN ('/'); }
"^"             {  RETURN ('^'); }
"("             {  RETURN ('('); }
")"             {  RETURN (')'); }
";"             {  RETURN (';'); }

{NOTNUMBER}     { lexer::badtoken (yytext);
                   RETURN (TOK_INTCON); }
{NOTIDENT}      { lexer::badtoken (yytext);
                   RETURN (TOK_IDENT); }
.               { lexer::badchar (*yytext); }

%%

