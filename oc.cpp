/*
 * oc.cpp
 * Compiler main
 */

#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

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

int check_file_extension(string filename){
   string ext;
   try{
      ext = filename.substr(filename.size() - 4, 3);
   } catch(const out_of_range& oor){
      cerr << "Filename doesn't have correct extension";
      return -1;
   }
   if(ext.compare(".oc") != 0){
      cerr << "Filename doesn't have correct extension";
      return -2;
   }
   return 0;
}

void chomp(char* string, char delim){
   if(strlen(string) == 0)
      return

   char* nlpos = string + len - 1;
   if(*nlpos == delim)
      *nlpos = '\0';
}

int run_preprocessor(FILE* pipe, const char* filename){
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy(inputname, filename);
   for(;;){
      char buffer[LINESIZE];
      if(fgets(buffer, LINESIZE, pipe) == NULL){
         cerr << "fgets error in run_preprocessor";
         return -1;
      }
      chomp(buffer, '\n');
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, inputname);
         continue;
      }
      char* savepos = NULL;
      char *bufptr = buffer;
      for(int tokenct = 1;; ++tokenct){
         char* token = strtok_r(bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if(token == NULL)
            break;
      }
      ++linenr;
   }
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
   if(check_file_extension(filename))
      exit(EXIT_FAILURE);

   string command = CPP + " " + filename;
   FILE* pipe = popen(command.c_str(), "r");
   if(pipe == NULL){
      cerr << "Invalid command run, pipe not opened."
      exit(EXIT_FAILURE);
   }

   if(run_preprocessor(pipe, filename) != 0)
      exit(EXIT_FAILURE);

   if(pclose(pipe) != 0){
      cerr << "Pipe close failed."
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}

