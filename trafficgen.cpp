#include "trafficgen.h"
#include <math.h>
#include<cstdlib>
using namespace std;

int TrafficGen::generateNumber(double param){
  double L = exp(-param);
  // cout << "L:" << L << endl;
  int k = 0;
  double p = 1;
  do {
      k++;
      p *= ((double)rand()/(RAND_MAX));
  }while( p > L);
  return (k-1);
}
// randomly selects a xtime based on remaining time which is equal to
// genTime + deadline - curTime
double TrafficGen::estimateTime(int cond, double curtime, double dline)
{
  double xtime = INFTY;
  double range;
  // the formula is a + (b -a)u where u is drawn from (0,1)
  switch(cond){
  case COND_GOOD:
    range = (-curtime + dline)*4/3;
    xtime = range * ((double)rand()/RAND_MAX);
    break;
  case COND_MED:
    range = (-curtime + dline)*2;
    xtime = range * ((double)rand()/RAND_MAX);
    break;
  case COND_POOR:
    range = (-curtime + dline)*5/2;
    xtime = range * ((double)rand()/RAND_MAX);
    //cout << "range1 " << curtime << " "<< dline<< " "<< range << endl;
    break;
  case COND_RELAY:
    range = (-curtime + dline)/3; /* we need to change it */
    xtime = range * ((double)rand()/RAND_MAX);
    //cout << "range2 " << curtime << " "<< dline<< " "<< range << endl;
    break;
  }
  //cout << "range3 " << curtime << " "<< dline<< " "<< range << endl;
  //cout << xtime << endl;
  return xtime;
}
