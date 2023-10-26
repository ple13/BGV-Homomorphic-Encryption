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


class IPManager 
{
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

	~IPManager(){
	}

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
		if (starts_with(next, "P00")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P0 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[0][i-1] = machineIps[tok[i]];
			}
		} 
		else if (starts_with(next, "P01")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P1 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[1][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P02")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P2 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[2][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P03")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[3][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P04")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[4][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P05")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[5][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P06")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[6][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P07")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[7][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P08")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[8][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P09")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[9][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P10")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[10][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P11")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[11][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P12")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[12][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P13")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[13][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P14")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[14][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P15")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[15][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P16")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[16][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P17")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[17][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P18")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[18][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P19")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[19][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P20")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[20][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P21")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[21][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P22")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[22][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P23")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[23][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P24")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[24][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P25")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[25][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P26")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[26][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P27")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[27][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P28")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[28][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P29")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[29][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P30")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[30][i-1] = machineIps[tok[i]];
			}
		}
		else if (starts_with(next, "P31")) {
			vector<string> tok;
			split(tok, next, is_any_of(" "), token_compress_on);
			if (tok.size() != (machines+1)) {
				cout << "You have specified " << sizeof(tok)-1 << "  machines for P3 instead of " << machines << "\n";
			}
			for (int i = 1; i < tok.size(); i++) {
				P_Ip[31][i-1] = machineIps[tok[i]];
			}
		}
	}
};


#endif
