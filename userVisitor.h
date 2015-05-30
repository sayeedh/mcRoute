#ifndef USERVISITOR_H
#define USERVISITOR_H
#include<map>
#include "user.h"

/*
class UserVisitor is the base visitor class for statistics
*/
class UserVisitor{
 private:
 
 public:
  UserVisitor(){}
  virtual ~UserVisitor(){}
  virtual void visitDefault(User* u){};
  virtual int getDirPkts(){return 0;}
  virtual int getRlPkts(){return 0;}
  virtual int getDropPkts(){return 0;}
  virtual int getGenPkts(){return 0;}
  virtual int getNeighbors(){return 0;}
  virtual int getNumActivePU(){return 0;}
  virtual int getNumActiveSU(){return 0;}
  virtual double getUtilPU(){return 0;}
  virtual double getUtilSU(){return 0;}
  virtual void print(){}
};

/*
This derived visitor class that summarizes the profit of each and every user in the network
*/
class ProfitVisitor : public UserVisitor
{
 private:
  int numPU;
  int numSU;
  double utilPU;
  double utilSU;
 public:
  ProfitVisitor();
  void visitDefault(User* u){};
  int getNumActivePU(){return numPU;}
  int getNumActiveSU(){return numSU;}
  double getUtilPU(){return utilPU;}
  double getUtilSU(){return utilSU;}
};

/* 
This derived class PathVisitor summarizes the statistics related to cooperative routing path
*/
class PathVisitor : public UserVisitor
{
 private:
  std::map<int, double> coopNodes;
  std::map<int, int> freqNodes;
  int numPU;
  int numSU;
  double utilPU;
  double utilSU;

  int numRoundMin;
  int numRoundMax;
  int numRoundAvg;

  int lenRelayMin;
  int lenRelayMax;
  int lenRelayAvg;

  double sumPUNeigh;
  double sumRelayNeigh;
 public:
  int getNumActivePU(){return numPU;}
  int getNumActiveSU(){return numSU;}
  double getUtilPU(){
    if(numPU != 0) return utilPU/numPU;
    else return utilPU;
  }
  double getUtilSU(){
    if(numSU != 0) return utilSU/numSU;
    else return utilSU;
  }
 public:
  PathVisitor();
  ~PathVisitor();
  void visitDefault(User* u);
  void print();
};
#endif
