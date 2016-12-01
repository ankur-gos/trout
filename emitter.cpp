/*
 * Emitter implementation
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include "emitter.h"

string emitter::vreg(astree *at){
    string name = "";
    if(at->symblattributes->attributes[ATTR_int])
        name = "i";
    name = name + to_string(++counter);
    return name;
}

string get_name(astree *at){
    astree* namenode;
    if(at->symblattributes->attributes[ATTR_array])
        namenode = at->children[1];
    else
        namenode = at->children[0];
    string name = *namenode->lexinfo;
    return name;
}

string declaration(astree *at){
    auto block_nr = at->symblattributes->block_nr;
    return "_" + to_string(block_nr) + "_" + get_name(at);
}

string emitter::type(astree *at){
    auto attributes = at->symblattributes->attributes;
    string type;
    if(attributes[ATTR_array]){
        if(attributes[ATTR_int])
            type = "int* ";
        else if(attributes[ATTR_string])
            type = "char** ";
        else{
            string sname = *at->symblattributes->struct_name;
            type = "struct s_" + sname + "** ";
        }
    } else if (attributes[ATTR_int])
        type = "int ";
    else if(attributes[ATTR_string])
        type = "char* ";
    else{
        string sname = *at->symblattributes->struct_name;
        type = "struct s_" + sname + "* ";
    }
    return type;
}

void emitter::structgen(FILE* file, astree *root){
    for(auto child: root->children){
        if(child->symbol == TOK_STRUCT){
            string name = *at->symblattributes->struct_name;
            string struct_header = "struct s_" + name + " {\n";
            fprintf(file, struct_header.c_str());
            for(size_t i = 1; i < at->children.size(); i++){
                fprintf(file, "%*s", 8, "");
                auto child = at->children[i];
                string field = type(child) + " f_" + get_name(child) + ";\n";
                fprintf(file, field.c_str());
            }
            fprintf(file, "};\n");
        }
    }

}

string emitter::codegen(FILE* file, astree *at){
    switch(at->symbol){
        case TOK_ROOT:
        case TOK_BLOCK:
            for(auto child: at->children)
                codegen(file, child);
        case TOK_INTCON:
            return *at->lexinfo;
        case TOK_VARDECL:{
            string line = type(at->children[0]) + declaration(at->children[0]);
            line = line + " = " + codegen(file, at->children[1]) + ";\n";
            fprintf(file, line.c_str());
            return line;
            }
        case TOK_PARAMLIST:
            break;
    }
    return "";
}
