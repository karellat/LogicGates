#pragma once
#include <memory>
#include <vector>
#include "graph.h" 

using namespace std;

class Gate;
class Signal; 
using gvertex = Vertex<unique_ptr<Gate>>*;
using gedge = Edge<Gate*,unique_ptr<Signal>>*; 
enum Status
{
	Zero, One, Floating
};

class Signal
{
public:
	Signal(Status s, size_t to_id, size_t from_id) : status(s), toID(to_id), fromID(from_id) {}; 
	~Signal();
	Status status;
	size_t toID;
	size_t fromID;
private:

};



class Gate {
public:
	Gate(size_t inputSize, size_t outputSize, string name, size_t id) 
	: input_size(inputSize), output_size(outputSize), id(id),name(name) {}
	virtual ~Gate();
	virtual vector<bool> Update(vector<bool> input) = 0; 
	size_t GetLengthOfInput() const { return input_size; }
	size_t GetLengthOfOutput() const { return output_size; }
	//Logging info 
	size_t Id() const { return id; };
	string Name() const { return name; };
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
	InputGate(size_t ID); 
	~InputGate();
	void Set(Status s);
	vector<bool> Update(vector<bool> input) override; 
};


class OutputGate : public Gate
{
public:
	OutputGate(size_t ID);
	~OutputGate();
	vector<bool> Update(vector<bool> input) override;
};

class BlankGate : public Gate
{
public:
	BlankGate(size_t ID); 
	~BlankGate();
	vector<bool> Update(vector<bool> input) override;
};

class ConstGate0 : public InputGate
{
public: 
	ConstGate0(size_t ID);
	~ConstGate0();
	vector<bool> Update(vector<bool> input) override;

};

class ConstGate1 : public InputGate
{
public: 
	ConstGate1(size_t ID );
	~ConstGate1(); 
	vector<bool> Update(vector<bool> input) override;
};

class NotGate : public Gate
{
public:
	NotGate(size_t ID);
	~NotGate();
	vector<bool> Update(vector<bool> input) override;

};

class OrGate : public Gate
{
public: 
	OrGate(size_t ID);
	~OrGate();
	vector<bool> Update(vector<bool> input) override;
};

class AndGate : public Gate
{
public:
	AndGate(size_t ID);
	~AndGate();
	vector<bool> Update(vector<bool> input) override;
};

class XorGate : public Gate
{
public:
	XorGate(size_t ID);
	~XorGate();
	vector<bool> Update(vector<bool> input) override;
};

class NandGate : public Gate
{
public:
	NandGate(size_t ID);
	~NandGate();
	vector<bool> Update(vector<bool> input) override;
};

class NorGate : public Gate
{
public:  NorGate(size_t ID);
		 ~NorGate();
		 vector<bool> Update(vector<bool> input) override;
};

class XnorGate : public Gate
{
public: XnorGate(size_t ID);
		~XnorGate();
		vector<bool> Update(vector<bool> input) override;

};

class UserDefinedGate : public Gate
{
public:
	UserDefinedGate(); 
	~UserDefinedGate();

	vector<bool> Update(vector<bool> input) override;
};

class UserDefinedGateModel : public Gate
{
public: 
	UserDefinedGateModel(Graph<unique_ptr<Gate>, unique_ptr<Signal>> graph, vector<gvertex> InputGates, vector<gvertex> OutputGates, string name, size_t id);
	~UserDefinedGateModel(); 

	unique_ptr<UserDefinedGate> getGate(size_t id);
	vector<bool> Update(vector<bool> input) override;
};