// $Id: astree.cpp,v 1.8 2016-09-21 17:13:03-07 - - $

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "stringset.h"
#include "lyutils.h"

astree::astree (int symbol_, const location& lloc_, const char* info) {
   symbol = symbol_;
   lloc = lloc_;
   lexinfo = stringset::intern_stringset(info);
   // vector defaults to empty -- no children
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      fprintf (stderr, "Deleting astree (");
      astree::dump (stderr, this);
      fprintf (stderr, ")\n");
   }
}

astree* astree::adopt (astree* child1, astree* child2) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   return this;
}

astree* astree::adopt_sym (astree* child, int symbol_) {
   symbol = symbol_;
   return adopt (child);
}

astree* astree::adopt_2_sym (astree* child1, astree* child2, int sym){
    symbol = sym;
    return adopt(child1, child2);
}

astree* astree::adopt_children (astree* node) {
    for(auto c: node->children){
        children.push_back (c);
    }
    return this;
}

astree* astree::adopt_front (astree* node, astree* dest) {
    destroy(dest);
    children.insert(children.begin(), node);
    return this;
}

astree* astree::generate_root (){
    return new astree(TOK_ROOT, {0, 0, 0}, "");
}

astree* astree::generate_function_tree (astree* identdecl, astree* paramlist, astree* block, int prototok, int functok) {
    astree* func;
    // Check if it's a function prototype
    if (block->symbol == ';') {
         func = new astree(prototok, identdecl->lloc, "");
         return func->adopt(identdecl, paramlist);
    } else {
         func = new astree(functok, identdecl->lloc, "");
         func->adopt(identdecl, paramlist);
         return func->adopt(block);
    }
}

astree* astree::struct_empty_arg (astree* ident, astree* lb, astree* rb, int sym){
    destroy(lb, rb);
    ident->symbol = sym;
    return adopt(ident);
}

astree* astree::struct_arg(astree* ident, astree* lb, astree* stmt, astree* sc, astree* rb, int sym){
    destroy(lb, sc);
    destroy(rb);
    ident->symbol = sym;
    return adopt(ident, stmt);
}

astree* astree::struct_mult_args(astree* ident, astree* lb, astree* fdeclarray, astree* rb, int sym){
    destroy(lb, rb);
    ident->symbol = sym; 
    adopt(ident);
    adopt_children(fdeclarray);
    destroy(fdeclarray);
    return this;
}

astree* astree::fn(astree* lp, astree* rp, astree* block, int sym, int prototok, int functok){
    destroy(rp);
    lp->symbol = sym;
    return astree::generate_function_tree(this, lp, block, prototok, functok);
}

astree* astree::adopt_child_sym(int sym, astree* d1, astree* d2, astree* child1, astree* child2 ){
    destroy(d1, d2);
    child1->symbol = sym;
    return adopt(child1, child2);
}

astree* astree::destroy_adopt(astree* dest, astree* a1, astree* a2) {
    destroy(dest);
    return adopt(a1, a2);
}

astree* astree::destroy_sym_adopt(astree* dest, int sym, astree* child1, astree* child2) {
    destroy(dest);
    symbol = sym;
    return adopt(child1, child2);
}

astree* astree::destroy_2_adopt(astree* d1, astree* d2, astree* child1, astree* child2){
    destroy(d1, d2);
    return adopt(child1, child2);
}

astree* astree::destroy_3_sym_adopt_3(astree* d1, astree* d2, astree* d3, int sym, astree* child1, astree* child2, astree* child3){
    destroy(d1, d2);
    destroy(d3);
    symbol = sym;
    adopt(child1, child2);
    return adopt(child3);
}

astree* astree::adopt_child_2_sym(int sym, astree* child1, astree* child2){
    child2->symbol = sym;
    return adopt(child1, child2);
}

astree* astree::destroy_paren(astree* lp, astree* rp){
    destroy(lp, rp);
    return this;
}

void astree::dump_node (FILE* outfile) {
   fprintf (outfile, "  %-2zu %2zu.%03zu %3d %-15s (%s)\n",
                     lloc.filenr, lloc.linenr, lloc.offset,
                     symbol, parser::get_tname(symbol),
                     lexinfo->c_str());
   for (size_t child = 0; child < children.size(); ++child) {
      fprintf (outfile, " %p", children.at(child));
   }
}

void astree::dump_tree (FILE* outfile, int depth) {
   fprintf (outfile, "%*s", depth * 3, "");
   dump_node (outfile);
   fprintf (outfile, "\n");
   for (astree* child: children) child->dump_tree (outfile, depth + 1);
   fflush (NULL);
}

void astree::dump (FILE* outfile, astree* tree) {
   if (tree == nullptr) fprintf (outfile, "nullptr");
                   else tree->dump_node (outfile);
}

void astree::print (FILE* outfile, astree* tree, int depth) {
   fprintf (outfile, "; %*s", depth * 3, "");
   fprintf (outfile, "%s \"%s\" (%zd.%zd.%zd)\n",
            parser::get_tname (tree->symbol), tree->lexinfo->c_str(),
            tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
   for (astree* child: tree->children) {
      astree::print (outfile, child, depth + 1);
   }
}

void destroy (astree* tree1, astree* tree2) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
   static char buffer[0x1000];
   assert (sizeof buffer > strlen (format) + strlen (arg));
   snprintf (buffer, sizeof buffer, format, arg);
   errprintf ("%s:%zd.%zd: %s", 
              lexer::filename (lloc.filenr), lloc.linenr, lloc.offset,
              buffer);
}
