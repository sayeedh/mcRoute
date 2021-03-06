#include<fstream>
using namespace std;

#include "agent.h"
#include "packet.h"
#include "packethandler.h"
#include "report.h"
#include "packetfactory.h"
#include "sim.h"

/*
The constructor of the base Agent class
*/
Agent::Agent(User* u1, int srcId)
: u(u1), src(srcId), dest(0), state(ST_IDLE), fwdTimer(0), beta(1), coopRound(1), directDataRate(0), directDelay(0.0), directTrafficRate(0), isAccepted(false), lastOfferId(-1) {
    isActive = true;
    u->numOfActiveAgents++;
}

/*
The destructor clears resources
*/
Agent::~Agent() {
    clear();
}

/*
The destructor of the su agent
*/
SUAgent::~SUAgent() {
    if (fwdPkt != NULL) {
        delete fwdPkt;
        fwdPkt = NULL;
    }
}

/*
The method adds a user into its recipient list and if all neighbors are configured either as recipient/ provider, the agent changes to decision state
*/
void Agent::addRecipient(int sender) {
    if (!isOfferProvider(sender) && !isRecipient(sender)) {
        recipientList[sender] = 1;
        if (coopOfferList.size() == getUser()->numOfNeighbors() - recipientList.size()) {
             setState(ST_DECIDE);
        }
    }
}

/*
The method adds an offer from the sender into the offer list  and if all neighbors are configured either as recipient/ provider, the agent changes to decision state. Otherwise, delete the packet
*/
void Agent::addOffer(Packet* pkt, int sender) {
    if (!isOfferProvider(sender) && !isRecipient(sender)) {
        coopOfferList[sender] = pkt;
        if (coopOfferList.size() == getUser()->numOfNeighbors() - recipientList.size()) {
            setState(ST_DECIDE);
        }
    } else {
        delete pkt;
    }
}

/*
The method clears the offer and recipient list
*/
void Agent::clear() {
    clearOffer();
    recipientList.clear();
}

void Agent::clearOffer() {
    std::map<int, Packet*>::iterator it;
    for (it = coopOfferList.begin(); it != coopOfferList.end(); it++) {
        delete (*it).second;
    }
    coopOfferList.clear();
}

/*
The method sets a pkt to forward it in next time period
*/
void SUAgent::setFwdPacket(Packet* pkt) {

    if (fwdPkt == NULL) {
        fwdPkt = pkt;
        setFwdTimer(Simulation::getInstance()->getCurrentTime() + 1);
    } else {
        delete pkt;
        pkt = NULL;
    }
}

/*
The method starts the process for an agent at a given time. Based on the agent condition, it performs different tasks e.g. waits for a new packet, or forwards a packet
*/
void SUAgent::step(int curTime) {
    switch (getState()) {
        case ST_IDLE:
            break;
        case ST_WAITING:
        case ST_WAIT2CONFIRM:
            if (fwdPkt != NULL && getFwdTimer() == curTime) {
                forwardPacket(fwdPkt);
                delete fwdPkt;
                fwdPkt = NULL;
            }
            break;
        case ST_DECIDE:
            sendPacket();
            break;
        default:
            break;
    }
}

/*
The method forwards the packet pkt to its neighbors. The packet can be either route_req or route_confirm packet
*/
void SUAgent::forwardPacket(Packet *pkt) {
    User* u = getUser();
    switch (pkt->getPktType()) {
        case ROUTE_REQ:
            for (int i = 0; i < u->numOfNeighbors(); i++) {
                if (!isRecipient(u->getNeighbor(i)->getId()) && u->getNeighbor(i)->getId() != pkt->getDest()) {
                    Report::getInstance()->addReport(u, u->getNeighbor(i), pkt, "f(rq)");
                    u->getNeighbor(i)->receivePacket(pkt->clone(), u->getId());
                }
            }
            break;
        case ROUTE_CONFIRM:
            int uid;
            uid = getLastOfferId();
            if (uid != -1 && uid != getDest()) {
                Report::getInstance()->addReport(u, u->getNeighborById(uid), pkt, "f(cf)");
                u->getNeighborById(uid)->receivePacket(pkt->clone(), u->getId());
            }
            break;
        default:
            break;
    }

}

/*
The method sends a reply (packet) to a previous request (packet). There are two cases, it can initiate the reply or it adds its own offer and contribution with it
The reply packet deals with neighbors' offers and also considers its own demand and contribution
*/ 
void SUAgent::sendPacket() {
    User* u = getUser();
    PacketFactory* pktFactory;
    Packet* repPkt;
    switch (getState()) {
        case ST_DECIDE:
            pktFactory = new ReplyPacketFactory();
            repPkt = pktFactory->create(this);
            for (int i = 0; i < u->numOfNeighbors(); i++) {
                if (isRecipient(u->getNeighbor(i)->getId())) {
                    Report::getInstance()->addReport(u, u->getNeighbor(i), repPkt, "s(rp)");
                    u->getNeighbor(i)->receivePacket(repPkt->clone(), u->getId());
                }
            }
            setState(ST_WAIT2CONFIRM);
            delete repPkt;
            delete pktFactory;
            repPkt = NULL;
            pktFactory = NULL;
            break;
        default:
            break;
    }
}

/*
The method receives a packet from its neighbors. The packet can be either route_req, route_reply, or route_confirm packet
*/
void SUAgent::receivePacket(Packet* pkt, int sender) {
    PacketHandler *h;
    switch (pkt->getPktType()) {
        case ROUTE_REQ:
            h = new RouteReqPktHandler(this);
            h->handle(pkt, sender);
            delete h;
            h = NULL;
            break;
        case ROUTE_REPLY:
            h = new RouteRepPktHandler(this);
            h->handle(pkt, sender);
            delete h;
            h = NULL;
            break;
        case ROUTE_CONFIRM:
            h = new RouteConfirmPktHandler(this);
            h->handle(pkt, sender);
            delete h;
            delete pkt;
            pkt = NULL;
            break;
        default:
            delete pkt;
            pkt = NULL;
            break;
    }
}

/*
The method starts the process of an agent of a primary user at given time. Based on the agent condition, it performs different tasks e.g. initiates a search, or sends a route_confirm packet
*/
void PUAgent::step(int curTime) {
    switch (getState()) {
        case ST_IDLE:
            if (getBeta() > THRESHOLD) {
                clear();
                setState(ST_WAITING);
                sendPacket();
            }
            break;
        case ST_WAITING:
            break;
        case ST_DECIDE:
            sendPacket();
            setState(ST_IDLE);
            break;
        default:
            break;
    }
}

/*
The method sends a request (packet). There are two cases, it can initiate the request or sends a route confirmation
*/ 

void PUAgent::sendPacket() {
    PacketFactory *reqPktFactory, *cfirmPktFactory;
    Packet *reqPkt, *cfPkt;
    int uid;
    User* u = getUser();
    switch (getState()) {
        case ST_WAITING:
            reqPktFactory = new RequestPacketFactory();
            reqPkt = reqPktFactory->create(this);
            for (int i = 0; i < u->numOfNeighbors(); i++) {
                if (!isRecipient(u->getNeighbor(i)->getId())) {
                    Report::getInstance()->addReport(u, u->getNeighbor(i), reqPkt, "s(rq)");
                    u->getNeighbor(i)->receivePacket(reqPkt->clone(), u->getId());
                }
            }
            delete reqPkt;
            delete reqPktFactory;
            break;
        case ST_DECIDE:
            coopOfferPU dummy;
            uid = selectBestOfferFromNeighbors(dummy);
            if (uid != -1) {
                setLastOfferId(uid);
                setStatus(true);
                cfirmPktFactory = new ConfirmPacketFactory();
                cfPkt = cfirmPktFactory->create(this);
                Report::getInstance()->addReport(u, u->getNeighborById(uid), cfPkt, "s(cf)");
                u->getNeighborById(uid)->receivePacket(cfPkt->clone(), u->getId());
                delete cfPkt;
                delete cfirmPktFactory;
                // update records
                compareOffer(dummy);
                setCurrentOffer(dummy);
                setCoopRound(getCoopRound() + 1);

            } else {
                setStatus(false);
                setCoopRound(getCoopRound() + 1);
                setBeta(getBeta() / getCoopRound());
            }
            break;
        default:
            break;
    }
}

/*
The method receives packet from a sender. NOTE: special case of dummy packet handling
*/
void PUAgent::receivePacket(Packet* pkt, int sender) {
    PacketFactory *dummyPktFactory;
    User *u = getUser();
    Packet *dummyPkt;
    switch (pkt->getPktType()) {
        case ROUTE_REQ:
            dummyPktFactory = new DummyPacketFactory();
            dummyPkt = dummyPktFactory->create(this);
            dummyPkt->setSrc(pkt->getSrc());
            dummyPkt->setDest(pkt->getDest());
            Report::getInstance()->addReport(u, u->getNeighborById(sender), dummyPkt, "d(rq)");
            u->getNeighborById(sender)->receivePacket(dummyPkt->clone(), u->getId());
            delete dummyPkt;
            delete dummyPktFactory;
            delete pkt;
            break;
        case ROUTE_CONFIRM:
            delete pkt;
            break;
        case ROUTE_REPLY:
            addOffer(pkt, sender);
            break;
    }
}

/*
The method selects the best offer in terms of profit, and returns the id of the sender NOTE: rethink initialization of the dummy offer
*/
int Agent::selectBestOfferFromNeighbors(coopOfferPU &dummy) {
    double pt, lastPt = 0;
    int uid = -1;
    dummy.coopDataRate = 10000;
    dummy.coopDelay = 0;
    dummy.coopDemand = 0;
    if (getUser()->isNeighbor(getDest())) {
        return getDest();
    }
    if (coopOfferList.size() <= 0) {
        return -1; 
    }
    std::map<int, Packet*> ::iterator it;
    for (it = coopOfferList.begin(); it != coopOfferList.end(); it++) {
        if (it->second->getId() == -1) continue; // this is a dummy packet from a primary user
        pt = calculatePUProfit(it->second);
        if (pt > lastPt) {
            lastPt = pt;
            uid = it->first;
        }
    }
    if (uid != -1)
        dummy = coopOfferList[uid]->getCoopOffer();

    return uid;
}

/*need2change*/
void PUAgent::updateRecord() {
    double b = 0;
    setBeta(b);

}

/* The method calculates pu profit based on original infromation with the cooperation offer 
*/
double PUAgent::calculatePUProfit(Packet* pkt) {
    double rateProfit, delayProfit, demand, wfactor;

    rateProfit = (pkt->getCoopDataRate() - pkt->getDirectDataRate()) / pkt->getDirectDataRate();
    delayProfit = (pkt->getCoopDelay() - pkt->getDirectDelay()) / pkt->getDirectDelay();
    ;
/*** this part of the code is for debug purposes****/
    if (getUser()->getId() == 0) {
        //   cout << "pu " << rateProfit << " " << delayProfit << endl;
    }
    if (rateProfit / delayProfit >= 10)
        cout << "Wrong " << rateProfit << "," << delayProfit << endl;
    else if (delayProfit / rateProfit >= 10)
        cout << "Wrong ??? " << rateProfit << "," << delayProfit << endl;
/*** end of debug code ****/
    demand = pkt->getCoopDemand();
    wfactor = getUser()->getFactor();

    double result = wfactor * (rateProfit - delayProfit) + (1 - wfactor) * demand;
    return result;
}

/* The method calculates pu profit based on original infromation with the cooperation offer 
*/
double SUAgent::calculatePUProfit(Packet* pkt) {
    double rateProfit, delayProfit, demand;
    double wfactor;
    rateProfit = (pkt->getCoopDataRate() - pkt->getDirectDataRate()) / pkt->getDirectDataRate();
    delayProfit = (pkt->getCoopDelay() - pkt->getDirectDelay()) / pkt->getDirectDelay();

    demand = pkt->getCoopDemand();
    wfactor = getUser()->getFactor();
/** this part of the code is for debug purpose **/
    if (getSrc() == 0) {
        //  cout << "su "  << getUser()->getId()<< " "<<rateProfit << " " << delayProfit << endl;
    }
/** end of the debug code **/
    double res = wfactor * (rateProfit - delayProfit)+(1 - wfactor) * demand;
    return res;
}

/* The method compares offer with the previous offer and sets the value of beta */
void Agent::compareOffer(coopOfferPU& pOffer) {
    User* u = getUser();
    double rateProfit = (pOffer.coopDataRate - u->getTargetLinkRate(0)) / u->getTargetLinkRate(0);
    double delayProfit = (pOffer.coopDelay - u->getTargetDelay()) / u->getTargetDelay();
    double demand = pOffer.coopDemand;
    double wfactor = getUser()->getFactor();

    double p1 = wfactor * (rateProfit - delayProfit)+(1 - wfactor) * demand;
    if (getCoopRound() == 1) {
        beta = p1;
        setBeta(beta);
        return;
    }


    rateProfit = (dynamic_cast<PUAgent*> (this)->lastCoopOffer.coopDataRate - u->getTargetLinkRate(0)) / u->getTargetLinkRate(0);
    delayProfit = (dynamic_cast<PUAgent*> (this)->lastCoopOffer.coopDelay - u->getTargetDelay()) / u->getTargetDelay();
    demand = dynamic_cast<PUAgent*> (this)->lastCoopOffer.coopDemand;
    double p2 = wfactor * (rateProfit - delayProfit)+(1 - wfactor) * demand;
    beta = (p1 - p2) / getCoopRound();
    setBeta(beta);

}

/* The method calculates the pu profit */
double PUAgent::getProfit() {

    double rateProfit, delayProfit, demand, wfactor;

    if (getStatus() == false) return 0;
    User* u = getUser();
    rateProfit = (lastCoopOffer.coopDataRate - u->getTargetLinkRate(0)) / u->getTargetLinkRate(0);
    delayProfit = (lastCoopOffer.coopDelay - u->getTargetDelay()) / u->getTargetDelay();
    demand = lastCoopOffer.coopDemand;
    wfactor = getUser()->getFactor();

    double result = wfactor * (rateProfit - delayProfit) + (1 - wfactor) * demand;

    return result;
}

double SUAgent::getProfit() {
    return 0;
}

/* The method calculates the delay offer from su cooperation offer. su sets its cooperation offer according to the following principle */
    // check the last offer and also check it status (whether accepted or not)
    // determine the power level of this route with overall power level
    // if powerLevel of this route is greater than overall power level
    //    if offer is accepted
    //        reduce yCycleTime newyCycleTime = yCycleTime - epsilon
    //    else if the offer is rejected
    //        reduce xCycleTime newxCycleTime = xCycleTime - epsilon
    // else
    //    if offer is accepted
    //       increase xCycleTime newxCycleTime = xCycleTime + epsilon
    //    else if the offer is rejected
    //       increase yCycleTime newyCycleTime = yCycleTime + epsilon
    // end
int SUAgent::calculateDelayOffer(coopOfferSU& newOffer, int uid) {
    double powerLevel, epsilonX, epsilonY;
    User* u = dynamic_cast<SecondaryUser*> (getUser());

    if (uid == getDest() || uid == -1) {
        powerLevel = u->getTargetPower(getDest());
    } else {
        powerLevel = TXPOWER1;
    }

    newOffer.powerLevel = powerLevel;
    double optimalPowerLevel = TXPOWER1; // NOTE: we can calculate the optimal power level over all agents

    if (getCoopRound() == 1) {
        newOffer.xCycleTime = u->getResourceNeeded() / u->numOfActiveAgents;
        newOffer.yCycleTime = (CYCLETIME - u->getResourceNeeded()) / u->numOfActiveAgents;
        return 0;

    }

    if (powerLevel > optimalPowerLevel) {
        if (getStatus()) {
            if (isActive == false) {
                u->numOfActiveAgents++;
                isActive = true;
            }
            epsilonY = lastCoopOffer.yCycleTime / 2;
            newOffer.yCycleTime = max(YMIN, lastCoopOffer.yCycleTime - epsilonY);
            newOffer.xCycleTime = lastCoopOffer.xCycleTime;
        } else {
            if (lastCoopOffer.xCycleTime == XMIN && isActive == true) {
                u->numOfActiveAgents--;
                isActive = false;

            }
            epsilonX = lastCoopOffer.xCycleTime / 2;
            newOffer.xCycleTime = max(XMIN, lastCoopOffer.xCycleTime - epsilonX);
            newOffer.yCycleTime = lastCoopOffer.yCycleTime;
        }
    } else {
        if (getStatus()) {
            if (isActive == false) {
                u->numOfActiveAgents++;
                isActive = true;
            }
            epsilonX = dynamic_cast<SecondaryUser*> (u)->getRemainingXtime(getSrc());
            newOffer.xCycleTime = min(lastCoopOffer.xCycleTime + epsilonX / 2, u->getResourceNeeded());
            newOffer.yCycleTime = lastCoopOffer.yCycleTime;
        } else {
            if (lastCoopOffer.yCycleTime == YMIN && isActive == true) {
                u->numOfActiveAgents--;
                isActive = false;
            }
            epsilonY = dynamic_cast<SecondaryUser*> (u)->getRemainingYtime(getSrc());
            newOffer.yCycleTime = min(lastCoopOffer.yCycleTime + epsilonY / 2, CYCLETIME - u->getResourceNeeded());
            newOffer.xCycleTime = lastCoopOffer.xCycleTime;
        }
    }
    return 0;
}

/*
The method combines su offer with the best offer from neighbor su
*/
void SUAgent::combineOffer(coopOfferSU& sOffer, coopOfferPU& pOffer) {
    int i;
    User* u = getUser();
    double yTime = 0, xTime = 0;
    for (i = 0; i < u->numOfAgents(); i++) {
        SUAgent* sa = dynamic_cast<SUAgent*> (u->getAgent(i));
        if (sa->isActive == false || sa == this) continue;
        yTime = yTime + sa->lastCoopOffer.yCycleTime;
        xTime = xTime + sa->lastCoopOffer.xCycleTime;
    }
    xTime = xTime + yTime + sOffer.xCycleTime;
    double dRate;

    if (getLastOfferId() == -1) {
        dRate = u->getTargetLinkRate(getDest());
    } else
        dRate = u->getNeighborLinkRate(getLastOfferId());

    double numOfPkts = (dRate * sOffer.yCycleTime) / (1000 * PKTLEN);
    double delay = xTime / numOfPkts;

    pOffer.coopDelay = pOffer.coopDelay + delay;

    pOffer.coopDataRate = min(pOffer.coopDataRate, dRate);
    pOffer.coopDemand = pOffer.coopDemand + sOffer.xCycleTime;

    return;
}
