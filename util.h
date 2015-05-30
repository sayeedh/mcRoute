/* 
 * File:   util.h
 * Author: sayeed
 *
 * Created on October 4, 2013, 6:10 PM
 */

#ifndef UTIL_H
#define	UTIL_H

typedef struct a{
  int ns;
  int avgpunb;
  int avgsunb;
  double pupr;
  double supr;
}resultSet;

// different states of a user
#define ST_IDLE       1
#define ST_WAITING    2
#define ST_DECIDE     3
#define ST_WAIT2CONFIRM 4

// types of casting
#define UNICAST       1
#define BROADCAST     2
#define MULTICAST     3

// type of packet
#define PKT_DATA	0
#define PKT_CTRL	1
#define PKT_OPP         2

// types of algorithm
#define ALG_PURE 1
#define ALG_WIN  2
#define ALG_NOCP 3

// type of routing request
#define ROUTE_REQ     1
#define ROUTE_REPLY   2
#define ROUTE_CONFIRM 3
#define ROUTE_UPDATE  4

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

#define COND_GOOD   1
#define COND_MED    2
#define COND_POOR   3
#define COND_RELAY  4

#define INFTY 1000.0
#define THRESHOLD 0.01 // previously it was 0.001
#define PI 3.14159
#define PKTLEN 1024 // bits
#define NOISE 10e-10//W/Hz
#define SHADOWFACTOR 4
#define PATHLOSS 2.0
#define BANDWIDTH 625 // KHz
#define BASED 10.0 //meter
#define RXPOWER -50 //dB
#define TXRANGE 250
#define TXPOWER1 0.1
#define TXPOWER2 0.5
#define TXPOWER3 1.0

#define XMIN 100 
#define YMIN 100
#define CYCLETIME 1000
#define VACATIONTIME 300
#endif	/* UTIL_H */

