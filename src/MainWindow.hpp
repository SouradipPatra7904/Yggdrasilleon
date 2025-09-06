#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QWidget>
#include <QTimer>

#include <vector>

#include "Graph.hpp"
#include "GraphWidget.hpp"
#include "ThemeManager.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void addNode();
    void addEdge();
    void runAlgorithm();
    void toggleTheme();
    void updateAlgorithmControls(int index);
    void showHelp();       // new
    void clearGraph();     // new

private:
    Graph graph;
    GraphWidget *graphWidget;

    ThemeManager *themeManager;
    QPushButton *themeToggleBtn;
    QPushButton *saveOutputBtn;
    QPushButton *helpBtn;         
    QPushButton *clearGraphBtn;   
    bool isDarkMode;

    QLineEdit *nodeInput;
    QLineEdit *edgeFromInput;
    QLineEdit *edgeToInput;
    QSpinBox *weightInput;
    QCheckBox *directedInput;

    QComboBox *algorithmBox;
    QLineEdit *startNodeInput;
    QLineEdit *endNodeInput;
    QPushButton *runBtn;

    QTextEdit *outputBox;
    QPushButton *clearOutputBtn;

    QWidget* createControlPanel();

    QTimer *stepTimer;
    std::vector<std::string> currentSteps;
    int currentStepIndex;

    void startStepAnimation();
    void showNextStep();
    void appendHighlightedStep(const QString &text, bool isFinal = false);

    bool isFirstRun = true;   // NEW FLAG
};

#endif // MAINWINDOW_HPP
