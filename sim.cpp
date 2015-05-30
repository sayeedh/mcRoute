#include<math.h>
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

#include "sim.h"
#include "userVisitor.h"

Simulation* Simulation::simInstance = 0;

Simulation::Simulation(char* f)
: userList(0) {
    //topo = new RoundTopology(np, ns, nr,tr,f);
    initialize(f);

}

Simulation::~Simulation() {
    int i;
    for (i = 0; i < nPUser + nPUser + nSUser; i++) {
        if (userList[i] != NULL) {
            delete userList[i];
        }
    }
    userList.clear();
}

void Simulation::initialize(char* f) {
    ifstream inFile;
    int i, nrow, ncol, nUser, x, y;

    double prate = 1.0, srate = 0.5;
    inFile.open(f);
    inFile >> simTime >> nPUser >> nSUser >> nrow >> ncol;
    userList.resize(nPUser + nPUser + nSUser);
    nUser = nPUser + nPUser + nSUser;

    // sets the location of primary sources
    for (i = 0; i < nPUser; i++) {
        userList[i] = new PrimaryUser(i, prate);
        userList[i + nPUser] = new PrimaryUser(i + nPUser, 0);
        userList[i]->setTarget(userList[i + nPUser]);
        inFile >> x >> y;
        userList[i]->setLoc(x, y);
       // cout << x << y << endl;
    }
    // sets the location of primary destinations
    for (; i < nPUser + nPUser; i++) {
        inFile >> x >> y;
       // cout << x << y << endl;
        userList[i]->setLoc(x, y);
    }
    // sets the location of secondary users
    for (; i < nUser; i++) {
        userList[i] = new SecondaryUser(i, srate);
        inFile >> x >> y;
        userList[i]->setLoc(x, y);
    }
    // sets the primary arrival rate from the range
    x = 2.5;
    y = 3.5;
    //inFile >> x  >> y;	 
    for (i = 0; i < nPUser; i++) {
        userList[i]->setTrafficRate((x + rand() % (y - x)) / 1000);
    }
    // sets the secondary arrival rate from the range
    x = 2.0;
    y = 3.0;
    // inFile >> x >> y;
    for (i = nPUser + nPUser; i < nUser; i++) {
        userList[i]->setTrafficRate((x + rand() % (y - x)) / 1000);
    }
    // each line lists the neighbors of each primary user
    string str;
    getline(inFile,str);
    for (i = 0; i < nPUser; i++) {
        getline(inFile, str);
        stringstream iss(str);
        while (iss >> x) {
            userList[i]->addNeighbor(userList[x - 1]);
        }
    }
    // each line lists the neighbors of each secondary user and also set the neighbors of the pu targets
    string str1;
    for (i = nPUser + nPUser; i < nUser; i++) {
        getline(inFile, str1);
        stringstream iss(str1);
        while (iss >> x) {
            userList[i]->addNeighbor(userList[x - 1]);
	    if(x > nPUser and x <= nPUser + nPUser)
	    {
		userList[x-1]->addNeighbor(userList[i]);
	    }	 
        }
    }
    inFile.close();
}

Simulation* Simulation::getInstance(char* f) {
    if (simInstance == NULL) {
        simInstance = new Simulation(f);
    }
    return simInstance;
}

double Simulation::getDistance(int i, int j) {
    double distance;
    Loc iNode = userList[i]->getLoc();
    Loc jNode = userList[j]->getLoc();
    distance = (iNode.x - jNode.x)*(iNode.x - jNode.x)+ (iNode.y -
            jNode.y)*(iNode.y - jNode.y);
    distance = sqrt(distance);
    return distance;
}

bool Simulation::isNeighbor(int i, int j) {
    return false;
}

void Simulation::init() {
    createNeighborAndLinkRateList();
    //printNumOfNeighbors();
}

void Simulation::run() {
    bool isConverged = false;
    for (curTime = 0; curTime < simTime && !isConverged; curTime++) {
        routeSetup(curTime);
    }
}

bool Simulation::checkStopCondition(int curTime) {
    int count = 0;
    for (int i = 0; i < nPUser; i++) {
        count += (userList[i]->checkStopCondition());
    }
    return (count == nPUser);
}

/*For each pu
  1. set su neighbors
  2. set link rate to neighbors
  3. set direct data rate to destination
  4. set direct delay to  destination

  For each su
  1. set pu and su neighbors
  2. set link rate to each neighbor
  3. set link rate to each pu destination
  4. set power profile to each pu destination
 */
void Simulation::createNeighborAndLinkRateList() {
    double distance, linkRate, receivedPower, powerDB;
    int nu = nPUser + nPUser + nSUser;

    // PUs to SUs
    for (int i = 0; i < nPUser + nPUser; i++) {
        for (int j = 0; j < userList[i]->numOfNeighbors(); j++) {
            int neighborId = userList[i]->getNeighbor(j)->getId();
            distance = getDistance(i, neighborId);
            receivedPower = pow(BASED / distance, PATHLOSS);
            powerDB = 10 * log10(TXPOWER1) + 10 * log10(receivedPower);
            //cout << i << " " << j <<" "<< distance <<" "<< powerDB << " " << endl;
            if (powerDB >= RXPOWER) {
                linkRate = BANDWIDTH * log2(1 + TXPOWER1 * receivedPower / (NOISE * BANDWIDTH));
                /*userList[i]->addNeighbor(userList[j]);
                userList[j]->addNeighbor(userList[i]);
                 */
                userList[i]->setNeighborLinkRate(neighborId, linkRate);
                userList[neighborId]->setNeighborLinkRate(i, linkRate);
                if (i >= nPUser) {
                    userList[neighborId]->setTargetPower(i, TXPOWER1);

                }

            }
        }
    }

    //  cout << "su to su" << endl;

    // SUs to SUs
    for (int i = nPUser + nPUser; i < nu; i++) {
        for (int j = 0; j < userList[i]->numOfNeighbors(); j++) {
            int neighborId = userList[i]->getNeighbor(j)->getId();
            distance = getDistance(i, neighborId);
            receivedPower = pow(BASED / distance, PATHLOSS);
            powerDB = 10 * log10(TXPOWER1) + 10 * log10(receivedPower);
            // cout << i << " " << j <<" "<< distance <<" "<< powerDB << " " << endl;
            if (powerDB >= RXPOWER) {
                linkRate = BANDWIDTH * log2(1 + TXPOWER1 * receivedPower / (NOISE * BANDWIDTH * 1000));
                /*
                userList[i]->addNeighbor(userList[j]);
                userList[neighborId]->addNeighbor(userList[i]);
                 */
                userList[i]->setNeighborLinkRate(neighborId, linkRate);
                userList[neighborId]->setNeighborLinkRate(i, linkRate);
            }
        }
    }

    // source PUs to target PUs
    for (int i = 0; i < nPUser; i++) {
        distance = getDistance(i, i + nPUser);
        receivedPower = pow(BASED / distance, PATHLOSS);
        powerDB = 10 * log10(TXPOWER1) + 10 * log10(receivedPower); //(NOISE*BANDWIDTH);
        //cout << i << " " << i+nPUser <<" "<< distance <<" "<< powerDB << " " << endl;
        linkRate = BANDWIDTH * log2(1 + TXPOWER1 * receivedPower / (NOISE * BANDWIDTH * 1000));
        userList[i]->setTargetLinkRate(0, linkRate);
        double lambda = userList[i]->getTrafficRate();
        double delay = lambda * PKTLEN * PKTLEN * 1000 / (2 * linkRate * (linkRate - lambda * PKTLEN)) + PKTLEN * 1000 / linkRate; // microsecond
       // cout << delay << endl;
        userList[i]->setTargetDelay(delay);
    }


    // SUs to target PUs
    double powerLevel;
    for (int i = nPUser; i < nPUser + nPUser; i++) {
        for (int j = nPUser + nPUser; j < nu; j++) {
            if (userList[j]->isNeighbor(userList[i]->getId())) continue;
            powerLevel = TXPOWER1;

            distance = getDistance(i, j);
            while (1) {
                powerLevel += 0.1;
                receivedPower = pow((BASED / distance), PATHLOSS);
                powerDB = 10 * log10(powerLevel) + 10 * log10(receivedPower);
                if (powerDB >= RXPOWER) {
                    linkRate = BANDWIDTH * log2(1 + powerLevel * receivedPower / (NOISE * BANDWIDTH * 1000));
                    userList[j]->setTargetLinkRate(i, linkRate);
                    userList[j]->setTargetPower(i, powerLevel);
                    break;

                }

            }
            // cout << i << " " << j <<" "<< distance <<" "<< powerDB << " " << endl;
        }
    }
}

void Simulation::generatePkts(double t) {
    int nU = nPUser + nSUser + nPUser;
    for (int i = 0; i < nPUser; i++) {
        userList[i]->generateTraffic(t);
    }
    for (int i = nPUser + nPUser; i < nU; i++) {
        userList[i]->generateTraffic(t);
    }
}

void Simulation::routeSetup(double curTime) {
    // send the route request packet
    int nU = nPUser + nSUser + nPUser;
    for (int i = 0; i < nPUser; i++) {
        userList[i]->step(curTime);
    }
    for (int i = nPUser + nPUser; i < nU; i++) {
        userList[i]->step(curTime);
    }

}

void Simulation::printNumOfNeighbors() {
    int i;
    double avgNeighbors = 0;
    for (i = 0; i < nPUser + nSUser + nPUser; i++) {
        avgNeighbors += userList[i]->numOfNeighbors();
        cout << i << " has " << userList[i]->numOfNeighbors() << endl;
    }
    cout << "Average :" << avgNeighbors / i << endl;
}

void Simulation::show_results() {
    int i;
    UserVisitor* pathVisitor = new PathVisitor();
    for (i = 0; i < nPUser; i++) {
        userList[i]->accept(pathVisitor);
    }

    pathVisitor->print();
    delete pathVisitor;
    UserVisitor* profitVisitor = new ProfitVisitor();
    for (i = 0; i < nPUser; i++)
        userList[i]->accept(profitVisitor);
    for (i = nPUser + nPUser; i < nPUser + nPUser + nSUser; i++) {
        userList[i]->accept(profitVisitor);
    }
    delete profitVisitor;
}

/*
void Simulation::createNodeList(int chcond, int alg, double prate, double srate)
{
  int i, nUser;
  nUser = nPUser + nSUser;

  for(i = 0;i<nPUser;i++){
    userList[i] = (User *) new PrimaryUser(i,prate);
    userList[i]->setTarget(i+nUser);
  }    
  for(;i<nUser;i++){
    userList[i] = (User *) new SecondaryUser(i,srate);
  }
  for(;i<nUser+nPUser;i++){
    userList[i] = (User*) new PrimaryUser(i, 0.0);
  }
  std::cout<< nUser << " Users created" << endl;

}
 */


void Simulation::createNeighborList(bool isPrint = false) {
    /*  int i,j;

      int nU = nPUser + nPUser + nSUser;
      for(i=0;i < nPUser; i++){
        for(j=nPUser+nPUser;j<nU;j++){
          if(isNeighbor(i,j)){
            if(!isPrint){
              userList[i]->addNeighbor(userList[j]);
              userList[j]->addNeighbor(userList[i]);
            }
            cout <<"   :User " << j<< endl;
          }
        }
      }
      for(int i=nPUser+nPUser;i<nU;i++){
        for(int j=i+1;j<nU;j++){
          if(topo->isNeighbor(i,j)){
            if(!isPrint){
              userList[i]->addNeighbor(userList[j]);
              userList[j]->addNeighbor(userList[i]);
            }
            cout <<"user " << i <<": user " << j<< endl;
          }
        }
      }
  
      // determine the target neighborhood
      for(int i = nPUser; i < nU; i++){
        for(int j = nU; j < nU + nPUser; j++){
          if(topo->isNeighbor(i,j)){
            userList[i]->addNeighbor(userList[j]);
            //		cout <<"user " << i <<": user " << j<< endl;
          }
        }
      }*/
}

void Simulation::applyNeighborList() {
    /*  ifstream read("in.txt");
      int lines,a,b,n;
      read>>lines;//number of lines
      for(int i=1;i<=lines;i++){
        read>>a;
        read>>n;
        for (int j=1;j<=n;j++){
          read>>b;
          userList[a]->addNeighbor(userList[b]);
          userList[b]->addNeighbor(userList[a]);
        }
      }*/
}

void Simulation::createLinkRateList() {
    /*  double distance, linkRate;
      int neighborId;
      int nU = nPUser + nSUser;
      for(int i=0;i<nU;i++){
        //    cout << "uid " << i << endl;
        for(int k = 0; k < userList[i]->numOfNeighbors();k++){
          neighborId = userList[i]->getNeighbor(k)->getId();
          distance = topo->getDistance(userList[i]->getId(), neighborId);
          linkRate = BANDWIDTH*log2(1 + txPower*pow((BASED/distance),PATHLOSS)/(NOISE*BANDWIDTH)); 
          userList[i]->setNeighborLinkRate(userList[i]->getNeighbor(k)->getId(), linkRate);
          //      cout << "nid: " << neighborId << " " << linkRate << endl;
        }
      }*/
}

void Simulation::applyLinkRateList() {
    /*  ifstream read("link.txt");
      int lines,a,b,n;
      read>>lines;//number of lines
      for(int i=1;i<=lines;i++){
        read>>a;
        read>>n;
        for (int j=0;j<userList[a]->numOfNeighbors();j++){
          if(userList[a]->getNeighbor(j)->getId()<= userList[a]->getId())
            continue;
          read>>b;
          userList[a]->setNeighborLinkRate(userList[a]->getNeighbor(j)->getId(),b);
          userList[a]->getNeighbor(j)->setNeighborLinkRate(userList[a]->getId(),b);
        }
      }
     */
}
