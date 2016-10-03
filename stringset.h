// stringset.h
// StringSet class

#include <unordered_set>
#include <string>

using namespace std;

class stringset{
   unordered_set<string*> set;
   public:
      stringset(){};
      const string* intern_stringset(const char*);
      void dump_stringset(FILE*);
}
