// $Id: astree.h,v 1.7 2016-10-06 16:13:39-07 - - $

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;

#include "auxlib.h"

struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node

   // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt (astree* child1, astree* child2 = nullptr);
   astree* adopt_sym (astree* child, int symbol);
   astree* adopt_2_sym (astree* child1, astree* child2, int sym);
   astree* adopt_children (astree* node);
   astree* adopt_front(astree* child, astree* dest);
   astree* struct_empty_arg(astree* ident, astree* lb, astree* rb, int sym);
   astree* struct_arg(astree* ident, astree* lb, astree* stmt, astree* sc, astree* rb, int sym);
   astree* struct_mult_args(astree* ident, astree* lb, astree* argarray, astree* rb, int sym);
   astree* destroy_adopt(astree* destroy, astree* a1, astree* a2 = nullptr);
   astree* destroy_sym_adopt(astree* destroy, int sym, astree* child1, astree* child2 = nullptr);
   astree* fn(astree* lp, astree* rp, astree* block, int sym, int prototok, int functok);
   astree* adopt_child_sym(astree* d1, astree* d2, int sym, astree* child1, astree* child2 = nullptr);
   astree* adopt_child_2_sym(int sym, astree* child1, astree* child2);
   astree* destroy_paren(astree* lp, astree* rp);
   astree* destroy_2_adopt(astree* d1, astree* d2, astree* child1, astree* child2);
   astree* destroy_3_sym_adopt_3(astree* d1, astree* d2, astree* d3, int sym, astree* child1, astree* child2, astree* child3);
   static astree* generate_root();
   static astree* generate_function_tree(astree* identdecl, astree* paramlist, astree* block, int prototok, int functok);
   void dump_node (FILE*);
   void dump_tree (FILE*, int depth = 0);
   static void dump (FILE* outfile, astree* tree);
   static void print (FILE* outfile, astree* tree, int depth = 0);
};

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif

