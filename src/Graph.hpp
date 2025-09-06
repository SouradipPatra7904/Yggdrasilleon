#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

struct Edge {
    std::string from;
    std::string to;
    std::optional<int> weight;
    bool directed;
};

class Graph {
public:
    void addNode(const std::string &id);

    void addEdge(const std::string &from, const std::string &to,
                 std::optional<int> weight = std::nullopt,
                 bool directed = false);

    void removeEdge(const std::string &from, const std::string &to, bool directed = false);


    bool hasNode(const std::string &id) const;
    std::vector<std::string> nodes() const;
    std::vector<Edge> edges() const;
    std::vector<Edge> neighbors(const std::string &id) const;

    void clear();

private:
    std::unordered_map<std::string, std::vector<Edge>> adjacency;
};

#endif // GRAPH_HPP
