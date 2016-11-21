/**
 * symtable.cpp
 * symtable function implementations
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include "symtable.h"
#include "lyutils.h"
#include "assert.h"

string get_attributes(symbol *sym);

int next_block = 1;

static bool occurs(symbol_table st, const string* key){
    auto found = st.find(key);
    return found == st.end();
} 

static void assign_attributes(symbol* sym, astree* type_ast, symbol_table struct_st){
        if (type_ast->symbol == TOK_INT)
        {
            sym->attributes[ATTR_int] = true;
            sym->attributes[ATTR_vreg] = true;
        }
        else
        {
            sym->attributes[ATTR_vaddr] = true;
            if (type_ast->symbol == TOK_IDENT){
                sym->attributes[ATTR_struct] = true;
                sym->struct_name = type_ast->lexinfo;
                if (!occurs(struct_st, type_ast->lexinfo)){
                    // TODO: Fail here
                    cout << "STRUCT NOT FOUND" << endl;
                }
            }
            if (type_ast->symbol == TOK_STRING)
                sym->attributes[ATTR_string] = true;
            if (type_ast->symbol == TOK_ARRAY){
                sym->attributes[ATTR_array] = true;
                astree* type_child = type_ast->children[0];
                assert(type_child);
                switch(type_child->symbol){
                case TOK_IDENT:
                    // Field value is a struct, check it's in the table
                    if (!occurs(struct_st, type_ast->lexinfo)){
                        // TODO: Fail here
                        cout << "STRUCT NOT FOUND" << endl;
                    }
                    sym->attributes[ATTR_struct] = true;
                    sym->struct_name = type_child->lexinfo;
                    break;
                case TOK_STRING:
                    sym->attributes[ATTR_string] = true;
                    break;
                case TOK_INT:
                    sym->attributes[ATTR_int] = true;
                }
            }
        }
}

void new_block (vector<symbol_table*> &st) {
   next_block++;
   st.push_back(nullptr);
}

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
        field_sym->attributes[ATTR_field] = true;
        field_sym->attributes[ATTR_lval] = true;
        field_sym->block_nr = 0;
        field_sym->lloc = child->lloc;
        field_sym->struct_name = child->lexinfo;
        assign_attributes(field_sym, child, struct_st);
        
        astree *child_child_node;
        if(child->symbol == TOK_ARRAY)
            child_child_node = child->children[1];
        else
            child_child_node = child->children[0];
        assert(child_child_node);
        const string* child_name = child_child_node->lexinfo;
        symbol *str = struct_st[ident];
        if(!str->fields){
            str->fields = new symbol_table();
        }
        (*str->fields)[child_name] = field_sym;
    }
}

void insert_variable(FILE *file, vector<symbol_table*> &st, symbol_table struct_st, astree *at){
    if(st.back() == nullptr){
        st.pop_back();
        st.push_back(new symbol_table());
    }
    auto sym = new symbol();
    symbol_table symtbl = *st.back();
    auto type_child = at->children[0];
    // Check if the child is already in the symbol table
    auto val_child = type_child->children[0];
    if(occurs(symtbl, val_child->lexinfo)){
        cout << "WOOOPS VARIABLE ALREADY DECLARED!!" << endl;
        exit(-2);
    }

    sym->block_nr = next_block - 1;
    sym->lloc = at->lloc;
    assign_attributes(sym, type_child, struct_st);
    sym->attributes[ATTR_variable] = true;
    symtbl[val_child->lexinfo] = sym;
    fprintf(file, "%*s", (int)sym->block_nr, "");
    fprintf(file, "%s (%zd.%zd.%zd)", val_child->lexinfo->c_str(), sym->lloc.filenr, sym->lloc.linenr, sym->lloc.offset);
    fprintf(file, "{%zd} %s\n", sym->block_nr, get_attributes(sym).c_str());
}

void symbol::parse_astree(FILE* file, vector<symbol_table*> &st, symbol_table &struct_st, astree *at)
{
    switch (at->symbol) {
        case TOK_STRUCT:
            insert_struct(struct_st, at);
            break;
        case TOK_FUNCTION:
            new_block(st);
            break;
        case TOK_VARDECL:
            insert_variable(file, st, struct_st, at);
            break;
        case TOK_BLOCK:
            new_block(st);
            break;
    }

    // pre-order traversal, left node is first in vector
    if (!at->children.empty())
    {
        for (auto child : at->children)
        {
            parse_astree(file, st, struct_st, child);
        }
    }

    if (at->symbol == TOK_FUNCTION || at->symbol == TOK_BLOCK) {
        next_block--;
        st.pop_back();
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
        build = build + "\" ";
    }
    if(abit[ATTR_array]){
        build = build + "[] ";
    }
    if(abit[ATTR_variable])
        build = build + "variable ";
    if(abit[ATTR_lval]){
        build = build + "lval ";
    }
    return build;
}

void symbol::print_structtable(FILE* file, symbol_table st){
    for (auto val: st){
        auto sym = val.second;
        fprintf(file, "%s (%zd.%zd.%zd)", val.first->c_str(), sym->lloc.filenr, sym->lloc.linenr, sym->lloc.offset);
        fprintf(file, "{%zd} %s\n", sym->block_nr, get_attributes(sym).c_str());
        auto fields = *sym->fields;
        cout << fields.size() << endl;
        for(auto field: fields){
            fprintf(file, "   ");
            auto field_sym = field.second;
            fprintf(file, "%s (%zd.%zd.%zd)", field.first->c_str(), field_sym->lloc.filenr, field_sym->lloc.linenr, field_sym->lloc.offset);
            fprintf(file, " field {%s} %s\n", val.first->c_str(), get_attributes(field_sym).c_str());
        }
    }
}

