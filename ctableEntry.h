#ifndef CTABLEENTRY_H
#define CTABLEENTRY_H

// this class holds the latest bid sent by a node for a flow with the maximum and minimum accepted/ rejected resource information

class CTableEntry{
 public:
  CTableEntry();
  ~CTableEntry();
  // getter methods
  inline double getOfferedResource() const {return offeredResource;}
  inline double getDemandedResource() const{return grantedResource;}
  inline double getMaxOffer()const{return maxOfferAccepted;}
  inline double getMinOffer()const{return minOfferRejected;}
  inline bool getStatus()const{return isConfirmed;}
  // setter methods
  inline void setOfferedResource(double s){offeredResource = s;}
  inline void setDemandedResource(double s){grantedResource = s;}
  inline void setMaxOffer(double s){maxOfferAccepted = s;}
  inline void setMinOffer(double s){minOfferRejected = s;}
  inline void setStatus(bool t){isConfirmed = t;}
 private:
  //  int srcId;              // node which the request is received
  int initiatorId;        // actual initiator (primary source id)
  double offeredResource; // spectrum time granted by the initiator
  double grantedResource; // spectrum time used for relaying
  double maxOfferAccepted;// max accepted offer
  double minOfferRejected;// min rejected offer
  bool isConfirmed;       // is the offer accepted?
};
#endif
