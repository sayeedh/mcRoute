#include "coopTable.h"
#include "ctableEntry.h"

#include <iostream>
using namespace std;
CoopTable :: CoopTable(){
}

CoopTable :: ~CoopTable(){
  std::map<int, CTableEntry*>::iterator it;
  for(it = cTable.begin();it != cTable.end(); it++){
    delete (*it).second;
  }
  cTable.clear();
}
CTableEntry* CoopTable :: getEntry(int sender){
  return cTable[sender];
}

void CoopTable :: addEntry(CTableEntry* t, int sender){
 cTable[sender] = t;
}

void CoopTable::removeEntry(int sender){
  cTable.erase(sender);
}
