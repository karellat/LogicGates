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
	Signal();
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
	virtual vector<bool> Update(vector<bool>) = 0;
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
	InputGate(); 
	~InputGate();
	void Set(Status s);
};


class OutputGate : public Gate
{
	OutputGate();
	~OutputGate();
};

class BlankGate : public Gate
{
public:
	BlankGate(); 
	~BlankGate();
};

class ConstGate0 : public InputGate
{
public: 
	ConstGate0();
	~ConstGate0();

};

class ConstGate1 : public InputGate
{
public: 
	ConstGate1();
	~ConstGate1(); 
};

class NotGate : public Gate
{
public:
	NotGate();
	~NotGate();

};

class OrGate : public Gate
{
public: 
	OrGate();
	~OrGate();
};

class AndGate : public Gate
{
public:
	AndGate();
	~AndGate();
};

class XorGate : public Gate
{
public:
	XorGate();
	~XorGate();
};

class NandGate : public Gate
{
public:
	NandGate();
	~NandGate();
};

class NorGate : public Gate
{
public:  NorGate();
		 ~NorGate();
};

class XnorGate : public Gate
{
public: XnorGate();
		~XnorGate();

};

class UserDefinedGate : public Gate
{
public:

};
