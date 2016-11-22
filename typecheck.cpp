/**
 * typecheck.cpp
 * oc typechecker implementation file
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

 #include "typecheck.h"
 #include "lyutils.h"

 void type_err (int status, location lloc) {
     errllocprintf (lloc, "%s", "type error");
     exit(status);
 }

void handle_int_binop (astree* at) {
    astree* left = at->children[0];
    astree* right = at->children[1];
    if (!left->symblattributes->attributes[ATTR_int]) {
        type_err(-20, left->lloc);
    }
    if (!right->symblattributes->attributes[ATTR_int]) {
        type_err(-20, right->lloc);
    }
    at->symblattributes = left->symblattributes;
}

void handle_int_unop (astree* at) {
    astree* child = at->children[0];
    if (!child->symblattributes->attributes[ATTR_int]) {
        type_err(-20, child->lloc);
    }
    at->symblattributes = child->symblattributes;
}

void check_types (astree* at) {
    // postorder traversal
    for (auto child: at->children) {
        check_types(child);
    }

    switch (at->symbol) {
    case '+':
        handle_int_binop (at);
    break;
    case '-':
        handle_int_binop (at);
    break;
    case '*':
        handle_int_binop (at);
    break;
    case '/':
        handle_int_binop (at);
    break;
    case '%':
        handle_int_binop (at);
    break;
    case TOK_POS:
        handle_int_unop (at);
    break;
    case TOK_NEG:
        handle_int_unop (at);
    break;
    case '!':
        handle_int_unop (at);
    break;
    }
}
