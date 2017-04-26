#include "gates.h" 
#include <set>


std::unique_ptr<UserDefinedGate> UserDefinedGateModel::getGate(size_t id)
{
	unique_ptr<UserDefinedGate> o = make_unique<UserDefinedGate>(UserDefinedGate(this->input_size, this->output_size, this->name, id,this));
	return std::move(o);
}

std::vector<bool> UserDefinedGateModel::Update(std::vector<bool> input)
{
	for (auto o : outputGates)
		o->value->Reset();
	if (input.size() != input_size)
		throw new runtime_error("Not suitable lenght of input ");

	//Same code as for workbench
	size_t tacts = 0;
	set<gvertex> following_tact_gates;
	for (std::size_t i = 0; i < inputGates.size(); i++)
	{
		gvertex v = inputGates[i];
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

	for (auto i : constGates)
	{
		vector<gedge> edges = graph->edges_from(i);
		vector<bool> blank;
		for (auto e : edges)
		{
			following_tact_gates.insert(e->to);
			if (i->value->Update(blank)[0])
				e->value->status = One;
			else
				e->value->status = Zero;
		}
	}

	set<gvertex> actual_tact_gates = following_tact_gates;
	following_tact_gates.clear();
	while (any_of(outputGates.begin(), outputGates.end(), [](gvertex o_g) {return !(o_g->value->result); }))
	{
		for (auto av : actual_tact_gates)
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
			for (auto oe : output_edges)
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

	vector<bool> o; 
	for (auto i : outputGates)
	{
		o.push_back(i->value->resultValues[0]);
	}

	return o;

}
