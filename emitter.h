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
    static string type(astree* at);
    static void emit_oil(FILE* file, astree* root);
    static void structgen(FILE* file, astree* root);
    static string codegen(FILE* file, astree* at);
};

#endif
