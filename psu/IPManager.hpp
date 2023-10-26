#ifndef IPMANAGER_H__
#define IPMANAGER_H__

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;

class IPManager {
public:
  int machines;
  string A_Ip[100];
  string B_Ip[100];
  string C_Ip[100];
  string D_Ip[100];
  string P_Ip[100][100];

  unordered_map <string, string> machineIps;

  IPManager(int machines){
    loadIPs(machines);
  }

  ~IPManager(){}

  void loadIPs(int machines) {
    ifstream file ("./machine_spec/" + to_string(machines) + ".txt");
    if( !file ) {
      cout << "Error opening input file" << endl ;
    }
    string line = "";
    while (file.peek() != EOF) {
      getline (file , line);
      while (starts_with(line, "#")) {
        getline(file, line);
      }
      if (starts_with(line, "processor_spec")) {
        break;
      }
      vector<string> token;
      split(token, line, is_any_of(" "), token_compress_on);
      machineIps.emplace(token[0], token[1]);
    }
    for (int idx = 0; idx < 32; idx++) {
      getline(file, line);
      while (starts_with(line, "#")) {
        getline(file, line);
      }
      setIps(machines, line);
    }
    file.close();
  }

  void setIps(int machines, string next) {
    for (int i = 0; i < 31; i++) {
      string name = "P";
      if (i < 10) name += "0";
      name += to_string(i);
      if (starts_with(next, name)) {
        vector<string> tok;
        split(tok, next, is_any_of(" "), token_compress_on);
        if (tok.size() != (machines+1)) {
          cout << "You have specified " << sizeof(tok)-1 << "  machines for " << name << " instead of " << machines << "\n";
        }
        for (int i = 1; i < tok.size(); i++) {
          P_Ip[0][i-1] = machineIps[tok[i]];
        }
      }
    }
  }
};


#endif
