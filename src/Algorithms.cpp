#include "Algorithms.hpp"
#include <queue>
#include <set>
#include <map>
#include <sstream>

// ------------------ DFS ------------------
void dfs(const Graph &graph, const std::string &start, StepCallback callback) {
    std::unordered_set<std::string> visited;
    std::function<void(const std::string&)> visit = [&](const std::string &node){
        visited.insert(node);
        callback("Visited: " + node);
        for (auto &edge : graph.neighbors(node)) {
            if (!visited.count(edge.to)) {
                visit(edge.to);
            }
        }
    };
    callback("Starting DFS from " + start);
    visit(start);
}

// ------------------ BFS ------------------
void bfs(const Graph &graph, const std::string &start, StepCallback callback) {
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;
    visited.insert(start);
    q.push(start);
    callback("Starting BFS from " + start);

    while (!q.empty()) {
        std::string node = q.front(); q.pop();
        callback("Visited: " + node);
        for (auto &edge : graph.neighbors(node)) {
            if (!visited.count(edge.to)) {
                visited.insert(edge.to);
                q.push(edge.to);
                callback("Queued: " + edge.to);
            }
        }
    }
}

// ------------------ Dijkstra ------------------
void dijkstra(const Graph &graph, const std::string &start, StepCallback callback) {
    std::unordered_map<std::string, int> dist;
    for (auto &node : graph.nodes()) dist[node] = std::numeric_limits<int>::max();
    dist[start] = 0;

    using P = std::pair<int, std::string>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0, start});
    callback("Starting Dijkstra from " + start);

    while (!pq.empty()) {
        auto [d, node] = pq.top(); pq.pop();
        if (d > dist[node]) continue;
        callback("Processing: " + node + " (distance " + std::to_string(d) + ")");
        for (auto &edge : graph.neighbors(node)) {
            int w = edge.weight.value_or(1);
            if (dist[edge.to] > dist[node] + w) {
                dist[edge.to] = dist[node] + w;
                pq.push({dist[edge.to], edge.to});
                callback("Updating: " + edge.to + " to " + std::to_string(dist[edge.to]));
            }
        }
    }
}

// ------------------ Bellman-Ford ------------------
void bellmanFord(const Graph &graph, const std::string &start, StepCallback callback) {
    std::unordered_map<std::string, int> dist;
    for (auto &node : graph.nodes()) dist[node] = std::numeric_limits<int>::max();
    dist[start] = 0;
    callback("Starting Bellman-Ford from " + start);

    int N = graph.nodes().size();
    for (int i = 0; i < N-1; ++i) {
        for (auto &edge : graph.edges()) {
            int w = edge.weight.value_or(1);
            if (dist[edge.from] != std::numeric_limits<int>::max() &&
                dist[edge.to] > dist[edge.from] + w) {
                dist[edge.to] = dist[edge.from] + w;
                callback("Updated " + edge.to + " to " + std::to_string(dist[edge.to]));
            }
        }
    }
}

// ------------------ Floyd-Warshall ------------------
void floydWarshall(const Graph &graph, StepCallback callback) {
    auto nodes = graph.nodes();
    std::unordered_map<std::string, std::unordered_map<std::string, int>> dist;
    for (auto &u : nodes) {
        for (auto &v : nodes) {
            dist[u][v] = (u == v) ? 0 : std::numeric_limits<int>::max();
        }
    }
    for (auto &edge : graph.edges()) {
        dist[edge.from][edge.to] = edge.weight.value_or(1);
    }

    callback("Starting Floyd-Warshall");
    for (auto &k : nodes)
        for (auto &i : nodes)
            for (auto &j : nodes) {
                if (dist[i][k] != std::numeric_limits<int>::max() &&
                    dist[k][j] != std::numeric_limits<int>::max() &&
                    dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    callback("Updated distance: " + i + " -> " + j + " = " + std::to_string(dist[i][j]));
                }
            }
}

// ------------------ Prim's MST ------------------
void primMST(const Graph &graph, const std::string &start, StepCallback callback) {
    std::unordered_set<std::string> inMST;
    using P = std::pair<int, std::pair<std::string, std::string>>; // weight, {u,v}
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    // Result container: list of edges in MST
    std::vector<Edge> mstEdges;
    long long totalWeight = 0;

    // start
    inMST.insert(start);
    for (auto &e : graph.neighbors(start))
        pq.push({e.weight.value_or(1), {start, e.to}});

    callback("Starting Prim's MST from " + start);
    while (!pq.empty()) {
        auto [w, nodesPair] = pq.top(); pq.pop();
        auto [u, v] = nodesPair;
        if (inMST.count(v)) {
            callback("Skipping edge (already in MST or would form cycle): " + u + " - " + v + " (weight " + std::to_string(w) + ")");
            continue;
        }

        // accept edge u-v
        inMST.insert(v);
        Edge chosen; chosen.from = u; chosen.to = v; chosen.weight = w; chosen.directed = false;
        mstEdges.push_back(chosen);
        totalWeight += w;
        callback("Edge added to MST: " + u + " - " + v + " (weight " + std::to_string(w) + ")");

        for (auto &e : graph.neighbors(v)) {
            if (!inMST.count(e.to))
                pq.push({e.weight.value_or(1), {v, e.to}});
        }
    }

    // Compose final machine-parsable MST_RESULT message
    std::ostringstream oss;
    oss << "MST_RESULT: total=" << totalWeight << "; edges=";
    bool first = true;
    for (auto &e : mstEdges) {
        if (!first) oss << ",";
        oss << e.from << "-" << e.to << ":" << e.weight.value_or(1);
        first = false;
    }
    callback(oss.str());
}

// ------------------ Kruskal's MST ------------------
void kruskalMST(const Graph &graph, StepCallback callback) {
    auto nodes = graph.nodes();
    std::unordered_map<std::string, std::string> parent;
    std::unordered_map<std::string, int> rankv;
    for (auto &n : nodes) { parent[n] = n; rankv[n] = 0; }

    std::function<std::string(const std::string&)> find = [&](const std::string &x)->std::string {
        std::string r = x;
        while (parent[r] != r) r = parent[r];
        // path compression
        std::string cur = x;
        while (parent[cur] != r) {
            std::string next = parent[cur];
            parent[cur] = r;
            cur = next;
        }
        return r;
    };
    auto unite = [&](const std::string &x, const std::string &y) {
        std::string rx = find(x), ry = find(y);
        if (rx == ry) return;
        if (rankv[rx] < rankv[ry]) parent[rx] = ry;
        else if (rankv[ry] < rankv[rx]) parent[ry] = rx;
        else { parent[ry] = rx; rankv[rx]++; }
    };

    auto edges = graph.edges();
    std::sort(edges.begin(), edges.end(),
              [](const Edge &a, const Edge &b) { return a.weight.value_or(1) < b.weight.value_or(1); });

    callback("Starting Kruskal's MST");

    std::vector<Edge> mstEdges;
    long long totalWeight = 0;
    for (auto &e : edges) {
        std::string u = e.from, v = e.to;
        std::string ru = find(u), rv = find(v);
        callback("Considering edge " + u + " - " + v + " (weight " + std::to_string(e.weight.value_or(1)) + ")");
        if (ru != rv) {
            unite(u, v);
            mstEdges.push_back(e);
            totalWeight += e.weight.value_or(1);
            callback("Edge added to MST: " + u + " - " + v + " (weight " + std::to_string(e.weight.value_or(1)) + ")");
        } else {
            callback("Rejected (would form cycle): " + u + " - " + v);
        }
    }

    // Compose final machine-parsable MST_RESULT message
    std::ostringstream oss;
    oss << "MST_RESULT: total=" << totalWeight << "; edges=";
    bool first = true;
    for (auto &e : mstEdges) {
        if (!first) oss << ",";
        oss << e.from << "-" << e.to << ":" << e.weight.value_or(1);
        first = false;
    }
    callback(oss.str());
}
