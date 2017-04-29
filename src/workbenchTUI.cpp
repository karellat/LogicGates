#include "workbenchTUI.h"
#include <iterator>
#include <iostream>

using pin = std::pair<std::string, std::size_t>;

bool WorkbenchTUI::ReadFile(const string& path)
{
	log << "Reading file " + path << endl;
	string line;
	if (!OpenFile(path)) return false;
	//Reading construction file && construct actual workbech if necessarily 
	if (!ReadDefinitonHeader()) return false;
	log << "Header part sucessfully read\n";

	if (!ReadDeclarativeLine()) return false;
	log << "Declarative part sucessfully read\n";

	if (!ReadConnectionLine()) return false;
	log << "Connection part sucessfully read\n";

	try
	{
		workbench->ConstructBench(); 
	}
	catch(exception& e) 
	{
		log << "Bench construction failed at: " << e.what() << endl;
		return false; 
	}
	
	log << "Actual bench was constructed.\n";
	//ending tag check
	output << "Successfull loaded Gate from file " << endl;
	inputFile.close();
	//set status tag, that next construction file reading can create new Gate 
	readyForConstruction = true;
	return true;
}

void WorkbenchTUI::InteraktiveMode()
{
}

void WorkbenchTUI::PassiveMode(const vector<string>& filePaths, const vector<vector<bool>>& inputSet, bool tryAllinputs)
{
	//Construct all construction files;
	for (auto path : filePaths)
	{
		if (!ReadFile(path))
			return;
	}
	//Generate vector of all possible inputs of given lenght or use given vector of bools
	//Set inputs & read eval if  possible 
	if(tryAllinputs)
	{
		vector<vector<bool>> generatedInputSet = GenerateInput(workbench->SizeOfInput());
		for (auto input : generatedInputSet)
		{
			if (SetInput(input))
			{
				vector<bool> o;
				if (ReadOutputs(o))
				{
					output << "OUTPUT: " << boolsToString(o) << endl;
					//log << "OUTPUT: " << boolsToString(o); 
				}
			}
		}

	}
	else {
		for (auto input : inputSet)
		{
			if (SetInput(input))
			{
				vector<bool> o;
				if (ReadOutputs(o))
				{
					output << "OUTPUT: " << boolsToString(o) << endl;
					//log << "OUTPUT: " << boolsToString(o); 
				}
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
					output << "Output : " << boolsToString(out)<< endl;
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

bool WorkbenchTUI::SetInput(const vector<bool>& inputSettings)
{
	if (inputSettings.size() != workbench->SizeOfInput())
	{
		log << "Invalid size of input " << endl;
		return false;
	}
	try {
		workbench->SetInput(inputSettings);
		string s;
		output << "Input was set to: " << boolsToString(inputSettings) << endl;
		return true;
	}
	catch(exception& e)
	{
		log << "Invalid output or situation: " << e.what() << endl;
		return false;
	}
}

bool WorkbenchTUI::ReadOutputs(vector<bool>& outputValue) const
{
	outputValue = workbench->ReadOutput();
	return true;
}


bool WorkbenchTUI::OpenFile(const std::string& path)
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

bool WorkbenchTUI::ReadDefinitonHeader()
{

	//Check first line of file 
	string line;
	size_t newInputSize; 
	size_t newOutputSize;
	if (inputFile.eof())
	{
		log << "Unexpected end of file\n";
		return false;
	}
	std::getline(inputFile, line);
	vector<string> tokens = Split(line, '\t', RemoveEmptyEntries);
	if (tokens.size() != 2)
	{
		log << "Wrong amount of arguments in header line\n";
		return false;
	}

	for (size_t i = 0; i < tokens.size(); i++)
	{
		tokens[i] = ToUpper(tokens[i]);
	}

	if (tokens[0] != definitionTag)
	{
		log << "Wrong format of defition tag " << tokens[0] << endl;
		return false;
	}
	//Name 
	if (!nameSizeCheck(tokens[1]) || !nameCharsCheck(tokens[1]))
		return false;

	string new_name = tokens[1];
	//Get Input size 
	if (inputFile.eof())
	{
		log << "Unexpected end of file \n";
		return false;
	}
	getline(inputFile, line);
	tokens = Split(line, '\t', RemoveEmptyEntries);
	if (tokens.size() != 2)
	{
		log << "Invalid amount of tokens in inputSize line\n";
		return false;
	}
	tokens[0] = ToUpper(tokens[0]);
	if (tokens[0] != inputTag)
	{
		log << "Wrong format of input  tag\n";
		return false;
	}
	//size of input 
	try
	{
		newInputSize = stoi(tokens[1]);
	}
	catch(exception& e)
	{
		log << "Wrong format of input size: " << tokens[1] << "; " << e.what() << endl;
		return false;
	}
	//Get Output size 
	if (inputFile.eof())
	{
		log << "Unexpected end of file \n";
		return false;
	}
	getline(inputFile, line);
	tokens = Split(line, '\t', RemoveEmptyEntries);
	if (tokens.size() != 2)
	{
		log << "Invalid amount of tokens in outputSize line\n";
		return false;
	}
	tokens[0] = ToUpper(tokens[0]);
	if (tokens[0] != outputTag)
	{
		log << "Wrong format of output tag\n";
		return false;
	}
	//size of input 
	try
	{
		newOutputSize = stoi(tokens[1]);
	}
	catch (exception& e)
	{
		log << "Wrong format of output size: " << tokens[1] << "; " << e.what() << endl;
		return false;
	}

	//Construct Gate on the bench 
	if(readyForConstruction)
	{
		if (!ConstructGate(newInputSize, newOutputSize, name))
			return false; 
	}
	else
	{
		workbench = make_unique<Workbench>(newInputSize, newOutputSize,std::cin.rdbuf());
	}

	//Change name 
	name = new_name;

	return true; 
}

bool WorkbenchTUI::ReadDeclarativeLine()
{
	//make names for vertex
	string line;
	if(input.eof())
	{
		log << "Unexpected end of file\n"; 
		return false;
	}
	getline(inputFile, line);
	while (line[0] != '#')
	{
		//read tokens[0] - name tokens[1] - type
		vector<string> tokens = Split(line, '\t', RemoveEmptyEntries);
		if(tokens.size() != 2)
		{
			log << "Wrong amount of tokens " << line << endl;
 			return false; 
		}
		for (size_t i = 0; i < tokens.size(); i++)
		{
			tokens[i] = ToUpper(tokens[i]);
		}
		if (!nameSizeCheck(tokens[0]) || !nameCharsCheck(tokens[0]))
			return false;
		try {
			workbench->Add(tokens[0], tokens[1]);
		}
		catch(exception& e)
		{
			log << "Invalid naming line: " << line << endl << "E:" << e.what() << endl;
			return false;
		}
		if (input.eof())
		{
			log << "Unexpected end of file\n";
			return false;
		}
		getline(inputFile, line);
	}

	//Check connection tag
	if(line != connectionTag)
	{
		log << "Wrong format of connection tag\n";
		return false; 
	}
	
	return true;
}

bool WorkbenchTUI::ReadConnectionLine()
{
	string line; 
	if(input.eof())
	{
		log << "Unexpected end of file\n";
		return false; 
	}
	getline(inputFile,line);
	while(line[0] != '#')
	{
		vector<string> tokens = Split(line, '\t', RemoveEmptyEntries);
		if(tokens.size() != 3)
		{
			log << "Wrong amount of tokens: " << line << endl;
		}
		pair<string, size_t> from;
		pair<string, size_t> to;

		try {
			if (tokens[1] == "->")
			{
				if(!ParsePin(tokens[0], from))
				{
					log << "Wrong format of name and pin: " << line << endl;
					return false; 
				}
				if (!ParsePin(tokens[2], to))
				{
					log << "Wrong format of name and pin: " << line << endl;
					return false;
				}
			}
			else if (tokens[1] == "<-")
			{
				if (!ParsePin(tokens[2], from))
				{
					log << "Wrong format of name and pin: " << line << endl;
					return false;
				}
				if (!ParsePin(tokens[0], to))
				{
					log << "Wrong format of name and pin: " << line << endl;
					return false;
				}
			}
			else
			{
				log << "Wrong format of connection line " << endl;
				return false;
			}

			workbench->Connect(from.first, from.second, to.first, to.second);
		}
		catch(exception& e)
		{
			log << "Invalid connection: " << e.what() << endl;
			log << "This line: " << line;
			return false;
		}

		if (input.eof())
		{
			log << "Unexpected end of file\n";
			return false;
		}
		getline(inputFile, line);
	}
	//Check the ending tag
	if(line != "#")
	{
		log << "Wrong ending tag\n"; 
		return false; 
	}
	return true;
}

bool WorkbenchTUI::ConstructGate(size_t newInputSize, size_t newOutputSize, const string& actualName)
{
	try {
		workbench->ConstructUserGate(actualName, newInputSize, newOutputSize);
	}
	catch(exception& e)
	{
		log << "Construction failed: " << e.what();
		return false;
	}
	return true;
}

vector<vector<bool>> WorkbenchTUI::GenerateInput(const size_t size_of_input)
{
	//bit adding
	vector<vector<bool>> out; 
	vector<bool> v; 
	for (size_t i=0;i< size_of_input; i++)
	{
		v.push_back(false);
	}
	out.push_back(v);
	while (any_of(v.cbegin(), v.cend(), [](bool b) {return !b; }))
	{
		size_t index = 0;
		while(v[index])
		{
			v[index] = false; 
			index++;
		}
		v[index] = true;
		out.push_back(v);
	}



	return std::move(out);
}


bool WorkbenchTUI::ParsePin(const string& input, std::pair<string, std::size_t>& pair)
{
	if (input[input.size() - 1] != ']')
	{
		log << "Wrong format of pin missing \"]\" : " + input;
		return false;
	}
	std::size_t i = input.find_first_of('[');
	if (i == input.npos)
	{
		log << "Wrong format of pin missing \"[\" : " + input;
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

bool WorkbenchTUI::nameSizeCheck(const string& name)
{
	return nameSizeCheck(name.size());
}

bool WorkbenchTUI::nameSizeCheck(std::size_t size)
{
	if (size > maxSizeOfTag)
	{
		log << "Gate name is too long (" + to_string(size) + ")" + " max=" + std::to_string(maxSizeOfTag) << endl;
		return false;
	}
	return true;
}

bool WorkbenchTUI::nameCharsCheck(const string& name)
{
	if (any_of(name.begin(), name.end(), [this](char c) {
		return any_of(forbidenChars.begin(), forbidenChars.end(),
			[c](char  fc) { return fc == c; });
	}))
	{
		log << "Name contains one of the forbidden chars \"" + name + "\"";
		return false;
	}
	else return true;

}

std::vector<string> WorkbenchTUI::Split(const std::string& s, char delimeter, StringSplitOption option)
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

std::string WorkbenchTUI::ToUpper(const std::string& s)
{
	string o;
	o.resize(s.size());
	std::locale loc;
	std::transform(s.begin(), s.end(), o.begin(), [loc](char c) {return toupper(c,loc); });
	return std::move(o);
}

string WorkbenchTUI::boolsToString(const std::vector<bool>& v)
{
	string s;
	transform(v.cbegin(), v.cend(), std::inserter(s, s.begin()), [](bool c)
	{
		if (c) return '1'; else return '0';
	});

	return std::move(s); 
}

bool WorkbenchTUI::stringToBools(const string& s, std::vector<bool>& b)
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
		log << "Wrong format of input: " << s << endl;
		return false;
	}
	return true;
}

bool WorkbenchTUI::ParseKeyWords(const string& word)
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

bool WorkbenchTUI::ShowHelp(const string& word)
{
	if(word == "help" || word == "h" || word == "man")
	{
		output << "\t EXIT: \"exit\" ; \"e\", RESET: \"reset\";\"r\",CONSTR: \"const\", \"c\" \n";
		return true;
	}
	return false;
}


 