#ifndef PACKET_H
#define PACKET_H
/*
#include<vector>
using namespace std;
*/
typedef struct{
 double coopDataRate;
 double coopDelay;
 double coopDemand;
}coopOfferPU;

class Packet{
 private:
  int    id;          // round number
  int    srcId;       // initiator
  int    destId;      // final destination
  int    pktType;     // type of packet: see utils.h
 
  double directDataRate;
  double directDelay;
  double directTrafficRate;

  coopOfferPU coopOffer;
/*  double coopDataRate;
  double coopDelay;
  double coopDemand;
  */
  Packet();        // no implementation for empty constructor

 public:
  Packet(int);
  Packet(int i, int src, int dest, int pkt, double r, double d, double tr)
   :id(i),srcId(src), destId(dest), pktType(pkt), directDataRate(r), directDelay(d), directTrafficRate(tr)
  {
  }
  Packet(const Packet &pkt);
  virtual ~Packet();

  // get and set methods
  inline int    getId()const{return id;}
  inline int    getPktType()const{return pktType;}
  inline int    getDest()const{return destId;}
  inline int    getSrc()const{return srcId;}
  
  inline double getDirectDataRate(){return directDataRate;}
  inline double getDirectDelay(){return directDelay;}
  inline double getDirectTrafficRate(){return directTrafficRate;}

  inline double getCoopDataRate(){return coopOffer.coopDataRate;}
  inline double getCoopDelay(){return coopOffer.coopDelay; }
  inline double getCoopDemand(){return coopOffer.coopDemand;}
  inline coopOfferPU  getCoopOffer(){return coopOffer;}

  inline void setId(int i){id = i;}
  inline void setPktType(int type){pktType = type;}
  inline void setDest(int id){destId = id;}
  inline void setSrc(int id){srcId = id;}

  inline void setDirectDataRate(double rate){directDataRate = rate;}
  inline void setDirectDelay(double d){directDelay = d;}
  inline void setDirectTrafficRate(double d){directTrafficRate = d;}

  inline void setCoopDataRate(double rate){coopOffer.coopDataRate = rate;}
  inline void setCoopDelay(double d) {coopOffer.coopDelay = d;}
  inline void setCoopDemand(double d){coopOffer.coopDemand = d;}
  inline void setCoopOffer(coopOfferPU of){coopOffer = of;}
  virtual Packet* clone(){ return 0;}

 private:
};

class RequestPacket:public Packet{
 public:
 RequestPacket(int t):Packet(t){}
  RequestPacket(int i, int src, int dest, int pkt, double r, double d, double tr):Packet( i,src, dest,pkt, r, d, tr){}
  Packet* clone(){
    Packet* req = new RequestPacket(*this);
    return req;
  }
 RequestPacket(const RequestPacket &rPkt):Packet(rPkt){
  }

};

class ReplyPacket:public Packet{
 public:
 ReplyPacket(int t):Packet(t){}

  ReplyPacket(int i, int src, int dest, int pkt, double r, double d, double tr)	:Packet( i,src, dest,pkt, r, d, tr){}
  Packet* clone(){
    return new ReplyPacket(*this);
  }
 ReplyPacket(const ReplyPacket &rPkt):Packet(rPkt){
  }

 private:
};

class ConfirmPacket:public Packet{
 public:
 ConfirmPacket(int t):Packet(t){}
  Packet* clone(){
    return new ConfirmPacket(*this);
  }
  ConfirmPacket(int i, int src, int dest, int pkt, double r, double d, double tr):Packet( i,src, dest,pkt, r, d, tr){}
 ConfirmPacket(const ConfirmPacket &cPkt):Packet(cPkt){
  }
 private:
};

class DummyPacket:public Packet{
 public:
  DummyPacket(int i, int src, int dest, int pkt, double r, double d, double tr):Packet( i,src, dest,pkt, r, d, tr){}
  // DummyPacket(int t): Packet(t){}
  Packet* clone(){
    return new DummyPacket(*this);
  }
 DummyPacket(const DummyPacket &dPkt):Packet(dPkt){ }
};
#endif

