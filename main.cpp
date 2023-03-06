#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_set>
#include <bits/stdc++.h>
#include "transistor.h"
#include "map.h"
using namespace std;

int main(int argc, char** argv)
	{
	
	string line;
	string subcircuit;
	char delim[]=" ";
	vector<string> power_pins;
	vector<string> ground_pins;
	vector<string> pins;
	vector<string> in_pins;
	vector<string> out_pins;
	vector<string> common_nets;
	vector<string> arcs;
	vector<Transistor> PDN;
	vector<Transistor> PUN;
	
	cout << "You have entered " << argc << " arguments:" << "\n";

	if (argc < 3 ){
		cout << "call: ./main [*].sp [max_stack]" << endl;
	}
	ifstream myfile (argv[1]);
	int input_stack = strtol(argv[2], NULL, 10);

	if (0==(myfile.is_open())){
		cout << "Cannot open file:" << argv[1] << endl;
	}
	print_logo();

	while(getline(myfile,line)){
    	stringstream lineStream(line);
    	string token;
    	lineStream >> token;
    	//------------------------------------------------------------------
    	// IF FIRST LINE TOKEN == "*.pininfo"
    	//------------------------------------------------------------------
  		//------------------------------------------------------------------
  		//IF FIRST LINE TOKEN STARTS WITH "M" (Transistor)
  		//------------------------------------------------------------------
		
		if((token == ".SUBCKT") | (token == ".subckt")){
  			//cout << "--------------------------------------\n" << "Fetching Pinage Information\n";
			lineStream >> token;
			subcircuit = token;
			cout << "Subcircuit:" << token << endl;
  			while(lineStream >> token)
    		{
        		if ((token == "VDD")|(token == "vdd")|(token == "VPWR")|(token == "VPB")) {;
        			power_pins.push_back(token);
					//cout << "vdd:" << token << endl;
				}
				else if((token == "GND")|(token == "gnd")|(token == "VSS")|(token == "vss")|(token == "VGND")|(token == "VNB")){
					ground_pins.push_back(token);
					//cout << "gnd:" << token << endl;
				}
				else
				{
        			pins.push_back(token);
					//cout << "pin:" << token << endl;
				}
    		}
  		}
  		else if((token[0]=='M')|(token[0]=='m')|(token[0]=='X')|(token[0]=='x')){
        string alias;
        string source;
        string drain;
        string gate;
        string bulk;
        string type; //PMOS,NMOS,LVT,HVT
        double diff_width;
        int fingers=0;
        double gate_lenght;
        int stack = 1;
        char * tail;

  			alias = token;
  			lineStream >> token;
  			source = token;
  			lineStream >> token;
  			gate = token;
  			lineStream >> token;
  			drain = token;
  			lineStream >> token;
  			bulk = token;
  			lineStream >> token;
  			type = token;

  			/*
  			cout << "Transistor: " << endl;
  			cout << "Alias:" << alias << endl;
  			cout << "Source:" << source << endl;
  			cout << "Gate:" << gate << endl;
  			cout << "Drain:" << drain << endl;
  			cout << "Bulk:" << bulk << endl;
  			cout << "Type:" << type << endl;
			*/

			while(lineStream >> token){
        		if ((token.find("L=") != string::npos)|token.find("l=") != string::npos) {

        			token.erase(token.begin(),token.begin()+2);
        			gate_lenght = strtod(token.c_str(),&tail);
        			//cout << "Gate Lenght: " << gate_lenght << endl;
				}
				else if ((token.find("W=") != string::npos)|(token.find("w=") != string::npos))
				{
        			token.erase(token.begin(),token.begin()+2);
        			diff_width = strtod(token.c_str(),&tail);
        			//cout << "Width: " << diff_width << endl;
				}
				else if ((token.find("F=") != string::npos)|(token.find("f=") != string::npos))
				{
        			token.erase(token.begin(),token.begin()+2);
        			fingers = atoi(token.c_str());
        			//cout << "Fingers: " << fingers << endl;
				}
			}
			if((type[0]=='P')|(type[0]=='p')|(type.find("pfet") != string::npos)|(type.find("pch") != string::npos)){
				Transistor p_transistor(alias, source, drain, gate, bulk, type, diff_width, fingers, gate_lenght, stack);
				PUN.push_back(p_transistor);
				//cout << "PMOS ADDED TO PUN LIST" << endl;
			}
			else{
				Transistor n_transistor(alias, source, drain,	gate, bulk,	type, diff_width, fingers, gate_lenght, stack);
				PDN.push_back(n_transistor);
				//cout << "NMOS ADDED TO PDN LIST" << endl;
			}
		}
  		else{

    	}
    
	   }
	
	/*
    int x=0;
    cout << "----------------------------------------" << endl;
    cout << "PMOS:" << endl;
    for (auto it = begin(PUN); it != end(PUN); ++it){
      cout << x << ":" << it->get_alias() << " " << it->get_source() << " " << it->get_drain() << endl;
      x++;
    }

    cout << "NMOS:" << endl;
    for (auto it = begin(PDN); it != end(PDN); ++it){
      cout << x << ":" << it->get_alias() << " " << it->get_source() << " " << it->get_drain() << endl;
      x++;
    }
	*/
	//Get All PDN and PUN Common Nodes
	
	for (Transistor p_transistor : PUN){
	  for (Transistor n_transistor : PDN){
      	if (p_transistor.get_source() == n_transistor.get_source()){
			//cout << p_transistor.get_source() << "=" << n_transistor.get_source() << endl;
			common_nets.push_back(p_transistor.get_source());
		}
		else if(p_transistor.get_source() == n_transistor.get_drain()){
			//cout << p_transistor.get_source()  << "=" << n_transistor.get_drain() << endl;
			common_nets.push_back(p_transistor.get_source());
		}
		else if(p_transistor.get_drain() == n_transistor.get_source()){
			//cout << p_transistor.get_drain() << "=" << n_transistor.get_source() << endl;
			common_nets.push_back(p_transistor.get_drain());
		}
		else if(p_transistor.get_drain() == n_transistor.get_drain()){
			//cout << p_transistor.get_drain() << "=" << n_transistor.get_drain() << endl;
			common_nets.push_back(p_transistor.get_drain());
    	}
		else{

		}
    	}
	}
	//Remove Duplicates
	sort(common_nets.begin(), common_nets.end());
    common_nets.erase(std::unique(common_nets.begin(), common_nets.end()), common_nets.end());
	
	
	//Remove Common Nodes from the pin list
	in_pins = pins;
	distribute_pins(common_nets, in_pins, out_pins);
	
	for (auto it = begin(in_pins); it != end(in_pins); ++it){
		cout << "input:" << *it << endl;
	}
	for (auto it = begin(out_pins); it != end(out_pins); ++it){
		cout << "output:" << *it << endl;
	}

	int circuit_columns = common_nets.size();
	FILE * out_file;
	if ((worst_stack_counter(circuit_columns, PUN, power_pins, ground_pins) > input_stack) | (worst_stack_counter(circuit_columns, PDN, power_pins, ground_pins) > input_stack)){
		out_file = fopen("filter.sdc","a");
		string dont_use = "set_dont_use " + subcircuit + " TRUE \n";
		cout << dont_use << endl;
		fprintf(out_file, dont_use.c_str());
		fclose (out_file);
	}
	else{
		return 0;
	}
    return 0;
	}

