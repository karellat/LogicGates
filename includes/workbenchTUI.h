#pragma once
#include "workbench.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <locale>

class WorkbenchTUI
{
public:
	WorkbenchTUI(std::streambuf* output, std::streambuf* input, streambuf* log) : output(output), input(input), log(log), exiting(false), reseting(false), constructing(false), readyForConstruction(false)
	{
	}

	~WorkbenchTUI()
	{
		output.flush();
		log.flush();
	}

	void InteraktiveMode();
	
	void PassiveMode(const vector<string>& filePaths, const vector<vector<bool>>& inputSet,bool tryAllInputs);
protected:
	bool logTag = true;
	enum StringSplitOption
	{
		RemoveEmptyEntries,
		None
	};
	//Read construction file, fill  the log file, if any problem with construction return false
	bool ReadFile(const string& path);
	//Interactive mode of setting inputs to gate
	void InteractiveSeting();
	//Interactive mode of reading and constructing construction files 
	bool InteractiveReadingFile();
	//Set input of the logical network 
	bool SetInput(const vector<bool>& inputSettings);
	//Reads output from network
	bool ReadOutputs(vector<bool>& outputValue) const;
	//Check if file exists and open it
	bool OpenFile(const std::string& path);

	//File reading parse 
		//Reads the first three line (definition tag,input & output line)
	bool ReadDefinitonHeader();
		//Set name to given type of gate, add the gate to the workbench, ends correctly with connection part tag if any problem of naming or format problem 
	bool ReadDeclarativeLine();
		//Set connection between gates in the workbench, ends corretly with  end tag, return false if any problem of connection or format problem 
	bool ReadConnectionLine(); 

	//Construction 
		//Construct user defined Gate
	bool ConstructGate(size_t newInputSize, size_t newOutputSize, const string& actualName);
	//Generate all possible inputs
	vector<vector<bool>> GenerateInput(const size_t size_of_input);


	// Streams: 
	std::ostream output;
	std::ostream log;
	std::istream input;
	std::ifstream inputFile;
	//workbench
	std::unique_ptr<Workbench> workbench;
	string name;
	//String parsing 
	bool nameSizeCheck(const string& name);
	bool nameSizeCheck(std::size_t size);
	bool nameCharsCheck(const string& name);
	string definitionTag = "#GATE";
	string connectionTag = "#CONNECT";
	std::size_t maxSizeOfTag = 40;
	std::string forbidenChars = "\n\t ";
	string inputTag = "#INPUT";
	string outputTag = "#OUTPUT"; 
	std::vector<string> Split(const std::string& s, char delimeter, StringSplitOption option);
	std::string ToUpper(const std::string& s);
	bool ParsePin(const string& input, std::pair<string, std::size_t>& pair);
	string  boolsToString(const std::vector<bool>& v);
	bool stringToBools(const string& s, std::vector<bool>& b);
	bool ShowHelp(const string& word);	
	bool ParseKeyWords(const string& word); 
	//status tag 
	bool exiting;
	bool reseting;
	bool constructing;
	bool readyForConstruction;
};
