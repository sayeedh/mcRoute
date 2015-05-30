#ifndef COOPTABLE_H
#define COOPTABLE_H
#include<map>
//using namespace std;

class CTableEntry;

class CoopTable{
 public:
  CoopTable();
  ~CoopTable();
  CTableEntry* getEntry(int sender);
  //  CTableEntry* getEntryBySrc(int src);
  void addEntry(CTableEntry *t, int sender);
  //  void addEntryBySrc(CTableEntry *t, int src);
  void removeEntry(int sender);
 private:
  std::map<int, CTableEntry*> cTable;
};

#endif
