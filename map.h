#ifndef MAP_H
#define MAP_H
#include "transistor.h"
#include <vector>
using namespace std;

//------------------------- CLI -------------------------------------------
void print_logo();

//------------------------- Pin Related -----------------------------------
vector<string> remove_pin(vector<string> pin_list, string pin);

void distribute_pins(vector<string>& common_nets, vector<string>& in_pins, vector<string>& out_pins);

//-------------------------Checks------------------------------------------

bool check_common_net(Transistor& T0, string& common_net);

bool check_pg_pin(string pin, vector<string>& power_pins, vector<string>& ground_pins);

bool check_series(Transistor& A, Transistor& B, vector<string>& power_pins, vector<string>& ground_pins);

//------------------------- Flattening ------------------------------------


Transistor merge_series(Transistor A, Transistor B, vector<string> power_pins, vector<string> ground_pins);

int worst_stack_counter(int circuit_columns, vector<Transistor> PDN, vector<string>& power_pins, vector<string>& ground_pins);

string flatten_expression(vector<string> common_nets, vector<string> expressions);


#endif
