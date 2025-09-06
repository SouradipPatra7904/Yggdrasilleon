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

private:
    // Core graph + visualization
    Graph graph;
    GraphWidget *graphWidget;

    // Theme
    ThemeManager *themeManager;
    QPushButton *themeToggleBtn;
    bool isDarkMode;

    // Node/Edge controls
    QLineEdit *nodeInput;
    QLineEdit *edgeFromInput;
    QLineEdit *edgeToInput;
    QSpinBox *weightInput;
    QCheckBox *directedInput;

    // Algorithm controls
    QComboBox *algorithmBox;
    QLineEdit *startNodeInput;
    QLineEdit *endNodeInput;
    QPushButton *runBtn;

    // Output area
    QTextEdit *outputBox;

    // Layout helpers
    QWidget* createControlPanel();
};

#endif // MAINWINDOW_HPP
