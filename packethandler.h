#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

class Agent;
class Packet;

class PacketHandler{
 public:
  PacketHandler(Agent*);
  virtual ~PacketHandler(){}
  Agent* getAgent(){return agent;}
  virtual void handle(Packet*,int) = 0;
 private:
  Agent *agent;
};

class RouteReqPktHandler : public PacketHandler{
 public:
  RouteReqPktHandler(Agent*);
  void handle(Packet*,int);
};

class RouteRepPktHandler : public PacketHandler{
 public:
  RouteRepPktHandler(Agent*);
  void handle(Packet*,int);
};

class RouteConfirmPktHandler : public PacketHandler{
 public:
  RouteConfirmPktHandler(Agent*);
  void handle(Packet*,int);
};
#endif
