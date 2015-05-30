#include<iostream>
#include<fstream>
using namespace std;

#include "user.h"
#include "userVisitor.h"
#include "agent.h"
#include "report.h"

/*
Constructor of class User
two parameters : i - id of the user, r - traffic rate
*/
User::User(int i, double r) {
    id = i;
    trafficRate = r;
    round = 1;
    wfactor = 0.1;
}

/*
Constructor of class PrimaryUser
*/
PrimaryUser::PrimaryUser(int i, double r)
: User(i, r) {
    agentList.push_back(new PUAgent(this, -1));
}

/*
Constructor of class SecondaryUser
*/
SecondaryUser::SecondaryUser(int i, double rate)
: User(i, rate) {
    reqPeriod = VACATIONTIME; 
    numOfActiveAgents = 0;
}

/*
returns the total profit earned by the primary user
*/
double PrimaryUser::getProfit() {
    return agentList[0]->getProfit();
}

/*
returns the total profit earned by the secondary user
*/
double SecondaryUser::getProfit() {
    double total = 0;
    double x = 0, y = 0;
    for (int i = 0; i < (int) agentList.size(); i++) {
        SUAgent* sa = dynamic_cast<SUAgent*> (agentList[i]);
        if (sa->getStatus() == true && sa->isActive == true) {
            coopOfferSU sOffer = sa->lastCoopOffer;
            x = x + sOffer.xCycleTime;
            y = y + sOffer.yCycleTime * sOffer.powerLevel;
        }
    }
    if (y == 0)
        return 0;
    total = x / y; // should mutiply it with data rate to get bit per energy
    return total;
}

void User::generateTraffic(double curTime) {
    // for now, we are considering single traffic generator
}

void User::accept(UserVisitor *uv) {
    uv->visitDefault(this);
}

/*
The method searches for a neighbor of given id. if found, the pointer to the user is returned, otherwise NULL
*/
User* User::getNeighborById(int id) {
    for (unsigned int i = 0; i < neighborList.size(); i++) {
        if (id == neighborList[i]->getId()) {
            return neighborList[i];
        }
    }
    return NULL;
}

/*
The method returns the maximum of the round of all the agents of a secondary user
*/
int SecondaryUser::getRound() {
    int r = 1;
    for (int i = 0; i < (int) agentList.size(); i++) {
        r = max(r, agentList[i]->getCoopRound());
    }
    return r;
}

/*
The method adds a user to the neighbor list
*/
void User::addNeighbor(User* u) {
    if (u == this) return;
    neighborList.push_back(u);
}

/*
The method checks whether the given id is a neighbor or not
*/
bool User::isNeighbor(int id) {
    for (unsigned int i = 0; i < neighborList.size(); i++) {
        if (id == neighborList[i]->getId()) {
            return true;
        }
    }
    return false;
}

/*
The destructor method deletes all the agents of the user
*/
User::~User() {
    for (unsigned int i = 0; i < agentList.size(); i++) {
        delete agentList[i];
    }
    agentList.clear();
}

/*
The method duo sets and gets the link rate of a given id
*/
void User::setNeighborLinkRate(int uid, double rate) {
    linkRateList[uid] = rate;
}

double User::getNeighborLinkRate(int uid) {
    return linkRateList[uid];
}

/*
The method starts operation of a user at each time period 
*/
void User::step(int curTime) {
    for (unsigned int i = 0; i < agentList.size(); i++) {
        agentList[i]->step(curTime);
    }
}

/*
The method calculates the remaining cycle time of a secondary user considering all active agents
*/
double SecondaryUser::getRemainingXtime(int index) {
    double total = 0;
    for (int i = 0; i < (int) agentList.size(); i++) {
        if (index == agentList[i]->getSrc() || agentList[i]->isActive == false) continue;
        total += dynamic_cast<SUAgent*> (agentList[i])->lastCoopOffer.xCycleTime;
    }
    return total;
}

/*
The method calculates the remaining pu time of a secondary user considering all active agents
*/
double SecondaryUser::getRemainingYtime(int index) {
    double total = 0;
    for (int i = 0; i < (int) agentList.size(); i++) {
        if (index == agentList[i]->getSrc() || agentList[i]->isActive == false) continue;
        total += dynamic_cast<SUAgent*> (agentList[i])->lastCoopOffer.yCycleTime;
    }
    return total;
}

double User::getRate(int id) {
    return linkRateList[id];
}

/*
The method returns a pointer to an agent of a given source
*/
Agent* PrimaryUser::findAgent(int src) {
    return agentList[0];
}

/*
The method checks the stop condition for the primary user
*/
bool PrimaryUser::checkStopCondition() {
    return (agentList[0]->getBeta() < THRESHOLD);
}

/*
The method returns a pointer to an agent of a given primary user. If not found, a new agent is created and returned
*/
Agent* SecondaryUser::findAgent(int src) {
    for (unsigned int i = 0; i < agentList.size(); i++) {
        if (agentList[i]->getSrc() == src)
            return agentList[i];
    }
    Agent* agent = new SUAgent(this, src);
    agentList.push_back(agent);

    return agent;
}

/*
The method receives a packet from a sender and passes it to an appropriate agent
*/
void PrimaryUser::receivePacket(Packet* pkt, int sender) {
    agentList[0]->receivePacket(pkt, sender);
}

/*
The method receives a packet from a sender and passes it to an appropriate agent
*/
void SecondaryUser::receivePacket(Packet* pkt, int sender) {
    int src = pkt->getSrc();
    Agent* agent = findAgent(src);
    agent->receivePacket(pkt, sender);
}

/*
specific get/ set methods for a secondary user
*/
double SecondaryUser::getTargetLinkRate(int id) {
    return destLinkRate[id];
}
double SecondaryUser::getTargetPower(int id) {
    return destPower[id];
}
void SecondaryUser::setTargetLinkRate(int id, double r) {
    destLinkRate[id] = r;
}
void SecondaryUser::setTargetPower(int id, double pw) {
    destPower[id] = pw;
}
