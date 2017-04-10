#include "workbench.h"
#include <string>
Workbench::Workbench() : lastID(0)
{

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

bool Workbench::Connect(const std::size_t& freeInputPosition, const std::size_t& freeInputID, const std::size_t& freeOutputPosition, const std::size_t& freeOutputID)
{
	//TODO: Mark specific errors
	StatusCheck(UnderConstruction); 
	
	if (freeInputPosition > freeInputGates.size() || freeInputPosition < 0 ||
		freeOutputPosition > freeOutputGates.size() || freeOutputPosition < 0)
		return false; 

	gvertex from = freeOutputGates[freeOutputPosition];
	gvertex to = freeInputGates[freeInputPosition]; 

	auto free_id_from = freeOutputPins(from); 
	auto free_id_to = freeInputPins(to);

	if (!any_of(free_id_from.begin(), free_id_from.end(), [freeOutputID](auto i) {return i == freeOutputID; }))
		return false; 
	if (!any_of(free_id_to.begin(), free_id_to.end(), [freeInputID](auto i) {return i == freeInputID; }))
		return false; 
	unique_ptr<Signal> s = make_unique<Signal>(Signal(Floating, freeInputID, freeOutputID));
	graph->connect(from, to, std::move(s));

	return true; 
}

bool Workbench::Add(const std::size_t& num)
{
	StatusCheck(UnderConstruction);
	
	if (StandardGate.size() >= num)
		throw new runtime_error("Argument of gate adding out of index");
	
	unique_ptr<Gate> added; 
	    switch (num)
	    {
		case 0:
			added = make_unique<NotGate>(NotGate(GetNewID())); 
			break;
		case 1: 
			added = make_unique<AndGate>(AndGate(GetNewID()));
			break; 
		case 2:
			added = make_unique<OrGate>(OrGate(GetNewID()));
			break; 
		case 3:
			added = make_unique<XorGate>(XorGate(GetNewID())); 
			break; 
		case 4: 
			added = make_unique<NandGate>(NandGate(GetNewID()));
			break; 
		case 5: 
			added = make_unique<NorGate>(NorGate(GetNewID()));
			break; 
		case 6:
			added = make_unique<XnorGate>(XnorGate(GetNewID()));
			break;
		case 7: 
			added = make_unique<BlankGate>(BlankGate(GetNewID()));
			break; 
		case 8: 
			added = make_unique<ConstGate1>(ConstGate1(GetNewID()));
			break; 
		case 9: 
			added = make_unique<ConstGate0>(ConstGate0(GetNewID()));
	    default: 
	    	throw new runtime_error("Unknown adding situation, index num = " + std::to_string(num) + " out of index");
	    }
    
	auto v = graph->add_vertex(std::move(added));
	if (v->value->GetLengthOfInput() > 0)
		freeInputGates.push_back(v);
	if (v->value->GetLengthOfOutput() > 0)
		freeOutputGates.push_back(v);
	return true; 
}

bool Workbench::AddUserDefineGate(const std::size_t& positionInList)
{
	StatusCheck(UnderConstruction);
	//TODO: Mark errors 
	if (positionInList > UserDefinedGates.size() || positionInList < 0)
		return false; 

	unique_ptr<Gate> g = UserDefinedGates[positionInList]->getGate(GetNewID());
	gvertex v = graph->add_vertex(std::move(g));
	if (v->value->GetLengthOfInput() > 0)
		freeInputGates.push_back(v);
	if (v->value->GetLengthOfOutput() > 0)
		freeOutputGates.push_back(v); 

	return true; 
}

bool Workbench::ConstructBench()
{
}

bool Workbench::AddInputGate()
{
	StatusCheck(UnderConstruction);
	unique_ptr<Gate> i = make_unique<InputGate>(InputGate(GetNewID()));
	gvertex v = graph->add_vertex(std::move(i));
	InputGates.push_back(v);
	return true; 
}

bool Workbench::AdddOutputGate()
{
	unique_ptr<Gate> o = make_unique<OutputGate>(OutputGate(GetNewID()));
	gvertex v = graph->add_vertex(std::move(o));
	OutputGates.push_back(v);
	return true; 
}


void Workbench::StatusCheck(WorkbenchStatus s) const
{
	if (status != s)
		throw new WorkbenchStatusException(status, s);
}

