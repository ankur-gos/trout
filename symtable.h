/**
 * symtable.h
 * symbol table struct and functions
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

 #ifndef __SYMTABLE_H__
 #define __SYMTABLE_H__


#include <bitset>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "astree.h"

extern int next_block;

struct astree;
struct location;

// ATTR_bitset_size unused, signifies # of attributes for bitset
enum { ATTR_void, ATTR_int, ATTR_null, ATTR_string,
       ATTR_struct, ATTR_array, ATTR_function, ATTR_variable,
       ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval, ATTR_const,
       ATTR_vreg, ATTR_vaddr, ATTR_prototype, ATTR_bitset_size };

// bitset
using attr_bitset = bitset<ATTR_bitset_size>;

struct symbol;
using symbol_table = unordered_map<const string*, symbol*>;

struct symbol{
    symbol(symbol *);
    symbol();

    attr_bitset attributes;
    symbol_table* fields;
    location *lloc;
    size_t block_nr;
    vector<symbol*> parameters;
    const string* struct_name;
    const string* field_struct;

    static void parse_astree(FILE* file, vector<symbol_table*> &st,
                             symbol_table &struct_st, astree *at);
    static void print_structtable(FILE *file, symbol_table st);
    string get_attributes();
    bool compare(symbol s);
    static bool occurs(symbol_table st, const string* key);
    static vector<astree*> strings;
};

#endif
