#include "workbench.h"



Workbench::Workbench() : lastID(0)
{
	graph = std::make_unique<Graph<std::unique_ptr<Gate>, std::unique_ptr<Signal>>>(); 
	std::size_t s_size = 13;
	std::string s[] = { "NOT","AND","OR","XOR","NAND","NOR","XNOR", "BLANK","CONST1", "CONST0","INPUT","OUTPUT","DOUBLE" };
	for(size_t i = 0; i < s_size; i++)
	{
		StandardGate.push_back(s[i]);
		TypeNames[s[i]] = i + 1; 
	}
	

}

Workbench::~Workbench()
{
}

std::vector<std::string> Workbench::ListOfFreeInputGates()
{
	std::vector<std::string> o;
	for ( gvertex  i : freeInputGates)
	{
		std::string name = i->value->Name() + " ID: " + std::to_string(i->value->Id());
		std::vector<std::size_t> freePins = freeInputPins(i);
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

vector<string> Workbench::ListOfNamedVertex()
{
	vector<string> o; 
	o.resize(VertexNames.size());
	transform(VertexNames.begin(), VertexNames.end(), o.begin(), [](pair<string, gvertex> p) { return  p.first + " - " + p.second->value->Name(); });
	return o;
}

bool Workbench::Connect(const std::size_t& freeInputPosition, const std::size_t& freeInputID, const std::size_t& freeOutputPosition, const std::size_t& freeOutputID)
{
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
bool Workbench::Connect(gvertex from, gvertex to, std::size_t fromPin, std::size_t toPin)
{
	StatusCheck(UnderConstruction);
	
	auto free_id_from = freeOutputPins(from);
	auto free_id_to = freeInputPins(to);
	//Check if the pins are not ocuppied or even exist
	if (!any_of(free_id_from.begin(), free_id_from.end(), [fromPin](auto i) {return i == fromPin; }))
		return false;
	if (!any_of(free_id_to.begin(), free_id_to.end(), [toPin](auto i) {return i == toPin; }))
		return false;

	unique_ptr<Signal> s = make_unique<Signal>(Signal(Floating, toPin, fromPin));
	graph->connect(from, to, std::move(s));
	free_id_from = freeOutputPins(from);
	free_id_to = freeInputPins(to);
	
	if (free_id_from.size() == 0)
	{
		freeOutputGates.erase(std::remove_if(freeOutputGates.begin(), freeOutputGates.end(), [from](gvertex v) {return from == v; }),freeOutputGates.end());
	}
	if (free_id_to.size() == 0) {
		freeInputGates.erase(std::remove_if(freeInputGates.begin(), freeInputGates.end(), [to](gvertex v) {return to == v; }),freeInputGates.end());
	}
	return true;
}
gvertex Workbench::Add(const std::size_t& num)
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
			return AddConstGate(true);
		case 9: 
			return AddConstGate(false);
	    case 10: 
			return AddInputGate();
		case 11: 
			return AddOutputGate();
		case 12:
			added = make_unique<DoubleGate>(DoubleGate(GetNewID()));
			break;
	    default: 
	    	throw new runtime_error("Unknown adding situation, index num = " + std::to_string(num) + " out of index");
	    }
    
	auto v = graph->add_vertex(std::move(added));
	if (v->value->GetLengthOfInput() > 0)
		freeInputGates.push_back(v);
	if (v->value->GetLengthOfOutput() > 0)
		freeOutputGates.push_back(v);
	return v; 
}
gvertex Workbench::GetType(string typeName)
{
	int index = TypeNames.at(typeName);
	//User defined Gates: 
	//TODO: resolve
	if(index < 0 )
	{
		throw new runtime_error("Not Implemented yet");
	}
	//Predefined Gates 
	else
	{
		std::size_t i = index - 1; 
		return Add(i);
	}
}

bool Workbench::GetVertex(string name,gvertex& v)
{
	try
	{
		v = VertexNames.at(name);
	}
	catch(std::out_of_range)
	{
		return false; 
	}
	return true;
}

bool Workbench::AddNamedGate(string gateName, string typeName)
{
	//Check if there is no name conflict 
	if(VertexNames.find(gateName) != VertexNames.end())
	{
		return false;
	}
	else
	{
		VertexNames[gateName] = GetType(typeName);
		return true;
	}
}

bool Workbench::AddUserDefineGate(const std::size_t& positionInList)
{
	StatusCheck(UnderConstruction);
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

gvertex Workbench::AddInputGate()
{
	StatusCheck(UnderConstruction);
	unique_ptr<Gate> i = make_unique<InputGate>(InputGate(GetNewID()));
	gvertex v = graph->add_vertex(std::move(i));
	InputGates.push_back(v);
	freeOutputGates.push_back(v);
	return v; 
}

gvertex Workbench::AddOutputGate()
{
	unique_ptr<Gate> o = make_unique<OutputGate>(OutputGate(GetNewID()));
	gvertex v = graph->add_vertex(std::move(o));
	OutputGates.push_back(v);
	freeInputGates.push_back(v);
	return v; 
}

bool Workbench::SetInput(vector<bool> input)
{
	for (auto o : OutputGates)
		o->value->Reset();
	std::size_t tacts = 0; 

	vector<WorkbenchStatus> statuses; 
	statuses.push_back(Constructed);
	statuses.push_back(Calculated);
	StatusCheck(statuses);
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

	for (auto i : ConstGates)
	{
		vector<gedge> edges = graph->edges_from(i);
		vector<bool> blank;
		for (auto e : edges)
		{
			following_tact_gates.insert(e->to);
			if(i->value->Update(blank)[0])
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
	vector<WorkbenchStatus> ws; 
	ws.push_back(Calculated); 
	ws.push_back(Constructed);
	StatusCheck(ws);
	unique_ptr<UserDefinedGateModel> model = make_unique<UserDefinedGateModel>(UserDefinedGateModel(std::move(graph), InputGates, OutputGates, ConstGates, name, GetNewID()));
	UserDefinedGates.push_back(std::move(model));
	//TODO: Make new type name
	ResetWorkbench(false);
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

void Workbench::ResetWorkbench(bool deleteUDG)
{
	status = UnderConstruction;
	graph = make_unique<Graph<unique_ptr<Gate>, unique_ptr<Signal>>>();
	InputGates.clear();
	OutputGates.clear();
	ConstGates.clear();
	freeOutputGates.clear();
	freeInputGates.clear();
	if (deleteUDG) {
		//TODO: clear Type names as well
		UserDefinedGates.clear();
	}
	VertexNames.clear();
	testOutput.clear();
}

gvertex Workbench::AddConstGate(bool c)
{
	unique_ptr<Gate> g;
	if(c)
		g = make_unique<ConstGate1>(ConstGate1(GetNewID()));
	else 
		g = make_unique<ConstGate0>(ConstGate0(GetNewID()));
	gvertex v = graph->add_vertex(std::move(g));
	ConstGates.push_back(v);
	freeOutputGates.push_back(v);
	return v; 
}


bool Workbench::TestOfCorrection()
{
	testOutput = "Test of correction: \n";
	// All connection  and io tests 
	if (InputGates.size() == 0)
	{
		testOutput += "Input size is zero\n";
		return false; 
	}
	if (OutputGates.size() == 0)
	{
		testOutput += "Output size is zero\n";
		return false; 
	}
	if(freeOutputGates.size() != 0)
	{
		testOutput += "All of ouput pins are not connected\n"; 
		return false; 
	}
	if (freeInputGates.size() != 0)
	{
		testOutput += "All of input pins are not connected\n";
		return false;
	}

	//Test empty circuits 
	vector<gvertex> startingV; 
	std::for_each(InputGates.begin(), InputGates.end(), [&startingV](gvertex g) {startingV.push_back(g); });
	std::for_each(ConstGates.begin(), ConstGates.end(), [&startingV](gvertex g) {startingV.push_back(g); });
	if(graph->all_vertices_available_from(startingV))
	{
		testOutput += "\tAccessibility check: OK \n";
	}
	else
	{
		testOutput += "Invalid graph,some part of graph is not connected to inputs or const inputs";
		return false;
	}

	if(!graph->cycle_detection())
	{
		testOutput += "\t No cycle detected\n";
	}
	else
	{
		testOutput += "Invalid graph, cycle detected.\n"; 
		return false; 
	}


	return true; 
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
	std::set_difference(all_id.begin(), all_id.end(), connected_id.begin(), connected_id.end(), std::inserter(output, output.begin()));
	return output;

}