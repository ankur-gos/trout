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

astree* astree::adopt_children (astree* node) {
    for(auto c: node->children){
        children.push_back (c);
    }
    return this;
}

astree* astree::adopt_front (astree* node) {
    children.insert(children.begin(), node);
    return this;
}

astree* astree::generate_root (){
    return new astree(TOK_ROOT, {0, 0, 0}, "");
}

astree* astree::generate_function_tree (astree* identdecl, astree* paramlist, astree* block) {
    astree* func;
    // Check if it's a function prototype
    if (block->symbol == ';') {
         func = new astree(TOK_PROTOTYPE, identdecl->lloc, "");
         return func.adopt(identdecl, paramlist);
    } else {
         func = new astree(TOK_FUNCTION, identdecl->lloc, "");
         func.adopt(identdecl, paramlist);
         return func.adopt(block);
    }
}

astree* astree::struct_empty_arg (astree* ident, astree* lb, astree* rb){
    destroy(lb, rb);
    ident->symbol = TOK_TYPEID;
    return this->adopt(ident);
}

astree* astree::struct_arg(astree* ident, astree* lb, astree* stmt, astree* sc; astree* rb){
    destroy(lb, sc);
    destroy(rb);
    ident->symbol = TOK_TYPEID;
    return this->adopt(ident, stmt);
}

astree* astree::struct_mult_args(astree* ident, astree* lb, astree* fdeclarray, astree* rb){
    destroy(lb, rb);
    ident->symbol = TOK_TYPEID;
    this->adopt(ident);
    this->adopt_children(fdeclarray);
    destroy(fdeclarray);
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
