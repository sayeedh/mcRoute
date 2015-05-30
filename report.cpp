#include "report.h"
#include "user.h"
#include "packet.h"


Report* Report::rInstance = 0;

Report::Report(){}

Report* Report :: getInstance(){
  if(rInstance == NULL)
    rInstance = new Report();
  return rInstance;

}

Report :: ~Report()
{
  iofile.close();
}

void Report :: addReport(string str)
{
  iofile << str << endl;

}
void Report :: addInput(int a, int pu, int su, double txP, double txR)
{
  iofile.precision(2);
  iofile << a << " " << pu << " " << su << " " << txP << " "<<txR << " ";
}
void Report :: addReport(User* src, User* dest, Packet* pkt, string s)
{
/*
  iofile << s <<" "<< src->getId() << " " << dest->getId()<<" "
  << pkt->getPktType()<<" "<<pkt->getSrc()<<" "<<pkt->getDest()<<endl;
*/
}

void Report :: addReport(User* src, int dest, Packet* pkt, string s)
{
/*
  iofile << s <<" "<<src->getId() << " " << dest <<" "
  << pkt->getPktType()<<" "<<pkt->getSrc()<<" "<<pkt->getDest()<<endl;
*/
}

void Report :: addReport(double apn, double arn, double asuperpu, double overlap, double activepu, double activesu, double utilpu, double utilsu, double converge)
{
  iofile.precision(2);
  iofile << apn << " " << arn << " " << asuperpu << " " << overlap 
	 << " " << activepu << " " << activesu << " " << utilpu << " "
	 << utilsu << " " << converge << endl;
}
