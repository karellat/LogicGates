#include "workbenchTUI.h"
#include <iterator>

using pin = std::pair<std::string, std::size_t>;

WorkbenchTUI::~WorkbenchTUI()
{
}

bool WorkbenchTUI::ReadFile(string path)
{
	log << "Reading file " + path << endl;
	string line;
	if (!OpenFile(path)) return false;
	//Reading construction file && construct actual
	if (!ReadDefinitonHeader()) return false;
	log << "Header part sucessfully read\n";

	if (!ReadDeclarativeLine()) return false;
	log << "Declarative part sucessfully read\n";

	if (!ReadConnectionLine()) return false;
	log << "Connection part sucessfully read\n";

	//bench construction
	if (workbench->ConstructBench())
		log << "Workbench sucessfully constructed\n";
	else
		log << "Workbench constraction failed\n"; 

	//ending tag check
	if (line[0] == '#')
	{
		output << "Successfull loaded Gate from file " << endl;
		inputFile.close();
		//set status tag, that next construction file reading can create new Gate 
		readyForConstruction = true;
		return true;
	}
	else
	{
		output << "Ending tag missing, reading file failed" << endl;
		return false;
	}

	
}

void WorkbenchTUI::InteraktiveMode()
{
	while(!exiting && InteractiveReadingFile())
	{
		InteractiveSeting();
		if (constructing)
		{
			constructing = false;
			if (workbench->ConstructUserGate(name))
			{
				output << "Gate was successful constructed, name: " << name << endl;
				name = "";
			}
			else
			{
				output << "Gate construction failed, exiting " << endl;
				exiting = true;
			}

		}
		if (reseting)
		{
			workbench->ResetWorkbench(true);
			output << "Reseting readed workbench, deleting user defined gates" << endl;
			reseting = false;
		}
	}
		output << "Exiting interactive mode" << endl;
}

void WorkbenchTUI::PassiveMode(vector<string> filePaths, vector<vector<bool>> inputSet)
{
	//Construct all construction files;
	for (auto path : filePaths)
	{
		if (readyForConstruction)
		{
			readyForConstruction = false; 
			if (!ConstructGate())
				return;			
		}
		if (!ReadFile(path))
			return;
	}
	//Set inputs & read eval if  possible 
	for (auto input: inputSet)
	{
		if(SetInput(input))
		{
	 		vector<bool> o; 
			if (ReadOutputs(o))
			{
				output << "OUTPUT: " << boolsToString(o);
				log << "OUTPUT: " << boolsToString(o); 
			}
		}
	}
}

void WorkbenchTUI::InteractiveSeting()
{
	string line;
	vector<bool> in;
	vector<bool> out;
	string s;
	while (!exiting && !reseting && !constructing)
	{
		string line;
		output << "Set input(" + to_string(workbench->SizeOfInput())<< "):\t";
		input >> line;
		if (ShowHelp(line) || ParseKeyWords(line))
			continue;
		if (stringToBools(line, in))
		{
			if (SetInput(in))
			{
				if (ReadOutputs(out))
				{
					boolsToString(out, s);
					output << "Output : " << s << endl;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			output << "Wrong format of input, ";
			continue;
		}
	}
}
bool WorkbenchTUI::InteractiveReadingFile()
{
	string line;
	output << "Insert path of input file: " << endl << "\t";

	while (!exiting)
	{
		input >> line;
		if (ShowHelp(line)) continue;
		if (  line == "exit" || line == "q" || line == "kill" || line == "quit")
		{
			exiting = true;
			continue;
		}
		

		if (!ReadFile(line)) {
			inputFile.close();
			output << "Invalid name, insert path of correct input file: " << endl << "\t";
		}
		else
		{
			return true;
		}
	}
	return true;
}

bool WorkbenchTUI::SetInput(vector<bool> inputSettings)
{
	if (inputSettings.size() != workbench->SizeOfInput())
	{
		output << "Invalid size of input " << endl;
		return false;
	}
	if (workbench->SetInput(inputSettings))
	{
		string s;
		boolsToString(inputSettings, s);
		output << "Input was set to: " << s << endl;
		return true;
	}
	else
	{
		output << "Input was not correctly set, workbench invalid status" << endl;
		return false;
	}
}

bool WorkbenchTUI::ReadOutputs(vector<bool>& outputValue)
{
	outputValue = workbench->ReadOutput();
	return true;
}


bool WorkbenchTUI::OpenFile(std::string path)
{
	log << "\tOpening file " + path << endl;
	inputFile.open(path);
	if (inputFile.is_open())
			return true;
	else
	{
		log << "\tUnable to open file" << std::endl;
		return false;
	}
}

bool WorkbenchTUI::ParsePin(string input, std::pair<string, std::size_t>& pair)
{
	if (input[input.size() - 1] != ']')
	{
		output << "Wrong format of pin missing \"]\" : " + input;
		return false;
	}
	std::size_t i = input.find_first_of('[');
	if (i == input.npos)
	{
		output << "Wrong format of pin missing \"[\" : " + input;
		return false;
	}

	std::string num = input.substr(i + 1, input.size() - i - 2);
	std::string name = ToUpper(input.substr(0, i));

	std::size_t t;
	try {
		long n = stoul(num, nullptr, 0);
		t = (std::size_t) n;
	}
	catch (invalid_argument)
	{
		output << "Invalid id of pin " + num;
		return false;
	}
	pair.first = name;
	pair.second = t;
	return true;
}

bool WorkbenchTUI::nameSizeCheck(string name)
{
	return nameSizeCheck(name.size());
}

bool WorkbenchTUI::nameSizeCheck(std::size_t size)
{
	if (size > maxSizeOfTag)
	{
		output << "Gate name is too long (" + to_string(size) + ")" + " max=" + std::to_string(maxSizeOfTag) << endl;
		return false;
	}
	return true;
}

bool WorkbenchTUI::nameCharsCheck(string name)
{
	if (any_of(name.begin(), name.end(), [this](char c) {
		return any_of(forbidenChars.begin(), forbidenChars.end(),
			[c](char  fc) { return fc == c; });
	}))
	{
		output << "Name contains one of the forbidden chars \"" + name + "\"";
		return false;
	}
	else return true;

}

std::vector<string> WorkbenchTUI::Split(string s, char delimeter, StringSplitOption option)
{
	vector<string> tokens;
	string token;
	istringstream in(s);
	while (getline(in, token, delimeter))
	{
		if (token != "" || option == None)
			tokens.push_back(token);
	}
	return tokens;
}

std::string WorkbenchTUI::ToUpper(std::string s)
{
	string o;
	o.resize(s.size());
	std::locale loc;
	std::transform(s.begin(), s.end(), o.begin(), [loc](char c) {return toupper(c,loc); });
	return o;
}

string WorkbenchTUI::boolsToString(std::vector<bool>& v)
{
	string s;
	transform(v.cbegin(), v.cend(), std::inserter(s, s.begin()), [](bool c)
	{
		if (c) return '1'; else return '0';
	});

	return std::move(s); 
}

bool WorkbenchTUI::stringToBools(string s, std::vector<bool>& b)
{
	b.clear();
	try
	{
		transform(s.begin(), s.end(), std::inserter(b, b.begin()), [](char c)
		{
			if (c == '1') return true; else if (c == '0') return false;
			else throw runtime_error("UnknownChar");
		});
	}
	catch (runtime_error)
	{
		output << "Wrong format of input: " << s << endl;
		return false;
	}
	return true;
}

bool WorkbenchTUI::ParseKeyWords(string word)
{
	if (word == "exit" || word == "q" || word == "kill" || word == "quit")

	{
		exiting = true;
		return true;
	}

	if (word == "reset" || word == "r")
	{
		reseting = true;
		return true;
	}

	if (word == "const" || word == "c" || word == "construct")
	{
		constructing = true;
		return true; 
	}
	return false;

}

bool WorkbenchTUI::ShowHelp(string word)
{
	if(word == "help" || word == "h" || word == "man")
	{
		output << "\t EXIT: \"exit\" ; \"e\", RESET: \"reset\";\"r\",CONSTR: \"const\", \"c\" \n";
		return true;
	}
	return false;
}


 