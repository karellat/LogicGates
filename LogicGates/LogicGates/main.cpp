#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <memory>
#include "workbenchTUI.h"
using namespace std;

//DEBUG CODE:	
//static void WriteStringVector(vector<string> list)
//{
//	for_each(list.begin(), list.end(), [](auto s) {cout << s << endl << endl; });
//}

bool argumentTest(char* text)
{
	return (std::strcmp(text, "-i") == 0) || (std::strcmp(text, "-f") == 0) || (std::strcmp(text, "-h") == 0) || (std::strcmp(text, "-d") == 0) || (std::strcmp(text, "-a") == 0);
}

vector<bool> stringToBools(const string& text)
{
	vector<bool> o; 
	for_each(text.begin(),text.end(), [&o] (char c)
	{
		if (c == '1')
			o.push_back(true);
		else if (c == '0')
			o.push_back(false);
		else
			throw iinput;
	});
	return std::move(o);
	
}

int main(int argc, char *argv[])
{

	string blank;

	//bool debugMode = false;
	vector<string> fileNames;
	vector<vector<bool>>   inputs;
	unique_ptr<WorkbenchTUI> w;



	if (argc <= 1) {
		w = make_unique<WorkbenchTUI>(cout.rdbuf(), cin.rdbuf(), cout.rdbuf(), false);
		w->InteractiveMode();
		return 0;
	}

	bool debugMode = false;
	bool tryAllInputs = false;
	int index = 1;
	//Parse parameters:
	while (index < argc)
	{
		if (std::strcmp(argv[index], "-d") == 0)
		{
			debugMode = true;
			index++;
		}
		else if (std::strcmp(argv[index], "-f") == 0)
		{
			index++;
			while (index < argc && !argumentTest(argv[index]))
			{
				fileNames.push_back(argv[index]);
				index++;

			}
		}
		else if (std::strcmp(argv[index], "-i") == 0) {
			index++;
			while (index < argc && !argumentTest(argv[index]))
			{
				try {
					if (!tryAllInputs) inputs.push_back(stringToBools(argv[index]));
				}
				catch (invalidinputformat& e)
				{
					std::cout << e.what() << endl;
					return 1;
				}
				index++;

			}
		}
		else if (std::strcmp(argv[index], "-h") == 0)
		{
			index++;
			std::cout << "Logic Gates" << endl;
			std::cout << "DESCRIPTIONS:" << endl;
			std::cout << "\tArguments: " << endl;
			std::cout << "\t-h \tprint help " << endl;
			std::cout << "\t-f [file..] \tconstruction files given to construct in given order" << endl;
			std::cout << "\t-i [STRING..] \tinputs for last constructed logic network, 1 - for true, 0 - for false, if wrong format throws exception " << endl;
			std::cout << "\t-a \t generate all possible outputs and ignore previous inputs read afer -i" << endl;
		}
		else if (std::strcmp(argv[index], "-a") == 0)
		{
			index++;
			tryAllInputs = true;
		}
	}
	//Run workbench in passive mode: 
	if (!fileNames.empty() && (!inputs.empty() || tryAllInputs))
	{		w = make_unique<WorkbenchTUI>(cout.rdbuf(), cin.rdbuf(), cout.rdbuf(), debugMode);
	w->PassiveMode(std::move(fileNames), std::move(inputs), tryAllInputs);
	}
	else
	{
		std::cout << "At least one construction file & at least one input has to be given " << endl;
	}
	std::cout << "Press ENTER to exit" << endl;
	getline(cin, blank);
}

