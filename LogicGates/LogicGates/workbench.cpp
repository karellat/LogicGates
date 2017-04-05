#include "workbench.h"
#include <string>
Workbench::Workbench()
{
	std::string s[] = { "Not","And","Or","Xor","Nand","Nor","Xnor", "Blank","Const 1", "Const 0" };
	for (auto i : s)
	{
		StandardGate.push_back(i);
	}
	
}

  std::vector<std::string>  Workbench::ListOfFreeInputGates()
{
	 StatusCheck(UnderConstruction);
	std::vector<std::string> output; 
	for (auto i : FreeInputPins)
	{
		output.push_back(i->Id() + " - " + i->Name() + "(" + std::to_string(i->GetLengthOfFreeInputs()) + ")"); 
	}
	return output;
}

 std::vector<std::string>  Workbench::ListOfFreeOutputGates()
{
	StatusCheck(UnderConstruction);
	std::vector<std::string> output; 
	for (auto i : FreeOutputPins)
	{
		output.push_back(i->Id() + " - " + i->Name() + "(" + std::to_string(i->GetLengthOfFreeOutputs()) + ")");
	}
	return output;
}

std::vector<std::string> Workbench::ListOfUserDefinedGates()
{
	StatusCheck(UnderConstruction);
	std::vector<std::string> output;
	for (std::size_t i = 0; i < UserDefinedGates.size();i++)
	{
		output.push_back(UserDefinedGates[i]->Id() + " - " + UserDefinedGates[i]->Name() + "(" + std::to_string(UserDefinedGates[i]->GetLengthOfFreeOutputs()) + ")");
	}
	return output;
}

std::size_t Workbench::SizeOfInput() const
{
	return FreeInputPins.size();
}

std::size_t Workbench::SizeOfOutput() const
{
	return FreeOutputPins.size(); 
}

bool Workbench::Connect(const std::size_t& freeInputPosition, const std::size_t& freeOutputPosition)
{

	
	StatusCheck(UnderConstruction);
	//Check arguments 
	if (freeInputPosition < 0 || freeInputPosition >= FreeInputPins.size() ||
		freeOutputPosition < 0 || freeOutputPosition >= FreeOutputPins.size())
		throw new runtime_error("Index out of connecting gates"); 
	
	//test arguments 
	if(FreeOutputPins[freeOutputPosition]->ConnectOutput(FreeInputPins[freeInputPosition]))
	{
		FreeOutputPins.erase(FreeOutputPins.begin() + freeOutputPosition);
		FreeInputPins.erase(FreeInputPins.begin() + freeInputPosition);
		return true; 
	}
	else
	{
		return false;
	}


}

bool Workbench::Add(const std::size_t& num)
{
	if (StandardGate.size() + UserDefinedGates.size() <= num)
		throw new runtime_error("Argument of adding out of index");
	
	unique_ptr<Gate> added;
	//Standard Gate addition
    if(StandardGate.size() > num)
    {
		
	    switch (num)
	    {
		case 0:
			added = make_unique<NotGate>(NotGate()); 
			break;
		case 1: 
			added = make_unique<AndGate>(AndGate());
			break; 
		case 2:
			added = make_unique<OrGate>(OrGate());
			break; 
		case 3:
			added = make_unique<XorGate>(XorGate()); 
			break; 
		case 4: 
			added = make_unique<NandGate>(NandGate());
			break; 
		case 5: 
			added = make_unique<NorGate>(NorGate());
			break; 
		case 6:
			added = make_unique<XnorGate>(XnorGate());
			break;
		case 7: 
			added = make_unique<BlankGate>(BlankGate());
			break; 
		case 8: 
			added = make_unique<ConstGate1>(ConstGate1());
			break; 
		case 9: 
			added = make_unique<ConstGate0>(ConstGate0());
	    }
    }
	//User Gate addition 
	else
	{
		
	}

	//Mark inputs and outpus
	if (added->GetLengthOfFreeInputs() > 0)
		FreeInputPins.push_back(added.get());
	if (added->GetLengthOfFreeOutputs() > 0)
		FreeOutputPins.push_back(added.get());

	Gates.push_back(std::move(added));
}


void Workbench::StatusCheck(WorkbenchStatus s) const
{
	if (status != s)
		throw new WorkbenchStatusException(status, s);
}
