#pragma once
#include "workbench.h"

class WorkbenchTUI
{
public:
	WorkbenchTUI(streambuf* output, streambuf* input) : output(output), input(input)
	{
		workbench = make_unique<Workbench>();
	}
	~WorkbenchTUI();
	void WaitForFile();
	bool ReadFile(string path);
	void InteraktiveMode();
	void InteraktiveMode(string path);
	void PassiveMode(string path, string inputSettings);
protected:
	enum StringSplitOption
	{
		RemoveEmptyEntries,
		None
	};
	void InteractiveSeting();
	bool InteractiveReadingFile();
	bool SetInput(vector<bool> inputSettings);
	bool ReadOutputs(vector<bool>& outputValue);
	std::vector<string> Split(std::string s, char delimeter, StringSplitOption option);
	std::string ToUpper(std::string s);
	std::ostream output;
	std::istream input;
	std::ifstream inputFile;
	std::unique_ptr<Workbench> workbench;
	bool nameSizeCheck(string name);
	bool nameSizeCheck(std::size_t size);
	bool nameCharsCheck(string name);
	bool OpenFile(std::string path);
	string definitionTag = "#GATE";
	string connectionTag = "#CONNECT";
	std::size_t maxSizeOfTag = 40;
	std::string forbidenChars = "\n\t ";
	bool ParsePin(string input, std::pair<string, std::size_t>& pair);
	bool  boolsToString(std::vector<bool> v, string& s);
	bool stringToBools(string s, std::vector<bool>& b);
};