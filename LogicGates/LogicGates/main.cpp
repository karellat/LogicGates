#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include "workbench.h"
using namespace std;


static void WriteStringVector(vector<string> list)
{
	for_each(list.begin(), list.end(), [](auto s) {cout << s << endl << endl; });
}
int main(int argc, char *argv[])
{
	std::string blank;
	string path = argv[0];
	std::size_t  b = path.find_last_of('\\');
	path.erase(b,path.size()-1);
	
	
	
	unique_ptr<WorkbenchTUI> w = make_unique<WorkbenchTUI>(cout.rdbuf(), cin.rdbuf());
	if (argc == 3)
	{

		string file = argv[1];
		w->PassiveMode(path +'\\'+file, argv[2]);
	}
	else 
	cout << endl; 
	
	
	getline(cin, blank);
}

