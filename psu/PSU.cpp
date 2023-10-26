#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Machine.hpp"
#include "MPC.hpp"

using namespace std;

int main(int argc, char** argv) {
  int totalParties = atoi(argv[1]);
  int party = atoi(argv[2]);
  int itemLength = atoi(argv[3]);
  int nBuckets = atoi(argv[4]);
  int max_size = atoi(argv[5]);
  int use_ot = atoi(argv[6]);
  int machineId = 0;
  int peerBasePort = 40000;
  int partyBasePort = 50000;
  int test = 0;
  float rate = 0.75;

//   std::cout << totalParties << " " << machineId << " " << party << std::endl;
  
  Machine * machine = new Machine(totalParties, 1, machineId, party);
  machine->connecting(peerBasePort, partyBasePort);
  
//   cout << "Networking Done." << endl;
  
  usleep(5000);
  
  MPC * mpc = new MPC(machineId, party, totalParties, itemLength, nBuckets, max_size, use_ot, machine, rate);
  
  Timer tt;
  mpc->MPCComputation(test);
  tt.Tick("GraphComputation()");

  delete machine;
}
