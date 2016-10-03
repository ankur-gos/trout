// stringset.cpp
// stringset implementation

#include "stringset.h"

using namespace std;

const string* stringset::intern_stringset(const char* val){
   string sval = val;
   auto pair = set.insert(sval);
   auto iterator = pair.first;
   return &(*iterator);
}

void stringset::dump_stringset(ofstream* file){
   for(auto val: set){
      *file << "hash[" << set.bucket(val) << "]: ";
      auto hfn = set.hash_function();
      auto strptr = &(*(set.find(val)));
      *file << hfn(val) << " " << strptr << "->\"" << val << "\""
            << endl;
   }
   *file << "load_factor = " << set.load_factor() << endl;
   *file << "bucket_count = " << set.bucket_count() << endl;
   unsigned max_bucket_size = 0;
   for(unsigned i = 0; i < set.bucket_count(); ++i){
      if(max_bucket_size < set.bucket_size(i))
         max_bucket_size = set.bucket_size(i);
   }
   *file << "max_bucket_size = " << max_bucket_size << endl;
}
