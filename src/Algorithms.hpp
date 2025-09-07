#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "Graph.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

using StepCallback = std::function<void(const std::string&)>;

// ---- Traversals ----
void dfs(const Graph &graph, const std::string &start, StepCallback callback);
void bfs(const Graph &graph, const std::string &start, StepCallback callback);

// ---- Shortest Paths ----
void dijkstra(const Graph &graph, const std::string &start, StepCallback callback);
void bellmanFord(const Graph &graph, const std::string &start, StepCallback callback);
void floydWarshall(const Graph &graph, StepCallback callback);

// ---- Minimum Spanning Trees ----
void primMST(const Graph &graph, const std::string &start, StepCallback callback);
void kruskalMST(const Graph &graph, StepCallback callback);

// ---- Cycle detection entry points ----
    void detectCycles(const Graph &graph, std::function<void(const std::string&)> callback);

#endif // ALGORITHMS_HPP
