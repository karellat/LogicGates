#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include "workbenchTUI.h"
using namespace std;


static void WriteStringVector(vector<string> list)
{
	for_each(list.begin(), list.end(), [](auto s) {cout << s << endl << endl; });
}


bool argumentTest(char* text)
{
	return strcmp(text, "-i") || strcmp(text, "-f") || strcmp(text, "-h") || strcmp(text, "-d");
}

vector<bool> stringToBools(string text)
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
	unique_ptr<WorkbenchTUI> w = make_unique<WorkbenchTUI>(cout.rdbuf(), cin.rdbuf(),cout.rdbuf());
	bool debugMode = false;
	vector<string> fileNames;
	vector<vector<bool>>   inputs;


	if (argc == 1)
		w->InteraktiveMode();
	else
	{
		size_t index = 1;
		//Parse parameters: 
		while(index < argc)
		{
			if(strcmp(argv[index],"-d"))
			{
				debugMode = true; 
				index++;
			}
			else if(strcmp(argv[index],"-f"))
			{
				index++;
				while(index < argc && !argumentTest(argv[index]))
				{
					fileNames.push_back(argv[index]);
					index++;
				}
			}
			else if(strcmp(argv[index],"-i"))
			{
				index++;
				while (index < argc && !argumentTest(argv[index]))
				{
					try {
						inputs.push_back(stringToBools(std::move(argv[index])));
					}
					catch(invalidinputformat& e)
					{
						std::cout << e.what() << endl;
						return 1;
					}
						index++;
				}
			}
			else if(strcmp(argv[index],"-h"))
			{
				index++;
				std::cout << "Logic Gates" << endl;
				std::cout << "DESCRIPTIONS:" << endl;
				std::cout << "\tArguments: " << endl; 
				std::cout << "\t-h \tprint help " << endl;
				std::cout << "\t-f [file..] \tconstruction files given to construct in given order" << endl; 
				std::cout << "\t-i [STRING..] \tinputs for last constructed logic network, 1 - for true, 0 - for false, if wrong format throws exception " << endl; 
			}
		}
		//Run workbench in passive mode: 
		w->PassiveMode(std::move(fileNames), std::move(inputs));
	}

	w.release();
}

