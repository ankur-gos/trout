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

using namespace std;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

void handle_debug(const char *flags){
   set_debugflags(flags);
}

void handle_yyflex_debug(){
   // STUB
}

void handle_yyparse_debug(){
   // STUB
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
      cerr << "Filename doesn't have correct extension: " << ext << endl;
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

void dump_stringset_file(string filename, stringset strset){
   auto fno_ext = filename.substr(0, filename.size() - 3);
   auto fout_name = fno_ext + ".str";
   ofstream fout;
   fout.open(fout_name);
   strset.dump_stringset(&fout);
   fout.close();
}

int run_preprocessor(FILE* pipe, string filename){
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy(inputname, filename.c_str());
   stringset strset;
   for(;;){
      char buffer[LINESIZE];
      if(fgets(buffer, LINESIZE, pipe) == NULL)
         break;
      chomp(buffer, '\n');
      auto ret = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if(ret == 2)
         continue;
      char* savepos = NULL;
      char *bufptr = buffer;
      for(int tokenct = 1;; ++tokenct){
         char* token = strtok_r(bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if(token == NULL)
            break;
         strset.intern_stringset(token);
      }
      ++linenr;
   }
   dump_stringset_file(filename, strset);
   return 0;
}

int main(int argc, char** argv){
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
      cerr << "No filename found";
      exit(EXIT_FAILURE);
   }

   string filename = argv[optind];

   // Check file extension is .oc
   if(check_file_extension(filename))
      exit(EXIT_FAILURE);

   string command = CPP + " " + cppflag + filename;
   FILE* pipe = popen(command.c_str(), "r");
   if(pipe == NULL){
      cerr << "Invalid command run, pipe not opened.";
      exit(EXIT_FAILURE);
   }

   if(run_preprocessor(pipe, filename) != 0)
      exit(EXIT_FAILURE);

   if(pclose(pipe) != 0){
      cerr << "Pipe close failed.";
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}

