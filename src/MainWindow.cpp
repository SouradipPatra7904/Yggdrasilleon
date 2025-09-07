#include "MainWindow.hpp"
#include "ThemeManager.hpp"
#include "Algorithms.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QCoreApplication>
#include <cstring>

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

    setWindowTitle("Yggdrasill - World Tree Graph Engine");
    resize(1200, 800);

    // Set initial Light theme
    themeManager->applyTheme(this, ThemeManager::Light);

    // Update Start/End fields initially
    updateAlgorithmControls(algorithmBox->currentIndex());
}

QWidget* MainWindow::createControlPanel() {
    QWidget *panel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(panel);

    // Top layout: Help + Save + Theme + Clear Graph
    QHBoxLayout *topLayout = new QHBoxLayout();
    QPushButton *helpBtn = new QPushButton("Help", panel);
    saveOutputBtn = new QPushButton("Save Output", panel);
    themeToggleBtn = new QPushButton("Toggle Theme", panel);
    QPushButton *clearGraphBtn = new QPushButton("Clear Graph", panel);

    topLayout->addWidget(helpBtn);
    topLayout->addStretch();
    topLayout->addWidget(saveOutputBtn);
    topLayout->addWidget(clearGraphBtn);
    topLayout->addWidget(themeToggleBtn);
    layout->addLayout(topLayout);

    connect(themeToggleBtn, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    connect(clearGraphBtn, &QPushButton::clicked, this, &MainWindow::clearGraph);
    connect(helpBtn, &QPushButton::clicked, this, &MainWindow::showHelp);

    connect(saveOutputBtn, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Graph Output", "", "Text Files (*.txt)");
        if (fileName.isEmpty()) return;
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

        QTextStream out(&file);
        out << "Nodes:\n";
        for (auto &node : graph.nodes()) out << QString::fromStdString(node) << "\n";

        out << "\nEdges:\n";
        for (auto &edge : graph.edges())
            out << QString::fromStdString(edge.from) << " -> "
                << QString::fromStdString(edge.to)
                << " Weight: " << edge.weight.value_or(1)
                << " Directed: " << (edge.directed ? "Yes" : "No") << "\n";

        out << "\nAdjacency List:\n";
        for (auto &node : graph.nodes()) {
            out << QString::fromStdString(node) << ": ";
            for (auto &e : graph.neighbors(node))
                out << QString::fromStdString(e.to) << "("
                    << e.weight.value_or(1) << ") ";
            out << "\n";
        }

        out << "\nAlgorithm Output:\n";
        out << outputBox->toPlainText();
    });

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
    QPushButton *deleteEdgeBtn = new QPushButton("Delete Edge", edgeBox);

    connect(addEdgeBtn, &QPushButton::clicked, this, &MainWindow::addEdge);
    connect(deleteEdgeBtn, &QPushButton::clicked, [this]() {
        QString from = edgeFromInput->text().trimmed();
        QString to = edgeToInput->text().trimmed();
        if (from.isEmpty() || to.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "From/To cannot be empty.");
            return;
        }
        bool directed = directedInput->isChecked();
        graph.removeEdge(from.toStdString(), to.toStdString(), directed);
        graphWidget->setGraph(graph);
        edgeFromInput->clear(); edgeToInput->clear();
    });

    edgeLayout->addRow("From:", edgeFromInput);
    edgeLayout->addRow("To:", edgeToInput);
    edgeLayout->addRow("Weight:", weightInput);
    edgeLayout->addRow("", directedInput);
    edgeLayout->addRow("", addEdgeBtn);
    edgeLayout->addRow("", deleteEdgeBtn);

    // Algorithm Controls
    QGroupBox *algoBox = new QGroupBox("Algorithm", panel);
    QFormLayout *algoLayout = new QFormLayout(algoBox);
    algorithmBox = new QComboBox(algoBox);

    algorithmBox->addItems({"DFS", "BFS", "Detect Cycles","Dijkstra","Bellman-Ford","Floyd-Warshall","Prim's MST","Kruskal's MST"});

    startNodeInput = new QLineEdit(algoBox);
    endNodeInput = new QLineEdit(algoBox);
    runBtn = new QPushButton("Run", algoBox);
    connect(runBtn, &QPushButton::clicked, this, &MainWindow::runAlgorithm);

    algoLayout->addRow("Algorithm:", algorithmBox);
    algoLayout->addRow("Start Node:", startNodeInput);
    algoLayout->addRow("Goal Node:", endNodeInput);
    algoLayout->addRow("", runBtn);

    // Disable Start/End automatically for MST
    connect(algorithmBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateAlgorithmControls);

    // Output Box
    QGroupBox *outputGroup = new QGroupBox("Output", panel);
    QVBoxLayout *outLayout = new QVBoxLayout(outputGroup);
    outputBox = new QTextEdit(outputGroup);
    outputBox->setReadOnly(true);
    clearOutputBtn = new QPushButton("Clear Output", outputGroup);
    connect(clearOutputBtn, &QPushButton::clicked, [this](){
        outputBox->clear();
        // graphWidget->reset();
    });
    outLayout->addWidget(outputBox);
    outLayout->addWidget(clearOutputBtn);

    // Layout assembly
    layout->addWidget(nodeBox);
    layout->addWidget(edgeBox);
    layout->addWidget(algoBox);
    layout->addWidget(outputGroup);
    layout->addStretch();

    return panel;
}

// ---- Core Functions ----
void MainWindow::addNode() {
    QString id = nodeInput->text().trimmed();
    if (id.isEmpty()) { QMessageBox::warning(this,"Invalid Input","Node ID cannot be empty."); return; }
    graph.addNode(id.toStdString());
    graphWidget->setGraph(graph);
    nodeInput->clear();
}

void MainWindow::addEdge() {
    QString from = edgeFromInput->text().trimmed();
    QString to = edgeToInput->text().trimmed();
    if (from.isEmpty() || to.isEmpty()) { QMessageBox::warning(this,"Invalid Input","From/To cannot be empty."); return; }
    bool directed = directedInput->isChecked();
    int weight = weightInput->value();
    graph.addEdge(from.toStdString(), to.toStdString(),
                  directed ? std::make_optional(weight) : std::nullopt,
                  directed);
    graphWidget->setGraph(graph);
    edgeFromInput->clear(); edgeToInput->clear(); weightInput->setValue(0);
}

void MainWindow::runAlgorithm() {
    // Reset state
    outputBox->clear();

    // First run, GraphWidget is not destroyed, but
    // from the 2nd run onwards, the GraphWidget is destroyed and rebuilt.
    if (!isFirstRun) {
        graphWidget->reset();       // clear visuals
        graphWidget->setGraph(graph); // redraw from Graph model
    }

    isFirstRun = false;

    std::string algo = algorithmBox->currentText().toStdString();

    std::string start = startNodeInput->text().toStdString();
    std::string end = endNodeInput->text().toStdString();

    // BFS, DFS, Shortest Path Algorithms must have a start and goal nodes.
    if(algo == "BFS" || algo == "DFS"){
        if (start.empty() || end.empty()) {
            QMessageBox::warning(this,"Invalid Input","Start/End cannot be empty."); 
            return; 
        }
    }
    // Prim's Algorithm must take the starting node as input for it's greedy approach.
    if(algo == "Prim's MST" && start.empty()){
        QMessageBox::warning(this,"Invalid Input","Start cannot be empty."); 
        return;
    }

    currentSteps.clear();
    currentStepIndex = 0;

    StepCallback callback = [&](const std::string &msg) {
        currentSteps.push_back(msg);
    };

    if (algo=="DFS") dfs(graph, start, callback);
    else if (algo=="BFS") bfs(graph, start, callback);
    else if (algo=="Dijkstra") dijkstra(graph, start, callback);
    else if (algo=="Bellman-Ford") bellmanFord(graph, start, callback);
    else if (algo=="Floyd-Warshall") floydWarshall(graph, callback);
    else if (algo=="Prim's MST") primMST(graph, start, callback);
    else if (algo=="Kruskal's MST") kruskalMST(graph, callback);
    else if (algo == "Detect Cycles") detectCycles(graph, callback);

    // Start animation timer
    startStepAnimation();
}

void MainWindow::startStepAnimation() {
    if (stepTimer) {
        stepTimer->stop();
        delete stepTimer;
    }
    stepTimer = new QTimer(this);
    connect(stepTimer, &QTimer::timeout, this, &MainWindow::showNextStep);
    stepTimer->start(300); // 300ms delay
}

void MainWindow::showNextStep() {
    if (currentStepIndex >= (int)currentSteps.size()) {
        stepTimer->stop();
        return;
    }

    bool isFinal = (currentStepIndex == (int)currentSteps.size() - 1);
    QString stepText = QString::fromStdString(currentSteps[currentStepIndex]);

    appendHighlightedStep(stepText, isFinal);
    outputBox->verticalScrollBar()->setValue(outputBox->verticalScrollBar()->maximum());

    // also animate in GraphWidget
    graphWidget->animateSteps({ currentSteps[currentStepIndex] });

    currentStepIndex++;
}

void MainWindow::appendHighlightedStep(const QString &text, bool isFinal) {
    // Theme-aware highlight colors
    QString bgColor, textColor;

    if (isFinal) {
        bgColor = isDarkMode ? "#2e7d32" : "#c8f7c5";   // green shades
        textColor = isDarkMode ? "#e8f5e9" : "#1b5e20"; // light green text vs dark green text
    } else {
        bgColor = isDarkMode ? "#bfa93a" : "#fef7c0";   // yellow shades
        textColor = isDarkMode ? "#000000" : "#000000"; // dark vs brownish for contrast
    }

    QString styled = QString(
        "<div style='background-color:%1; color:%2; padding:8px; margin:4px; "
        "border-radius:6px; font-size:14px;'>%3</div><br/>")
        .arg(bgColor)
        .arg(textColor)
        .arg(text.toHtmlEscaped());

    outputBox->moveCursor(QTextCursor::End);
    outputBox->insertHtml(styled);
    outputBox->moveCursor(QTextCursor::End);
}



void MainWindow::toggleTheme() {
    isDarkMode = !isDarkMode;
    themeManager->applyTheme(this, isDarkMode ? ThemeManager::Dark : ThemeManager::Light);
    graphWidget->setTheme(isDarkMode);
    
}

// Update Start/End enable for MST
void MainWindow::updateAlgorithmControls(int index) {
    QString algo = algorithmBox->itemText(index);

    //bool isMST = algo.contains("MST");
    //startNodeInput->setDisabled(isMST);
    //endNodeInput->setDisabled(isMST);

    if(algo == "Prim's MST"){
        endNodeInput->setDisabled(true);
    }

    if(algo == "Kruskal's MST"){
        startNodeInput->setDisabled(true);
        endNodeInput->setDisabled(true);
    }

    if(algo.contains("Cycle")){
        startNodeInput->setDisabled(true);
        endNodeInput->setDisabled(true);
    }
}

// Clear graph manually
void MainWindow::clearGraph() {
    graph.clear();
    graphWidget->reset();
    outputBox->clear();
}

// Show Help window
void MainWindow::showHelp() {
    QDialog *helpDialog = new QDialog(this);
    helpDialog->setWindowTitle("Yggdrasill Help & Documentation");
    helpDialog->resize(900, 750);

    QVBoxLayout *layout = new QVBoxLayout(helpDialog);
    QLabel *label = new QLabel(helpDialog);

    label->setWordWrap(true);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setTextFormat(Qt::RichText);
    label->setOpenExternalLinks(true);

    label->setText(
    "<div style='padding:15px; font-family:Segoe UI, sans-serif; font-size:12pt;'>"

    "<h2 style='color:#4a90e2; text-align:center;'>🌳 Yggdrasilleon Graph Algorithm Visualizer – User Guide 🌳</h2>"
    "<p><b>Yggdrasill</b> is an interactive graph engine for building, visualizing, and simulating algorithms on graphs. "
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

    "</div>"
);

    QScrollArea *scroll = new QScrollArea(helpDialog);
    scroll->setWidget(label);
    scroll->setWidgetResizable(true);

    layout->addWidget(scroll);
    helpDialog->setLayout(layout);
    helpDialog->exec();
}
