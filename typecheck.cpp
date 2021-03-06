/**
 * typecheck.cpp
 * oc typechecker implementation file
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

 #include "typecheck.h"
 #include "lyutils.h"

 void check_attributes(symbol *a, symbol *b);

 void type_err (location lloc, string expected) {
     errllocprintf (lloc, "type error - expected %s\n",
                                 expected.c_str());
     exit(EXIT_FAILURE);
 }

void handle_int_binop (astree* at) {
    astree* left = at->children[0];
    astree* right = at->children[1];
    if (!left->symblattributes->attributes[ATTR_int]) {
        type_err(left->lloc, "int");
    }
    if (!right->symblattributes->attributes[ATTR_int]) {
        type_err(right->lloc, "int");
    }
    auto sym = new symbol(left->symblattributes);
    sym->attributes[ATTR_vaddr] = false;
    sym->attributes[ATTR_vreg] = true;
    at->symblattributes = sym;
}

void handle_compare (astree* at){
    astree* left = at->children[0];
    astree* right = at->children[1];
    check_attributes(left->symblattributes, right->symblattributes);
    auto sym = new symbol(left->symblattributes);
    sym->attributes[ATTR_void] = false;
    sym->attributes[ATTR_null] = false;
    sym->attributes[ATTR_string] = false;
    sym->attributes[ATTR_struct] = false;
    sym->attributes[ATTR_int] = true;
    sym->attributes[ATTR_vaddr] = false;
    sym->attributes[ATTR_vreg] = true;
    at->symblattributes = sym;
}

void handle_int_unop (astree* at) {
    astree* child = at->children[0];
    if (!child->symblattributes->attributes[ATTR_int]) {
        type_err(child->lloc, "int");
    }
    at->symblattributes = child->symblattributes;
}

symbl* returnptr = nullptr;

void handle_return (astree* at) {
    if (returnptr != nullptr) {
        check_attributes(returnptr, at->children[0]->symblattributes);
    }
    else {
        returnptr = at->children[0]->symblattributes;
    }
}

void handle_function (astree* at) {
    if (at->symblattributes->attributes[ATTR_void]) {
        if (returnptr != nullptr) {
            type_err(at->lloc, "void return type");
        }
    }
    else {
        if (returnptr == nullptr) {
            type_err(at->lloc, "non-void return type");
        }
        if (at->symblattributes->attributes[ATTR_int] 
            && !returnptr->attributes[ATTR_int]) {
            type_err(at->lloc, "int return type");
        }
        else if (at->symblattributes->attributes[ATTR_string] 
            && !returnptr->attributes[ATTR_string]) {
            type_err(at->lloc, "string return type");
        }
        else if (at->symblattributes->attributes[ATTR_struct]
            && !returnptr->attributes[ATTR_struct]) {
            type_err(at->lloc, "struct return type");
        }
        else if (at->symblattributes->attributes[ATTR_struct] 
            && !returnptr->attributes[ATTR_struct]) {
            auto sname = at->symblattributes->struct_name;
            if (sname->compare(*returnptr->struct_name) != 0) {
                type_err(at->lloc, "matching struct name");
            }
        }
        else if (at->symblattributes->attributes[ATTR_array] 
            && !returnptr->attributes[ATTR_array]) {
            type_err (at->lloc, "array return type");
        }
    }
    returnptr = nullptr;
}

void check_attributes(symbol *a, symbol *b){
    if(!a->attributes[ATTR_vaddr] 
        && b->attributes[ATTR_null])
        type_err(*a->lloc, "non null value.");

    if(a->attributes[ATTR_vaddr]
         && b->attributes[ATTR_null])
        return;

    if (a->attributes[ATTR_int] 
        && !b->attributes[ATTR_int]) 
        type_err(*b->lloc, "expression of type int or int array");

    if (a->attributes[ATTR_string] 
        && !b->attributes[ATTR_string])
        type_err(*b->lloc, "expression of type string or string array");
    
    if (a->attributes[ATTR_array] 
        && !b->attributes[ATTR_array]) {
        type_err(*b->lloc, "expression with type array");
    }

    if (a->attributes[ATTR_struct] 
        && !b->attributes[ATTR_struct])
        type_err(*b->lloc, "expression with type struct");

    if (a->attributes[ATTR_struct]){
        if((*a->struct_name).compare(*b->struct_name) != 0){
            type_err(*b->lloc, 
                "expression with type struct " + *a->struct_name);
        }
    }

    
}

void handle_assignment (astree* at) {
    astree* left = at->children[0];
    astree* right = at->children[1];

    check_attributes(left->symblattributes,
                        right->symblattributes);

    if (!left->symblattributes->attributes[ATTR_lval])
        type_err(left->lloc, "value to have attribute lval");

    auto sym = new symbol(left->symblattributes);
    sym->attributes[ATTR_vaddr] = false;
    sym->attributes[ATTR_vreg] = true;
    // If the right side is a string constant, we need to know
    if(right->symblattributes->attributes[ATTR_const])
        sym->attributes[ATTR_const] = true;

    at->symblattributes = sym;
}

void handle_index(astree* at){
    auto typechild = at->children[0];
    auto intchild = at->children[1];
    auto intsym = intchild->symblattributes;
    if(!intsym->attributes[ATTR_int] || intsym->attributes[ATTR_array])
        type_err(intchild->lloc, "index operator with type int");
    auto typesym = typechild->symblattributes;
    if(!typesym->attributes[ATTR_array]
         && !typesym->attributes[ATTR_string])
        type_err(typechild->lloc, 
       "index operator to be used on variable of type array or string");
    
    if(typesym->attributes[ATTR_array]){
        symbol *newsym = new symbol(typesym);
        newsym->attributes[ATTR_lval] = true;
        newsym->attributes[ATTR_array] = false;
        at->symblattributes = newsym;
        return;
    }

    symbol *newsym = new symbol(typesym);
    newsym->attributes[ATTR_lval] = true;
    newsym->attributes[ATTR_string] = false;
    newsym->attributes[ATTR_int] = true;
    at->symblattributes = newsym;
}

void handle_vardecl(astree* at){
    auto declattr = at->children[0]->symblattributes;
    auto exprattr = at->children[1]->symblattributes;

    check_attributes(declattr, exprattr);
    auto sym = new symbol(declattr);
    if(exprattr->attributes[ATTR_const])
        sym->attributes[ATTR_const] = true;

    at->symblattributes = sym;

}

void handle_new_array(astree* at){
    auto basetype = at->children[0];
    auto index = at->children[1];
    if(!index->symblattributes->attributes[ATTR_int] 
        || index->symblattributes->attributes[ATTR_array])
        type_err(index->lloc, "array index to be of type int");

    auto sym = new symbol(basetype->symblattributes);
    sym->attributes[ATTR_array] = true;
    sym->attributes[ATTR_vaddr] = false;
    sym->attributes[ATTR_vreg] = true;
    at->symblattributes = sym;
}

void handle_new_empty_arg(astree* at){
    auto basetype = at->children[0];
    auto sym = new symbol(basetype->symblattributes);
    sym->attributes[ATTR_vaddr] = false;
    sym->attributes[ATTR_vreg] = true;
    at->symblattributes = sym;
}

void handle_new_arg(astree* at){
    auto arg = at->children[1];
    if(!arg->symblattributes->attributes[ATTR_int] 
        || arg->symblattributes->attributes[ATTR_array])
        type_err(arg->lloc, 
        "string construct argument to be of type int");
    auto sym = new symbol(at->children[0]->symblattributes);
    sym->attributes[ATTR_vaddr] = false;
    sym->attributes[ATTR_vreg] = true;
    at->symblattributes = sym;
}

void handle_new(astree* at){
    handle_new_empty_arg(at);
}

void handle_selector(astree* at){
    auto structarg = at->children[0];
    auto fieldarg = at->children[1];
    if(!structarg->symblattributes->attributes[ATTR_struct])
        type_err(structarg->lloc,
            "field selector must be used on struct");
    if(!symbol::occurs(*structarg->symblattributes->fields,
         fieldarg->lexinfo))
        type_err(fieldarg->lloc,
            *fieldarg->lexinfo + " is be a part of struct's fields");
    auto syms = *structarg->symblattributes->fields;
    fieldarg->symblattributes = syms[fieldarg->lexinfo];
    auto fields = *structarg->symblattributes->fields;
    auto sym = new symbol((fields)[fieldarg->lexinfo]);
    sym->attributes[ATTR_lval] = true;
    sym->attributes[ATTR_vaddr] = true;
    sym->attributes[ATTR_vreg] = false;
    at->symblattributes = sym;
}

void handle_call(astree* at){
    auto fnname = at->children[0];

    if(!fnname->symblattributes->attributes[ATTR_function]
     && !fnname->symblattributes->attributes[ATTR_prototype])
        type_err(fnname->lloc, "a valid function");
    
    if((at->children.size()-1) 
        != fnname->symblattributes->parameters.size())
        type_err(fnname->lloc, 
            *fnname->lexinfo + " called with correct parameters.");
    int index = 0;
    for(auto child: at->children){
        if(!index){
            index++;
            continue;
        }
        auto sym = fnname->symblattributes->parameters[index-1];
        check_attributes(sym, child->symblattributes);
        index++;
    }

    auto sym = new symbol(fnname->symblattributes);
    sym->attributes[ATTR_vreg] = true;
    sym->attributes[ATTR_vaddr] = false;
    at->symblattributes = sym;
}

void assign_child(astree* at){
    at->symblattributes = at->children[0]->symblattributes;
    at->symblattributes->attributes[ATTR_array] = true;
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
    case TOK_EQ:
        handle_compare (at);
        break;
    case TOK_NE:
        handle_compare (at);
        break;
    case TOK_LE:
        handle_compare (at);
        break;
    case TOK_GE:
        handle_compare (at);
        break;
    case TOK_LT:
        handle_compare (at);
        break;
    case TOK_GT:
        handle_compare (at);
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
    case '=':
        handle_assignment (at);
        break;
    case TOK_INDEX:
        handle_index(at);
        break;
    case TOK_VARDECL:
        handle_vardecl(at);
        break;
    case TOK_NEW:
        handle_new(at);
        break;
    case TOK_NEWARRAY:
        handle_new_array(at);
        break;
    case TOK_NEWSTRING:
        handle_new_arg(at);
        break;
    case '.':
        handle_selector(at);
        break;
    case TOK_CALL:
        handle_call(at);
        break;
    case TOK_ARRAY:
        assign_child(at);
        break;
    }
}
