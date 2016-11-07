%{
// Dummy parser for scanner project.

#include <cassert>

#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT

%start start

%%

start          : program               { parser::root = $1}
               ;

program        : program structdef     { $$ = $1->adopt($2) }
               | program function      { $$ = $1->adopt($2) }
               | program statement     { $$ = $1->adopt($2) }
               |                       { $$ = astree::dump(stderr, parser::root) }
               ;

structdef      : TOK_STRUCT TOK_IDENT '{' '}'                { $2->symbol = TOK_TYPEID; $$ = $1->adopt($2);     }
               | TOK_STRUCT TOK_IDENT '{' fielddecl ';' '}'  { $2->symbol = TOK_TYPEID; $$ = $1->adopt($2, $4); }
               | TOK_STRUCT TOK_IDENT '{' fielddeclarray '}' { $2->symbol = TOK_TYPEID; $1->adopt($2); $1->adopt_children($4) }
               ;

fielddeclarray : fielddeclarray fielddecl ';' { $$ = $1->adopt($2) }
               | fielddecl ';' fielddecl ';'  { $$ = $2->adopt($1, $3) }
               ;

fielddecl      : basetype TOK_FIELD           { $$ = $1->adopt($2) }
               | basetype TOK_ARRAY TOK_FIELD { $$ = $2->adopt($1, $3) }
               ;

basetype       : TOK_VOID   { $$ = $1 }
               | TOK_INT    { $$ = $1 }
               | TOK_STRING { $$ = $1 }
               | TOK_IDENT  { $$ = $1 }
               ;

indentdecl     : basetype TOK_IDENT { $$ = $1->adopt($2) }
               | basetype TOK_ARRAY TOK_IDENT { $$ = $2->adopt($1, $3)}
               ;

block          : '{' '}'                { $$ = $1->adopt_sym(nullptr, TOK_BLOCK) }
               | ';'                    { $$ = $1->adopt_sym(nullptr, TOK_BLOCK) }
               | '{' statement '}'      { $$ = $1->adopt_sym($2, TOK_BLOCK) }
               | statementarray '}' { $$ = $2 }
               ;

statement      : block { $$ = $1 }
               | vardecl { $$ = $1 }
               | while { $$ = $1 }
               | ifelse { $$ = $1 }
               | return { $$ = $1 }
               | expr ';' { $$ = $1 }
               ;

statementarray : statementarray statement { $$ = $1->adopt($2) }
               | '{' statement statement  { $$ = $1->adopt($2, $3) }
               ;

vardecl        : indentdecl '=' expr ';' { $$ = }
               ;

while          : TOK_WHILE '(' expr ')' statement { $$ = }
               ;

ifelse         : TOK_IF '(' expr ')' statement { $$ = }
               | TOK_IF '(' expr ')' statement TOK_ELSE statement { $$ = }
               ;

return         : TOK_RETURN ';' { $$ = }
               | TOK_RETURN expr ';' { $$ = }
               ;

expr           : expr '=' expr
               | expr TOK_EQ expr
               | expr TOK_GT expr
               | expr TOK_GE expr
               | expr TOK_LT expr
               | expr TOK_LE expr
               | expr TOK_NE expr
               | expr '+' expr
               | expr '-' expr
               | expr '*' expr
               | expr '/' expr
               | expr '%' expr
               | expr '^' expr
               | '+' expr
               | '-' expr
               | allocator
               | call
               | '(' expr ')'
               | variable
               | constant




%%


const char *parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

/*
static void* yycalloc (size_t size) {
   void* result = calloc (1, size);
   assert (result != nullptr);
   return result;
}
*/

