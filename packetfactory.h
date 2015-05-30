#ifndef PACKETFACTORY_H
#define PACKETFACTORY_H

#include "packet.h"
class Agent;

class PacketFactory{
 public:
  PacketFactory(){}
  virtual ~PacketFactory(){}
  virtual Packet* create(Agent* a)= 0;
 private:
};

class RequestPacketFactory:public PacketFactory{
 public:
 RequestPacketFactory():PacketFactory(){}
  Packet* create(Agent* u);
};

class ReplyPacketFactory:public PacketFactory{
 public:
 ReplyPacketFactory():PacketFactory(){}
  Packet* create(Agent* u);
};

class ConfirmPacketFactory:public PacketFactory{
 public:
 ConfirmPacketFactory():PacketFactory(){}
  Packet* create(Agent* u);
};

class DummyPacketFactory:public PacketFactory{
 public:
 DummyPacketFactory():PacketFactory(){}
  Packet* create(Agent* u);
};
#endif
