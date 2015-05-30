#include <sstream>
#include <iostream>
#include<vector>
#include<map>
#include <fstream>

using std::cout;
using std::ostringstream;
using std::map;
using std::vector;

#include "userVisitor.h"
#include "agent.h"
#include "report.h"

/*
The constructor of the class ProfitVisitor
*/
ProfitVisitor :: ProfitVisitor()
  :UserVisitor(){
  numPU = 0;
  numSU = 0;
  utilPU = 0.0;
  utilSU = 0.0;
}

/* 
The constructor of the class PathVisitor
all path related parameters are initialized
*/
PathVisitor :: PathVisitor()
  :UserVisitor(){
 numPU = 0;
 numSU = 0;
 utilPU = 0.0;
 utilSU = 0.0;
 numRoundMin = 100;
 numRoundMax = 0;
 numRoundAvg = 0;

 lenRelayMax = 0;
 lenRelayMin = 100;
 lenRelayAvg = 0;

 sumPUNeigh = 0;
 sumRelayNeigh = 0;
}

/*
The destructor clears the resources
*/
PathVisitor :: ~PathVisitor()
{
  coopNodes.clear();
  freqNodes.clear();
}

/*
The method goes through all users and summarizes the path statistics e.g. max, min, and average path length
*/
void PathVisitor :: visitDefault(User* u){
  int ownId, targetId, relayId, sumRelay =0;
  if(u->getTarget() == -1) return;
  if(u->getProfit()<=0) return;
  ownId = u->getId();
  numPU++;
  sumPUNeigh += u->numOfNeighbors();

  numRoundMin = min(numRoundMin,u->findAgent(0)->getCoopRound());
  numRoundMax = max(numRoundMax,u->findAgent(0)->getCoopRound());
  numRoundAvg += u->findAgent(0)->getCoopRound();

  coopNodes[ownId] = u->getProfit();
  utilPU += u->getProfit();
  cout << ownId ;
  targetId = u->getTarget();
  User *temp = u;

  relayId = temp->findAgent(0)->getLastOfferId();
  while(relayId != targetId && relayId != -1){

    cout << "-->" << relayId;
    temp = temp->getNeighborById(relayId);   
    sumRelay++;
    freqNodes[relayId] += 1;
    if(coopNodes.count(relayId)==0){
      numSU++;
      sumRelayNeigh += temp->numOfNeighbors();
      coopNodes[relayId] = temp->getProfit();
      utilSU += temp->getProfit();
    }
    relayId = temp->findAgent(ownId)->getLastOfferId();
  }
  if(relayId == targetId || relayId == -1){
    cout << "-->"<< targetId;
    lenRelayMin = min(lenRelayMin, sumRelay+1);
    lenRelayMax = max(lenRelayMax, sumRelay+1);
    lenRelayAvg += sumRelay+1;
  }
  cout << endl;
}

/*
The method prints the statistics in a defined output format to a file
*/
void PathVisitor :: print(){
  std::map<int, double>::iterator it;
  int numOverlapNodes = 0;
  std::map<int, int>::iterator it1;
  for(it1 = freqNodes.begin(); it1 != freqNodes.end(); it1++){
    if(it1->second > 1){
      numOverlapNodes++;
    }
  }
  ostringstream output;
  numRoundAvg = numRoundAvg/numPU;
  lenRelayAvg = lenRelayAvg/numPU;
/** The statistics are shown in the console **/
  cout << "#pus:  " << getNumActivePU() << endl;
  cout << "#sus:  " << getNumActiveSU() << endl;
  cout << "u pu:  " << getUtilPU()      << endl;
  cout << "u su:  " << getUtilSU()      << endl;
  cout << "#onode " << numOverlapNodes  << endl;
  cout << "min r: " << numRoundMin      << endl;
  cout << "max r: " << numRoundMax      << endl;
  cout << "avg r: " << numRoundAvg      << endl;

  cout << "min l: " << lenRelayMin      << endl;
  cout << "max l: " << lenRelayMax      << endl;
  cout << "avg l: " << lenRelayAvg      << endl;  

  for(it = coopNodes.begin(); it != coopNodes.end(); it++){
   if(it->second > 0){
     cout << it->first << " : " << it->second << endl;
   } 
  }
  output << getNumActivePU() << " " << getNumActiveSU() << " "
         << getUtilPU()      << " " << getUtilSU()      << " "
         << numOverlapNodes  << " " << numRoundMin      << " "
         << numRoundMax      << " " << numRoundAvg      << " "
         << lenRelayMin      << " " << lenRelayMax      << " "
         << lenRelayAvg; 
  Report::getInstance()->addReport(output.str());
}


