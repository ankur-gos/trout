/**
 * symtable.cpp
 * symtable function implementations
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include "symtable.h"

static void symbol::parse_astree(symbol_table &st, astree* at){
    // Post-order traversal, left node is first in vector
    if (!at->children.empty()){
        for(auto child: at->children){
            parse_astree(symbol_table, child);
        }
    }
    
}