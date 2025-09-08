#include "ManPageTexts.hpp"

class ManPagetexts{
    public :
        static const QString page_One_Text = 
        "<div style='padding:15px; font-family:Segoe UI, sans-serif; font-size:12pt;'>"

    "<h2 style='color:#4a90e2; text-align:center;'>🌳 Yggdrasilleon Graph Algorithm Visualizer – User Guide 🌳</h2>"
    "<p><b>Yggdrasilleon</b> is an interactive graph engine for building, visualizing, and simulating algorithms on graphs. "
    "Use the controls on the left to create and manipulate your graph, then run algorithms to see step-by-step execution.</p>"

    "<h3 style='color:#2d8659;'>⚡ Core Features</h3>"
    "<ul>"
    "<li>➕ <b>Add Node</b> – Enter a node ID and insert it into the graph.</li>"
    "<li>🔗 <b>Add Edge</b> – Provide From/To nodes, optional weight, and direction.</li>"
    "<li>❌ <b>Delete Edge</b> – Remove an edge between two nodes.</li>"
    "<li>🧹 <b>Clear Graph</b> – Reset the entire graph.</li>"
    "<li>📝 <b>Clear Output</b> – Clear only the algorithm results.</li>"
    "</ul>"

    "<h3 style='color:#2d8659;'>🧮 Algorithms Supported</h3>"
    "<ul>"
    "<li>🔍 <b>DFS</b> (Depth-First Search)</li>"
    "<li>🔎 <b>BFS</b> (Breadth-First Search)</li>"
    "<li>🛣️ <b>Dijkstra</b> (Shortest Path)</li>"
    "<li>⚖️ <b>Bellman-Ford</b> (Shortest Path with negatives)</li>"
    "<li>🌐 <b>Floyd-Warshall</b> (All-Pairs Shortest Paths)</li>"
    "<li>🌲 <b>Prim's Algorithm to find Minimal Spanning Tree </b></li>"
    "<li>🌳 <b>Kruskal's Algorithm to find Minimal Spanning Tree </b></li>"
    "</ul>"
    "<p style='color:#cc6600;'><i>Note: Start/End node fields are disabled for MST algorithms.</i></p>"

    "<h3 style='color:#2d8659;'>🎨 Visualization & Output</h3>"
    "<ul>"
    "<li>✨ Step-by-step simulation with node/edge highlighting.</li>"
    "<li>📜 Output box shows traversal, paths, or MST steps.</li>"
    "<li>🔄 Graph updates automatically as you edit it.</li>"
    "</ul>"

    "<h3 style='color:#2d8659;'>🛠️ Utility Features</h3>"
    "<ul>"
    "<li>💾 <b>Save Output</b> – Export graph + results to file.</li>"
    "<li>🌞🌙 <b>Toggle Theme</b> – Switch between Light and Dark modes.</li>"
    "<li>❔ <b>Help</b> – Opens this guide.</li>"
    "</ul>"

    "<h3 style='color:#2d8659;'>👨‍💻 Developer Info</h3>"
    "<p><b>Created by:</b> <span style='color:#4a90e2;'>Souradip Patra</span><br>"
    "Project: <i>Yggdrasilleon – Graph Algorithm Visualizer </i></p>"

    "</div>";

    static const QString page_Two_Text = 
    "<h2 style='color:#2196f3;'>🧮 Algorithms</h2>"
            "<ul>"
                "<li><b>DFS (Depth-First Search):</b> Explores as far as possible along each branch before backtracking. It uses a stack (implicit via recursion or explicit). Useful for path finding, cycle detection, and topological sorting in directed acyclic graphs.</li>"
                "<li><b>BFS (Breadth-First Search):</b> Explores neighbors level by level, ensuring the shortest path in terms of edge count is found in unweighted graphs. It uses a queue and is also applied in bipartite checking and shortest path problems without weights.</li>"
                "<li><b>Dijkstra’s Algorithm:</b> A greedy algorithm that finds the shortest path from a single source to all other nodes, provided all edge weights are non-negative. It repeatedly selects the node with the smallest tentative distance and updates its neighbors.</li>"
                "<li><b>Bellman-Ford Algorithm:</b> Handles graphs with negative weights by relaxing all edges N−1 times (where N is the number of vertices). It can also detect negative weight cycles, which Dijkstra cannot handle.</li>"
                "<li><b>Floyd-Warshall Algorithm:</b> A dynamic programming algorithm that computes shortest paths between all pairs of nodes. It systematically considers each node as an intermediate point and updates distances accordingly. Can detect negative cycles as well.</li>"
                "<li><b>Prim’s MST Algorithm:</b> A greedy algorithm that builds a Minimum Spanning Tree by starting from a chosen node and repeatedly adding the smallest edge that connects a visited node to an unvisited one.</li>"
                "<li><b>Kruskal’s MST Algorithm:</b> Builds a Minimum Spanning Tree by sorting all edges by weight and adding them one by one, provided they don’t form a cycle (detected using a union-find/disjoint-set structure).</li>"
                "<li><b>Cycle Detection:</b> Determines if the graph contains cycles. In undirected graphs, DFS detects back-edges. In directed graphs, DFS with a recursion stack or topological sort can detect cycles.</li>"
        "</ul>";


    static const QString page_Three_Text =
    "<h2 style='color:#2196f3;'>📖 Graph Terms</h2>"
            "<ul>"
                "<li><b>Node / Vertex:</b> A fundamental unit in a graph that represents an entity (e.g., a city, person, or computer). Nodes are connected by edges.</li>"
                "<li><b>Edge:</b> A link connecting two nodes. Edges may have weights (representing cost, distance, or time) and may be directed or undirected.</li>"
                "<li><b>Directed Edge:</b> An edge that has a direction (A → B) meaning traversal is allowed only one way. Common in road networks or dependency graphs.</li>"
                "<li> <b>Undirected Edge:</b> An edge with no direction (A — B) meaning traversal is possible in both directions. Common in social networks or physical connections.</li>"
                "<li><b>Weighted Edge:</b> An edge with an associated numerical value, representing cost, distance, time, or capacity.</li>"
                "<li><b>Path:</b> A sequence of edges connecting one node to another without necessarily being unique. The length of a path can be the number of edges or the sum of edge weights.</li>"
                "<li><b>Cycle:</b> A path that starts and ends at the same node without repeating edges (in simple cycles). Cycles are crucial in detecting deadlocks or infinite loops.</li>"
                "<li><b>Connected Graph:</b> An undirected graph where every pair of nodes has at least one path connecting them. If not, the graph is disconnected.</li>"
                "<li><b>Strongly Connected Graph:</b> A directed graph where every node can reach every other node following directed edges.</li>"
                "<li><b>Tree:</b> A connected, acyclic graph with N nodes and N−1 edges. It’s a minimal connected structure with no cycles.</li>"
                "<li><b>Spanning Tree:</b> A subgraph that connects all nodes of the original graph with no cycles. Used in MST algorithms.</li>"
                "<li><b>MST (Minimum Spanning Tree):</b> A spanning tree with the minimum possible total edge weight. Ensures connectivity with minimal cost.</li>"
                "<li><b>Degree:</b> The number of edges incident on a node. In directed graphs, distinguished as in-degree (incoming edges) and out-degree (outgoing edges).</li>"
            "</ul>";
};

