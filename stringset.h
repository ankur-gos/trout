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
   public:
   stringset();
   static unordered_set<string> set;
   static const string* intern_stringset(const char*);
   static void dump_stringset(ofstream*);
};
