#include<iostream>
#include<fstream>
#include<cstring>
using namespace std;
#include "sim.h"
#include "util.h"
#include "report.h"

/* input is expected to be a single input file which should contain
time np ns
pu1x pu1y ... puMpx puMpy
su1x su2y ... suNsx suNsy
*/
int main(int argc, char **argv){
  char filename[40];
  int numParams = 3;
  int index = 0;
  
  if(argc != numParams + index){
    cout << "[progname] [infile] [outfile]" << endl;
    return 0;
    }
 
  Report *r = Report::getInstance();
  r->setFileName(argv[index+2]);
  strcpy(filename, argv[index+1]);

  Simulation* sim = Simulation :: getInstance(filename);

  /* create nodes, place them, calculate neighbors */
  sim->init();	
  sim->run();
  sim->show_results();
  delete sim;
  delete r;
  return 0;
}
