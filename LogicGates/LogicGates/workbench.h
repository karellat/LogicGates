#pragma once
#include "gates.h"
#include <memory>
#include <sstream>
#include <list>
#include <fstream>

enum WorkbenchStatus
{
	UnderConstruction, Constructed, Calculating, Calculated
};
class Workbench
{
public:

	Workbench();
	~Workbench();

	WorkbenchStatus status = UnderConstruction;

	//Output
	vector<string> ListOfFreeInputGates();
	vector<string> ListOfFreeOutputGates();
	vector<string> ListOfUserDefinedGates();
	const vector<string> & ListOfStandartGates() const;
	std::size_t SizeOfInput() const { return InputGates.size(); }
	std::size_t SizeOfOutput() const { return OutputGates.size(); }
	//Actions while constructing
	bool Connect(const std::size_t & freeInputPosition,const std::size_t & freeInputID,
		const std::size_t & freeOutputPosition, const std::size_t & freeOutputID);
	bool Add(const std::size_t & num);
	bool AddUserDefineGate(const std::size_t & positionInList);
	bool ConstructBench();
	bool AddInputGate();
	bool AdddOutputGate();
	//Actions while constructed
	bool SetInput(vector<bool> input);
	vector<bool> ReadOutput();
	bool ConstructUserGate(string name);
	vector<string> ListAllGates(); 


protected:
	unique_ptr<Graph<unique_ptr<Gate>, unique_ptr<Signal>>> graph;
	vector<gvertex>   InputGates;
	vector<gvertex>   OutputGates;
	vector<gvertex>   freeOutputGates;
	vector<gvertex>   freeInputGates; 
	vector<unique_ptr<UserDefinedGateModel>> UserDefinedGates;
	bool TestOfCorrection();
	vector<string> StandardGate;
	void StatusCheck(WorkbenchStatus s) const;
	void StatusCheck(vector<WorkbenchStatus> s) const;
	vector<std::size_t> freeInputPins(gvertex v) const; 
	vector<std::size_t> freeOutputPins(gvertex v) const;
	std::size_t GetNewID()
	{
		lastID++; 
		return (lastID - 1); 
	}
	std::size_t lastID; 
};


inline const vector<string> & Workbench::ListOfStandartGates() const
{ 
	return StandardGate;
}

class WorkbenchStatusException : public runtime_error
{
public:
	WorkbenchStatusException(WorkbenchStatus actualStatus, WorkbenchStatus wantedStatus) 
	:runtime_error("Workbench is actualy in status = " + to_string(actualStatus) + ",but these operation requires " + to_string(wantedStatus)){}	
	//TODO: Something more explicit
	WorkbenchStatusException(WorkbenchStatus actualStatus)
		:runtime_error("Workbench is actualy in status = " + to_string(actualStatus) + ",but these operation requires different status"){} 
};



class WorkbenchTUI
{
public:
	WorkbenchTUI(streambuf* output, streambuf* input) : output(output),input(input)
	{
		workbench = make_unique<Workbench>();
	}
	~WorkbenchTUI();
	void WaitForFile(); 
	bool ReadFile(string path);
	void InteraktiveMode();

protected:
	enum StringSplitOption
	{
		RemoveEmptyEntries,
		None
	};
	std::vector<string> Split(std::string s,char delimeter, StringSplitOption option);
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
	std::size_t maxSizeOfTag = 40;
	std::string forbidenChars = "\n\t ";
};
