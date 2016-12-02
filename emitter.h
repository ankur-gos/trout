/*
 * emitter.h
 * emitter header
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant  (mlgrant@ucsc.edu)
 */

#ifndef __EMITTER_H_
#define __EMITTER_H_

#include <string>
#include "astree.h"
#include "lyutils.h"

struct emitter{
    static string vreg(astree* at);
    static void emit_oil(FILE* file, astree* root);
    static void structgen(FILE* file, astree* root);
    static void funcgen(FILE* file, astree* root);
    static string codegen(FILE* file, astree* at);
    static void stringgen(FILE* file);
    static string handle_call(FILE* file, astree* at);
    static void define_globals(FILE* file, astree* root);
    static string handle_while(FILE* file, astree* at);
};

#endif
