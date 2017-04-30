#include "gates.h" 
#include <set>
#include "workbench.h"


std::vector<bool> UserDefinedGateModel::Update(const std::vector<bool>& input)
{
	vector<gedge> toOutput = graph->edges_to(outputVertex);
	for (auto i : toOutput)
	{
		i->value->status = Floating;
	}
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
			//Clean value from ingoing signal 
			for (auto i : toG)
			{
				i->value->status = Floating;
			}
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
				//if no discovered vertex add to the following tact
				if (actualTact.find(i->to) == actualTact.end())
					followingTact.insert(i->to);
			}

		}
		actualTact = followingTact;
		followingTact.clear();
	}

	vector<bool> outputBool;
	outputBool.resize(outputGate->GetLengthOfInput());
	vector<gedge> toOutputEdges = graph->edges_to(outputVertex);
	for (auto i : toOutputEdges)
	{
		if (i->value->status == One)
			outputBool[i->value->toID] = true;
		else if (i->value->status == Zero)
			outputBool[i->value->toID] = false;
		else
		{
			//TODO: own expection 
			throw new exception;
		}
	}
	return std::move(outputBool);
	
}
