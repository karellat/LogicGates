#include "workbench.h"


Workbench::Workbench(size_t inputSize, size_t outputSize) : status(UnderConstruction)
{
	if(outputSize == 0 )
	{
		throw isize; 
	}
	//Graph & Standart Gates  init  
	graph = std::make_unique<Graph<Gate*, std::unique_ptr<Signal>>>(); 
	gateTypes["BLANK"] =(make_unique<BlankGate>());
	gateTypes["CONST0"] = (make_unique<ConstGate0>());
	gateTypes["CONST1"] = (make_unique<ConstGate1>());
	gateTypes["NOT"] = (make_unique<NotGate>());
	gateTypes["OR"] = (make_unique<OrGate>());
	gateTypes["AND"] = (make_unique<AndGate>());
	gateTypes["XOR"] = (make_unique<XorGate>());
	gateTypes["NAND"] = (make_unique<NandGate>());
	gateTypes["NOR"] = (make_unique<NorGate>());
	gateTypes["XNOR"] = (make_unique<XnorGate>());
	gateTypes["DOUBLE"] = (make_unique<DoubleGate>());

	//prepare io gates, add their names 
	inputGate = make_unique<InputGate>(inputSize);
	outputGate = make_unique<OutputGate>(outputSize);
	inputVertex = graph->add_vertex(inputGate.get()); 
	outputVertex = graph->add_vertex(outputGate.get());
	vertexNames["I"] = inputVertex;
	vertexNames["O"] = outputVertex;
	//add empty pins 
	unordered_set<size_t>& in = unconnectedFromPins[inputVertex]; 
	unordered_set<size_t>& out = unconnectedToPins[outputVertex];
	
	for (size_t i = 0; i < inputSize; i++)
	{
		in.insert(i);
	}
	for (size_t i = 0; i < outputSize; i++)
	{
		out.insert(i);
	}

	
	
}
//List all names of actual network 
const unique_ptr<vector<string>> Workbench::ListOfNamedVertex() const
{
	unique_ptr<vector<string>> out; 
	//TODO: Fix
	//for_each(vertexNames.begin(), vertexNames.end(), [&out](pair<string, Gate*>& i) {out->push_back(i.first); });
	return std::move(out);
}
//List all types
const unique_ptr<vector<string>> Workbench::ListOfType() const
{
	unique_ptr<vector<string>> out; 
	//TODO: Fix
	//for_each(gateTypes.begin(), gateTypes.end(), [&out](pair<string,unique_ptr<Gate>> p) {out->push_back(p.first); });
	return std::move(out);
}

//Add new gate to the network with "name" of "typeName". If typeName does not exist as a type throws out_of_range exception. If vertex named same as name throws. 
void Workbench::Add(const std::string& name, const std::string& typeName)
{
	//Check existing type and unambiguity of name
	if (gateTypes.find(typeName) == gateTypes.end())
		throw utype;
	if (vertexNames.find(name) != vertexNames.end())
		throw iname;
	//Get type 
	Gate * g = gateTypes[typeName].get();
	//Add new vertex of given type to the Graph 
	vertexNames[name] = graph->add_vertex(g);
	if(typeid(*g) == typeid(ConstGate0) || typeid(*g) == typeid(ConstGate1))
	{
		constGates.push_back(vertexNames[name]);
	}
	//add free ids of pins 
	for (size_t i = 0; i < vertexNames[name]->value->GetLengthOfInput(); i++)
	{
		unconnectedToPins[vertexNames[name]].insert(i);
	}
	for (size_t i = 0; i < vertexNames[name]->value->GetLengthOfOutput(); i++)
	{
		unconnectedFromPins[vertexNames[name]].insert(i);
	}
}
//Connect fromName vertex to toName vertex using given pins IDs - if names of vector does not exists throw 
void Workbench::Connect(const std::string& fromName, std::size_t fromPin, const std::string& toName, std::size_t toPin)
{
	//Find vertex
	if (vertexNames.find(fromName) == vertexNames.end())
		throw uname;
	if (vertexNames.find(toName) == vertexNames.end())
		throw uname;

	gvertex from = vertexNames[fromName];
	gvertex to = vertexNames[toName];

	//check if pins are free
    //TODO: remove debug code;
	//unordered_set<size_t>& fromP = unconnectedFromPins[from];
	//unordered_set<size_t>& toP = unconnectedToPins[to];

	if (unconnectedToPins[to].find(toPin) == unconnectedToPins[to].end())
		throw opin; 
	if (unconnectedFromPins[from].find(fromPin) == unconnectedFromPins[from].end())
		throw opin; 

	//make connection between from and  to 
	graph->connect(from, to, make_unique<Signal>(Floating, fromPin, toPin));

	//Remove connected pins 
	unconnectedToPins[to].erase(toPin);
	unconnectedFromPins[from].erase(fromPin);
}
//Check correction of logic network
void Workbench::ConstructBench()
{
	//TODO: log 
	//Check if all pins are connected
	if (any_of(unconnectedToPins.begin(), unconnectedToPins.end(), [](auto&& p) {return !p.second.empty(); }))
		throw fpin;
	if (any_of(unconnectedFromPins.begin(), unconnectedFromPins.end(), [](auto&& p) {return !p.second.empty(); }))
		throw fpin;
	//Check availability, 
	//TODO:copy of vertex pointers
	vector<gvertex> startingPoints = constGates;
	startingPoints.push_back(inputVertex);
	if (!graph->all_vertices_available_from(startingPoints))
		throw npart;

	//Check cycles 
	//TODO:copy of vertex pointers
	if (graph->cycle_detection())
		throw dcycle;
	
	status = Constructed;
}

//Simulate evaluation of the logical network 
void Workbench::SetInput(const vector<bool>& input)
{
	//Prepare starting vertex, const & input 
	unordered_set<gvertex> followingTact; 
	unordered_set<gvertex> actualTact; 
	bool outputSet = false; 
	vector<gedge> inputFrom = graph->edges_from(inputVertex);
	for (auto i : inputFrom)
	{
		i->value->status = input[i->value->fromID] ? One : Zero;
		actualTact.insert(i->to);
	}
	for (auto c : constGates)
	{
		vector<gedge> constFrom = graph->edges_from(c);
		for (auto i : constFrom)
		{
			vector<bool> blank; 
			i->value->status = c->value->Update(blank)[0] ? One : Zero;
 			actualTact.insert(i->to);
		}
	}
	//While not all output sets due to cycle and availability check, there is always a way to set all outputs 
	while (!outputSet)
	{

		for (auto g : actualTact)
		{
			//Read inputs, if any of them  floating add vertex to  following tact
			bool evaluated = true;
			vector<gedge> toG = graph->edges_to(g);
			vector<bool> inputG;
			inputG.resize(g->value->GetLengthOfInput());
			for (auto i : toG)
			{
				if (i->value->status == One)
				{
					inputG[i->value->toID] = true;
				}
				else if (i->value->status == Zero)
				{
					inputG[i->value->toID] = false;
				}
				else
				{
					evaluated = false;
					followingTact.insert(g);
					break;
				}
			}
			if (!evaluated) continue;

			//decided if OutputVertex is set;
			if (g == outputVertex)
			{
				outputSet = true;
				break;

			}
			//Count logical function of gate & set outputs
			vector<bool> outputG = g->value->Update(inputG);
			vector<gedge> fromG = graph->edges_from(g);
			for (auto i : fromG)
			{
				if (outputG[i->value->fromID])
				{
					i->value->status = One;
				}
				else
				{
					i->value->status = Zero;
				}
				followingTact.insert(i->to);
			}

		}
		actualTact = followingTact;
		followingTact.clear();
	}
	status = Calculated;
}
//Return  read output 
vector<bool> Workbench::ReadOutput() const
{
	if (status != Calculated) throw istat;
	vector<bool> outputBool;
	outputBool.resize(SizeOfOutput());
	vector<gedge> toOutput = graph->edges_to(outputVertex);
	for (auto i : toOutput)
	{
		if (i->value->status == One)
			outputBool[i->value->toID] = true;
		else if (i->value->status == Zero)
			outputBool[i->value->toID] = false;
		else
			throw istat;
	}

	return std::move(outputBool);
}

void Workbench::ConstructUserGate(const string& name, size_t newInputSize, size_t newOutputSize)
{
	if (status == UnderConstruction)
		throw istat;
	//Create new gate type with selected name 
	unique_ptr<UserDefinedGateModel> newUserGate = make_unique<UserDefinedGateModel>(std::move(graph), std::move(inputGate), inputVertex,
		std::move(outputGate), outputVertex,constGates, name);
	//Check name of type
	if (gateTypes.find(name) != gateTypes.end())
		throw itype; 
	gateTypes[name] = std::move(newUserGate);
	ResetWorkbench(false, newInputSize, newOutputSize);
}

void Workbench::ResetWorkbench(bool deleteUDG, size_t newInputSize, size_t newOutputSize)
{
	if (newOutputSize == 0)
	{
		throw isize; 
	}
	//Create new graph for logical network 
	graph = std::make_unique<Graph<Gate*, std::unique_ptr<Signal>>>();
	//if deleteUDG reset created types 
	if(deleteUDG)
	{
		gateTypes.clear(); 
		gateTypes["BLANK"] = (make_unique<BlankGate>());
		gateTypes["CONST0"] = (make_unique<ConstGate0>());
		gateTypes["CONST1"] = (make_unique<ConstGate1>());
		gateTypes["NOT"] = (make_unique<NotGate>());
		gateTypes["OR"] = (make_unique<OrGate>());
		gateTypes["AND"] = (make_unique<AndGate>());
		gateTypes["XOR"] = (make_unique<XorGate>());
		gateTypes["NAND"] = (make_unique<NandGate>());
		gateTypes["NOR"] = (make_unique<NorGate>());
		gateTypes["XOR"] = (make_unique<XnorGate>());
		gateTypes["DOUBLE"] = (make_unique<DoubleGate>());
	}
	//clear old constGates
	constGates.clear();
	//clear old vertex names
	vertexNames.clear();
	unconnectedFromPins.clear();
	unconnectedToPins.clear();
	//create new io gate of given sizes 
	inputGate = make_unique<InputGate>(newInputSize);
	outputGate = make_unique<OutputGate>(newOutputSize);
	inputVertex = graph->add_vertex(inputGate.get());
	outputVertex = graph->add_vertex(outputGate.get());
	vertexNames["I"] = inputVertex;
	vertexNames["O"] = outputVertex;
	//add empty pins 
	unordered_set<size_t>& in = unconnectedFromPins[inputVertex];
	unordered_set<size_t>& out = unconnectedToPins[outputVertex];

	for (size_t i = 0; i < newInputSize; i++)
	{
		in.insert(i);
	}
	for (size_t i = 0; i < newOutputSize; i++)
	{
		out.insert(i);
	}
}

