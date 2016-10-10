// stringset.h
// StringSet class
/*
 * Ankur Goswami (agoswam3@ucsc.edu)
 * Morgan Grant (mlgrant@ucsc.edu)
 */

#include <unordered_set>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class stringset{
   unordered_set<string> set;
   public:
      stringset(){};
      const string* intern_stringset(const char*);
      void dump_stringset(ofstream*);
};
