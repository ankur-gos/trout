/**
 * typecheck.cpp
 * oc typechecker implementation file
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

 #include "typecheck.h"
 #include "lyutils.h"

 void type_err (int status, location lloc, string expected) {
     errllocprintf (lloc, "type error - expected %s\n", expected.c_str());
     exit(status);
 }

void handle_int_binop (astree* at) {
    astree* left = at->children[0];
    astree* right = at->children[1];
    if (!left->symblattributes->attributes[ATTR_int]) {
        type_err(-20, left->lloc, "int");
    }
    if (!right->symblattributes->attributes[ATTR_int]) {
        type_err(-20, right->lloc, "int");
    }
    at->symblattributes = left->symblattributes;
}

void handle_int_unop (astree* at) {
    astree* child = at->children[0];
    if (!child->symblattributes->attributes[ATTR_int]) {
        type_err(-20, child->lloc, "int");
    }
    at->symblattributes = child->symblattributes;
}

symbl* returnptr = nullptr;

void handle_return (astree* at) {
    if (returnptr != nullptr) {
        if(!returnptr->compare(*at->children[0]->symblattributes)) {
            type_err(-21, at->lloc, "matching return types");
        }
    }
    else {
        returnptr = at->children[0]->symblattributes;
    }
}

void handle_function (astree* at) {
    if (at->symblattributes->attributes[ATTR_void]) {
        if (returnptr != nullptr) {
            type_err(-21, at->lloc, "void return type");
        }
    }
    else {
        if (returnptr == nullptr) {
            type_err(-21, at->lloc, "non-void return type");
        }
        if (at->symblattributes->attributes[ATTR_int] && !returnptr->attributes[ATTR_int]) {
            type_err(-21, at->lloc, "int return type");
        }
        else if (at->symblattributes->attributes[ATTR_string] && !returnptr->attributes[ATTR_string]) {
            type_err(-21, at->lloc, "string return type");
        }
        else if (at->symblattributes->attributes[ATTR_struct] && !returnptr->attributes[ATTR_struct]) {
            type_err(-21, at->lloc, "struct return type");
        }
        else if (at->symblattributes->attributes[ATTR_struct] && returnptr->attributes[ATTR_struct]) {
            if (at->symblattributes->struct_name->compare(*returnptr->struct_name) != 0) {
                type_err(-21, at->lloc, "matching struct name");
            }
        }
        else if (at->symblattributes->attributes[ATTR_array] && returnptr->attributes[ATTR_array]) {
            type_err (-21, at->lloc, "array return type");
        }
    }
    returnptr = nullptr;
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
    case TOK_RETURN:
        handle_return (at);
    break;
    case TOK_FUNCTION:
        handle_function (at);
    break;
    }
}
