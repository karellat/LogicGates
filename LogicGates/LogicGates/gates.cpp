#include "gates.h" 

Gate::Gate(size_t inputSize, size_t outputSize)
{
	input_size = inputSize;
	for (size_t i = 0; i < outputSize;i++)
	{
		output.push_back(make_unique<Signal>(Signal()));
		//TODO: Connect to the Gate
	}
	status = Floating;
	Evaluated = false; 
	//TODO: Set different id
	id = 42; 
	name = "Gate";
}

Gate::~Gate()
{
	output.clear();
}

InputGate::InputGate()
	:Gate(0,1)
{

}

BlankGate::BlankGate()
	:Gate(1,0)
{
}

ConstGate0::ConstGate0()
	:InputGate()
{
}

ConstGate1::ConstGate1()
	:InputGate()
{
}

NotGate::NotGate()
	:Gate(1,1)
{
}

OrGate::OrGate()
	:Gate(2,1)
{
}

AndGate::AndGate()
	:Gate(2,1)
{
}


XorGate::XorGate()
	:Gate(2,1)
{
}

NandGate::NandGate()
	:Gate(2,1)
{
}

NorGate::NorGate()
	:Gate(2,1)
{
}

XnorGate::XnorGate()
	:Gate(2,1)
{
}
