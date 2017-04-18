#pragma once
#include <memory>
#include <vector>
#include "graph.h" 

using namespace std;

class Gate;
class Signal; 
using gvertex = Vertex<unique_ptr<Gate>>*;
using gedge = Edge<unique_ptr<Gate>,unique_ptr<Signal>>*; 
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
	Gate(size_t inputSize, size_t outputSize, string name, size_t id) 
	: result(false), input_size(inputSize), output_size(outputSize),id(id),name(name)
	{
		resultValues.resize(outputSize);
	}
	virtual ~Gate(){}
	virtual vector<bool> Update(vector<bool> input) = 0; 
	size_t GetLengthOfInput() const { return input_size; }
	size_t GetLengthOfOutput() const { return output_size; }
	//Logging info 
	size_t Id() const { return id; };
	string Name() const { return name; };
	bool result; 
	vector<bool> resultValues; 
protected:
	//std::vector<std::unique_ptr<Signal>> output;
	size_t input_size; 
	size_t output_size; 
	//Logging info
	size_t id; 
	string name; 
};

class InputGate : public Gate
{
public:
	InputGate(size_t ID) : Gate(0, 1, "Input Gate", ID){}
	~InputGate() {}
	void Set(Status s)
	{
		result = true;
		if (s == One) resultValues[0] = true;
		else if (s == Zero) resultValues[0] = false;
		else result = false;
	}
	vector<bool> Update(vector<bool> input) override
	{
		result = true; 
		resultValues = input; 
		return input;
	}
};


class OutputGate : public Gate
{
public:
	OutputGate(size_t ID) : Gate(1,0,"Output Gate",ID){}
	~OutputGate(){}
	vector<bool> Update(vector<bool> input) override 
	{ 
		result = true; 
		resultValues = input; 
		return input; 
	}
};

class BlankGate : public Gate
{
public:
	BlankGate(size_t ID) : Gate(1, 0, "Blank Gate", ID) {}
	~BlankGate(){}
	vector<bool> Update(vector<bool> input) override
	{
		return input; 
	}
};

class ConstGate0 : public InputGate
{
public:
	ConstGate0(size_t ID) : InputGate(ID)
	{
		result = true;
		resultValues[0] = false; 
	}
	~ConstGate0(){}
	vector<bool> Update(vector<bool> input) override { return resultValues; }

};

class ConstGate1 : public InputGate
{
public: 
	ConstGate1(size_t ID) : InputGate(ID) { result = true; resultValues[0] = true; }
	~ConstGate1(){}
	vector<bool> Update(vector<bool> input) override { return resultValues; }
};

class NotGate : public Gate
{
public:
	NotGate(size_t ID) : Gate(1,1,"Not Gate", ID){} 
	~NotGate(){}
	vector<bool> Update(vector<bool> input) override 
	{ 
		result = true; 
		resultValues[0] = !input[0];
		return resultValues; 
	}

};

class OrGate : public Gate
{
public: 
	OrGate(size_t ID) : Gate(2,1,"Or Gate", ID) {} 
	~OrGate(){}
	vector<bool> Update(vector<bool> input) override
	{
		result = true; 
		resultValues[0] = input[0] || input[1];
		return resultValues; 
	}
};

class AndGate : public Gate
{
public:
	AndGate(size_t ID) : Gate(2,1,"And Gate",ID) {} 
	~AndGate(){}
	vector<bool> Update(vector<bool> input) override
	{
		result = true; 
		resultValues[0] = input[0] && input[1]; 
		return resultValues; 
	}
};

class XorGate : public Gate
{
public:
	XorGate(size_t ID) : Gate(2,1,"Xor Gate",ID) {} 
	~XorGate(){}
	vector<bool> Update(vector<bool> input) override {
		result = true; 
		resultValues[0] = (input[0] || input[1]) && !(input[0] && input[1]);
		return resultValues; 
	}
};

class NandGate : public Gate
{
public:
	NandGate(size_t ID) : Gate(2,1,"Nand Gate",ID) {} 
	~NandGate(){}
	vector<bool> Update(vector<bool> input) override 
	{
		result = true; 
		resultValues[0] = !(input[0] && input[1]);
		return resultValues; 
	}
};

class NorGate : public Gate
{
public:  NorGate(size_t ID) : Gate(2,1,"Nor Gate",ID) {} 
		 ~NorGate(){}
		 vector<bool> Update(vector<bool> input) override
		 {
			 result = true; 
			 resultValues[0] = !(input[0] && input[1]);
			 return resultValues; 
		 }
};

class XnorGate : public Gate
{
public: XnorGate(size_t ID):Gate(2,1,"Xnor Gate",ID){}
		~XnorGate(){}
		vector<bool> Update(vector<bool> input) override
		{
			result = true; 
			resultValues[0] = !((input[0] || input[1]) && !(input[0] && input[1])); 
			return resultValues; 
		}

};

class UserDefinedGate : public Gate
{
public:
	UserDefinedGate(size_t input_size, size_t output_size,string name, size_t id): Gate(input_size,output_size,name, id){}
	~UserDefinedGate() {}

	vector<bool> Update(vector<bool> input) override { throw new runtime_error("Not Implemented yet, UserDefinedGate::Update"); }
};

class UserDefinedGateModel : public Gate
{
public: 
	UserDefinedGateModel(unique_ptr<Graph<unique_ptr<Gate>, unique_ptr<Signal>>> graph, vector<gvertex> InputGates, vector<gvertex> OutputGates, string name, size_t id)
		: Gate(InputGates.size(), OutputGates.size(),name,id) {}
	~UserDefinedGateModel() {throw new runtime_error("Not Implemented yet, UserDefinedGate::destructor"); }

	unique_ptr<UserDefinedGate> getGate(size_t id) { throw new runtime_error("Not Implemented yet, UserDefinedGateModel::getGate"); }
	vector<bool> Update(vector<bool> input) override { throw new runtime_error("Not Implemented yet, UserDefinedGateModel::Update"); }
};