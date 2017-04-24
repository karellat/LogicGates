#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include "workbench.h"
#include "workbenchTUI.h"
using namespace std;


static void WriteStringVector(vector<string> list)
{
	for_each(list.begin(), list.end(), [](auto s) {cout << s << endl << endl; });
}
int main(int argc, char *argv[])
{
	string blank;
	unique_ptr<WorkbenchTUI> w = make_unique<WorkbenchTUI>(cout.rdbuf(), cin.rdbuf());
	switch (argc)
	{
	case 3:
		{
		w->PassiveMode(argv[1], argv[2]);
		break;
		}
	case 2:
		{
		w->InteraktiveMode(argv[1]);
		break;
		}
	default:
		{
		w->InteraktiveMode();
		}
	}
	
	
	getline(cin, blank);
}

