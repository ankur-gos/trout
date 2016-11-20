/*
 * oc.cpp
 * Compiler main
 *
 *
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */


#include <string>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "string.h"
#include "stringset.h"
#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"
#include "symtable.h"

using namespace std;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

int next_block = 1;


void handle_debug(const char *flags){
   set_debugflags(flags);
}

void handle_yyflex_debug(){
   yy_flex_debug = 1;
}

void handle_yyparse_debug(){
   yydebug = 1;
}

int check_file_extension(string filename){
   string ext;
   try{
      ext = filename.substr(filename.size() - 3, 3);
   } catch(const out_of_range& oor){
      cerr << "Filename doesn't have correct extension";
      return -1;
   }
   if(ext.compare(".oc") != 0){
      cerr << "Filename doesn't have correct extension: "
         << ext << endl;
      return -2;
   }
   return 0;
}

void chomp(char* string, char delim){
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if(*nlpos == delim)
      *nlpos = '\0';
}

void dump_stringset_file(string filename){
   string fout_name = filename + ".str";
   ofstream fout;
   fout.open(fout_name);
   stringset::dump_stringset(&fout);
   fout.close();
}

void scan_tok_to_file (string filename, string command) {
   string fout_name = filename + ".tok";
   tok_out = fopen(fout_name.c_str(), "w");
   lexer::newfilename (command);
   yyparse();
}

void create_symbol_table(string filename){
      string fout_name = filename + ".sym";
      symbol_table symbols, structs;
      symbol::parse_astree(symbols, structs, parser::root);
      symbol::print_structtable((fopen(fout_name.c_str(), "w")), structs);
}

void dump_ast(string filename) {
      string fout_name = filename + ".ast";
      astree::print((fopen(fout_name.c_str(), "w")), parser::root);
}

int main(int argc, char** argv){
   yy_flex_debug = yydebug = 0;
   int opt;
   string cppflag = "";
   string stroptarg;
   while((opt = getopt(argc, argv, "@:D:ly")) != -1){
      switch(opt){
         case '@':
            handle_debug(optarg);
            break;
         case 'D':
            stroptarg = optarg;
            cppflag = "-D " + stroptarg + " "; 
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
      cerr << "No filename found" << endl;
      exit(EXIT_FAILURE);
   }

   exec::execname = basename (argv[0]);
   string filename = argv[optind];
   string no_ext = filename.substr(0, filename.size() - 3);

   // Check file extension is .oc
   if(check_file_extension(filename))
      exit(EXIT_FAILURE);

   string command = CPP + " " + cppflag + filename;
   yyin = popen(command.c_str(), "r");
   if(yyin == NULL){
      cerr << "Invalid command run, pipe not opened.";
      exit(EXIT_FAILURE);
   }

   if (yy_flex_debug) {
      fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
               command.c_str(), fileno (yyin));
   }

   scan_tok_to_file (no_ext, command);
   dump_stringset_file (no_ext);
   dump_ast(no_ext);
   if(pclose(yyin) != 0){
      cerr << "Pipe close failed." << endl;
      exit(EXIT_FAILURE);
   }
   yylex_destroy();

   exit(EXIT_SUCCESS);
}

