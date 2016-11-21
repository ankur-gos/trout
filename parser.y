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
%token TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT
%token TOK_RETURNVOID
%token TOK_FUNCTION TOK_PROTOTYPE TOK_PARAMLIST
%token TOK_VARDECL TOK_NEWSTRING
%token TOK_DECLID

%initial-action {
   parser::root = astree::generate_root(TOK_ROOT);
}

%right TOK_IF TOK_ELSE
%right '='
%left  TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left  '+' '-'
%left  '*' '/' '%'
%right TOK_POS TOK_NEG '!' TOK_NEW
%left TOK_INDEX '.' TOK_CALL

%start start

%%

start          : program               { $$ = $1 = nullptr; }
               ;

program        : program structdef     { $$ = $1->adopt($2); }
               | program function      { $$ = $1->adopt($2); }
               | program statement     { $$ = $1->adopt($2); }
               |                       { $$ = parser::root; }
               ;

structdef      : TOK_STRUCT TOK_IDENT '{' '}'                { $$ = $1->struct_empty_arg($2, $3, $4, TOK_TYPEID); }
               | TOK_STRUCT TOK_IDENT '{' fielddecl ';' '}'  { $$ = $1->struct_arg($2, $3, $4, $5, $6, TOK_TYPEID); }
               | TOK_STRUCT TOK_IDENT '{' fielddeclarray '}' { $$ = $1->struct_mult_args($2, $3, $4, $5, TOK_TYPEID); }
               ;

fielddeclarray : fielddeclarray fielddecl ';' { $$ = $1->destroy_adopt($3, $2); }
               | fielddecl ';' fielddecl ';'  { $$ = $2->destroy_adopt($4, $1, $3); }
               ;

fielddecl      : basetype TOK_IDENT          { $$ = $1->adopt_child_2_sym(TOK_FIELD, nullptr, $2); }
               | basetype TOK_ARRAY TOK_IDENT { $$ = $2->adopt_child_2_sym(TOK_FIELD, $1, $3); }
               ;

basetype       : TOK_VOID   { $$ = $1; }
               | TOK_INT    { $$ = $1; }
               | TOK_STRING { $$ = $1; }
               | TOK_IDENT  { $$ = $1; }
               ;

function       : identdecl '(' ')' block            { $$ = $1->fn($2, $3, $4, TOK_PARAMLIST, TOK_PROTOTYPE, TOK_FUNCTION); }
               | identdecl '(' identdecl ')' block  { $$ = $1->fn($2->adopt($3), $4, $5, TOK_PARAMLIST, TOK_PROTOTYPE, TOK_FUNCTION); }
               | identdecl identdeclarray ')' block { $$ = $1->fn($2, $3, $4, TOK_PARAMLIST, TOK_PROTOTYPE, TOK_FUNCTION); }
               ;
              
identdeclarray : '(' identdecl ',' identdecl { $$ = $1->destroy_adopt($3, $2, $4); }
               | identdeclarray ',' identdecl { $$ = $1->destroy_adopt($2, $3); }

identdecl      : basetype TOK_IDENT                 { $$ = $1->adopt_child_sym(TOK_DECLID, nullptr, nullptr, $2); }
               | basetype TOK_ARRAY TOK_IDENT       { $$ = $2->adopt($1, $3); }
               ;

block          : '{' '}'                { $$ = $1->destroy_sym_adopt($2, TOK_BLOCK, nullptr); }
               | ';'                    { $$ = $1; }
               | '{' statement '}'      { $$ = $1->destroy_sym_adopt($3, TOK_BLOCK, $2); }
               | statementarray '}'     { $$ = $1->destroy_paren($2, nullptr); }
               ;

statement      : block { $$ = $1; }
               | vardecl { $$ = $1; }
               | while { $$ = $1; }
               | ifelse { $$ = $1; }
               | return { $$ = $1; }
               | expr ';' { $$ = $1->destroy_adopt($2, nullptr); }
               ;

statementarray : statementarray statement { $$ = $1->adopt($2); }
               | '{' statement statement  { $$ = $1->destroy_sym_adopt(nullptr, TOK_BLOCK, $2, $3); }
               ;

vardecl        : identdecl '=' expr ';' { $$ = $2->destroy_sym_adopt($4, TOK_VARDECL, $1, $3); }
               ;

while          : TOK_WHILE '(' expr ')' statement { $$ = $1->destroy_2_adopt($2, $4, $3, $5); }
               ;

ifelse         : TOK_IF '(' expr ')' statement { $$ = $1->destroy_2_adopt($2, $4, $3, $5); }
               | TOK_IF '(' expr ')' statement TOK_ELSE statement { $$ = $1->destroy_3_sym_adopt_3($2, $4, $6, TOK_IFELSE, $3, $5, $7); }
               ;

return         : TOK_RETURN ';' { $$ = $1->adopt_sym(nullptr, TOK_RETURNVOID); }
               | TOK_RETURN expr ';' { $$ = $1->adopt($2); }
               ;

expr           : expr '=' expr          { $$ = $2->adopt($1, $3); }
               | expr TOK_EQ expr       { $$ = $2->adopt($1, $3); }
               | expr TOK_GT expr       { $$ = $2->adopt($1, $3); }
               | expr TOK_GE expr       { $$ = $2->adopt($1, $3); }
               | expr TOK_LT expr       { $$ = $2->adopt($1, $3); }
               | expr TOK_LE expr       { $$ = $2->adopt($1, $3); }
               | expr TOK_NE expr       { $$ = $2->adopt($1, $3); }
               | expr '+' expr          { $$ = $2->adopt($1, $3); }
               | expr '-' expr          { $$ = $2->adopt($1, $3); }
               | expr '*' expr          { $$ = $2->adopt($1, $3); }
               | expr '/' expr          { $$ = $2->adopt($1, $3); }
               | expr '%' expr          { $$ = $2->adopt($1, $3); }
               | expr '^' expr          { $$ = $2->adopt($1, $3); }
               | '!' expr               { $$ = $1->adopt($2); }
               | '+' expr               { $$ = $1->adopt_sym($2, TOK_POS); }
               | '-' expr               { $$ = $1->adopt_sym($2, TOK_NEG); }
               | allocator              { $$ = $1; }
               | call                   { $$ = $1; }
               | '(' expr ')'           { $$ = $2->destroy_paren($1, $3); }
               | variable               { $$ = $1; }
               | constant               { $$ = $1; }
               ;
             
allocator      : TOK_NEW TOK_IDENT '(' ')'      { $$ = $1->adopt_child_sym(TOK_TYPEID, $3, $4, $2); }
               | TOK_NEW TOK_STRING '(' ')'     { $$ = $1->adopt_child_sym(TOK_NEWSTRING, $3, $4, $2); }
               | TOK_NEW basetype   '[' expr ']'{ $$ = $1->adopt_child_sym(TOK_NEWARRAY, $3, $5, $2, $4); }
               ;
              
call           : TOK_IDENT '(' ')'      { $$ = $2->adopt_sym($1, TOK_CALL); }
               | TOK_IDENT '(' expr ')' { $$ = $2->adopt_2_sym($1, $3, TOK_CALL); }
               | TOK_IDENT arglist ')'  { $$ = $2->adopt_front($1, $3); }
               ;

arglist        : arglist ',' expr       { $$ = $1->destroy_adopt($2, $3); }
               | '(' expr ',' expr      { $$ = $1->adopt_2_sym($2, $4, TOK_CALL);}
               ;

variable       : TOK_IDENT              { $$ = $1; }
               | expr '[' expr ']'      { $$ = $2->destroy_sym_adopt($4, TOK_INDEX, $1, $3); }
               | expr '.' TOK_IDENT     { $$ = $2->adopt_child_2_sym(TOK_FIELD, $1, $3);}
               ;

constant       : TOK_INTCON     { $$ = $1; }
               | TOK_CHARCON    { $$ = $1; }
               | TOK_STRINGCON  { $$ = $1; }
               | TOK_NULL       { $$ = $1; }
               ;




%%


const char *parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

void yyerror(char *s){
    fprintf(stderr, "%s\n", s);
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

