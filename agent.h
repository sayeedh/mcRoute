#ifndef AGENT_H
#define AGENT_H
#include "user.h"
#include "util.h"

class Packet;

/*
The cooperation offer from a secondary user consists of three information
*/
typedef struct{
 double yCycleTime; // cycle time offered to the cooperating primary user
 double xCycleTime; // cycle time demanded from the cooperating primary user
 double powerLevel; // power level to reach the cooperating primary user
}coopOfferSU;

/*
The base class of an agent. Each agent is designated to a specific source
*/
class Agent
{
 private:
  /* agent properties */
  User* u;  
  int src;        // represents which primary user's flow
  int dest;       // represents the corresponding destination
  int state;      // the state of the agent
  int timer;      // alarm to trigger an event
  int fwdTimer;   

  /* routing properties */ 
  double beta;             // calculates the profit difference for each flow
  int coopRound;

  /* benchmark parameters of the non-cooperative approach */
  double directDataRate;   // original data rate from src to dest (will be set after the first route_req pkt received
  double directDelay;      // original delay from src to dest (will be set after the first route_req pkt received
  double directTrafficRate;// original traffic rate 

  bool isAccepted;         // boolean condition of the offer
  int lastOfferId;         // id of the user who sent the last offer
 public:
  Agent(User* u, int a);
  virtual ~Agent();
  bool isActive;          // condition of the agent 
  User* getUser()const{return u;}
  /* basic get/set methods */	  
  inline int     getSrc(){return src;}
  inline int     getDest(){return dest;}
  inline int     getState() const {return state;}
  inline bool    getStatus()const{return isAccepted;}
  inline void    setStatus(bool b){isAccepted = b;}
  inline double  getBeta()const{return beta;}
  inline int     getFwdTimer()const{return fwdTimer;}

  /* get methods to access original information */ 
  inline double  getDirectDataRate()const{return directDataRate;}
  inline double  getDirectDelay() const{return directDelay;}
  inline double  getDirectTrafficRate()const{return directTrafficRate;}

  /* get/ set methods related to cooperation parameters */
  virtual double getProfit() {return 0;}
  inline int     getCoopRound()const{return coopRound;}
  inline void    setCoopRound(int r){coopRound = r;}
  inline void    setDirectDataRate(double s){directDataRate = s;}
  inline void    setDirectDelay(double s){directDelay = s;}
  inline void    setDirectTrafficRate(double s){directTrafficRate = s;}

  inline void    setBeta(double s){beta = s;}
  inline void    setSrc(int id){src = id;}
  inline void    setDest(int id){dest = id;}
  inline void    setState(int s){ state = s; }
  inline void    setFwdTimer(int t){fwdTimer = t;}

  virtual void   addRecipient(int sender);
  inline bool    isOfferProvider(int sender) {return coopOfferList.count(sender)>0;}
  inline bool    isRecipient(int sender){return recipientList.count(sender)>0;}

  /* methods related to cooperation offer */
  virtual void   addOffer(Packet*, int);
  inline int     numOfOffers()const{return coopOfferList.size();}
  Packet*        getOffer(int id){return coopOfferList[id];}
  virtual void   clearOffer();  
 
  inline int     getLastOfferId()const{return lastOfferId;} // the id of the neighbor node of the last offer
  virtual int    selectBestOfferFromNeighbors(coopOfferPU&); // selects the best offer among neighbors' offers

  virtual void   updateRecord(){}
  virtual int    calculateDelayOffer(coopOfferSU&, int uid){return 0;} // this is the singlemost important function
  virtual void   combineOffer(coopOfferSU&, coopOfferPU&){return;}
   
  inline void    setLastOfferId(int uid){lastOfferId = uid;} // sets the id of the neighbor node of the last offer
  virtual void   setCurrentOffer(coopOfferPU){}
  virtual void   setCurrentOffer(coopOfferSU){}
  virtual void   compareOffer(coopOfferPU&);
 
  inline void    setTimer(int t){timer = t;}
  inline bool    isExpired(){return timer--<= 0;}
  virtual double calculatePUProfit(Packet*)=0;

  /* methods related to packet transmission */ 
  virtual void   step(int curTime){}
  virtual void   sendPacket()=0;
  virtual void   receivePacket(Packet*, int)=0;
  virtual void   setFwdPacket(Packet*){}
  virtual void   forwardPacket(Packet*){}
  virtual void   clear();
 protected:
  std :: map <int,Packet*> coopOfferList; // list of offers received from neighbors
  std :: map <int, int> recipientList;    // list of recipients of the agent
};

/*
class PUAgent represents the agent of a primary user.
*/
class PUAgent : public Agent
{
 private:
 public:
 PUAgent(User* u, int a):Agent(u,a){
	setDirectDataRate(u->getTargetLinkRate(0));
	setDirectDelay(u->getTargetDelay());
  }
  void step(int curTime);
  void sendPacket();
  void receivePacket(Packet*, int);
  virtual double calculatePUProfit(Packet*);
  void updateRecord();
  double getProfit();

  void setCurrentOffer(coopOfferPU p){lastCoopOffer = p;}
 public:
  coopOfferPU lastCoopOffer;         // the cooperation offer selected in previous round
};

/*
class SUAgent represents the agent of a secondary user.
*/

class SUAgent : public Agent
{
 private:
 public:
 SUAgent(User* u, int a): Agent(u,a){fwdPkt = NULL;setCoopRound(1);}
  ~SUAgent();
  void step(int curTime);
  void sendPacket();
  void receivePacket(Packet*, int);
  void forwardPacket(Packet* pkt);
  virtual double calculatePUProfit(Packet*);
  int calculateDelayOffer(coopOfferSU&, int);
  void setCurrentOffer(coopOfferSU p){lastCoopOffer = p;}
  void combineOffer(coopOfferSU&, coopOfferPU&);
  virtual void setFwdPacket(Packet* pkt);
  double getProfit();
 public:
  Packet* fwdPkt;
  coopOfferSU lastCoopOffer;     // the cooperation offer selected in previous round (NOTE: this is different from the primary cooperation offer)
 };
#endif
