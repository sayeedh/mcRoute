#include "packetfactory.h"
#include "util.h"
#include "user.h"
#include "agent.h"

Packet* RequestPacketFactory::create(Agent* a){
  User* u = dynamic_cast<PrimaryUser*>(a->getUser());
  Packet* pkt = new RequestPacket(a->getCoopRound(), u->getId(), u->getTarget(),ROUTE_REQ,u->getTargetLinkRate(0),u->getTargetDelay(),u->getTrafficRate());
  return pkt;
}

// first select the best offer from your neighbors, then call your method to add your offer with it
// add your offer with the selected one, update the ctable to indicate who is selected and what is your offer

 Packet* ReplyPacketFactory::create(Agent* a){

  Packet *pkt = new ReplyPacket(a->getCoopRound(),a->getSrc(),a->getDest(),ROUTE_REPLY,a->getDirectDataRate(), a->getDirectDelay(),a->getDirectTrafficRate());
   /* go through power levels to reach the destination directly */		
  coopOfferSU csuOffer;
  coopOfferPU cpuOffer;
  
  int uid = a->selectBestOfferFromNeighbors(cpuOffer); // in terms of pu profit
  a->setLastOfferId(uid);
  a->calculateDelayOffer(csuOffer,uid);
  a->setCurrentOffer(csuOffer);
  // so now we have csuOffer and cpuOffer, convert csuOffer to cpuOffer and add these two and add it to pkt.n
  
  a->combineOffer(csuOffer,cpuOffer);
  pkt->setCoopOffer(cpuOffer);   
  return pkt;

}

// since each sec user can directly reach primary destination, we don't need a dummy packet anymore
Packet* DummyPacketFactory::create(Agent* a){
  
  Packet* pkt = new DummyPacket(-1,-1,-1,ROUTE_REPLY,-1000,0,0);
  //  pkt->setId(a->getUser()->getRound());
  return pkt;
}

Packet* ConfirmPacketFactory::create(Agent* a){
  User* u = a->getUser();
  
  Packet* pkt = new ConfirmPacket(u->getRound(),u->getId(),u->getTarget(),ROUTE_CONFIRM,u->getTargetLinkRate(0), u->getTargetDelay(),u->getTrafficRate());
  return pkt;
}
