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

structdef      : TOK_STRUCT TOK_IDENT '{' '}'                { $$ = $1->adopt($2)}
               | TOK_STRUCT TOK_IDENT '{' fielddecl ';' '}'  { $$ = $1->adopt($2, $4) }
               | TOK_STRUCT TOK_IDENT '{' fielddeclarray '}' { $$ = $1->adopt($2, $4) }
               ;

fielddeclarray : fielddeclarray fielddecl ';' { $$ = $1->adopt($2) }
               | fielddecl ';' fielddecl ';'  { $$ = TOK_ROOT->adopt($1, $3) }
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

