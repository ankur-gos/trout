/*
 * Emitter implementation
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include "emitter.h"

int counter = 0;

string emitter::vreg(astree *at){
    string name = "";
    if(at->symblattributes->attributes[ATTR_int])
        name = "i";
    if(at->symblattributes->attributes[ATTR_string])
        name = "s";
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

string get_global_name(astree *at){
    string name = *at->lexinfo;
    return name;
}

string global(astree *at){
    return "__" + get_name(at);
}

string type(astree *at){
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

void globalgen(FILE* file, astree* root){
    for(auto child: root->children){
        if(child->symbol == TOK_VARDECL){
            string line = type(child->children[0]) + " ";
            line = line + global(child->children[0]) + ";\n";
            fprintf(file, line.c_str());
        }
    }
}

void write_main(FILE* file){
    fprintf(file, "void __ocmain (void)\n{\n");
}

void emitter::define_globals(FILE* file, astree* root){
    for(auto child: root->children){
        if(child->symbol == TOK_VARDECL){
            string line = global(child->children[0]) + " = ";
            if(child->children[1]->vreg.empty())
                line = line + codegen(file, child->children[1]);
            else
                line = line + child->children[1]->vreg;
            line = line + ";\n";
            fprintf(file, "%*s", 8, "");
            fprintf(file, line.c_str());
        }
    }
}

void end_main(FILE* file){
    fprintf(file, "}\n");
}

void emitter::emit_oil(FILE* file, astree* root) {
    structgen(file, root);
    stringgen(file);
    globalgen(file, root);
    write_main(file);
    define_globals(file, root);
    end_main(file);
}

void emitter::structgen(FILE* file, astree *root){
    for(auto child: root->children){
        if(child->symbol == TOK_STRUCT){
            string name = *child->symblattributes->struct_name;
            string struct_header = "struct s_" + name + " {\n";
            fprintf(file, struct_header.c_str());
            for(size_t i = 1; i < child->children.size(); i++){
                fprintf(file, "%*s", 8, "");
                auto struct_child = child->children[i];
                string field = type(struct_child) + " f_" + get_name(struct_child) + ";\n";
                fprintf(file, field.c_str());
            }
            fprintf(file, "};\n");
        }
    }
}

void emitter::stringgen(FILE* file){
    for(auto node: symbol::strings){
        if(!node)
            continue;
        node->vreg = vreg(node);
        string line = "char* " + node->vreg;
        line = line + " = ";
        string con = *node->lexinfo;
        line = line + con + ";\n";
        fprintf(file, line.c_str());
    }
}

string emitter::handle_call(FILE* file, astree* at){
    auto parameternode = at->children[1];
    vector<string> parameters;
    for(auto child: parameternode->children){
        auto gen = codegen(file, child);
        // POSSIBLE SEGFAULT HERE
        string vr = vreg(child);
        parameters.push_back(vr);
        string line = type(child) + " " + vr;
        line = line + " = " + gen;
        fprintf(file, "%*s", 8, "");
        fprintf(file, line.c_str());
    }
    string funcname = *at->children[0]->lexinfo;
    auto funcline = funcname + " (";
    for(size_t i = 0; i < parameters.size(); i++){
        if(i == 0){
            funcline = funcline + parameters[i];
            continue;
        }
        funcline = funcline + ", " + parameters[i];
    }
    funcline = funcline + ")";
    return funcline;
}

string emitter::codegen(FILE* file, astree* at){
    switch(at->symbol){
        case TOK_ROOT:
        case TOK_BLOCK:
            for(auto child: at->children)
                codegen(file, child);
        case TOK_INTCON:
            return *at->lexinfo;
        case TOK_STRINGCON:
            return at->vreg;
        case TOK_CALL:
            return handle_call(file, at);
        case TOK_VARDECL:
            // If the block is 0, skip it
            break;
    }
    return "";
}
