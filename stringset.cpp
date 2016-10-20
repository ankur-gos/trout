// stringset.cpp
// stringset implementation

#include <iomanip>
#include "stringset.h"

using namespace std;

const string* stringset::intern_stringset(const char* val){
   string sval = val;
   auto pair = set.insert(sval);
   auto iterator = pair.first;
   return &(*iterator);
}

void stringset::dump_stringset(ofstream* file){
   unsigned max_bucket_size = 0;
   auto hfn = set.hash_function();
   for(unsigned int i = 0; i < set.bucket_count(); ++i){
      if(set.bucket_size(i) == 0)
         continue;

      if(max_bucket_size < set.bucket_size(i))
         max_bucket_size = set.bucket_size(i);

      auto itr = set.begin(i);
      *file << "hash[" << setw(4) << i << "]: ";
      auto strptr = &(*(set.find(*itr)));
      *file << setw(20) << hfn(*itr) << " " << strptr << "->\"" 
         << *itr << "\"" << endl;
      ++itr;

      for(; itr != set.end(i); ++itr) {
         *file << "            ";
         auto strptr = &(*(set.find(*itr)));
         *file << setw(20) << hfn(*itr) << " " << strptr << "->\"" 
            << *itr << "\""
               << endl;
      }
   }
   *file << "max_bucket_size = " << max_bucket_size << endl;
   *file << "load_factor = " << set.load_factor() << endl;
   *file << "bucket_count = " << set.bucket_count() << endl;
}
