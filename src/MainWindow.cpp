#include "MainWindow.hpp"
#include "ThemeManager.hpp"
#include "Algorithms.hpp"
#include "ManPageTexts.hpp"

#include <QStackedWidget>
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

    setWindowTitle("Yggdrasilleon - Graph Algorithm Visualizer");
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

    // BFS, DFS, Prim's MST Algorithms must have a start node.
    // Bellman-Ford & Dijkstra's Algorithms must have a start node.

    if(algo == "DFS" || algo == "BFS" || algo == "Dijkstra" || algo == "Bellman-Ford" || algo == "Prim's MST")
    {
        if (start.empty()) {
            QMessageBox::warning(this,"Invalid Input","Start Node cannot be empty."); 
            return; 
        }
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

    // Special internal control to revert visuals
    bool isResetSignal = (stepText == "RESET_COLORS");

     if (!isResetSignal) {
        appendHighlightedStep(stepText, isFinal);
        outputBox->verticalScrollBar()->setValue(outputBox->verticalScrollBar()->maximum());
    }

    // This shit below was causing double outputs, as the above condition check already does the same
    // So the shit below is not needed 😍
    //appendHighlightedStep(stepText, isFinal);
    //outputBox->verticalScrollBar()->setValue(outputBox->verticalScrollBar()->maximum());

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

    // First, enable both input fileds by default
    startNodeInput->setEnabled(true);
    endNodeInput->setEnabled(true);

    //bool isMST = algo.contains("MST");
    //startNodeInput->setDisabled(isMST);
    //endNodeInput->setDisabled(isMST);

    // DFS / BFS / Dijkstra / Bellman-Ford / Prim's MST requires a start node
    if(algo == "DFS" || algo == "BFS" || algo == "Dijkstra" || algo == "Bellman-Ford" || algo == "Prim's MST"){
        endNodeInput->setDisabled(true);
        (*endNodeInput).clear();
    }

    if(algo == "Kruskal's MST" || algo == "Floyd-Warshall" || algo.contains("Cycle")){
        startNodeInput->setDisabled(true);
        (*startNodeInput).clear();

        endNodeInput->setDisabled(true);
        (*endNodeInput).clear();
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
    helpDialog->setWindowTitle("Yggdrasilleon Help & Documentation");
    helpDialog->resize(900, 750);

    QVBoxLayout *mainLayout = new QVBoxLayout(helpDialog);

    // Top navigation
    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton("⬅", helpDialog);
    QLabel *pageLabel = new QLabel(helpDialog);
    QPushButton *nextBtn = new QPushButton("➡", helpDialog);

    navLayout->addWidget(prevBtn);
    navLayout->addStretch();
    navLayout->addWidget(pageLabel);
    navLayout->addStretch();
    navLayout->addWidget(nextBtn);
    mainLayout->addLayout(navLayout);

    // Stacked widget for pages
    QStackedWidget *stack = new QStackedWidget(helpDialog);

    auto makePage = [&](const QString &htmlContent) -> QWidget* {
        QWidget *page = new QWidget(helpDialog);
        QVBoxLayout *layout = new QVBoxLayout(page);

        QLabel *label = new QLabel(page);
        label->setWordWrap(true);
        label->setTextFormat(Qt::RichText);

        // Theme-aware colors
        QString bgColor = isDarkMode ? "#121212" : "#ffffff";
        QString textColor = isDarkMode ? "#e0e0e0" : "#212121";
        QString headerColor = isDarkMode ? "#90caf9" : "#1565c0";

        QString styledHtml =
            "<div style='background-color:" + bgColor + "; color:" + textColor +
            "; padding:15px; font-family:Segoe UI, sans-serif; font-size:12pt;'>"
            + htmlContent +
            "</div>";

        label->setText(styledHtml);

        QScrollArea *scroll = new QScrollArea(page);
        scroll->setWidget(label);
        scroll->setWidgetResizable(true);

        layout->addWidget(scroll);
        return page;
    };

    ManPageTexts new_page_texts;
    // -------- Page 1: Overview + Features --------
    stack->addWidget(makePage(new_page_texts.page_One_Text));

    // -------- Page 2: Algorithm Descriptions --------
    stack->addWidget(makePage(new_page_texts.page_Two_Text));

    // -------- Page 3: Common Graph Terms --------
    stack->addWidget(makePage(new_page_texts.page_Three_Text));

    // -------------------- "How To" Page -----------------------------
    stack->addWidget(makePage(new_page_texts.page_Four_Text));

    mainLayout->addWidget(stack);

    // Initialize page display
    int currentPage = 0;
    auto updatePageLabel = [&]() {
        pageLabel->setText(QString("Page %1 / %2").arg(currentPage + 1).arg(stack->count()));
    };
    updatePageLabel();

    // Navigation buttons
    connect(prevBtn, &QPushButton::clicked, [&, stack]() {
        if (currentPage > 0) currentPage--;
        stack->setCurrentIndex(currentPage);
        updatePageLabel();
    });
    connect(nextBtn, &QPushButton::clicked, [&, stack]() {
        if (currentPage < stack->count() - 1) currentPage++;
        stack->setCurrentIndex(currentPage);
        updatePageLabel();
    });

    helpDialog->setLayout(mainLayout);
    helpDialog->exec();
}

