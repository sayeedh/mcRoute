#include "packet.h"
#include<iostream>
using namespace std;


Packet::Packet(int type){
  pktType           = type;
  id                = 0;
  destId            = -1;
  srcId             = -1;
  directDataRate    = 0;
  directTrafficRate = 0;
  coopOffer.coopDataRate      = -1;
  directDelay       = -1;
  coopOffer.coopDelay         = -1;
  coopOffer.coopDemand        = -1;

}

Packet :: ~Packet()
{

}

Packet::Packet(const Packet &pkt){
  id = pkt.id;
  pktType = pkt.pktType;
  srcId = pkt.srcId;
  destId = pkt.destId;
  directDataRate = pkt.directDataRate;
  directTrafficRate = pkt.directTrafficRate;
  directDelay = pkt.directDelay;
  coopOffer.coopDataRate = pkt.coopOffer.coopDataRate;
  coopOffer.coopDelay = pkt.coopOffer.coopDelay;
  coopOffer.coopDemand = pkt.coopOffer.coopDemand;
}

