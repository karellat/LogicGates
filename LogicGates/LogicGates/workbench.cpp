#include "workbench.h"
#include <string>
Workbench::Workbench()
{
	newID = 0; 
	std::string s[] = { "Not","And","Or","Xor","Nand","Nor","Xnor", "Blank","Const 1", "Const 0" };
	for (auto i : s)
	{
		StandardGate.push_back(i);
	}
	
}

Workbench::~Workbench()
{
}

std::vector<std::string> Workbench::ListOfFreeInputGates()
{
	std::vector<string> o;
	for ( gvertex  i : freeInputGates)
	{
		string name = i->value->Name() + " " + std::to_string(i->value->Id());
		vector<std::size_t> freePins = freeInputPins(i);
		name += " (";
		for (auto j : freePins)
	    {
			name += std::to_string(j) + ","; 
 	    }
		name += ")"; 

		o.push_back(name);
	}
	return o; 
}

std::vector<std::string> Workbench::ListOfFreeOutputGates()
{
	std::vector<string> o;
	for (gvertex i : freeOutputGates)
	{
		string name = i->value->Name() + " " + std::to_string(i->value->Id());
		vector<std::size_t> freePins = freeOutputPins(i);
		name += " (";
		for (auto j : freePins)
		{
			name += std::to_string(j) + ",";
		}
		name += ")";
		o.push_back(name);
	}
	return o;
}

std::vector<std::string> Workbench::ListOfUserDefinedGates()
{
	std::vector<std::string> o; 
	for (auto && i : UserDefinedGates)
	{
		o.push_back(i->Name() +  " " + std::to_string(i->Id()));
	}
	return o;
}


bool Workbench::Add(const std::size_t& num)
{
	StatusCheck(UnderConstruction);
	
	if (StandardGate.size() + UserDefinedGates.size() <= num)
		throw new runtime_error("Argument of gate adding out of index");
	
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
	    default: 
	    	throw new runtime_error("Unknown adding situation, index i = " + std::to_string(num) + " out of index");
	    }
    }
	//User Gate addition 
	else
	{
		
	}



}


void Workbench::StatusCheck(WorkbenchStatus s) const
{
	if (status != s)
		throw new WorkbenchStatusException(status, s);
}

