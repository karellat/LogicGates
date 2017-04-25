#pragma once
#include "gates.h"
#include <memory>
#include <sstream>
#include <list>
#include <fstream>
#include <string>
#include <set>
#include <iterator>
#include <numeric>
#include <map>
#include <unordered_map>
#include <locale>

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
	vector<string> ListOfNamedVertex();
	const vector<string> & ListOfStandartGates() const;
	std::size_t SizeOfInput() const { return InputGates.size(); }
	std::size_t SizeOfOutput() const { return OutputGates.size(); }
	//Actions while constructing
	bool Connect(const std::size_t & freeInputPosition,const std::size_t & freeInputID,
		const std::size_t & freeOutputPosition, const std::size_t & freeOutputID);
	bool Connect(gvertex from, gvertex to, std::size_t fromPin, std::size_t toPin);
	gvertex Add(const std::size_t & num);
	gvertex GetType(string typeName);
	bool GetVertex(string name, gvertex& v);
	bool AddNamedGate(string gateName, string typeName);
	bool AddUserDefineGate(const std::size_t & positionInList);
	bool ConstructBench();
	gvertex AddInputGate();
	gvertex AddOutputGate();
	//Actions while constructed
	bool SetInput(vector<bool> input);
	vector<bool> ReadOutput();
	bool ConstructUserGate(string name);
	vector<string> ListAllGates() const; 
	string GetTestOutput() const { return testOutput; }
	void ResetWorkbench(bool deleteUDG);

protected:
	gvertex AddConstGate(bool c);
	unique_ptr<Graph<unique_ptr<Gate>, unique_ptr<Signal>>> graph;
	vector<gvertex>   InputGates;
	vector<gvertex>   OutputGates;
	vector<gvertex>   ConstGates;
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
	std::unordered_map<string, gvertex> VertexNames;
	std::unordered_map<string, int> TypeNames; 
	string testOutput;

	
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

	WorkbenchStatusException(WorkbenchStatus actualStatus)
		:runtime_error("Workbench is actualy in status = " + to_string(actualStatus) + ",but these operation requires different status"){} 
};



