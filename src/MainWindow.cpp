#include "MainWindow.hpp"
#include "ThemeManager.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

// Include algorithms
#include "Algorithms.hpp"

using StepCallback = std::function<void(const std::string&)>;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      graphWidget(new GraphWidget(this)),
      themeManager(new ThemeManager()),
      isDarkMode(false) {

    // Central Split View
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(createControlPanel());
    splitter->addWidget(graphWidget);
    splitter->setStretchFactor(1, 2);
    setCentralWidget(splitter);

    setWindowTitle("Yggdrasil - World Tree Graph Engine");
    resize(1200, 800);
}

QWidget* MainWindow::createControlPanel() {
    QWidget *panel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(panel);

    // Node Controls
    QGroupBox *nodeBox = new QGroupBox("Nodes", panel);
    QHBoxLayout *nodeLayout = new QHBoxLayout(nodeBox);
    nodeInput = new QLineEdit(nodeBox);
    QPushButton *addNodeBtn = new QPushButton("Add Node", nodeBox);
    connect(addNodeBtn, &QPushButton::clicked, this, &MainWindow::addNode);
    nodeLayout->addWidget(new QLabel("Node ID:"));
    nodeLayout->addWidget(nodeInput);
    nodeLayout->addWidget(addNodeBtn);

    // Edge Controls
    QGroupBox *edgeBox = new QGroupBox("Edges", panel);
    QFormLayout *edgeLayout = new QFormLayout(edgeBox);
    edgeFromInput = new QLineEdit(edgeBox);
    edgeToInput = new QLineEdit(edgeBox);
    weightInput = new QSpinBox(edgeBox);
    weightInput->setRange(-9999, 9999);
    directedInput = new QCheckBox("Directed", edgeBox);
    QPushButton *addEdgeBtn = new QPushButton("Add Edge", edgeBox);
    connect(addEdgeBtn, &QPushButton::clicked, this, &MainWindow::addEdge);

    edgeLayout->addRow("From:", edgeFromInput);
    edgeLayout->addRow("To:", edgeToInput);
    edgeLayout->addRow("Weight:", weightInput);
    edgeLayout->addRow("", directedInput);
    edgeLayout->addRow("", addEdgeBtn);

    // Algorithm Controls
    QGroupBox *algoBox = new QGroupBox("Algorithm", panel);
    QFormLayout *algoLayout = new QFormLayout(algoBox);
    algorithmBox = new QComboBox(algoBox);
    algorithmBox->addItems({
        "DFS", "BFS",
        "Dijkstra", "Bellman-Ford", "Floyd-Warshall",
        "Prim's MST", "Kruskal's MST"
    });
    startNodeInput = new QLineEdit(algoBox);
    endNodeInput = new QLineEdit(algoBox);
    runBtn = new QPushButton("Run", algoBox);
    connect(runBtn, &QPushButton::clicked, this, &MainWindow::runAlgorithm);

    algoLayout->addRow("Algorithm:", algorithmBox);
    algoLayout->addRow("Start Node:", startNodeInput);
    algoLayout->addRow("End Node:", endNodeInput);
    algoLayout->addRow("", runBtn);

    // Output Box
    QGroupBox *outputGroup = new QGroupBox("Output", panel);
    QVBoxLayout *outLayout = new QVBoxLayout(outputGroup);
    outputBox = new QTextEdit(outputGroup);
    outputBox->setReadOnly(true);
    outLayout->addWidget(outputBox);

    // Theme Toggle
    themeToggleBtn = new QPushButton("Toggle Theme", panel);
    connect(themeToggleBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);

    // Assemble Layout
    layout->addWidget(nodeBox);
    layout->addWidget(edgeBox);
    layout->addWidget(algoBox);
    layout->addWidget(outputGroup);
    layout->addWidget(themeToggleBtn);
    layout->addStretch();

    return panel;
}

void MainWindow::addNode() {
    QString id = nodeInput->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Node ID cannot be empty.");
        return;
    }
    graph.addNode(id.toStdString());
    graphWidget->setGraph(graph);
    nodeInput->clear();
}

void MainWindow::addEdge() {
    QString from = edgeFromInput->text().trimmed();
    QString to = edgeToInput->text().trimmed();
    bool directed = directedInput->isChecked();
    int weight = weightInput->value();

    if (from.isEmpty() || to.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Both 'From' and 'To' nodes must be specified.");
        return;
    }

    graph.addEdge(from.toStdString(), to.toStdString(), 
                  directed ? std::make_optional(weight) : std::nullopt, directed);
    graphWidget->setGraph(graph);

    edgeFromInput->clear();
    edgeToInput->clear();
    weightInput->setValue(0);
}

void MainWindow::runAlgorithm() {
    std::string algo = algorithmBox->currentText().toStdString();
    std::string start = startNodeInput->text().toStdString();
    std::string end = endNodeInput->text().toStdString();

    std::vector<std::string> steps;

    // Callback to capture algorithm progress
    StepCallback callback = [&](const std::string &msg) {
        steps.push_back(msg);
        outputBox->append(QString::fromStdString(msg));
    };

    if (algo == "DFS") {
        dfs(graph, start, callback);
    } else if (algo == "BFS") {
        bfs(graph, start, callback);
    } else if (algo == "Dijkstra") {
        dijkstra(graph, start, callback);
    } else if (algo == "Bellman-Ford") {
        bellmanFord(graph, start, callback);
    } else if (algo == "Floyd-Warshall") {
        floydWarshall(graph, callback);
    } else if (algo == "Prim's MST") {
        primMST(graph, start, callback);
    } else if (algo == "Kruskal's MST") {
        kruskalMST(graph, callback);
    }

    // Animate after collecting steps
    graphWidget->animateSteps(steps);
}

void MainWindow::toggleTheme() {
    isDarkMode = !isDarkMode;
    themeManager->applyTheme(this, isDarkMode ? ThemeManager::Dark : ThemeManager::Light);
}

