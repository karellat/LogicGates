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
	//Copy constructor
	//
	
	WorkbenchStatus status = UnderConstruction; 
	
	//Output
	std::vector<std::string> ListOfFreeInputGates(); 
	std::vector<std::string> ListOfFreeOutputGates(); 
	std::vector<std::string> ListOfUserDefinedGates(); 
	std::vector<std::string> ListOfStandartGates(); 
	std::size_t SizeOfInput();
	std::size_t SizeOfOutput();
	//Actions while constructing
	bool Connect(const std::size_t & freeInputPosition, const std::size_t & freeOutputPosition); 
	bool Add(const std::size_t & num ); 
	bool AddUserDefineGate(const std::size_t & positionInList); 
	bool ConstructBench(); 
	bool AddInputGate(); 
	bool AdddOutputGate(); 
	//Actions while constructed
	bool SetInput(std::vector<bool> input); 
	std::vector<bool> ReadOutput(); 
	bool ConstructUserGate(); 
	void ErrorBuffer(const std::size_t & numOfError);
	
		
protected:
	std::vector<InputGate *> InputGates; 
	std::vector<OutputGate *>   OutputGates; 
	std::vector<Gate *>   FreeOutputPins; 
	std::vector<Gate *>   FreeInputPins; 
	std::vector<std::unique_ptr<UserDefinedGate>>   UserDefinedGates;
	std::vector <std::unique_ptr<Gate>> Gates; 
	bool TestOfCorrection(); 


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
	//Make specific errors 
	std::string Error(); 


};