#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QTimer>
#include <unordered_map>
#include <vector>
#include <string>
#include "Graph.hpp"
#include "Algorithms.hpp"
#include "ThemeManager.hpp"

class GraphWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    void setGraph(const Graph &graph);
    void animateSteps(const std::vector<std::string> &steps);
    void clearGraph();
    void reset(); // clears scene + data
    void setTheme(bool darkMode); // apply ThemeManager colors

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *scene;
    Graph currentGraph;
    ThemeManager themeManager;

    std::unordered_map<std::string, QGraphicsEllipseItem*> nodeItems;
    std::unordered_map<std::string, QGraphicsTextItem*> nodeLabels;
    std::vector<QGraphicsLineItem*> edgeItems;

    std::vector<std::string> stepMessages;
    int currentStep;
    QTimer stepTimer;
    bool isDarkMode = false;

    void drawGraph();
    void drawNode(const std::string &id, double x, double y);
    void drawEdge(const Edge &edge);

private slots:
    void showNextStep();
};
