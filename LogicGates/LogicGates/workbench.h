#pragma once
#include "gates.h"
#include <memory>
#include <sstream>
#include <list>

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

//class WorkbenchTUI
//{
//public:
//	WorkbenchTUI();
//	WorkbenchTUI(string outputFile, string inputFile);
//	WorkbenchTUI(string outputFile);
//	~WorkbenchTUI();
//	void Init();
//	void Start();
//
//protected:
//	unique_ptr<Workbench> actualWorkBench;
//	unique_ptr<ostringstream> output;
//	unique_ptr<istringstream> input;
//	string ListFreeOutputPins();
//	string ListFreeInputPins();
//	std::size_t ParseInput(string input);
//	// While workbench underconstruction 
//	void CreateNewSignal();
//	void CreateNewGate();
//	void CreateUserDefinedGate();
//	void ConnectFreeInput();
//	void ConnectFreeOutput();
//	void Construct();
//	//While workbench Constructed
//	void SetInput();
//	void CheckCalculation();
//	void ReadOutput();
//};
