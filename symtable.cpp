/**
 * symtable.cpp
 * symtable function implementations
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include "symtable.h"
#include "lyutils.h"
#include "assert.h"

static void insert_struct(symbol_table &struct_st, astree* at)
{
    if (next_block != 1)
    {
        // TODO: Handle error
    }
    const string* ident;
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
            struct_sym->lloc = at->lloc;
            struct_sym->fields = nullptr;
            struct_sym->struct_name = at->lexinfo;
            struct_st[ident] = struct_sym;
            continue;
        }
        // If the child token is a field, add it to the field table
        auto field_sym = new symbol();
        cout << "HELLO" << endl;
        field_sym->attributes[ATTR_field] = true;
        field_sym->attributes[ATTR_lval] = true;
        // Integer is the only primitive type
        if (child->symbol == TOK_INT)
        {
            field_sym->attributes[ATTR_vreg] = true;
        }
        else
        {
            field_sym->attributes[ATTR_vaddr] = true;
        }
        field_sym->block_nr = 0;
        field_sym->lloc = child->lloc;
        field_sym->struct_name = child->lexinfo;
        astree *child_child_node = child->children[0];
        assert(child_child_node);
        const string* child_name = child_child_node->lexinfo;
        symbol *str = struct_st[ident];
        if(!str->fields){
            str->fields = new symbol_table();
        }
        (*str->fields)[child_name] = field_sym;
    }
}

void symbol::parse_astree(symbol_table &st, symbol_table &struct_st, astree *at)
{
    // Post-order traversal, left node is first in vector
    if (!at->children.empty())
    {
        for (auto child : at->children)
        {
            parse_astree(st, struct_st, child);
        }
    }

   switch (at->symbol) {
      case TOK_STRUCT:
         insert_struct(struct_st, at);
         break;
      case TOK_FIELD:
         
         break;
   }
}

string get_attributes(symbol *sym){
    auto abit = sym->attributes;
    string build = "";
    if(abit[ATTR_void])
        build = build + "void ";
    if(abit[ATTR_int])
        build = build + "int ";
    if(abit[ATTR_null])
        build = build + "null ";
    if(abit[ATTR_string])
        build = build + "string ";
    if(abit[ATTR_struct]){
        build = build + "struct \"";
        string cpy = *sym->struct_name;
        build = build + cpy;
        build = build + "\"";
    }
    return build;
}

void symbol::print_structtable(FILE* file, symbol_table st){
    for (auto val: st){
        auto sym = val.second;
        fprintf(file, "%s (%zd.%zd.%zd)", val.first->c_str(), sym->lloc.filenr, sym->lloc.linenr, sym->lloc.offset);
        fprintf(file, "{%zd} struct \"%s\"\n", sym->block_nr, val.first->c_str());
        auto fields = *sym->fields;
        for(auto field: fields){
            fprintf(file, "   ");
            auto field_sym = field.second;
            fprintf(file, "%s (%zd.%zd.%zd)", field.first->c_str(), field_sym->lloc.filenr, field_sym->lloc.linenr, field_sym->lloc.offset);
            fprintf(file, " field {%s} %s\n", val.first->c_str(), get_attributes(field_sym).c_str());
        }
    }
}
