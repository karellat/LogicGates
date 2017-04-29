#pragma once
#include "gates.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <numeric>


enum WorkbenchStatus
{
	UnderConstruction, Constructed, Calculating, Calculated
};
class Workbench
{
public:

	Workbench(size_t inputSize, size_t outputSize);
	WorkbenchStatus status;

	//Names for user output
	const unique_ptr<vector<string>> ListOfNamedVertex() const;
	const unique_ptr<vector<string>> ListOfType() const;
	const std::size_t SizeOfInput() const { return inputGate->GetLengthOfOutput();}
	const std::size_t SizeOfOutput() const { return outputGate->GetLengthOfInput(); }
	//Actions while constructing
	//Add new vertex of typeName called name 
	void Add(std::string name, std::string typeName);
	void Connect(std::string fromName, std::size_t fromPin, std::string toName, std::size_t toPin);
	//Construction: 
	bool ConstructBench();


	//Actions while constructed
	// I/0 set up: 
	void SetInput(vector<bool> input);
	vector<bool> ReadOutput() const;

	void ConstructUserGate(string name, size_t newInputSize, size_t newOutputSize); 
	const string& GetTestOutput() const { return testOutput; }
	//delete current logic network 
	void ResetWorkbench(bool deleteUDG, size_t newInputSize, size_t newOutputSize);

protected:
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
	unordered_map<gvertex, unordered_set<size_t>> unconnectedInPins; 
	unordered_map<gvertex, unordered_set<size_t>> unconnectedOutPins;
	//Names of vertex, name must be unique
	std::unordered_map<string, gvertex> vertexNames;
	string testOutput;

};

class invalidworkbenchstatus : public exception
{
public:
	virtual const char* what() const throw()
	{
		return "Unexpected status";
	}
} istat;

class unexistingtypename : public exception
{
public:
	virtual const char* what() const throw()
	{
		return "Unexisting typename";
	}
} utype;

class invalidnameoftype : public exception
{
public: 
	virtual const char* what() const throw()
	{
		return "Choosen type name exists";
	}
}itype;

class invalidnameofvertex : public exception
{
public: 
	virtual const char* what() const throw()
	{
		return "Same name of vertex exists";
	}
} iname;

class unexistingnameofvertex : public exception
{
public: 
	virtual const char* what() const throw()
	{
		return "Unexisting vertex name";
	}
} uname;

class occupiedpin : public exception
{
public: 
	virtual const char* what() const throw()
	{
		return "Connected pin";
	}
} opin;

class invalidsizeofio : public exception
{
public:
	virtual const char* what() const throw()
	{
		return "Input and Output are set to 0, no point of creating that gate";
	}
} isize;

class invalidinputformat : public exception
{
public:
	virtual const char* what() const throw()
	{
		return "Input must have format 1 or 0";
	}
} iinput; 