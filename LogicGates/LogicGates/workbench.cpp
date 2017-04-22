#include "workbench.h"
#include <string>
#include <atomic>
#include <set>
#include <numeric>
#include <iterator>
#include <locale>

Workbench::Workbench() : lastID(0)
{
	graph = make_unique<Graph<unique_ptr<Gate>, unique_ptr<Signal>>>(Graph<unique_ptr<Gate>,unique_ptr<Signal>>()); 
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
		string name = i->value->Name() + " ID: " + std::to_string(i->value->Id());
		vector<std::size_t> freePins = freeInputPins(i);
		name += " Free Pins: (";
		for (std::size_t j=0; j<freePins.size(); j++)
	    {
			
			name += std::to_string(freePins[j]); 
			if (j != freePins.size() - 1) name += ",";
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
		string name = i->value->Name() + " ID: " + std::to_string(i->value->Id());
		vector<std::size_t> freePins = freeOutputPins(i);
		name += " Free Pins: (";
		for (std::size_t j = 0; j<freePins.size(); j++)
		{

			name += std::to_string(freePins[j]);
			if (j != freePins.size() - 1) name += ",";
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
	free_id_from = freeOutputPins(from);
	free_id_to = freeInputPins(to);
	if (free_id_from.size() == 0) freeOutputGates.erase(freeOutputGates.begin() + freeOutputPosition);
	if (free_id_to.size() == 0)   freeInputGates.erase(freeInputGates.begin() + freeInputPosition);
	
	return true; 
}

bool Workbench::Add(const std::size_t& num)
{
	StatusCheck(UnderConstruction);
	
	if (StandardGate.size() <= num)
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
	if (TestOfCorrection())
	{
		status = Constructed; 
		return true; 
	}
	else
		return false; 
}

bool Workbench::AddInputGate()
{
	StatusCheck(UnderConstruction);
	unique_ptr<Gate> i = make_unique<InputGate>(InputGate(GetNewID()));
	gvertex v = graph->add_vertex(std::move(i));
	InputGates.push_back(v);
	freeOutputGates.push_back(v);
	return true; 
}

bool Workbench::AdddOutputGate()
{
	unique_ptr<Gate> o = make_unique<OutputGate>(OutputGate(GetNewID()));
	gvertex v = graph->add_vertex(std::move(o));
	OutputGates.push_back(v);
	freeInputGates.push_back(v);
	return true; 
}

bool Workbench::SetInput(vector<bool> input)
{
	std::size_t tacts = 0; 
	//TODO: CHECK 
	StatusCheck(Constructed);
	if (input.size() != InputGates.size())
		throw new runtime_error("input vector does not fit input size");

	set<gvertex> following_tact_gates; 
	for (std::size_t i = 0; i < InputGates.size(); i++)
	{
		gvertex v = InputGates[i];
		vector<gedge> edges = graph->edges_from(v);
		for (auto e : edges)
		{
			following_tact_gates.insert(e->to);
			if (input[i])
				e->value->status = One;
			else
				e->value->status = Zero;
		}
	}
		set<gvertex> actual_tact_gates = following_tact_gates;
		following_tact_gates.clear();
		while (any_of(OutputGates.begin(), OutputGates.end(), [](gvertex o_g) {return !(o_g->value->result);}))
		{
			for(auto av : actual_tact_gates)
			{
				vector<gedge> input_edges = graph->edges_to(av); 
				vector<bool> input; 
				bool notFinished = false;
				input.resize(av->value->GetLengthOfInput(), false);
				for (auto e : input_edges)
				{
					if (e->value->status == Floating)
					{
						notFinished = true;
						break;
					}
					else if (e->value->status == One)
						input[e->value->toID] = true;
					else
						input[e->value->toID] = false;
				}
				if (notFinished) continue; 

				vector<bool> output = av->value->Update(input); 
				vector<gedge> output_edges = graph->edges_from(av);
				for(auto oe : output_edges)
				{
					if (output[oe->value->fromID])
						oe->value->status = One;
					else
						oe->value->status = Zero; 
					following_tact_gates.insert(oe->to);
				}

			}
			actual_tact_gates = following_tact_gates;
			tacts++; 
		}
	
	status = Calculated;
	return true; 
}

vector<bool> Workbench::ReadOutput()
{
	StatusCheck(Calculated);
	vector<bool> o; 
	for (auto i : OutputGates)
		o.push_back(i->value->resultValues[0]);
	return o; 
}



bool Workbench::ConstructUserGate(string name)
{
	//TODO:check constructed too
	StatusCheck(Calculated);
	unique_ptr<UserDefinedGateModel> model = 
		make_unique<UserDefinedGateModel>(UserDefinedGateModel(std::move(graph), InputGates, OutputGates,name,GetNewID()));
	UserDefinedGates.push_back(std::move(model));
	return true; 
}

vector<string> Workbench::ListAllGates()
{
	vector<string> output; 
	for (auto && v : graph->vertices)
	{
		string gate = "ID: " + std::to_string(v->value->Id()) + " Name: " + v->value->Name();
		gate += "\n InputSize: " + std::to_string(v->value->GetLengthOfInput()) + " OutputSize: " + std::to_string(v->value->GetLengthOfOutput()); 
		gate += "\n Input Edges: "; 
		for(auto i : graph->edges_to(v.get()))
		{
			gate += "\n\t from pin " + std::to_string(i->value->fromID) + " (" + i->from->value->Name() + ", ID: " + std::to_string(i->from->value->Id()) +
				")  ---------> to pin " +  std::to_string(i->value->toID);
		}
		/*gate += "\n Output Edges: ";
		for(auto i : graph->edges_from(v.get()))
		{
			gate += "\n\t from pin " + std::to_string(i->value->fromID) + " ---------> to pin " + std::to_string(i->value->toID) +
				" (" + i->to->value->Name() + ", ID: " + std::to_string(i->to->value->Id()) + ")";
		}
*/
		output.push_back(gate);
	}
	return output;
}


bool Workbench::TestOfCorrection()
{
	//TODO: infinit loop test, correction of output
	if (InputGates.size() == 0 || OutputGates.size() == 0 ||
		freeOutputGates.size() != 0 || freeInputGates.size() != 0)
		return false; 
	else return true; 
}

void Workbench::StatusCheck(WorkbenchStatus s) const
{
	if (status != s)
		throw new WorkbenchStatusException(status, s);
}

void Workbench::StatusCheck(vector<WorkbenchStatus> s) const
{
	for (auto i : s)
	{
		if (i == status)
			return;
	}
	throw  new WorkbenchStatusException(status);
}

vector<std::size_t> Workbench::freeInputPins(gvertex v) const
{
	vector<std::size_t> all_id;
	all_id.resize(v->value->GetLengthOfInput()); 
	std::iota(all_id.begin(), all_id.end(), 0);
	vector<gedge> edges_to = graph->edges_to(v);
	vector<std::size_t> connected_id;
	connected_id.resize(edges_to.size());
	std::transform(edges_to.begin(), edges_to.end(), connected_id.begin(), [](gedge v) {return v->value->toID; });
	vector<std::size_t> output; 
	output.resize(all_id.size() - connected_id.size());
	std::set_difference(all_id.begin(), all_id.end(), connected_id.begin(), connected_id.end(), std::inserter(output,output.begin()));
	return output;
}

vector<std::size_t> Workbench::freeOutputPins(gvertex v) const
{
	vector<std::size_t> all_id;
	all_id.resize(v->value->GetLengthOfOutput());
	std::iota(all_id.begin(), all_id.end(), 0);
	vector<gedge> edges_from = graph->edges_from(v);
	vector<std::size_t> connected_id;
	connected_id.resize(edges_from.size());
	std::transform(edges_from.begin(), edges_from.end(), connected_id.begin(), [](gedge v) {return v->value->fromID; });
	vector<std::size_t> output;
	output.resize(all_id.size() - connected_id.size());
	std::set_difference(all_id.begin(), all_id.end(), connected_id.begin(), connected_id.end(), output.begin());
	return output;

}
//WORKBENCHTUI

WorkbenchTUI::~WorkbenchTUI()
{
}

void WorkbenchTUI::WaitForFile()
{
}

bool WorkbenchTUI::ReadFile(string path)
{
	string line; 
	OpenFile(path);
	//Read name and check input tag
	getline(inputFile, line);
	vector<string> tokens = Split(line, '\t', RemoveEmptyEntries);
	
	if(tokens.size() != 2)
	{
		output << "Wrong format, definition line" << endl;
		return false; 
	}

	string tag = ToUpper(tokens[0]);
	string gate_name = tokens[1];
	if(tag != definitionTag)
	{
		output << line << std::endl << "incorrect definition tag format" << endl; 
		return false; 
	}
	
	if (!nameSizeCheck(gate_name.size())) return false;
	if (!nameCharsCheck(gate_name)) return false;
	gate_name.resize(gate_name.size());
	gate_name = ToUpper(gate_name);
	
	output << gate_name;
	
}

void WorkbenchTUI::InteraktiveMode()
{
}

bool WorkbenchTUI::OpenFile(std::string path)
{
	string s = "\tOpening file " + path;
	output << s << endl;
	inputFile.open(path);
	if (inputFile.is_open())
	{
		return true;
	}
	else
	{
		output << "\tUnable to open file" << std::endl;
		return false;
	}
}


bool WorkbenchTUI::nameSizeCheck(string name)
{
	return nameSizeCheck(name.size());
}

bool WorkbenchTUI::nameSizeCheck(std::size_t size)
{
	if (size > maxSizeOfTag)
	{
		output << "Gate name is too long (" + to_string(size) + ")" + " max=" + std::to_string(maxSizeOfTag) << endl;
		return false; 
	}
	return true;
}

bool WorkbenchTUI::nameCharsCheck(string name)
{
	if (any_of(name.begin(), name.end(), [this](char c){
		return any_of(forbidenChars.begin(), forbidenChars.end(),
			[c](char  fc) { return fc == c; });
	}))
	{
		output << "Name contains one of the forbidden chars ";
		return false; 
	}
	else return true;

}

std::vector<string> WorkbenchTUI::Split(string s, char delimeter, StringSplitOption option)
{
	vector<string> tokens; 
	string token;
	istringstream in(s);
	while(getline(in,token,delimeter))
	{
		if(token != "" || option == None )
			tokens.push_back(token);
	}
	return tokens; 
}

std::string WorkbenchTUI::ToUpper(std::string s)
{
	string o; 
	o.resize(s.size());
	std::transform(s.begin(), s.end(), o.begin(), toupper);
	return o;
}
