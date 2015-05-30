#ifndef SIM_H
#define SIM_H
#include<iostream>
#include<vector>
using namespace std;

#include "user.h"
//#include "topo.h"
#include "userVisitor.h"

class Simulation {
private:// simulation parameters
    // Topology *topo;
    vector<User *> userList;
    int curTime;
    int simTime;
    int nPUser;
    int nSUser;
    double txPower;
    static Simulation* simInstance;

    Simulation(char* f);
    void initialize(char* f);
    double getDistance(int, int);
    bool isNeighbor(int, int);

    void createNodeList(int, int, double, double);
    void applyLinkRateList();
    void createLinkRateList();
    void createNeighborList(bool);
    void createNeighborAndLinkRateList();
    void generatePkts(double);
    void routeSetup(double);
    void applyNeighborList();
    void printNumOfNeighbors();
    bool checkStopCondition(int);
public:
    static Simulation* getInstance(char* f = NULL);

    int getCurrentTime() {
        return curTime;
    }
    void init();
    void run();
    void show_results();
    ~Simulation();
};

#endif
