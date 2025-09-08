#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QTimer>
#include <unordered_map>
#include <vector>
#include <string>

#include "Graph.hpp"
#include "Algorithms.hpp"
#include "ThemeManager.hpp"

// GraphWidget: handles drawing and animating the graph visuals
class GraphWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    // Replace the graph model and redraw
    void setGraph(const Graph &graph);

    // Process one or more step messages (MainWindow sends one step per tick)
    // Widget will act on each message immediately (highlight nodes/edges, draw arrows, etc.)
    void animateSteps(const std::vector<std::string> &steps);

    // Clear visuals only
    void clearGraph();

    // Reset widget state
    void reset();

    // Apply theme (light/dark)
    void setTheme(bool darkMode);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *scene;
    Graph currentGraph;
    ThemeManager themeManager;

    // Node visuals
    std::unordered_map<std::string, QGraphicsEllipseItem*> nodeItems;
    std::unordered_map<std::string, QGraphicsTextItem*> nodeLabels;

    // Edge visual object groups (line + optional arrow polygon)
    struct VisualEdge {
        QGraphicsLineItem *line = nullptr;
        QGraphicsPolygonItem *arrow = nullptr; // only for directed edges
        std::string from;
        std::string to;
        bool directed = false;
    };
    std::vector<VisualEdge> edgeItems;

    // Step messages (widget processes immediately when animateSteps is called)
    std::vector<std::string>  Messages;
    int currentStep = 0;

    // Theme state
    bool isDarkMode = false;

    // Helpers
    void drawGraph();
    void drawNode(const std::string &id, double x, double y);
    void drawEdge(const Edge &edge);

    // Find visual edge matching endpoints (directed respect direction, undirected match either)
    VisualEdge* findVisualEdge(const std::string &from, const std::string &to, bool directed);

    // Apply highlight / color to an edge visual (line + arrow)
    void highlightEdgeVisual(VisualEdge *ve, const QColor &color, int width = 3);

    // Process a single message (previously showNextStep in the earlier commits)
    void processMessage(const QString &msg);

private slots:
    // kept private slot empty on header to avoid Qt moc issues if needed later
    void dummySlot() {}
};
