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

static bool occurs(symbol_table st, const string *key)
{
    auto found = st.find(key);
    return !(found == st.end());
}

static void assign_attributes(symbol *sym, astree *type_ast, symbol_table struct_st)
{
    if (type_ast->symbol == TOK_VOID){
        cout << "Invalid variable type void." << endl;
        exit(-12);
    }
    if (type_ast->symbol == TOK_INT)
    {
        sym->attributes[ATTR_int] = true;
        sym->attributes[ATTR_vreg] = true;
    }
    else
    {
        sym->attributes[ATTR_vaddr] = true;
        if (type_ast->symbol == TOK_TYPEID)
        {
            sym->attributes[ATTR_struct] = true;
            sym->struct_name = type_ast->lexinfo;
            if (!occurs(struct_st, type_ast->lexinfo))
            {
                // TODO: Fail here
                cout << "Struct " << *type_ast->lexinfo << " not found." << endl;
                exit(-10);
            }
        }
        if (type_ast->symbol == TOK_STRING)
            sym->attributes[ATTR_string] = true;
        if (type_ast->symbol == TOK_ARRAY)
        {
            sym->attributes[ATTR_array] = true;
            astree *type_child = type_ast->children[0];
            assert(type_child);
            switch (type_child->symbol)
            {
            case TOK_TYPEID:
                // Field value is a struct, check it's in the table
                if (!occurs(struct_st, type_child->lexinfo))
                {
                    // TODO: Fail here
                    cout << "Struct " << *type_child->lexinfo << " not found." << endl;
                    exit(-11);
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

void new_block(vector<symbol_table *> &st)
{
    next_block++;
    st.push_back(nullptr);
}

static void insert_struct(FILE *file, symbol_table &struct_st, astree *at)
{
    if (next_block != 1)
    {
        // TODO: Handle error
    }
    const string *ident;
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
            struct_sym->lloc = &at->lloc;
            struct_sym->fields = nullptr;
            struct_sym->struct_name = child->lexinfo;
            struct_st[ident] = struct_sym;
            fprintf(file, "%s (%zd.%zd.%zd)", ident->c_str(), struct_sym->lloc->filenr, struct_sym->lloc->linenr, struct_sym->lloc->offset);
            fprintf(file, "{%zd} %s\n", struct_sym->block_nr, get_attributes(struct_sym).c_str());
            continue;
        }
        // If the child token is a field, add it to the field table
        auto field_sym = new symbol();
        field_sym->attributes[ATTR_field] = true;
        field_sym->attributes[ATTR_lval] = true;
        field_sym->block_nr = 0;
        field_sym->lloc = &child->lloc;
        field_sym->struct_name = child->lexinfo;
        assign_attributes(field_sym, child, struct_st);

        astree *child_child_node;
        if (child->symbol == TOK_ARRAY)
            child_child_node = child->children[1];
        else
            child_child_node = child->children[0];
        assert(child_child_node);
        const string *child_name = child_child_node->lexinfo;
        symbol *str = struct_st[ident];
        if (!str->fields)
        {
            str->fields = new symbol_table();
        }
        (*str->fields)[child_name] = field_sym;
        fprintf(file, "   ");
        fprintf(file, "%s (%zd.%zd.%zd)", child_name->c_str(), field_sym->lloc->filenr, field_sym->lloc->linenr, field_sym->lloc->offset);
        fprintf(file, " field {%s} %s\n", child_name->c_str(), get_attributes(field_sym).c_str());
    }
}

void add_symtbl(vector<symbol_table *> &st)
{
    if (st.back() == nullptr)
    {
        st.pop_back();
        st.push_back(new symbol_table());
    }
}
void dump_symbol(FILE *file, astree *val_child, symbol *sym)
{
    fprintf(file, "%*s", (int)sym->block_nr * 3, "");
    fprintf(file, "%s (%zd.%zd.%zd)", val_child->lexinfo->c_str(), sym->lloc->filenr, sym->lloc->linenr, sym->lloc->offset);
    fprintf(file, "{%zd} %s\n", sym->block_nr, get_attributes(sym).c_str());
}

void insert_variable(FILE *file, vector<symbol_table *> &st, symbol_table struct_st, astree *at)
{
    add_symtbl(st);
    auto sym = new symbol();
    symbol_table &symtbl = *st.back();
    auto type_child = at->children[0];
    // Check if the child is already in the symbol table
    astree *val_child;
    if (type_child->symbol == TOK_ARRAY)
        val_child = type_child->children[1];
    else
        val_child = type_child->children[0];
    if (occurs(symtbl, val_child->lexinfo))
    {
        cout << "Variable " << *val_child->lexinfo << " is already declared." << endl;
        exit(-2);
    }

    sym->block_nr = next_block - 1;
    sym->lloc = &at->lloc;
    assign_attributes(sym, type_child, struct_st);
    sym->attributes[ATTR_variable] = true;
    sym->attributes[ATTR_lval] = true;
    symtbl[val_child->lexinfo] = sym;
    dump_symbol(file, val_child, sym);
}

astree *set_function_attributes(symbol *sym, symbol_table struct_st, astree *at)
{
    sym->attributes[ATTR_vaddr] = true;
    switch (at->symbol)
    {
    case TOK_INT:
        sym->attributes[ATTR_int] = true;
        sym->attributes[ATTR_vreg] = true;
        sym->attributes[ATTR_vaddr] = false;
        break;
    case TOK_STRING:
        sym->attributes[ATTR_string] = true;
        break;
    case TOK_TYPEID:
        if (!occurs(struct_st, at->lexinfo))
        {
            cerr << "Struct not defined: " << *at->lexinfo << endl;
            exit(-2);
        }
        sym->attributes[ATTR_struct] = true;
        sym->struct_name = at->lexinfo;
        break;
    case TOK_VOID
    case TOK_ARRAY:
        sym->attributes[ATTR_array] = true;
        set_function_attributes(sym, struct_st, at->children[0]);
        return at->children[0]->children[0];
    }

    return at->children[0];
}

astree *def_prototype(symbol *sym, symbol_table struct_st, astree *at)
{
    sym->block_nr = next_block - 1;
    auto return_child = at->children[0];
    sym->lloc = &return_child->lloc;
    auto func_name_node = set_function_attributes(sym, struct_st, return_child);
    auto param_child = at->children[1];
    for (auto child : param_child->children)
    {
        auto child_sym = new symbol();
        child_sym->attributes[ATTR_param] = true;
        child_sym->attributes[ATTR_variable] = true;
        set_function_attributes(child_sym, struct_st, child);
        child_sym->block_nr = next_block;
        child_sym->lloc = &child->lloc;
        sym->parameters.push_back(child_sym);
    }
    if(next_block != 1){
        cerr << "Function can only be defined at global scope: " << *func_name_node->lexinfo << endl;
        exit(-5);
    };
    return func_name_node;
}

bool symbol::compare(symbol s)
{
    // attributes should be the same
    auto attrstr = attributes.to_string();
    if (attrstr.compare(s.attributes.to_string()) != 0)
    {
        return false;
    }
    // If the struct is set, check the structname
    if (attributes[ATTR_struct])
    {
        if ((*struct_name).compare(*s.struct_name) != 0)
            return false;
    }
    // check that each of the parameters are the same
    if (parameters.size() != s.parameters.size())
        return false;
    int index = 0;
    for (auto param : parameters)
    {
        if (!param->compare(*s.parameters[index]))
            return false;
        index++;
    }
    return true;
}

void insert_prototype(vector<symbol_table *> &st, symbol_table struct_st, astree *at)
{
    
    add_symtbl(st);
    symbol_table &symtbl = *st.back();
    auto sym = new symbol();
    auto name_node = def_prototype(sym, struct_st, at);
    symtbl[name_node->lexinfo] = sym;
}

void add_parameters(FILE* file, vector<astree*> names, vector<symbol_table *> &st, symbol *sym)
{
    new_block(st);
    if (sym->parameters.size() != 0)
    {
        add_symtbl(st);
        symbol_table &newsymtbl = *st.back();
        int index = 0;
        for (auto name : names)
        {
            newsymtbl[name->lexinfo] = sym->parameters[index];
            dump_symbol(file, name, sym->parameters[index]);
            index++;
        }
    }
}

void insert_function(FILE *file, vector<symbol_table *> &st, symbol_table struct_st, astree *at)
{
    add_symtbl(st);
    symbol_table &symtbl = *st.back();
    auto sym = new symbol();
     sym->attributes[ATTR_function] = true;
    auto name_node = def_prototype(sym, struct_st, at);
    // I need a vector of names to hash on
    vector<astree*> names;
    astree *paramtree = at->children[1];
    for (auto child : paramtree->children)
    {
        names.push_back(child->children[0]);
    }
    if (occurs(symtbl, name_node->lexinfo))
    {
        // Check if the value is a function or a prototype
        auto protosym = symtbl[name_node->lexinfo];
        if (protosym->attributes[ATTR_function])
        {
            // Duplicate function, throw error
            cerr << "Duplicate function found: " << name_node->lexinfo << endl;
            exit(-3);
        }
        protosym->attributes[ATTR_function] = true;
        // prototype exists, verify each parameter
        if (!protosym->compare(*sym))
        {
            cerr << "Function does not match given prototype: " << name_node->lexinfo << endl;
            exit(-4);
        }
        dump_symbol(file, name_node, protosym);
        add_parameters(file, names, st, protosym);
    }
    else
    {
        symtbl[name_node->lexinfo] = sym;
        dump_symbol(file, name_node, sym);
        add_parameters(file, names, st, sym);
    }
}

symbol_table* check_st_stack(vector<symbol_table*> st, astree* at, bool &found){
    for(auto table: st){
        if(occurs(*table, at->lexinfo)){
            found = true;
            return table;
        }
    }
    found = false
    return nullptr;
}

symbol_table* check_st(vector<symbol_table*> st, astree* at){
    bool found;
    symbol_table* foundtable = check_st_stack(st, at, found);
    if(!found){
        cerr << "Variable used but not previously declared: " << *at->lexinfo << endl;
        exit(-6);
    }
    // Give the touched variable its attributes
    at->symblattributes = foundtable[at->lexinfo];
    return foundtable;
}

void check_struct(vector<symbol_table*> st, symbol_table struct_st, astree* structname, astree* field){
    auto table = check_st(st, structname);
    auto sym = (*table)[structname->lexinfo];
    auto struct_sym = struct_st[sym->struct_name];
    auto fields = struct_sym->fields;
    if(!occurs(*fields, field->lexinfo)){
        cerr << "Field " << *field->lexinfo << "is not a part of struct: " << *sym->struct_name << endl;
        exit(-7);
    }
}

void check_struct_type(symbol_table struct_st, astree* at){
    bool found;
    if(!occurs(struct_st, at->struct_name)){
        cerr << "Undeclared struct " << *at->struct_name << " defined." << endl;
        exit(-9);
    }
    at->symblattributes = struct_st[at->struct_name];
}

void check_fn(vector<symbol_table*> st, astree*at){
    bool found;
    check_st_stack(st, at, found);
    if(!found){
        cerr << "Function used but not previously declared: " << *at->lexinfo << endl;
        exit(-8);
    }
    // Give the touched variable its attributes
    at->symblattributes = foundtable[at->lexinfo];
}

void symbol::parse_astree(FILE *file, vector<symbol_table *> &st, symbol_table &struct_st, astree *at)
{
    switch (at->symbol)
    {
    case TOK_STRUCT:
        insert_struct(file, struct_st, at);
        break;
    case TOK_FUNCTION:
        insert_function(file, st, struct_st, at);
        break;
    case TOK_VARDECL:
        insert_variable(file, st, struct_st, at);
        break;
    case TOK_BLOCK:
        new_block(st);
        break;
    case TOK_PROTOTYPE:
        insert_prototype(st, struct_st, at);
        break;
    case TOK_IDENT:
        check_st(st, at);
        break;
    case TOK_CALL:
        check_fn(st, at);
        break;
    case TOK_TYPEID:
        check_struct_type(symbol_table struct_st, astree *at);
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

    if (at->symbol == TOK_FUNCTION || at->symbol == TOK_BLOCK)
    {
        next_block--;
        st.pop_back();
    }
}

string get_attributes(symbol *sym)
{
    auto abit = sym->attributes;
    string build = "";
    if (abit[ATTR_void])
        build = build + "void ";
    if (abit[ATTR_int])
        build = build + "int ";
    if (abit[ATTR_null])
        build = build + "null ";
    if (abit[ATTR_string])
        build = build + "string ";
    if (abit[ATTR_struct])
    {
        build = build + "struct \"";
        string cpy = *sym->struct_name;
        build = build + cpy;
        build = build + "\" ";
    }
    if (abit[ATTR_array])
    {
        build = build + "[] ";
    }
    if (abit[ATTR_variable])
        build = build + "variable ";
    if (abit[ATTR_lval])
        build = build + "lval ";
    if (abit[ATTR_param])
        build = build + "param ";
    return build;
}

void symbol::print_structtable(FILE *file, symbol_table st)
{
    for (auto val : st)
    {
        auto sym = val.second;
        fprintf(file, "%s (%zd.%zd.%zd)", val.first->c_str(), sym->lloc->filenr, sym->lloc->linenr, sym->lloc->offset);
        fprintf(file, "{%zd} %s\n", sym->block_nr, get_attributes(sym).c_str());
        auto fields = *sym->fields;
        cout << fields.size() << endl;
        for (auto field : fields)
        {
            fprintf(file, "   ");
            auto field_sym = field.second;
            fprintf(file, "%s (%zd.%zd.%zd)", field.first->c_str(), field_sym->lloc->filenr, field_sym->lloc->linenr, field_sym->lloc->offset);
            fprintf(file, " field {%s} %s\n", val.first->c_str(), get_attributes(field_sym).c_str());
        }
    }
}
