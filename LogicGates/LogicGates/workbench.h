#pragma once
#include "gates.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <numeric>
#include <assert.h>
#include <ostream>
static class freepin : exception
{
public:
	const char* what() const throw() override
	{
		return "Constructing gate with free pin";
	}
} fpin;

static class cycledetected :exception
{
public:
	const char* what() const throw() override
	{
		return "Cycle detected during construction";
	}
} dcycle;

static class notavailabepart :exception
{
public:
	const char* what() const throw() override
	{
		return "Disconnected part from the rest of the logical network";
	}
} npart;

static class invalidworkbenchstatus : public exception
{
public:
	const char* what() const throw() override
	{
		return "Unexpected status";
	}
} istat;

static class unexistingtypename : public exception
{
public:
	const char* what() const throw() override
	{
		return "Unexisting typename";
	}
} utype;

static class invalidnameoftype : public exception
{
public:
	const char* what() const throw() override
	{
		return "Choosen type name exists";
	}
}itype;

static class invalidnameofvertex : public exception
{
public:
	const char* what() const throw() override
	{
		return "Same name of vertex exists";
	}
} iname;

static class unexistingnameofvertex : public exception
{
public:
	const char* what() const throw() override
	{
		return "Unexisting vertex name";
	}
} uname;

static class occupiedpin : public exception
{
public:
	const char* what() const throw() override
	{
		return "Connected pin";
	}
} opin;

static class invalidsizeofio : public exception
{
public:
	const char* what() const throw() override
	{
		return "Input and Output are set to 0, no point of creating that gate";
	}
} isize;

static class invalidinputformat : public exception
{
public:
	const char* what() const throw() override
	{
		return "Input must have format 1 or 0";
	}
} iinput;
enum WorkbenchStatus
{
	UnderConstruction, Constructed, Calculating, Calculated
};
class Workbench
{
public:

	Workbench(size_t inputSize, size_t outputSize,streambuf* log,bool loging);
	WorkbenchStatus status;

	//Names for user output
	const unique_ptr<vector<string>> ListOfNamedVertex() const;
	const unique_ptr<vector<string>> ListOfType() const;
	const std::size_t SizeOfInput() const { return inputGate->GetLengthOfOutput();}
	const std::size_t SizeOfOutput() const { return outputGate->GetLengthOfInput(); }
	//Actions while constructing
	//Add new vertex of typeName called name 
	void Add(const std::string& name, const std::string& typeName);
	void Connect(const std::string& fromName, std::size_t fromPin, const std::string& toName, std::size_t toPin);
	//Construction: 
	void ConstructBench();
	//Actions while constructed
	// I/0 set up: 
	void SetInput(const vector<bool>& input);
	vector<bool> ReadOutput() const;

	void ConstructUserGate(const string& name, size_t newInputSize, size_t newOutputSize); 
	const string& GetTestOutput() const { return testOutput; }
	//delete current logic network 
	void ResetWorkbench(bool deleteUDG, size_t newInputSize, size_t newOutputSize);

protected:
	//loging info
	bool loging; 
	std::ostream log; 
	//Input & Output Gate
	unique_ptr<Gate> inputGate; 
	unique_ptr<Gate> outputGate;
	gvertex inputVertex; 
	gvertex outputVertex; 

	//Graph
	unique_ptr<Graph<Gate*, unique_ptr<Signal>>> graph;
	//Dictionary Key: name of the gate type Value: index in the gateTypes
	unordered_map<string, unique_ptr<Gate>> gateTypes;
	vector<gvertex>   constGates;
	//Gates with unconnected pins 
	vector<gvertex>   freeOutputGates;
	vector<gvertex>   freeInputGates;
	//Unconnected pins 
	unordered_map<gvertex, unordered_set<size_t>> unconnectedToPins; 
	unordered_map<gvertex, unordered_set<size_t>> unconnectedFromPins;
	//Names of vertex, name must be unique
	std::unordered_map<string, gvertex> vertexNames;
	string testOutput;
	//Check state, throws istat
	void CheckState(WorkbenchStatus s) const;
	void CheckState(WorkbenchStatus a, WorkbenchStatus b) const;
	//loging methods 
	static string boolsToString(const vector<bool>&  bools); 
	string logEdge(const gedge& edge) const;
	string logVertexInput(const gvertex& vertex,const vector<bool>& bools);
	string logVertexOutput(const gvertex& vertex, const vector<bool>& bools);
};


