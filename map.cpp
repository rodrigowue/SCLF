#include "map.h"
#include "transistor.h"
#include <unordered_set>
#include <vector>
#include <math.h>
using namespace std;

void print_logo(){
cout << "======================================" << endl;
cout << " STD-CELL LIBRARY FILTER BY STACK" << endl;
cout << "       [UNDER DEVELOPMENT]" << endl;
cout << "======================================" << endl;
};

vector<string> remove_pin(vector<string> pin_list, string pin){
	for (auto it_rm = begin(pin_list); it_rm != end(pin_list); ){
		if (*it_rm == pin){
			//cout << "deleting " << *it_rm << " from the list" << endl;
    		pin_list.erase(it_rm);
			}
		else{
			it_rm++;
			}
		}

	return pin_list;
}

void distribute_pins(vector<string>& common_nets, vector<string>& in_pins, vector<string>& out_pins){
	vector<string> in_pins_tmp;
	for (string common_net: common_nets){
		for (string pin : in_pins){
			if(common_net == pin){
				in_pins = remove_pin(in_pins, pin);
				out_pins.push_back(pin);
				if (in_pins.size() == 1){
					break;
				}
				else{
					distribute_pins(common_nets, in_pins, out_pins);
					break;
				}
			}
			
		}
	}
	return;
};

// -----------------------------------------------------------------------------------
//                            CHECKS
// -----------------------------------------------------------------------------------

bool check_common_net(Transistor& T0, string& common_net){
	if (((T0.get_source() == common_net)|(T0.get_drain() == common_net))){
		return true;
	}
	else{
		return false;
	}
}

bool check_pg_pin(string pin, vector<string>& power_pins, vector<string>& ground_pins){
		for (int i = 0; i < power_pins.size(); i++){
			if(pin == power_pins[i]){
				return false;
			}
		}
		for (int i = 0; i < ground_pins.size(); i++){
			if(pin == ground_pins[i]){
				return false;
			}
		}
		return true;
}

bool check_series(Transistor& A, Transistor& B, vector<string>& power_pins, vector<string>& ground_pins){
	if ( (( A.get_source() == B.get_source() ) & (check_pg_pin(A.get_source()  , power_pins, ground_pins)) & (A.get_drain()  != B.get_drain() )) |
		 (( A.get_drain() == B.get_drain()   ) & (check_pg_pin(A.get_drain() , power_pins, ground_pins)) & (A.get_source() != A.get_source())) |
		 (( A.get_source() == B.get_drain()  ) & (check_pg_pin(A.get_source()  , power_pins, ground_pins)) & (A.get_drain()  != B.get_source())) |
		 (( A.get_drain() == B.get_source()  ) & (check_pg_pin(A.get_drain() , power_pins, ground_pins)) & (A.get_source() != B.get_drain() )) ){
		return true;
	}
	else{
		return false;
	}
}

// -----------------------------------------------------------------------------------
//                            FLATTENING
// -----------------------------------------------------------------------------------


Transistor merge_series(Transistor A, Transistor B, vector<string> power_pins, vector<string> ground_pins){
		string type = "";
		string alias = "";
		string source = "";
		string bulk = "";
		string drain = "";
		int fingers=0;
		double diff_width = 0.0;
		double gate_lenght = 0.0;
		int stack = A.get_stack() + B.get_stack();
		alias.append("(");
		alias.append(A.get_gate());
		alias.append("*");
		alias.append(B.get_gate());
		alias.append(")");
   		//Find the connecting point and preserve the connection
   		if ( A.get_source() == B.get_source() & (check_pg_pin(A.get_source(), power_pins, ground_pins))){
			source = A.get_drain();
			drain  = B.get_drain();
        	}
        else if(A.get_drain() == B.get_drain() & (check_pg_pin(A.get_drain(), power_pins, ground_pins))){
        	source = A.get_source();
			drain  = B.get_source();
        	}
        else if(A.get_source() == B.get_drain() & (check_pg_pin(A.get_source(), power_pins, ground_pins))){
        	source = A.get_drain();
			drain  = B.get_source();
        	}
        else{
        	source = A.get_source();
			drain  = B.get_drain();
        }
		Transistor group_transistor(alias, source, drain, alias, bulk, type, diff_width, fingers, gate_lenght, stack);
		return group_transistor;
}

int worst_stack_counter(int circuit_columns, vector<Transistor> PDN, vector<string>& power_pins, vector<string>& ground_pins){
    vector<Transistor> PDN_TEMP = PDN;
	int max_stack = 0;
	
	for (int i = 0; i < PDN_TEMP.size() - 1; i++) {
        Transistor& t1 = PDN_TEMP[i];
        for (int j = i + 1; j < PDN_TEMP.size(); j++) {
            Transistor& t2 = PDN_TEMP[j];
	  				if ((check_series(t1, t2, power_pins, ground_pins))){
					Transistor group_transistor = merge_series(t1, t2, power_pins, ground_pins);
					PDN_TEMP.erase(PDN_TEMP.begin() + j);
                	PDN_TEMP.erase(PDN_TEMP.begin() + i);
					PDN_TEMP.push_back(group_transistor); // insert the merged item
					if(PDN_TEMP.size() == circuit_columns){
						for (int temp = 0; temp < PDN_TEMP.size(); temp++){
						if(PDN_TEMP[temp].get_stack() > max_stack){
							max_stack  = PDN_TEMP[temp].get_stack();
						}
						}
						return max_stack;
					}
					else{
						i = -1;
						break;
					}
	  			}
			}
	}
	
	for (int i = 0; i < PDN_TEMP.size(); i++){
		if(PDN_TEMP[i].get_stack() > max_stack){
			max_stack  = PDN_TEMP[i].get_stack();
		}
	}
	return max_stack;
}
