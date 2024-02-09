#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Machine.hpp"
#include "triple.h"

using namespace std;

int main(int argc, char** argv) {
  int totalParties = atoi(argv[1]);
  int party = atoi(argv[2]);
  int nItems = atoi(argv[3]);
  int machineId = 0;
  int peerBasePort = 40000;
  int partyBasePort = 50000;

//   std::cout << totalParties << " " << machineId << " " << party << std::endl;

  Machine * machine = new Machine(totalParties, 1, machineId, party);
  machine->connecting(peerBasePort, partyBasePort);

//   cout << "Networking Done." << endl;

  usleep(5000);

  Triple * mpc = new Triple(machineId, party, totalParties, nItems, machine);

  Timer tt;
  mpc->TripleGeneration();
  tt.Tick("GraphComputation()");

  delete machine;
}
