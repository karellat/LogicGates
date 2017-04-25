#include "workbenchTUI.h"

using pin = std::pair<std::string, std::size_t>;

WorkbenchTUI::~WorkbenchTUI()
{
}


bool WorkbenchTUI::ReadFile(string path)
{
	output << "Reading file " + path << endl;
	string line;
	if (!OpenFile(path)) return false;
	//Read name and check input tag
	getline(inputFile, line);
	vector<string> tokens = Split(line, '\t', RemoveEmptyEntries);

	if (tokens.size() != 2)
	{
		output << "Wrong format, definition line" << endl;
		return false;
	}

	string tag = ToUpper(tokens[0]);
	string gate_name = tokens[1];
	if (tag != definitionTag)
	{
		output << line << std::endl << "incorrect definition tag format" << endl;
		return false;
	}

	if (!nameSizeCheck(gate_name.size())) return false;
	if (!nameCharsCheck(gate_name)) return false;
	gate_name.resize(gate_name.size());
	gate_name = ToUpper(gate_name);

	output << "\tGate NAME: " << gate_name << endl;;
	name = gate_name;
	//Setting names: 

	string typeName;
	string vertexName;

	if (inputFile.eof())
	{
		output << "Unexpected eof" << endl;
		return false;
	}
	getline(inputFile, line);
	while (line[0] != '#')
	{
		tokens = Split(line, '\t', RemoveEmptyEntries);
		if (!nameSizeCheck(tokens[0]) || !nameCharsCheck(tokens[0]))
			return false;

		vertexName = ToUpper(tokens[0]);
		typeName = ToUpper(tokens[1]);
		output << "\tVertex Name: " << vertexName << " Type Name: " << typeName << endl;

		try {
			if (!workbench->AddNamedGate(vertexName, typeName))
			{
				output << "\tAdding same named vertex or unknown type";
				return false;
			}
		}
		catch(out_of_range)
		{
			output << "\tNon existing type name \"" << typeName << "\"" << endl;
			return false;
		}

		line = "";
		while (line == "")
		{
			if (inputFile.eof())
				output << "\tUnexpected end of file, in declaring section " << endl;
			getline(inputFile, line);
		}
	}
	output << "These named vertex were created: ";
	output << "Declaration part over " << endl;
	for (auto i : workbench->ListOfNamedVertex())
	{
		output << "\t " << i << endl;
	}
	//Connection part
	output << "Connection part: " << endl;
	tag = line;

	if (tag != connectionTag)
	{
		output << line << std::endl << "incorrect connection tag format" << endl;
		return false;
	}
	if (inputFile.eof())
	{
		output << "Unexpected eof" << endl;
		return false;
	}
	getline(inputFile, line);


	while (line[0] != '#')
	{
		tokens = Split(line, '\t', RemoveEmptyEntries);
		if (tokens.size() != 3)
		{
			output << "Incorrect format of connection line " + line << endl;
			return false;
		}
		pin aGate;
		pin bGate;
		if (!ParsePin(tokens[0], aGate) || !ParsePin(tokens[2], bGate))
			return false;
		if (tokens[1].size() != 2)
		{
			output << "Incorrect format of connection arrow: " + tokens[1];
			return false;
		}
		gvertex a;
		gvertex b;
		if (!workbench->GetVertex(aGate.first, a) || !workbench->GetVertex(bGate.first, b))
		{
			output << "Incorrect name, nick name does not exists:  " << aGate.first << ", " << bGate.first << endl;
			return false;
		}
		if (tokens[1] == "->")
		{
			if (!workbench->Connect(a, b, aGate.second, bGate.second))
			{
				output << "Incorrected pin,  ocuppied or not even exists :" << line << endl;
				return false;
			}
			else
			{
				output << "\tConnecting " << aGate.first << "[" << aGate.second << "]"
					<< " to " << bGate.first << "[" << bGate.second << "]" << endl;
			}
		}
		else if (tokens[1] == "<-")
		{
			if (!workbench->Connect(b, a, bGate.second, aGate.second))
			{
				output << "Incorrected pin,  ocuppied or not even exists :" << line << endl;
				return false;
			}
			else
			{
				output << "\tConnecting " << bGate.first << "[" << bGate.second << "]"
					<< " to " << aGate.first << "[" << aGate.second << "]" << endl;
			}
		}
		else
		{
			output << "Incorrect format of connection arrow " + tokens[1];
			return false;
		}





		line = "";
		while (line == "")
		{
			if (inputFile.eof())
			{
				output << "\tUnexpected end of file, in connecting section " << endl;
				return false;
			}
			getline(inputFile, line);
		}
	}

	//Construction part
	output << "Construction part: " << endl;
	if (workbench->ListOfFreeInputGates().size() != 0)
	{
		output << "Not connected input pins: " << endl;
		return false;
	}
	if (workbench->ListOfFreeOutputGates().size() != 0)
	{
		output << "Not connected output pins: " << endl;
		return false;
	}
	if (workbench->SizeOfInput() == 0)
	{
		output << "Input gates missing" << endl;
		return false;
	}
	if (workbench->SizeOfOutput() == 0)
	{
		output << "Output gates missing" << endl;
		return false;
	}

	output << "\tAll pins connected" << endl << "\tinput size: " << workbench->SizeOfInput() << " output size: " << workbench->SizeOfOutput() << endl;


	output << "\tConstructing workbench" << endl;
	if (workbench->ConstructBench())
	{
		output << "\tBench was successfully constructed" << endl;
		output << workbench->GetTestOutput();
	}
	else
	{
		output << "\tInvalid bench architecture, construction failed \n" << endl;
		output << workbench->GetTestOutput();
		return false;
	}

	//ending tag check
	if (line[0] == '#')
	{
		output << "Successfull loaded Gate from file " << endl;
		inputFile.close();
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
void WorkbenchTUI::InteraktiveMode(string path)
{
	bool  inRead = false; 
	while (!exiting && (inRead || ReadFile(path)))
	{
		InteractiveSeting();
		if(constructing)
		{
			constructing = false;
			if(workbench->ConstructUserGate(name))
			{
				output << "Gate was successful constructed, name: " << name << endl;
				name = "";
				while(!InteractiveReadingFile()){} 
				inRead = true; 
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

void WorkbenchTUI::PassiveMode(string path, string inputString)
{
	vector<bool> inputSettings;
	stringToBools(inputString, inputSettings);
	//Prepare bench: 
	if (!ReadFile(path))
		return;
	//Set input
	if (!SetInput(inputSettings))
		return;

	vector<bool> o;
	if (!ReadOutputs(o))
		return;
	else
	{
		string os;
		boolsToString(o, os);
		output << "Calculation was successfull, readed values: " << os << endl;
	}

	output << "Application closed" << endl;


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
	string s = "\tOpening file " + path;
	output << s << endl;
	inputFile.open(path);
	if (inputFile.is_open())
	{
		return true;
	}
	else
	{
		output << "\tUnable to open file" << std::endl;
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

bool WorkbenchTUI::boolsToString(std::vector<bool> v, string& s)
{
	s.clear();
	transform(v.begin(), v.end(), std::inserter(s, s.begin()), [](bool c)
	{
		if (c) return '1'; else return '0';
	});

	return true;
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


 