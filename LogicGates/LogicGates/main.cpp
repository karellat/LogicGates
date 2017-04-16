#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "workbench.h"
using namespace std;


static void WriteStringVector(vector<string> list)
{
	for_each(list.begin(), list.end(), [](auto s) {cout << s << endl; });
}
int main()
{
	std::string blank;

	unique_ptr<Workbench> workbench(new Workbench()); 

	workbench->AddInputGate();
	workbench->AdddOutputGate(); 
	cout << "Standart gates" << endl; 
	WriteStringVector(workbench->ListOfStandartGates());
	cout << "FreeOutputGates" << endl;
	WriteStringVector(workbench->ListOfFreeOutputGates());

	cout << "FreeInputGates" << endl; 
	WriteStringVector(workbench->ListOfFreeInputGates());

	workbench->Connect(0, 0, 0, 0);
	cout << "FreeOutputGates" << endl;
	WriteStringVector(workbench->ListOfFreeOutputGates());

	
	cout << "FreeInputGates" << endl;
	WriteStringVector(workbench->ListOfFreeInputGates()); 


	cout << "Logic Gates";
	getline(cin, blank);
}

