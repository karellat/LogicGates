#include "workbench.h"

using pin = std::pair<std::string, std::size_t>;

Workbench::Workbench() : lastID(0)
{
	graph = std::make_unique<Graph<std::unique_ptr<Gate>, std::unique_ptr<Signal>>>(Graph<std::unique_ptr<Gate>, std::unique_ptr<Signal>>()); 
	std::size_t s_size = 13;
	std::string s[] = { "NOT","AND","OR","XOR","NAND","NOR","XNOR", "BLANK","CONST 1", "CONST 0","INPUT","OUTPUT","DOUBLE" };
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
			added = make_unique<ConstGate1>(ConstGate1(GetNewID()));
			break; 
		case 9: 
			added = make_unique<ConstGate0>(ConstGate0(GetNewID()));
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
	//TODO: CHECK
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



////////////////////////
//WORKBENCHTUI

WorkbenchTUI::~WorkbenchTUI()
{
}

void WorkbenchTUI::WaitForFile()
{
}

bool WorkbenchTUI::ReadFile(string path)
{
	output << "Reading file " + path << endl;
	string line; 
	if (!OpenFile(path)) return false;
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
	
	output << "\tGate NAME: " << gate_name << endl;;
	
	//Setting names: 

	string typeName; 
	string vertexName; 
	
	//TODO: Check end of file
	getline(inputFile, line);
	while (line[0] != '#')
	{
		tokens = Split(line, '\t', RemoveEmptyEntries);
		if (!nameSizeCheck(tokens[0]) || !nameCharsCheck(tokens[0]))
			return false;

		vertexName = ToUpper(tokens[0]);
		typeName = ToUpper(tokens[1]);
		output << "\tVertex Name: " << vertexName << " Type Name: " << typeName << endl;

		if (!workbench->AddNamedGate(vertexName, typeName))
		{
			output << "\tAdding same named vertex or unknown type";
			return false;
		}
		line = "";
		while (line == "")
		{
			if (inputFile.eof())
				output << "\tUnexpected end of file, in declaring section " << endl;
			getline(inputFile, line);
		}
	}
	output << "These named vertex were created: ";
	output << "Declaration part over " << endl;
	for (auto i : workbench->ListOfNamedVertex())
	{
		output << "\t " << i << endl;
	}
	//Connection part
	output << "Connection part: " << endl;
	tag = line;

	if(tag != connectionTag)
	{
		output << line << std::endl << "incorrect connection tag format" << endl;
		return false;
	}
	//TODO:Check end of file 
	getline(inputFile, line);

	
	while (line[0] != '#')
	{
		tokens = Split(line, '\t', RemoveEmptyEntries);
		if (tokens.size() != 3)
		{
			output << "Incorrect format of connection line " + line << endl;
			return false;
		}
		pin aGate;
		pin bGate;
		if (!ParsePin(tokens[0], aGate) || !ParsePin(tokens[2], bGate))
			return false;
		if (tokens[1].size() != 2)
		{
			output << "Incorrect format of connection arrow: " + tokens[1];
			return false;
		}
		gvertex a;
		gvertex b;
		if( !workbench->GetVertex(aGate.first,a)  || !workbench->GetVertex(bGate.first,b))
		{
			output << "Incorrect name, nick name does not exists:  " << aGate.first << ", " << bGate.first << endl;
			return false;
		}
		if (tokens[1] == "->")
		{
			if(!workbench->Connect(a, b, aGate.second, bGate.second))
			{
				output << "Incorrected pin,  ocuppied or not even exists :" << line << endl;
				return false;
			}
			else
			{
				output << "\tConnecting " << aGate.first << "[" << aGate.second << "]"
					<< " to " << bGate.first << "[" << bGate.second << "]" << endl;
			}
		}
		else if (tokens[1] == "<-")
		{
			if (!workbench->Connect(b, a, bGate.second, aGate.second))
			{
				output << "Incorrected pin,  ocuppied or not even exists :" << line << endl;
				return false; 
			}
			else
			{
				output << "\tConnecting " << bGate.first << "[" << bGate.second << "]"
					<< " to " << aGate.first << "[" << aGate.second << "]" << endl;
			}
		}
		else
		{		output << "Incorrect format of connection arrow " + tokens[1];
		return false;
		}
		
	



		line = "";
		while (line == "")
		{
			if (inputFile.eof())
			{
				output << "\tUnexpected end of file, in connecting section " << endl;
				return false;
			}
			getline(inputFile, line);
		}
	}

	//Construction part
	output << "Construction part: " << endl;
	if(workbench->ListOfFreeInputGates().size() != 0)
	{
		//TODO: Specific gate
		output << "Not connected input pins: " << endl;
		return false;
	}
	if (workbench->ListOfFreeOutputGates().size() != 0)
	{
		//TODO: Specific gate
		output << "Not connected output pins: " << endl;
		return false;
	}
	if(workbench->SizeOfInput() == 0)
	{
		output << "Input gates missing" << endl;
		return false;
	}
	if(workbench->SizeOfOutput() == 0)
	{
		output << "Output gates missing" << endl;
		return false;
	}

	output << "\tAll pins connected" << endl << "\tinput size: " << workbench->SizeOfInput() << " output size: " << workbench->SizeOfOutput() << endl;


	output << "\tConstructing workbench" << endl;
	if(workbench->ConstructBench())
	{
		output << "\tBench was successfully constructed" << endl;
	}
	else
	{
		output << "\tInvalid bench architecture, construction failed" << endl;
		return false;
	}

	//ending tag check
	if(line[0] == '#')
	{
		output << "Successfull loaded Gate from file " << endl;
		inputFile.close();
		return true;
	}
	else
	{
		output << "Ending tag missing, reading file failed" << endl;
		return false;
	}

	
}

void WorkbenchTUI::InteraktiveMode()

{
	if(InteractiveReadingFile())
	{
		InteractiveSeting();
	}
	else
	{
		output << "Exiting interactive mode" << endl;
	}
}
void WorkbenchTUI::InteraktiveMode(string path)
{
	if(ReadFile(path))
	{
		InteractiveSeting();
	}
	else
	{
		output << "Exiting interactive mode" << endl; 
	}
}

void WorkbenchTUI::PassiveMode(string path, string inputString)
{
	vector<bool> inputSettings;
	stringToBools(inputString,inputSettings);
	//Prepare bench: 
	if (!ReadFile(path))
		return;
	//Set input
	if (!SetInput(inputSettings))
		return;
	
	vector<bool> o; 
	if (!ReadOutputs(o))
		return; 
	else
	{
		string os; 
		boolsToString(o, os); 
		output << "Calculation was successfull, readed values: " << os << endl;
	}

	output << "Application closed" << endl;


}

void WorkbenchTUI::InteractiveSeting()
{
	string line;
	vector<bool> in;
	vector<bool> out;
	string s;
	while (true)
	{


		string line;
		output << "Set input: ";
		input >> line;
		if (line == "exit" || line == "q" || line == "quit" || line == "break")
		{
			output << "Exiting inputs setting" << endl;
			return; 
		}
		if (stringToBools(line, in))
		{
			if (SetInput(in))
			{
				if (ReadOutputs(out))
				{
					boolsToString(out, s);
					output << "Output : " << s << endl;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			output << "Wrong format of input, ";
			continue;
		}
	}
}

bool WorkbenchTUI::InteractiveReadingFile()
{
	string line;
	output << "Insert path of input file: " << endl << "\t";

	while (true)
	{
		input >> line;
		if (line == "exit" || line == "q" || line == "quit" || line == "break")
		{
			output << "Exiting inputs setting" << endl;
			return false;
		}
		
		if (!ReadFile(line)) {
			output << "Invalid name, insert path of correct input file: " << endl << "\t";
		}
		else
		{
			return true;
		}
	}

	return true;
}

bool WorkbenchTUI::SetInput(vector<bool> inputSettings)
{
	if(inputSettings.size() != workbench->SizeOfInput())
	{
		output << "Invalid size of input " << endl;
		return false; 
	}
	if(workbench->SetInput(inputSettings))
	{
		string s;
		boolsToString(inputSettings, s);
		output << "Input was set to: " << s << endl;
		return true;
	}
	else
	{
		output << "Input was not correctly set, workbench invalid status" << endl;
		return false;
	}
}

bool WorkbenchTUI::ReadOutputs(vector<bool>& outputValue)
{	
	outputValue = workbench->ReadOutput();
	return true;
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

bool WorkbenchTUI::ParsePin(string input, std::pair<string, std::size_t>& pair)
{
	if (input[input.size() - 1] != ']')
	{
		output << "Wrong format of pin missing \"]\" : " + input;
		return false;
	}
	std::size_t i = input.find_first_of('[');
	if (i == input.npos)
	{
		output << "Wrong format of pin missing \"[\" : " + input;
		return false;
	}

	std::string num = input.substr(i + 1, input.size() - i - 2);
	std::string name = ToUpper(input.substr(0, i));
	
	std::size_t t;
	try {
		long n = stoul(num, nullptr, 0);
		t = (std::size_t) n;
	}
	catch (invalid_argument)
	{
		output << "Invalid id of pin " + num;
		return false;
	}
	pair.first = name;
	pair.second = t; 
	return true;
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
		output << "Name contains one of the forbidden chars \"" + name + "\"";
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

bool WorkbenchTUI::boolsToString(std::vector<bool> v,string& s)
{
	s.clear();
	transform(v.begin(), v.end(), std::inserter(s, s.begin()), [](bool c)
	{
		if (c) return '1'; else return '0';
	});

	return true;
}

bool WorkbenchTUI::stringToBools(string s, std::vector<bool>& b)
{
	b.clear();
	try
	{
		transform(s.begin(), s.end(), std::inserter(b, b.begin()), [](char c)
		{
			if (c == '1') return true; else if (c == '0') return false;
			else throw runtime_error("UnknownChar");
		});
	}
	catch(runtime_error)
	{
		output << "Wrong format of input: " << s << endl;
		return false;
	}
	return true;
}


