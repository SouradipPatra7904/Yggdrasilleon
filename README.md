# 🌳 Yggdrasilleon – Graph Algorithm Visualizer

**Yggdrasilleon** is an interactive **graph visualization and algorithm simulation tool**, written in **C++ with Qt**.  
It allows users to build graphs, run classical graph algorithms, and see **step-by-step visual execution** with a modern GUI.  

----------------------------------------------------------------------------------------------------

## ✨ Features :

- 🎨 **Interactive Graph Builder**  
  - Add / delete nodes  
  - Add / delete edges (directed or weighted)  
  - Clear graph / clear output  

- 🧮 **Algorithm Simulations**  
  - DFS (Depth-First Search)  
  - BFS (Breadth-First Search)  
  - Dijkstra (Shortest Path)  
  - Bellman-Ford (Shortest Path with negatives)  
  - Floyd-Warshall (All-Pairs Shortest Paths)  
  - Prim’s Algorithm to find Minimal Spanning Tree
  - Kruskal’s Algorithm to find Minimal Spanning Tree

- 📜 **Output Window**  
  - Traversal orders, shortest paths, or MST construction steps  
  - Can be cleared independently  

- 🛠️ **Utility Features**  
  - Save graph specification + output as `.txt`  
  - Toggle Light/Dark theme  
  - Help window with user guide and developer info  

----------------------------------------------------------------------------------------------------

## ⚡ Getting Started

### 🔧 Prerequisites

- **Qt 6.x** (Widgets module)  
- **CMake 3.16+**  
- A modern **C++17 compiler** (GCC, Clang, or MSVC)  

### 📥 Clone the Repository :

    [bash]
        $ git clone https://github.com/SouradipPatra/Yggdrasilleon.git
        $ cd Yggdrasilleon
        $ mkdir build && cd build
        $ cmake ..
        $ make
        $ ./Yggdrasilleon

----------------------------------------------------------------------------------------------------

### 🎮 Usage :

    1.  Add nodes & edges using the left panel.
    2.  Select an algorithm from the dropdown.
    3.  Enter start/end nodes (for path algorithms).
            Note: Start/End fields are disabled for MST algorithms.
    4.  Click Run Algorithm to see step-by-step simulation.
    5.  Use Clear Graph / Clear Output buttons to reset.
    6.  Save results with Save Output.
    7.  Switch between Light/Dark themes.
    8.  Access documentation anytime via the Help button.

----------------------------------------------------------------------------------------------------
