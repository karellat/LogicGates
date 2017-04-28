#pragma once
#include <memory>
#include <vector>
#include "graph.h" 



class Gate;
class Signal; 
using gvertex = Vertex<Gate*>*; 
using gedge = Edge<Gate*,std::unique_ptr<Signal>>*; 
enum Status
{
	Zero, One, Floating
};

class Signal
{
public:
	Signal(Status s, size_t to_id, size_t from_id) : status(s), toID(to_id), fromID(from_id) {}; 
	~Signal(){}
	Status status;
	size_t toID;
	size_t fromID;
private:

};



class Gate {
public:
	Gate(){}
	Gate(size_t inputSize, size_t outputSize, std::string name) 
	: result(false), input_size(inputSize), output_size(outputSize),name(name)
	{
		resultValues.resize(outputSize);
	}
	virtual ~Gate(){}
	virtual std::vector<bool> Update(std::vector<bool> input) = 0; 
	size_t GetLengthOfInput() const { return input_size; }
	size_t GetLengthOfOutput() const { return output_size; }
	//Logging info 
	std::string Name() const { return name; };
	bool result;
	std::vector<bool> resultValues; 
	void  Reset() { result = false; }
protected:
	//std::vector<std::unique_ptr<Signal>> output;
	size_t input_size; 
	size_t output_size; 
	//Logging info
	std::string name; 
};

class InputGate : public Gate
{
public:
	InputGate(std::size_t inputSize) : Gate(0, inputSize, "INPUT"){}
	~InputGate() {}

	std::vector<bool> Update(std::vector<bool> input) override
	{
		result = true; 
		resultValues = input; 
		return input;
	}
protected:
	InputGate(string name) : Gate(0, 1, name) {}
};
class OutputGate : public Gate
{
public:
	OutputGate(std::size_t outputSize) : Gate(outputSize,0,"OUTPUT"){}
	~OutputGate(){}

	std::vector<bool> Update(std::vector<bool> input) override 
	{ 
		result = true; 
		resultValues = input; 
		return input; 
	}
};

class BlankGate : public Gate
{
public:
	BlankGate() : Gate(1, 0, "BLANK") {}
	~BlankGate(){}

	std::vector<bool> Update(std::vector<bool> input) override
	{
		return input; 
	}
};

class ConstGate0 : public InputGate
{
public:
	ConstGate0() : InputGate("CONST0")
	{
		result = true;
		resultValues[0] = false; 
	}
	~ConstGate0(){}
	std::vector<bool> Update(std::vector<bool> input) override { return resultValues; }

};

class ConstGate1 : public InputGate
{
public: 
	ConstGate1() : InputGate("CONST1") 
	{ 
		result = true;
		resultValues[0] = true;
	}
	~ConstGate1(){}
	std::vector<bool> Update(std::vector<bool> input) override { return resultValues; }
};

class NotGate : public Gate
{
public:
	NotGate() : Gate(1,1,"NOT"){} 
	~NotGate(){}

	std::vector<bool> Update(std::vector<bool> input) override 
	{ 
		result = true; 
		resultValues[0] = !input[0];
		return resultValues; 
	}

};

class OrGate : public Gate
{
public: 
	OrGate() : Gate(2,1,"OR") {} 
	~OrGate(){}

	std::vector<bool> Update(std::vector<bool> input) override
	{
		result = true; 
		resultValues[0] = input[0] || input[1];
		return resultValues; 
	}
};

class AndGate : public Gate
{
public:
	AndGate() : Gate(2,1,"AND") {} 
	~AndGate(){}

	std::vector<bool> Update(std::vector<bool> input) override
	{
		result = true; 
		resultValues[0] = input[0] && input[1]; 
		return resultValues; 
	}
};

class XorGate : public Gate
{
public:
	XorGate() : Gate(2,1,"XOR") {} 
	~XorGate(){}

	std::vector<bool> Update(std::vector<bool> input) override {
		result = true; 
		resultValues[0] = (input[0] || input[1]) && !(input[0] && input[1]);
		return resultValues; 
	}
};

class NandGate : public Gate
{
public:
	NandGate() : Gate(2,1,"NAND") {} 
	~NandGate(){}

	std::vector<bool> Update(std::vector<bool> input) override 
	{
		result = true; 
		resultValues[0] = !(input[0] && input[1]);
		return resultValues; 
	}
};

class NorGate : public Gate
{
public:  NorGate() : Gate(2,1,"NOR") {} 
		 ~NorGate(){}

	std::vector<bool> Update(std::vector<bool> input) override
		 {
			 result = true; 
			 resultValues[0] = !(input[0] || input[1]);
			 return resultValues; 
		 }
};

class XnorGate : public Gate
{
public: XnorGate():Gate(2,1,"XNOR"){}
		~XnorGate(){}

	std::vector<bool> Update(std::vector<bool> input) override
		{
			result = true; 
			resultValues[0] = !((input[0] || input[1]) && !(input[0] && input[1])); 
			return resultValues; 
		}

};

class DoubleGate : public Gate
{
public: DoubleGate() :Gate(1,2,"DOUBLE"){}
		~DoubleGate(){}
	std::vector<bool> Update(std::vector<bool> input) override
	{
		result = true; 
		resultValues[0] = input[0];
		resultValues[1] = input[0];
		return resultValues;
	}
};



class UserDefinedGateModel : public Gate
{
public: 
	UserDefinedGateModel(std::unique_ptr<Graph<Gate*, unique_ptr<Signal>>> graph, std::unique_ptr<Gate> inputGate, gvertex inputVertex,
		std::unique_ptr<Gate> outputGate, gvertex outputVertex,vector<gvertex> constGates,string name) 
		: Gate(inputGate->GetLengthOfOutput(),outputGate->GetLengthOfInput(),name),graph(std::move(graph)), inputGate(std::move(inputGate)),inputVertex(inputVertex)
	, outputGate(std::move(outputGate)),outputVertex(outputVertex), constGates(std::move(constGates)){} 

	std::vector<bool> Update(std::vector<bool> input) override;

protected:
	std::unique_ptr<Graph<Gate*, std::unique_ptr<Signal>>> graph;
	std::unique_ptr<Gate> inputGate; 
	gvertex inputVertex;
	std::unique_ptr<Gate> outputGate;
	gvertex outputVertex; 
	std::vector<gvertex> constGates; 

};