#pragma once
#include "gates.h"
#include <queue>
#include <memory>
#include <tuple>
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
	std::vector<std::string> ListOfFreeInputGates();
	std::vector<std::string> ListOfFreeOutputGates();
	std::vector<std::string> ListOfUserDefinedGates();
	std::vector<std::string> ListOfStandartGates();
	std::size_t SizeOfInput() const;
	std::size_t SizeOfOutput() const;
	//Actions while constructing
	bool Connect(const std::size_t & freeInputPosition, const std::size_t & freeOutputPosition);
	bool Add(const std::size_t & num);
	bool AddUserDefineGate(const std::size_t & positionInList);
	bool ConstructBench();
	bool AddInputGate();
	bool AdddOutputGate();
	//Actions while constructed
	bool SetInput(std::vector<bool> input);
	std::vector<bool> ReadOutput();
	bool ConstructUserGate();


protected:
	std::vector<InputGate *> InputGates;
	std::vector<OutputGate *>   OutputGates;
	std::vector<Gate *>   FreeOutputPins;
	std::vector<Gate *>   FreeInputPins;
	std::vector<std::unique_ptr<UserDefinedGate>>   UserDefinedGates;
	std::vector <std::unique_ptr<Gate>> Gates;
	bool TestOfCorrection();
	std::vector<string> StandardGate;
	void StatusCheck(WorkbenchStatus s) const;

};


inline  std::vector<std::string> Workbench::ListOfStandartGates() 
{
	return StandardGate;
}

class WorkbenchStatusException : public runtime_error
{
public:
	WorkbenchStatusException(WorkbenchStatus actualStatus, WorkbenchStatus wantedStatus) :runtime_error("Workbench is actualy in status = " + to_string(actualStatus) + ",but these operation require " + to_string(wantedStatus)){}
};
class WorkbenchTUI
{
public:
	WorkbenchTUI();
	WorkbenchTUI(std::string outputFile, std::string inputFile);
	WorkbenchTUI(std::string outputFile);
	~WorkbenchTUI();
	void Init();
	void Start();

protected:
	std::unique_ptr<Workbench> actualWorkBench;
	std::unique_ptr<std::ostringstream> output;
	std::unique_ptr<std::istringstream> input;
	std::string ListFreeOutputPins();
	std::string ListFreeInputPins();
	std::size_t ParseInput(std::string input);
	// While workbench underconstruction 
	void CreateNewSignal();
	void CreateNewGate();
	void CreateUserDefinedGate();
	void ConnectFreeInput();
	void ConnectFreeOutput();
	void Construct();
	//While workbench Constructed
	void SetInput();
	void CheckCalculation();
	void ReadOutput();
	


};
