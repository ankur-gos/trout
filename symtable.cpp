/**
 * symtable.cpp
 * symtable function implementations
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include "symtable.h"
#include "lyutils.h"

static void symbol::insert_struct(symbol_table &struct_st, location lloc)
{
    if (next_block != 1)
    {
        // TODO: Handle error
    }
    string ident;
    bool flag = true;
    for (auto child : at->children)
    {
        if (flag)
        {
            ident = child->lexinfo;
            flag = false;
            auto struct_sym = new symbol();
            struct_sym->attributes[ATTR_struct] = true;
            struct_sym->block_nr = 0;
            struct_sym->location = at->location;
            struct_sym->fields = nullptr;
            struct_st[ident] = struct_sym;
            continue;
        }
        // If the child token is a field, add it to the field table
        auto field_sym = new symbol();
        field_sym->attributes[ATTR_field] = true;
        field_sym->attributes[ATTR_lval] = true;
        // Integer is the only primitive type
        if (child->symbol == TOK_INT)
        {
            field_sym->attributes[vreg] = true;
        }
        else
        {
            field_sym->attributes[vaddr] = true;
        }
        field_sym->block_nr = 0;
        field_sym->location = child->location;

        astree *child_child_node = child->children[0];
        assert(child_child_node);
        string child_name = child_child_node->lexinfo;
        symbol *str = struct_st[ident];
        str->fields[child_name] = field_sym;
    }
}

static void symbol::parse_astree(symbol_table &st, symbol_table &struct_st, astree *at)
{
    // Post-order traversal, left node is first in vector
    if (!at->children.empty())
    {
        for (auto child : at->children)
        {
            parse_astree(symbol_table, child);
        }
    }

   switch (at->symbol) {
      case TOK_STRUCT:
         insert_struct(struct_st, at);
      break;
   }
}
