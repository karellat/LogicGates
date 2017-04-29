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
	void PassiveMode(vector<string> filePaths, vector<vector<bool>> inputSet);
protected:
	bool logTag = true;
	enum StringSplitOption
	{
		RemoveEmptyEntries,
		None
	};
	//Read construction file, fill  the log file, if any problem with construction return false
	bool ReadFile(string path);
	//Interactive mode of setting inputs to gate
	void InteractiveSeting();
	//Interactive mode of reading and constructing construction files 
	bool InteractiveReadingFile();
	//Set input of the logical network 
	bool SetInput(vector<bool> inputSettings);
	//Reads output from network
	bool ReadOutputs(vector<bool>& outputValue);
	//Check if file exists and open it
	bool OpenFile(std::string path);

	//File reading parse 
		//Reads the first three line (definition tag,input & output line)
	bool ReadDefinitonHeader();
		//Set name to given type of gate, add the gate to the workbench, ends correctly with connection part tag if any problem of naming or format problem 
	bool ReadDeclarativeLine();
		//Set connection between gates in the workbench, ends corretly with  end tag, return false if any problem of connection or format problem 
	bool ReadConnectionLine(); 

	//Construction 
		//Construct user defined Gate
	bool ConstructGate(size_t newInputSize, size_t newOutputSize, string actualName);

	// Streams: 
	std::ostream output;
	std::ostream log;
	std::istream input;
	std::ifstream inputFile;
	//workbench
	std::unique_ptr<Workbench> workbench;
	string name;
	//String parsing 
	bool nameSizeCheck(string name);
	bool nameSizeCheck(std::size_t size);
	bool nameCharsCheck(string name);
	string definitionTag = "#GATE";
	string connectionTag = "#CONNECT";
	std::size_t maxSizeOfTag = 40;
	std::string forbidenChars = "\n\t ";
	string inputTag = "#INPUT";
	string outputTag = "#OUTPUT"; 
	std::vector<string> Split(std::string s, char delimeter, StringSplitOption option);
	std::string ToUpper(std::string s);
	bool ParsePin(string input, std::pair<string, std::size_t>& pair);
	string  boolsToString(std::vector<bool>& v);
	bool stringToBools(string s, std::vector<bool>& b);
	bool ShowHelp(string word);	
	bool ParseKeyWords(string word); 
	//status tag 
	bool exiting;
	bool reseting;
	bool constructing;
	bool readyForConstruction;
};
