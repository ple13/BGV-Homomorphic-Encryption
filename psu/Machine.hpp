#ifndef MACHINE_H__
#define MACHINE_H__

#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <NTL/ZZ.h>

#include "../PAHE/PAHE.h"
#include "Network.hpp"
#include "IPManager.hpp"

#define MAX_PACKAGE_SIZE 1073741824

// #define ALICE 0
// #define BOB 1
// #define CHARLIE 3
// #define DAVID 4

using namespace std;

class Machine { 
public:
  int totalMachines; // number of processor
  int totalParties;  // number of parties
  int machineId; 
  int party;
  int numberOfOutgoingParties;
  int numberOfIncomingParties;
  int numberOfOutgoingPeers;
  int numberOfIncomingPeers;
  int layerBasePort; // solve the problem of parties trying to connect to processor in other layers
  
  vector <Network *> partiesDown;
  vector <Network *> partiesUp;
  vector <Network *> peersDown;
  vector <Network *> peersUp;
  
  uint64_t num_bytes_send;
  uint64_t num_bytes_recv;

  Machine(int totalMachines, int machineId, int party) {
    cout << "Machine (int totalMachines, int machineId, int party)" << endl;
    this->totalMachines = totalMachines;
    this->totalParties = 2;
    this->machineId = machineId;
    this->party = party;
    this->numberOfIncomingParties = (this->totalParties - this->party - 1);
    this->numberOfOutgoingParties = (this->party);
    this->numberOfIncomingPeers = (this->totalMachines - this->machineId - 1);
    this->numberOfOutgoingPeers = this->machineId;
    layerBasePort = totalMachines * 2;

    partiesDown.resize(this->numberOfIncomingParties);
    partiesUp.resize(this->numberOfOutgoingParties);
    peersDown.resize(this->numberOfIncomingPeers);
    peersUp.resize(this->numberOfOutgoingPeers);
    
    num_bytes_send = 0;
    num_bytes_recv = 0;
  }
  
  Machine(int totalParties, int totalMachines, int machineId, int party) {
    cout << "Machine (int totalParties, int totalMachines, int machineId, int party)" << endl;
    this->totalMachines = totalMachines;
    this->totalParties = totalParties;
    this->machineId = machineId;
    this->party = party;
    this->numberOfIncomingParties = (this->totalParties - this->party - 1);
    this->numberOfOutgoingParties = this->party;
    this->numberOfIncomingPeers = (this->totalMachines - this->machineId - 1);
    this->numberOfOutgoingPeers = this->machineId;
    layerBasePort = totalMachines * totalParties;

    partiesDown.resize(this->numberOfIncomingParties);
    partiesUp.resize(this->numberOfOutgoingParties);
    peersDown.resize(this->numberOfIncomingPeers);
    peersUp.resize(this->numberOfOutgoingPeers);
    
    num_bytes_send = 0;
    num_bytes_recv = 0;
  }

  ~Machine() {
    for(int idx = 0; idx < this->numberOfIncomingParties; idx++) delete partiesDown[idx];
    for(int idx = 0; idx < this->numberOfOutgoingParties; idx++) delete partiesUp[idx];
    for(int idx = 0; idx < this->numberOfIncomingPeers; idx++)   delete peersDown[idx];
    for(int idx = 0; idx < this->numberOfOutgoingPeers; idx++)   delete peersUp[idx];
  }
  

  void connecting(int peerBasePort, int partyBasePort) {
    connectToOtherParties(partyBasePort, partiesDown, partiesUp, numberOfIncomingParties, numberOfOutgoingParties);

    usleep(1000);

    connectToOtherPeers(peerBasePort, peersDown, peersUp, numberOfIncomingPeers, numberOfOutgoingPeers);
  }

  void connectToOtherParties(int partyBasePort, vector <Network *> &partiesDown, vector <Network *> &partiesUp, int numberOfIncomingParties, int numberOfOutgoingParties) {
    listeningServers(party, partyBasePort + machineId*layerBasePort + party, partiesDown, numberOfIncomingParties);
    connectingClients(party, partyBasePort + machineId*layerBasePort, partiesUp, numberOfOutgoingParties, true);
  }

  void connectToOtherPeers(int peerBasePort, vector <Network *> &peersDown, vector <Network *> &peersUp, int numberOfIncomingPeers, int numberOfOutgoingPeers) {
    listeningServers(machineId, peerBasePort + party*layerBasePort + machineId, peersDown, numberOfIncomingPeers);
    connectingClients(machineId, peerBasePort + party*layerBasePort, peersUp, numberOfOutgoingPeers, false);
  }

  void listeningServers(int myId, int serverPort, vector <Network *> &DownList, int numberOfIncomingConnections) {
    int opt = 1, activity;
    int * index;
    char buffer[100];
    struct sockaddr_in serverSocket, clientSock;
    fd_set working_set;
    const char * msg = "Accept"; 
    struct timeval timeout;
    timeout.tv_sec  = 20;
    timeout.tv_usec = 0;
    socklen_t clientSocksize = sizeof(clientSock);

    int server_fd, client_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) { perror("socket failed"); exit(1); }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) { perror("setsockopt"); exit(1); } //| SO_REUSEPORT
    memset(&serverSocket, 0, sizeof(serverSocket));
    serverSocket.sin_family = AF_INET;
    serverSocket.sin_addr.s_addr = htonl(INADDR_ANY); // set our address to any interface
    serverSocket.sin_port = htons(serverPort);        // set the server port number 

    if (bind(server_fd, (struct sockaddr *)&serverSocket, sizeof(serverSocket)) < 0) { perror("error: bind"); exit(1); }

    if (listen(server_fd, 15) < 0) { perror("error: listen"); exit(1); }

    FD_ZERO(&working_set); //clear the socket set 
    FD_SET(server_fd, &working_set); //add master socket to set

    for (int k = 0; k < numberOfIncomingConnections; k++) {
      while(1) {
        activity = select(server_fd+1 , &working_set , NULL , NULL , NULL);   //&timeout

        if ((activity < 0) && (errno!=EINTR))  { perror("select failed"); exit(1); }

        if (FD_ISSET(server_fd, &working_set)) {
          if ((client_fd = accept(server_fd, (struct sockaddr *)&clientSock, &clientSocksize)) < 0) { perror("error: accept"); exit(1); }
          if (send(client_fd, msg, strlen(msg), 0) < 0) { perror("error: send"); exit(1); };
          Network * channel = new Network();
          channel->establishedChannel(client_fd);

          int bytes_received = recv(client_fd, buffer ,sizeof(buffer) , 0);
          int machineIndex = ((int *)buffer)[0];

          machineIndex = (machineIndex - myId - 1);
          DownList[machineIndex] = channel;
          break;
        }
      }
    }
  }


  void connectingClients(int myId, int serverPort, vector <Network *> &UpList, int numberOfOutgoingConnections, bool partyFlag) { 
    IPManager * ipManager = new IPManager(totalMachines);
    string serverIp;
    int client_fd;
    int accepted_flag = -1;
    char buffer[10];
    for (int i = 0; i < numberOfOutgoingConnections; i++) {
      if (partyFlag) {
        serverIp = ipManager->P_Ip[i][machineId];
      } else {
        serverIp = ipManager->P_Ip[party][i];
      }
      // cout << "Client: server " << i << " is at ip: " << serverIp << " port: " << serverPort + i << endl;			
      struct sockaddr_in serverSocket;
      memset(&serverSocket, 0, sizeof(serverSocket));
      serverSocket.sin_family = AF_INET;
      serverSocket.sin_addr.s_addr = inet_addr(serverIp.c_str());
      serverSocket.sin_port = htons(serverPort + i);
      if ((client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) { perror("socket failed"); exit(1); }

      do {
        usleep(1000);
        if (connect(client_fd, (struct sockaddr *)&serverSocket, sizeof(serverSocket)) < 0) {
          perror("connection failed"); close(client_fd); exit(1);
        }
        accepted_flag = read(client_fd, buffer, 10);
      } while (accepted_flag < 0);
      
      Network * channel = new Network();
      channel->establishedChannel(client_fd);
      UpList[i] = channel;
      if (send(client_fd, &myId, sizeof(myId), 0) < 0) { perror("error: send"); exit(1); };
    }
  }
  
  void sendToParty(int src, int dest, std::vector<Ciphertext> ciphertexts) {
    for (int cdx = 0; cdx < ciphertexts.size(); cdx++) {
      sendToParty(src, dest, ciphertexts[cdx]);
    }
  }
  
  int receiveFromParty(int dest, int src, std::vector<Ciphertext>& ciphertexts) {
    int bytes = 0;
    for (int cdx = 0; cdx < ciphertexts.size(); cdx++) {
      bytes += receiveFromParty(dest, src, ciphertexts[cdx]);
    }
    return bytes;
  }
  
  void sendToParty(int src, int dest, Ciphertext ciphertext) {
    std::vector<uint64_t> buffer;
    for (int i = 0; i < ciphertext.a.size(); i++) {
      buffer.insert(buffer.end(), ciphertext.a[i].begin(), ciphertext.a[i].end());
    }
    for (int i = 0; i < ciphertext.b.size(); i++) {
      buffer.insert(buffer.end(), ciphertext.b[i].begin(), ciphertext.b[i].end());
    }
    sendToParty(src, dest, (unsigned char *)buffer.data(), sizeof(uint64_t)*buffer.size());
  }
  
  int receiveFromParty(int dest, int src, Ciphertext& ciphertext) {
    int bytes = 0;
    std::vector<uint64_t> buffer(4096*2*3);
    bytes += receiveFromParty(dest, src, (unsigned char *)buffer.data(), sizeof(uint64_t)*buffer.size());

    ciphertext.a.resize(4096, std::vector<uint64_t>(3));
    ciphertext.b.resize(4096, std::vector<uint64_t>(3));

    int counter = 0;
    for (int i = 0; i < 4096; i++) {
      for (int j = 0; j < 3; j++) {
        ciphertext.a[i][j] = buffer[counter];
        counter++;
      }
    }

    for (int i = 0; i < 4096; i++) {
      for (int j = 0; j < 3; j++) {
        ciphertext.b[i][j] = buffer[counter];
        counter++;
      }
    }
    
    return bytes;
  }

    void sendToParty(int src, int dest, CompactedCiphertext ciphertext) {
    std::vector<uint64_t> buffer;
    for (int i = 0; i < ciphertext.a.size(); i++) {
      buffer.insert(buffer.end(), ciphertext.a[i].begin(), ciphertext.a[i].end());
    }
    buffer.insert(buffer.end(), ciphertext.b.begin(), ciphertext.b.end());

    sendToParty(src, dest, (unsigned char *)buffer.data(), sizeof(uint64_t)*buffer.size());
  }

  int receiveFromParty(int dest, int src, CompactedCiphertext& ciphertext) {
    int bytes = 0;
    std::vector<unsigned char> buffer(4096*sizeof(uint64_t)*3 + 2*SEED_SIZE);
    bytes += receiveFromParty(dest, src, buffer.data(), sizeof(uint64_t)*buffer.size());
    uint64_t *ptr = (uint64_t *)buffer.data();
    ciphertext.a.resize(4096, std::vector<uint64_t>(3));
    ciphertext.b.resize(2*SEED_SIZE);

    for (int i = 0; i < 4096; i++) {
      for (int j = 0; j < 3; j++) {
        ciphertext.a[i][j] = ptr[counter];
        ptr++;
      }
    }

    for (int i = 0; i < 2*SEED_SIZE) {
        ciphertext.b[i] = buffer[4096*sizeof(uint64_t)*3 + i];
      }
    }

    return bytes;
  }

  void sendToParty(int party1, int party2, unsigned char *msg, uint64_t size) { 
    // party1 sends to party2
    Network * channel;
    if (party1 < party2) {
      channel = partiesDown[party2-party1-1];
    } else if (party1 > party2) {
      channel = partiesUp[party2];
    }
    
    if(size > 1024*1024) std::cout << "sending " << size << " bytes" << std::endl;
    
    uint64_t count = 0;
    uint64_t bufferSize;
    
    while(count < size) {
      if (count + MAX_PACKAGE_SIZE < size) {
	bufferSize = MAX_PACKAGE_SIZE;
      } else {
	bufferSize = size - count;
      }
      
      uint64_t bytes_sent = 0;

      while(bytes_sent < bufferSize) {
	uint64_t bs = send(channel->sock_fd, msg + count, bufferSize - bytes_sent, 0);

	if(bs < 0) {
	  perror("error: send");
	  exit(1);
	}

	bytes_sent += bs;
	count += bs;
      }
    }
    num_bytes_send += size;
  }

  int receiveFromParty(int party1, int party2, unsigned char *msg, uint64_t size) {
    if(size > 1024*1024) std::cout << "receiving " << size << " bytes" << std::endl;
    // party1 receives from party2
    Network * channel;
    if (party1 < party2) {
      channel = partiesDown[party2-party1-1];
    } else if (party1 > party2) {
      channel = partiesUp[party2];
    }
    int sd = channel->sock_fd;
    fd_set reading_set;
    
    while (true) {
      FD_ZERO(&reading_set); //clear the socket set 
      FD_SET(sd, &reading_set); //add master socket to set
      int activity = select(sd+1, &reading_set , NULL , NULL , NULL);
      if ((activity < 0) && (errno!=EINTR))  { perror("select failed"); exit(1); }
      if (activity > 0) {
	if (FD_ISSET(sd, &reading_set)){
	  uint64_t count = 0;
	  uint64_t bufferSize;
	  uint64_t bytes_received = 0;
	  
	  while(count < size) {
	    if (count + MAX_PACKAGE_SIZE < size) {
	      bufferSize = MAX_PACKAGE_SIZE;
	    } else {
	      bufferSize = size - count;
	    }
	    
	    uint64_t arrived = 0;

	    while (arrived < bufferSize) {
	      uint64_t br = recv(sd, msg + count, bufferSize - arrived, 0);
	      arrived += br;
	      count += br;
	    }

	    bytes_received += arrived;
	  }
	  assert(bytes_received == size);
	  num_bytes_recv += size;
	  return bytes_received;
	}
      }
      else if (activity == 0){
	return 0;
      }
    }
  }

  void sendToPeer(int peer1, int peer2, unsigned char *msg, int size) {
// 		std::cout << "send to peer: " << peer1 << " --> " << peer2 << std::endl;

    Network * channel;
    if (peer1 < peer2) {
      channel = peersDown[peer2-peer1-1];
    } else if (peer1 > peer2) {
      channel = peersUp[peer2];
    }

    if(size > 1024*1024) std::cout << "sending " << size << " bytes" << std::endl;
    
    int count = 0;
    int bufferSize;
    
    while(count < size) {
      if (count + MAX_PACKAGE_SIZE < size) {
	bufferSize = MAX_PACKAGE_SIZE;
      } else {
	bufferSize = size - count;
      }
      
      int bytes_sent = 0;

      while(bytes_sent < bufferSize) {
	int bs = send(channel->sock_fd, msg + count, bufferSize - bytes_sent, 0);

	if(bs < 0) {
	  perror("error: send");
	  exit(1);
	}

	bytes_sent += bs;
	count += bs;
      }
    }
  }

  int receiveFromPeer(int peer1, int peer2, unsigned char *msg, int size) {
  // party1 receives from party2
  // 		std::cout << "receiver from peer: " << peer1 << " <-- " << peer2 << std::endl;
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    Network * channel;
    if (peer1 < peer2) {
      channel = peersDown[peer2-peer1-1];
    } else if (peer1 > peer2) {
      channel = peersUp[peer2];
    }

    int sd = channel->sock_fd;
    fd_set reading_set;
    // char buffer[256];
    while (true) {
      FD_ZERO(&reading_set); //clear the socket set 
      FD_SET(sd, &reading_set); //add master socket to set
      int activity = select(sd+1, &reading_set , NULL , NULL , NULL);   //&timeout
      if ((activity < 0) && (errno!=EINTR))  { perror("select failed"); exit(1); }
      // cout << "activity: " << activity << endl;
      if (activity > 0) {
	if (FD_ISSET(sd, &reading_set)) {
	  int count = 0;
	  int bufferSize;
	  int bytes_received = 0;
	  
	  while(count < size) {
	    if (count + MAX_PACKAGE_SIZE < size) {
	      bufferSize = MAX_PACKAGE_SIZE;
	    } else {
	      bufferSize = size - count;
	    }
	    
	    int arrived = 0;

	    while (arrived < bufferSize) {
	      int br = recv(sd, msg + count, bufferSize - arrived, 0);
	      arrived += br;
	      count += br;
	    }

	    bytes_received += arrived;
	  }
	  assert(bytes_received == size);
	  return bytes_received;
	}
      } else if (activity == 0) {
	return 0;
      }
    }
  }
};


#endif  //UNIX_PLATFORM
