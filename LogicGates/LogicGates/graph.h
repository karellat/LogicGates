#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>
using namespace std;

template <typename VertexValue>
struct Vertex {
	VertexValue value;
	Vertex(VertexValue v) : value(v) {}
};

template <typename VertexValue, typename EdgeValue>
struct Edge {
	using vtype = Vertex<VertexValue>;

	vtype* from;
	vtype* to;
	EdgeValue value;

	Edge(vtype* from, vtype* to, EdgeValue value) : from(from), to(to), value(value) {}
};

template <typename VertexValue, typename EdgeValue>
struct Graph {
	using vtype = Vertex<VertexValue>;
	using etype = Edge<VertexValue, EdgeValue>;

	vector<unique_ptr<vtype>> vertices;
	vector<unique_ptr<etype>> edges;


	vtype * add_vertex(VertexValue value) {
		vertices.push_back(make_unique<vtype>(value));
		auto&& last = vertices.back();
		return last.get();
	}

	etype* connect(vtype* a, vtype* b, EdgeValue value) {
		edges.push_back(make_unique<etype>(a, b, value));
		auto&& last = edges.back();
		return last.get();
	}

	void disconnect(etype* e) {
		edges.erase(std::remove_if(edges.begin(), edges.end(), [this, e](auto&& edge) { return edge.get() == e; }));
	}

	vector<etype*> edges_from(vtype* a)
	{ 
		vector<etype *> output; 
		for (auto&& edge : edges)
			if (edge.get()->from == a)
				output.push_back(edge.get());
		return output; 
	}

	vector<etype *> edges_to(vtype* a)
	{
		vector<etype *> output; 
		for (auto&& edge : edges)
			if (edge.get()->to == a)
				output.push_back(edge.get());
		return output;
	}

    vtype * find(VertexValue value,vtype * starting_vertex)
	{
		std::queue<vtype *> q; 
		q.push(starting_vertex); 

		while(q.size() != 0)
		{
			vtype* v = q.front();
			q.pop();
			
			if (v->value == value)
				return v; 

			for (auto i : edges_from(v))
				q.push(i->to);
		}
		return nullptr; 
	}
};


