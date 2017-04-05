#pragma once
#include <memory>
#include <vector>
using namespace std; 

class Gate;
enum Status
{
	Zero, One, Floating
};

class Signal
{
public:
	Signal();
	~Signal();
	Status Status; 
	Gate * In; 
	Gate * Out; 

	bool ConnectIn(Gate * In);
	bool ConnectOut(Gate * Out);
	void Update(); 

private:

};



class Gate {
public: 
	Gate(size_t inputSize, size_t ouputSize);
	virtual ~Gate(); 
	virtual void Update() =0;
	bool Evaluated; 
	Status status; 
	virtual bool ConnectInput(Signal * In);
	virtual bool ConnectOutput(Gate * Out); 
	virtual bool DisconnectInput(size_t numOfPin);
	virtual bool ReconnectOutput(size_t numOfPin, Gate * Out); 
	size_t GetLengthOfInput(); 
	size_t GetLengthOfOutput(); 
	size_t GetLengthOfFreeInputs();
	size_t GetLengthOfFreeOutputs(); 
protected:
	std::vector<std::unique_ptr<Signal>> output;
	std::vector<Signal *> input; 
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
	inline Status Read() { return status; }
};

class BlankGate : public OutputGate
{

};

class ConstGate0 : public InputGate
{
	
};

class ConstGate1 : public InputGate
{
	
};

class NotGate : public Gate
{
public: 
	NotGate();
	~NotGate(); 
	void Update() override; 

};

class OrGate : public Gate
{
	OrGate();
	~OrGate();
	void Update() override;
};

class AndGate : public Gate 
{
public: 
	AndGate(); 
	~AndGate(); 
	void Update() override;
};

class XorGate : public Gate
{
public:
	XorGate(); 
	~XorGate();
	void Update() override;
};

class NandGate : public Gate
{
public: 
	NandGate(); 
	~NandGate(); 
	void Update() override; 
};

class NorGate : public Gate
{
public:  NorGate();
		 ~NorGate(); 
		 void Update() override;
};

class XnorGate : public Gate
{
public: XnorGate();
		~XnorGate();
		void Update() override; 

};

class UserDefinedGate : public Gate
{
public: 

};
