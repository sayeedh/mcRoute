#ifndef REPORT_H
#define REPORT_H
#include<string>
//#include<iostream>
#include<fstream>
using namespace std;

class User;
class Packet;

class Report{
 private:
  char* fileName;
  std::ofstream iofile; 
  Report();
  static Report* rInstance;
 public:
  static Report* getInstance();
  ~Report();
  void setFileName(char* f){fileName = f;iofile.open(f,std::ofstream::app);}
  void addReport(string);
  void addInput(int, int, int, double, double);
  void addReport(User*, User*,Packet*, string);
  void addReport(User*, int, Packet*, string);
  void addReport(double, double, double, double, double, double, double, double, double);
};
#endif
