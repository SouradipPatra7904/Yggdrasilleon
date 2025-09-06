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

// GraphWidget: handles drawing and animating the graph
class GraphWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    // Set the graph data to display
    void setGraph(const Graph &graph);

    // Animate algorithm results (sequence of step messages)
    void animateSteps(const std::vector<std::string> &steps);

    // Clear all drawings
    void clearGraph();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *scene;
    Graph currentGraph;

    std::unordered_map<std::string, QGraphicsEllipseItem*> nodeItems;
    std::vector<QGraphicsLineItem*> edgeItems;

    // Animation state
    std::vector<std::string> stepMessages;
    int currentStep;
    QTimer stepTimer;

    // Drawing helpers
    void drawGraph();
    void drawNode(const std::string &id, double x, double y);
    void drawEdge(const Edge &edge);

private slots:
    void showNextStep();
};
