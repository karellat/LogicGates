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
	Gate(size_t inputSize, size_t outputSize);
	virtual ~Gate();
	virtual void Update() = 0;
	bool Evaluated;
	Status status;
	virtual bool ConnectInput(Signal * In);
	virtual bool ConnectOutput(Gate * Out);
	virtual bool DisconnectInput(size_t numOfPin);
	virtual bool ReconnectOutput(size_t numOfPin, Gate * Out);
	size_t GetLengthOfInput();
	size_t GetLengthOfOutput();
	size_t GetLengthOfFreeInputs() { return 0; }
	size_t GetLengthOfFreeOutputs() { return 0; }
	//Logging info 
	const size_t Id() { return id; };
	const string Name() { return name; };
protected:
	std::vector<std::unique_ptr<Signal>> output;
	std::vector<Signal *> input;
	size_t input_size; 
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
	void Update() override;
};


class OutputGate : public Gate
{
	OutputGate();
	~OutputGate();
	inline Status Read() { return status; }
	void Update()override;
};

class BlankGate : public Gate
{
public:
	BlankGate(); 
	~BlankGate();
	void Update() override;
};

class ConstGate0 : public InputGate
{
public: 
	ConstGate0();
	~ConstGate0();
	void Update() override;

};

class ConstGate1 : public InputGate
{
public: 
	ConstGate1();
	~ConstGate1(); 
	void Update() override;
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
public: 
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
