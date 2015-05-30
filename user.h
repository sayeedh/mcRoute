#ifndef USER_H
#define USER_H
#include<vector>
#include<queue>
#include<map>

#include "topo.h"
#include "packet.h"
#include "util.h"

class UserVisitor;
class Agent;
/* class User is the base class for any kind of user in the network.

*/
class User {
private:
    int id; 		// user id
    Loc userPos; 	// user location in (x,y) format
    double trafficRate; // traffic rate of the user
    int round; 		// current cooperation round
    double wfactor; 	// weight ratio between own benefit and contribution of a user
protected:
    User();
    User(int i, double r);
public:
    virtual ~User();
    void accept(UserVisitor *uv);

    /* get methods for user's properties */
    inline int getId()const {
        return id;
    }
    inline Loc getLoc() {
        return userPos;
    }
    inline double getFactor()const {
        return wfactor;
    }
    virtual int getRound() const {
        return round;
    }
    inline double getTrafficRate()const {
        return trafficRate;
    }

    /* set methods for user's properties */
    inline void setTrafficRate(double l) {
        trafficRate = l;
    }
    inline void setLoc(int x, int y) {
        userPos.x = x;
        userPos.y = y;
    }
    inline void advanceRound() {
        round++;
    }
    inline void setFactor(double w) {
        wfactor = w;
    }

    /* get  methods for target properties*/
    virtual int getTarget()const {
        return 0;
    }
    virtual double getTargetLinkRate(int i) {
        return 0;
    }
    virtual double getTargetDelay() {
        return 0;
    }
    virtual double getTargetPower(int i) {
        return 0;
    }

    /* set methods for target properties*/
    virtual void setTarget(User* u) {
    }
    virtual void setTargetLinkRate(int i, double r) {
    }
    virtual void setTargetDelay(double r) {
    }
    virtual void setTargetPower(int i, double r) {
    }

    /* get/set methods for routing properties */
    double getCurrentProfitFor(int);
    virtual double getResourceNeeded() = 0; /* only for SUs */
    virtual double getProfit() = 0;

    virtual double calculateDelayOffer() {
        return 0;
    }

    /* get methods for neighborhood */
    inline int numOfNeighbors() {
        return neighborList.size();
    }
    double getNeighborLinkRate(int);
    inline User* getNeighbor(int i) {
        return neighborList[i];
    }
    User* getNeighborById(int id);
    bool isNeighbor(int id);

    /* set method for neighborhood */
    void addNeighbor(User*);
    virtual void setNeighborLinkRate(int uid, double rate);

    /* get methods for agents */
    inline int numOfAgents() {
        return agentList.size();
    }

    inline Agent* getAgent(int index) {
        return agentList[index];
    }
    double getRate(int id);
    virtual Agent* findAgent(int id) = 0;

    /* packet related methods */
    virtual void generateTraffic(double t);
    virtual void step(int);
    void sendPacket(){} ;
    virtual void receivePacket(Packet*, int) = 0;
    inline virtual bool checkStopCondition() {
        return false;
    }
protected:
    std::vector<User*> neighborList; 	// list of users in its neighborhood
    std::vector<Agent*> agentList; 	// list of agents attached to the user
    std::map<int, double> linkRateList; // id and linkrate of users in its neighborhood
public:
    int numOfActiveAgents; // number of active agents of a user
};

/*
class PrimaryUser represents a secondary user in the network. It inherits all the basic properties of a user and adds its own distinct properties
*/
class PrimaryUser : public User {
public:
    PrimaryUser(int i, double r);

    /* agent related methods */		
    Agent* findAgent(int id);

    /* packet related methods */
    virtual void receivePacket(Packet*, int);

    /* routing properties methods*/
    double getProfit();
    double getResourceNeeded() {
        return 0;
    }
    bool checkStopCondition();

    /* get/set methods for target properties */
    inline int getTarget()const {
        return target->getId();
    }
    double getTargetLinkRate(int i) {
        return targetDataRate;
    }
    double getTargetDelay() {
        return targetDelay;
    }
    inline void setTarget(User* u) {
        target = u;
    }
    void setTargetLinkRate(int i, double r) {
        targetDataRate = r;
    }
    void setTargetDelay(double r) {
        targetDelay = r;
    }

private:
    User* target; 	   // primary destination of this user
    double targetDataRate; // target data rate to the destination
    double targetDelay;    // target delay to the destination
};

/*
class SecondaryUser represents a secondary user in the network. It inherits all the basic properties of a user and adds its own distinct properties
*/

class SecondaryUser : public User {
public:
    SecondaryUser(int i, double rate);

    /* agent related methods */		
    Agent* findAgent(int src);

    /* packet related methods */
    virtual void receivePacket(Packet*, int);

    /* routing propeties */
    double getResourceNeeded() {
        return reqPeriod;
    }
    double getProfit();
    int getRound();

    /* set/get methods for target */
    double getTargetLinkRate(int id);
    double getTargetPower(int id); 
    void setTargetLinkRate(int id, double r); 
    void setTargetPower(int id, double pw); 
    double getRemainingXtime(int index);
    double getRemainingYtime(int index);
private:
    double reqPeriod;				// required amount of cycle time
    std::map <int, double> destLinkRate;	// id and linkrate to each possible destination
    std::map <int, double> destPower;		// id and power level to each possible destination
    std::vector <User*> priDestinationList;	// list of all primary destinations
};
#endif
