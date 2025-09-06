#include "Graph.hpp"
#include <algorithm>

// Add a node to the adjacency map if it doesn't already exist
void Graph::addNode(const std::string &id) {
    if (adjacency.find(id) == adjacency.end()) {
        adjacency[id] = std::vector<Edge>{};
    }
}

// Add an edge (optionally weighted, optionally directed).
// If the edge is undirected, we also add the reverse edge.
void Graph::addEdge(const std::string &from, const std::string &to,
                    std::optional<int> weight, bool directed) {
    // ensure both nodes exist
    addNode(from);
    addNode(to);

    Edge e;
    e.from = from;
    e.to = to;
    e.weight = weight;
    e.directed = directed;

    adjacency[from].push_back(e);

    // If the edge is undirected, also add reverse edge (with directed=false).
    if (!directed) {
        Edge rev;
        rev.from = to;
        rev.to = from;
        rev.weight = weight;
        rev.directed = false;
        adjacency[to].push_back(rev);
    }
}

void Graph::removeEdge(const std::string &from, const std::string &to, bool directed)
{
    auto itFrom = adjacency.find(from);
    auto itTo = adjacency.find(to);
    if(itFrom != adjacency.end()) {
        itFrom->second.erase(std::remove_if(itFrom->second.begin(), itFrom->second.end(),
                                           [&](const Edge &e){ return e.to == to; }),
                             itFrom->second.end());
    }
    if(!directed && itTo != adjacency.end()) {
        itTo->second.erase(std::remove_if(itTo->second.begin(), itTo->second.end(),
                                         [&](const Edge &e){ return e.to == from; }),
                           itTo->second.end());
    }
}

bool Graph::hasNode(const std::string &id) const {
    return adjacency.find(id) != adjacency.end();
}

std::vector<std::string> Graph::nodes() const {
    std::vector<std::string> out;
    out.reserve(adjacency.size());
    for (const auto &p : adjacency) {
        out.push_back(p.first);
    }
    return out;
}

std::vector<Edge> Graph::edges() const {
    std::vector<Edge> out;
    out.reserve(64);

    for (const auto &pair : adjacency) {
        const std::string &u = pair.first;
        for (const auto &e : pair.second) {
            // For directed edges: include as-is.
            if (e.directed) {
                out.push_back(e);
            } else {
                // For undirected edges we've stored both directions in adjacency.
                // To avoid returning duplicates, only include the edge where from <= to lexicographically.
                if (e.from <= e.to) {
                    out.push_back(e);
                }
            }
        }
    }
    return out;
}

std::vector<Edge> Graph::neighbors(const std::string &id) const {
    auto it = adjacency.find(id);
    if (it == adjacency.end()) return {};
    return it->second; // copy
}

void Graph::clear()
{
    adjacency.clear();
}
