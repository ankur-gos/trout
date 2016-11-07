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
%token TOK_RETURNVOID
%token TOK_FUNCTION TOK_PARAMLIST

%initial-action {
   parser::root = astree::generate_function_tree();
}

%start start

%%

start          : program               { $$ = parser::root.adopt($1); }
               ;

program        : program structdef     { $$ = $1->adopt($2) }
               | program function      { $$ = $1->adopt($2) }
               | program statement     { $$ = $1->adopt($2) }
               |                       { $$ = astree::dump(stderr, parser::root) }
               ;

structdef      : TOK_STRUCT TOK_IDENT '{' '}'                { destroy($3); destroy($4); $2->symbol = TOK_TYPEID; $$ = $1->adopt($2);     }
               | TOK_STRUCT TOK_IDENT '{' fielddecl ';' '}'  { destroy($3); destroy($5); destroy($6); $2->symbol = TOK_TYPEID; $$ = $1->adopt($2, $4); }
               | TOK_STRUCT TOK_IDENT '{' fielddeclarray '}' { destroy($3); destroy($5); $2->symbol = TOK_TYPEID; $1->adopt($2); $1->adopt_children($4); destroy($4); $$ = $1; }
               ;

fielddeclarray : fielddeclarray fielddecl ';' { destroy($3); $$ = $1->adopt($2) }
               | fielddecl ';' fielddecl ';'  { destroy($4); $$ = $2->adopt($1, $3) }
               ;

fielddecl      : basetype TOK_FIELD           { $$ = $1->adopt($2) }
               | basetype TOK_ARRAY TOK_FIELD { $$ = $2->adopt($1, $3) }
               ;

basetype       : TOK_VOID   { $$ = $1 }
               | TOK_INT    { $$ = $1 }
               | TOK_STRING { $$ = $1 }
               | TOK_IDENT  { $$ = $1 }
               ;

function       : identdecl '(' ')' block            { destroy($3); $2->symbol = TOK_PARAMLIST; $$ = astree::generate_function_tree($1, $2, $4)}
               | identdecl '(' identdecl ')' block  { destroy($4); $2->symbol = TOK_PARAMLIST; $$ = astree::generate_function_tree($1, $2.adopt($3), $5)}

indentdecl     : basetype TOK_IDENT                 { $$ = $1->adopt($2) }
               | basetype TOK_ARRAY TOK_IDENT       { $$ = $2->adopt($1, $3)}
               ;

block          : '{' '}'                { $$ = $1->adopt_sym(nullptr, TOK_BLOCK) }
               | ';'                    { $$ = $1 }
               | '{' statement '}'      { $$ = $1->adopt_sym($2, TOK_BLOCK) }
               | statementarray '}'     { $$ = $1 }
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

vardecl        : indentdecl '=' expr ';' { $2->symbol = TOK_VARDECL; $$ = $2.adopt($1, $3); }
               ;

while          : TOK_WHILE '(' expr ')' statement { $$ = $1.adopt($3, $5); }
               ;

ifelse         : TOK_IF '(' expr ')' statement { $$ = $1.adopt($3, $5); }
               | TOK_IF '(' expr ')' statement TOK_ELSE statement { $1->symbol = TOK_IFELSE; $1.adopt($3, $5); $$ = $1.adopt($7); }
               ;

return         : TOK_RETURN ';' { $$ = $1.adopt_sym(nullptr, TOK_RETURNVOID); }
               | TOK_RETURN expr ';' { $$ = $1.adopt($2); }
               ;

expr           : expr '=' expr          { $$ = $2.adopt($1, $3); }
               | expr TOK_EQ expr       { $$ = $2.adopt($1, $3); }
               | expr TOK_GT expr       { $$ = $2.adopt($1, $3); }
               | expr TOK_GE expr       { $$ = $2.adopt($1, $3); }
               | expr TOK_LT expr       { $$ = $2.adopt($1, $3); }
               | expr TOK_LE expr       { $$ = $2.adopt($1, $3); }
               | expr TOK_NE expr       { $$ = $2.adopt($1, $3); }
               | expr '+' expr          { $$ = $2.adopt($1, $3); }
               | expr '-' expr          { $$ = $2.adopt($1, $3); }
               | expr '*' expr          { $$ = $2.adopt($1, $3); }
               | expr '/' expr          { $$ = $2.adopt($1, $3); }
               | expr '%' expr          { $$ = $2.adopt($1, $3); }
               | expr '^' expr          { $$ = $2.adopt($1, $3); }
               | '+' expr               { $$ = $1.adopt_sym($2, TOK_POS); }
               | '-' expr               { $$ = $1.adopt_sym($2, TOK_NEG); }
               | allocator              { $$ = $1; }
               | call                   { $$ = $1; }
               | '(' expr ')'           { $$ = $2; }
               | variable               { $$ = $1; }
               | constant               { $$ = $1; }
               ;
             
allocator      : TOK_NEW TOK_IDENT '(' ')'      { $2->symbol = TOK_TYPEID; $$ = $1.adopt($2); }
               | TOK_NEW TOK_STRING '(' ')'     { $2->symbol = TOK_NEWSTRING; $$ = $1..adopt($2); }
               | TOK_NEW basetype   '[' expr ']'{ $2->symbol = TOK_NEWARRAY; $$ = $1.adopt($2, $4); }
               ;
              
call           : TOK_IDENT '(' ')'      { $$ = $2.adopt_sym($1, TOK_CALL); }
               | TOK_IDENT '(' expr ')' { $2.adopt_sym($1, TOK_CALL); $$ = $2.adopt($3); }
               | TOK_IDENT arglist ')'  { $$ = $2->adopt_front($1); }
               ;

arglist        : arglist ',' expr       { $$ = $1.adopt($3); }
               : '(' expr ',' expr      { $1->symbol = TOK_CALL; $$ = $1.adopt($2, $4);}
               ;

variable       : TOK_IDENT
               | expr '[' expr ']'
               | expr '.' TOK_IDENT
               ;

constant       : TOK_INTCON
               | TOK_CHARCON
               | TOK_STRINGCON
               | TOK_NULL
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

