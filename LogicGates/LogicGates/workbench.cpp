#include "workbench.h"



Workbench::Workbench(size_t inputSize, size_t outputSize) : status(UnderConstruction), inputSize(inputSize), outputSize(outputSize)
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
	gateTypes["XOR"] = (make_unique<XnorGate>());
	gateTypes["DOUBLE"] = (make_unique<DoubleGate>());

	//prepare io gates, add their names 
	inputGate = make_unique<InputGate>(inputSize);
	outputGate = make_unique<OutputGate>(outputSize);
	inputVertex = graph->add_vertex(inputGate.get()); 
	outputVertex = graph->add_vertex(outputGate.get());
	vertexNames["I"] = inputVertex;
	vertexNames["O"] = outputVertex;
	//add empty pins 
	unordered_set<size_t>& in = unconnectedOutPins[inputVertex]; 
	unordered_set<size_t>& out = unconnectedInPins[outputVertex];
	
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
	for_each(vertexNames.begin(), vertexNames.end(), [&out](pair<string, Gate*>& i) {out->push_back(i.first); });
	return std::move(out);
}
//List all types
const unique_ptr<vector<string>> Workbench::ListOfType() const
{
	unique_ptr<vector<string>> out; 
	for_each(gateTypes.begin(), gateTypes.end(), [&out](auto&& i) {out->push_back(i->Name()); });
	return std::move(out);
}

//Add new gate to the network with "name" of "typeName". If typeName does not exist as a type throws out_of_range exception. If vertex named same as name throws. 
void Workbench::Add(std::string name, std::string typeName)
{
	//Check existing type and unambiguity of name
	if (gateTypes.find(typeName) == gateTypes.end())
		throw utype;
	if (vertexNames.find(name) != vertexNames.end())
		throw iname;
	//Get type 
	Gate * g = gateTypes[typeName].get();
	//Add new vertex of given type to the graph 
	vertexNames[name] = graph->add_vertex(g);
	//add free ids of pins 
	for (size_t i = 0; i < vertexNames[name]->value->GetLengthOfInput();i++)
	{
		unconnectedInPins[vertexNames[name]].insert(i);
	}
	for (size_t i = 0; i < vertexNames[name]->value->GetLengthOfOutput();i++)
	{
		unconnectedOutPins[vertexNames[name]].insert(i);
	}
}
//Connect fromName vertex to toName vertex using given pins IDs - if names of vector does not exists throw 
void Workbench::Connect(std::string fromName, std::size_t fromPin, std::string toName, std::size_t toPin)
{
	//Find vertex
	if (vertexNames.find(fromName) == vertexNames.end())
		throw uname;
	if (vertexNames.find(toName) == vertexNames.end())
		throw uname;

	gvertex from = vertexNames[fromName];
	gvertex to = vertexNames[toName];

	//check if pins are free
	if (unconnectedInPins[from].find(fromPin) == unconnectedInPins[from].end())
		throw opin; 
	if (unconnectedOutPins[to].find(toPin) == unconnectedOutPins[to].end())
		throw opin; 

	//make connection between from and  to 
	graph->connect(from, to, make_unique<Signal>(Floating, fromPin, toPin));

	//Remove connected pins 
	unconnectedInPins[to].erase(toPin);
	unconnectedOutPins[from].erase(fromPin);
}

//Check correction of logic network
bool Workbench::ConstructBench()
{
	//TODO: log 
	//Check if all pins are connected
	if (any_of(unconnectedInPins.begin(), unconnectedInPins.end(), [](unordered_set<size_t>& set) {return !set.empty(); }))
		return false; 
	if (any_of(unconnectedOutPins.begin(), unconnectedOutPins.end(), [](unordered_set<size_t>& set) {return !set.empty(); }))
		return false; 
	//Check availability, 
	//TODO:copy of vertex pointers
	vector<gvertex> startingPoints = ConstGates;
	startingPoints.push_back(inputVertex);
	if(!graph->all_vertices_available_from(startingPoints))
		return false;

	//Check cycles 
	//TODO:copy of vertex pointers
	if (graph->cycle_detection())
		return false; 
	status = Constructed;

	return true; 
}

bool Workbench::SetInput(vector<bool> input)
{
	vector<gvertex> followingTact; 

	vector<gvertex> actualTact = ConstGates; 
	actualTact.push_back(inputVertex);
	
	for (size_t i = 0; i < actualTact.size(); i++)
	{
		f
	}
	

}

