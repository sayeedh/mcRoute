#include<cstdlib>
#include<vector>
#include<cmath>
#include<fstream>
#include<iostream>
using namespace std;

#include "topo.h"
#include "util.h"

#define DEBUG 0

Topology::Topology(int p,int s,int r,int c, double  tx, char* f):nodeLocation(p+p+s)
{
    sizex = r;
    sizey = c;
    pu = p;
    su = s;
    srand(time(NULL));
    t = NULL;
    filename = f;
    // if(c != 0){
      //      t = new Tree();
    
      /* distance between any corner and the mid point of the area*/
      //    txRange = (sizex-1)*(sizex-1)/4+(sizey-1)*(sizey-1)/4;
      // t->insert((sizey-1)*(sizex-1)/2 + (sizey - 1)/2);
    // }
    //else
    txRange = r/tx;
}

RoundTopology :: RoundTopology(int p,int s, int r, double t, char* f):Topology(p,s,r,0,t,f)
{
  radius = r;

}

RoundTopology :: ~RoundTopology()
{
}

Topology:: ~Topology()
{
  if(t != NULL){
    delete t;
  }
}

void Topology::printLocation()
{
  cout << "nodes absolute location" << endl;
  t->inorder();
}

void Topology::addLocation()
{
  for(int i=0;i<pu+pu+su; i++){
    if(i<pu){
      addPULocation(i);
    }
    else if(i<pu+su){
      addSULocation(i);
    }
    else{
      addTargetLocation(i);
    }
  }
}

double Topology :: getDistance(int i, int j){
  double distance;
  distance = (nodeLocation[i].x - nodeLocation[j].x)*(nodeLocation[i].x - nodeLocation[j].x)+ (nodeLocation[i].y - nodeLocation[j].y)*(nodeLocation[i].y - nodeLocation[j].y);
  distance = sqrt(distance);
  return distance;
}

bool Topology :: isNeighbor(int i,int j)
{
  double distance;
  distance = (nodeLocation[i].x - nodeLocation[j].x)*(nodeLocation[i].x - nodeLocation[j].x)+ (nodeLocation[i].y - nodeLocation[j].y)*(nodeLocation[i].y - nodeLocation[j].y);
  //  cout << "distance " <<sqrt(distance) <<" " << i << " "<<j << endl;
  if(sqrt(distance) < txRange){
    return true;
  }
  return false;
}

void Topology::addPULocation(int id)
{
  int x, locx, locy;
  bool notLocated = true;
  while(notLocated){
    x = Topology::randindex(0,1);
    if(x){
      locx = Topology::randindex(0,1)==0? 0:sizex-1;
      locy = Topology::randindex(0,sizey-1);
     }
    else{
      locy = Topology::randindex(0,1) == 0? 0 : sizey -1;
      locx = Topology::randindex(0,sizex-1);
    }
    int value = locx*(sizex-1) + locy;
#if DEBUG
    cout << "value PU "<<id <<" Location: " << value << endl;
#endif
    if(t->search(value)== NULL){
      nodeLocation[id].x = locx;
      nodeLocation[id].y = locy;
#if DEBUG
      cout << "Primary user " <<id << "loc " << locx << " " << locy << endl;
#endif
      t->insert(value);
      notLocated = false;
    }
  }
}

void Topology :: readLocation()
{
  int i,k;
  char a;
  ifstream read(filename);
  //  cout << filename << endl;
  for(i = 0;i < pu; i++){
    read >> nodeLocation[i].x >> a >> nodeLocation[i].y;
    //    cout << i<< ":"<<nodeLocation[i].x << a << nodeLocation[i].y << endl;
  }
  for(i = pu + su, k = 0; k < pu; k++,i++){
    read >> nodeLocation[i].x >> a >> nodeLocation[i].y;
    //    cout << i<< ":"<<nodeLocation[i].x << a << nodeLocation[i].y << endl;
  }
  for(i = pu, k = 0; k < su; k++,i++){
    read >> nodeLocation[i].x >> a >> nodeLocation[i].y;
    //  cout <<i<< ":"<< nodeLocation[i].x << a << nodeLocation[i].y << endl;
  }


}
void RoundTopology :: addPULocation(int id)
{
  double locx, locy;
  double degree = rand()%180;
  double r = rand()%(int)getRadius();
  bool notLocated = true;
  while(notLocated){
    locx = r*cos(degree*PI/180);
    locy = r*sin(degree*PI/180);
    //cout << " PU: " <<locx << ", " << locy << endl;
    nodeLocation[id].x = locx;
    nodeLocation[id].y = locy;
    notLocated = false;
  }
}

void Topology :: addTargetLocation(int id)
{
  int nU;
  nU = pu + su;
  nodeLocation[id].x = -nodeLocation[id-nU].x;
  nodeLocation[id].y = -nodeLocation[id - nU].y;
  // cout << "Target: " << nodeLocation[id].x <<" " << nodeLocation[id].y << endl;
}
void Topology::addSULocation(int id)
{
  int locx, locy;
  bool notLocated = true;
  while(notLocated){
    locx = randindex(1,sizex-2);
    locy = randindex(1,sizey-2);
    int value = locx*(sizex-1) + locy;
    if(t->search(value)==NULL){
      nodeLocation[id].x=locx;
      nodeLocation[id].y=locy;
      t->insert(value);
      notLocated =false;
#if DEBUG
      cout << "Secondary user " <<id << "loc " << locx << " " << locy << endl;
#endif

    }
  }
}

void RoundTopology :: addSULocation(int id)
{
  double locx, locy;
  double degree, r;
  degree = rand()%360;
  r = rand()%(int)getRadius();
  locx = r*cos(degree*PI/180);
  locy = r*sin(degree*PI/180);
  //cout << "SU: " << locx << " " << locy << endl;
  nodeLocation[id].x = locx;
  nodeLocation[id].y = locy;
}

Loc Topology::getUserLocation(int in)
{
  return nodeLocation[in];
}

int Topology::randindex(int s, int e)
{

  int y = s+(int)(rand()%(e -s+1));
  return y;
}


