// stringset.cpp
// stringset implementation

#include "stringset.h"

using namespace std;

const string* stringset::intern_stringset(const char* val){
   auto pair = set.insert(val);
   auto iterator = get<0>pair;
   return begin(iterator);
}

void stringset::dump_stringset(FILE*){
   for(auto val: set){
      cout << "hash[" + set.bucket(val) + "]: ";
      auto hfn = set.hash_function();
      cout << hfn(val) + " " + val + "->\"" + *val + "\"";
            << endl;
   }
   cout << "load_factor = " + set.load_factor() << endl;
   cout << "bucket_count = " + set.bucket_count() << endl;
   cout << "max_bucket_size = " + set.bucket_size() << endl;
}
