#include<fstream>
using namespace std;
#include "packethandler.h"
#include "agent.h"
#include "packet.h"
#include "report.h"

PacketHandler :: PacketHandler(Agent* a){
  agent = a;
}

RouteReqPktHandler :: RouteReqPktHandler(Agent *a)
  :PacketHandler(a){}
RouteRepPktHandler :: RouteRepPktHandler(Agent *a)
  :PacketHandler(a){}
RouteConfirmPktHandler :: RouteConfirmPktHandler(Agent *a)
  :PacketHandler(a){}

void RouteReqPktHandler :: handle(Packet *pkt, int sender){
  Agent *agent = getAgent();
  switch(agent->getState()){
    
  case ST_WAIT2CONFIRM:// note that there is no breakpoint after this case
    if(agent->getCoopRound() < pkt->getId())
      {
	agent->clear();
	agent->setStatus(false);
	agent->setState(ST_IDLE);
      }
  case ST_IDLE:
    if(agent->getBeta()>THRESHOLD && pkt->getId()){
      Report::getInstance()->addReport(agent->getUser(), sender,pkt,"r");
     
      agent->setCoopRound(pkt->getId());// ignore offers if it is from previous round
      agent->addRecipient(sender);

      agent->setDest(pkt->getDest()); // check whether we need that or not
      agent->setSrc(pkt->getSrc()); // check whether we need that or not	

      agent->setDirectDataRate(pkt->getDirectDataRate());
      agent->setDirectDelay(pkt->getDirectDelay());
      agent->setDirectTrafficRate(pkt->getDirectTrafficRate());
      
      
      if(agent->getUser()->isNeighbor(pkt->getDest()) || agent->getUser()->numOfNeighbors() == 1){
	// no need to forward its to neighbors go directly to send reply packets
	//agent->setLastOfferId(pkt->getDest());
	agent->setState(ST_DECIDE);
	delete pkt;
      }
      else{
	agent->setState(ST_WAITING);
	agent->setFwdPacket(pkt);
      }
    }
    break;
  case ST_WAITING:
    Report::getInstance()->addReport(agent->getUser(), sender,pkt,"r");
    agent->addRecipient(sender);
    delete pkt;
    break;
  case ST_DECIDE:
    Report::getInstance()->addReport(agent->getUser(), sender,pkt,"r");
    agent->addRecipient(sender);
    delete pkt;
    // should not receive route-req pkt at this state
    break;
  }
}

void RouteRepPktHandler :: handle(Packet *pkt, int sender){
  Agent* agent = getAgent();
  switch(agent->getState()){
  case ST_IDLE:
    delete pkt;
    pkt = NULL;
    break;
  case ST_WAITING:
    Report::getInstance()->addReport(agent->getUser(), sender,pkt,"r(rp)");
    //cout << agent->getUser()->getId() << " receives from " << sender << " : " << pkt->getSrc() << endl;
    agent->addOffer(pkt, sender);
    break;
  case ST_DECIDE:
    delete pkt;
    pkt = NULL;
    break;
  default:
    delete pkt;
    pkt = NULL;
    break;
  }  
}

void RouteConfirmPktHandler :: handle(Packet *pkt, int sender){
  // either create new state or use st-idle for it
  Agent* agent = getAgent();
  Report::getInstance()->addReport(agent->getUser(), sender,pkt,"r(cf)");
  switch(agent->getState()){
  case ST_IDLE:
  case ST_WAIT2CONFIRM:  
    // check whether you have unconfirmed offer packet in cooptable
    // it means the user is waiting for confirmation
    agent->setStatus(true);
    if(agent->isActive == false){
	agent->isActive = true;
	agent->getUser()->numOfActiveAgents++;
    }
    // since it is a confirmation of acceptance of the last offer
    // we may want to update the offer records
    agent->forwardPacket(pkt);
    agent->clear();
    break;
  case ST_WAITING:
    // should not receive a confirmation pkt while waiting
    //cout << "should not be here" << endl;
    break;
  case ST_DECIDE:
    // should not receive a confirmation pkt while deciding
    break;
  }
}

// recipientList (who wants reply) + coopOfferList (who replied)
// = neighborList
