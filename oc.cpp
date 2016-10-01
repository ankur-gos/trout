/*
 * oc.cpp
 * Compiler main
 */

#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

const string CPP = "/usr/bin/cpp";

void handle_debug(){
   // STUB
}

void handle_cpp_arg(){
   // STUB
}

void handle_yyflex_debug(){
   // STUB
}

void handle_yyparse_debug(){
   // STUB
}

int main(int argc, char** argv){
   int opt;
   while((opt = getopt(argc, argv, "@:D:ly")) != -1){
      switch(opt){
         case '@':
            handle_debug();
            break;
         case 'D':
            handle_cpp_arg();
            break;
         case 'l':
            handle_yyflex_debug();
            break;
         case 'y':
            handle_yyparse_debug();
            break;
         default:
            cerr << "Invalid flag";
            exit(EXIT_FAILURE);
      }
   }
   if(optind >= argc){
      cerr << "No filename found";
      exit(EXIT_FAILURE);
   }

   string filename = argv[optind];

   // Check file extension is .oc
   string ext;
   try{
      ext = filename.substr(filename.size() - 4, 3);
   } catch(const out_of_range& oor){
      cerr << "Filename doesn't have correct extension";
      exit(EXIT_FAILURE);
   }
   if(ext.compare(".oc") != 0){
      cerr << "Filename doesn't have correct extension";
      exit(EXIT_FAILURE);
   }

}

