#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "workbench.h"
using namespace std;


static void WriteStringVector(vector<string> list)
{
	for_each(list.begin(), list.end(), [](auto s) {cout << s << endl << endl; });
}
int main()
{
	std::string blank;

	unique_ptr<Workbench> workbench = make_unique<Workbench>();

	workbench->AddInputGate();
	workbench->Add(0);

	workbench->Connect(0, 0, 0, 0);
	workbench->AdddOutputGate(); 
	workbench->Connect(0, 0, 0, 0);
	workbench->AddInputGate();
	workbench->Add(1);
	workbench->Connect(0, 0, 0, 0);
	workbench->AddInputGate();
	workbench->Connect(0, 1, 1, 0);
	workbench->AdddOutputGate();
	workbench->Connect(0, 0, 0, 0);

	workbench->ConstructBench();

	workbench->SetInput({ true,true,true });
	for( auto i : workbench->ReadOutput())
	{
		cout << i << endl; 
	}

	cout << endl; 
	
	
	getline(cin, blank);
}

