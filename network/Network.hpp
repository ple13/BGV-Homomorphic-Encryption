#ifndef NETWORK_H__
#define NETWORK_H__

#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define Alice 0
#define Bob 1
#define Charlie 2
#define David 3

using namespace std;

const static int NETWORK_BUFFER_SIZE = 1048576; //1024;

class Network {
public:
  FILE * stream = nullptr;
  char * buffer = nullptr;
  int port;
  int sock_fd = -1;

  Network(){}

  ~Network(){
    fflush(stream);
    close(sock_fd);
    delete[] buffer;
  }

  void establishedChannel(int sock_fd, bool quiet = false) {
    this->sock_fd = sock_fd;
    set_nodelay();
    stream = fdopen(this->sock_fd, "wb+");
    buffer = new char[NETWORK_BUFFER_SIZE];
    memset(buffer, 0, NETWORK_BUFFER_SIZE);
    setvbuf(stream, buffer, _IOFBF, NETWORK_BUFFER_SIZE);
  }

  void set_nodelay() {
    const int one=1;
    setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
  }

  void set_delay() {
    const int zero = 0;
    setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &zero, sizeof(zero));
  }

  void flush() {
    fflush(stream);
  }

  void send_data(const void * data, int len) {
    bool has_sent = false;
    uint64_t counter = 0;
    counter += len;
    int sent = 0;
    while(sent < len) {
      int res = fwrite(sent + (char*)data, sizeof(char), len - sent, stream);
      if (res >= 0)
        sent+=res;
      else
        fprintf(stderr,"error: net_send_data %d\n", res);
    }
    has_sent = true;
  }

  void recv_data(void * data, int len) {
    bool has_sent = false;
    if(has_sent)
      fflush(stream);
    has_sent = false;
    int sent = 0;
    while(sent < len) {
      int res = fread(sent + (char*)data, sizeof(char), len - sent, stream);
      if (res >= 0)
        sent += res;
      else
        fprintf(stderr,"error: net_send_data %d\n", res);
    }
  }
};


#endif
