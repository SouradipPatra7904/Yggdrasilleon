#include "GraphWidget.hpp"
#include <QGraphicsTextItem>
#include <QResizeEvent>
#include <QPen>
#include <QBrush>
#include <cmath>

GraphWidget::GraphWidget(QWidget *parent) : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    stepTimer.setInterval(300);
    connect(&stepTimer, &QTimer::timeout, this, &GraphWidget::showNextStep);
}

void GraphWidget::setGraph(const Graph &graph) {
    currentGraph = graph;
    drawGraph();
}

void GraphWidget::animateSteps(const std::vector<std::string> &steps) {
    stepMessages = steps;
    currentStep = 0;
    stepTimer.start();
}

void GraphWidget::clearGraph() {
    scene->clear();
    nodeItems.clear();
    nodeLabels.clear();
    edgeItems.clear();
}

void GraphWidget::reset() {
    clearGraph();
    stepMessages.clear();
    currentStep = 0;
    stepTimer.stop();
}

void GraphWidget::setTheme(bool darkMode) {
    isDarkMode = darkMode;
    setBackgroundBrush(themeManager.backgroundColor());

    // recolor nodes + edges if already drawn
    for (auto &pair : nodeItems) {
        pair.second->setBrush(themeManager.nodeColor());
    }
    for (auto *edge : edgeItems) {
        QPen pen(themeManager.nodeColor());
        pen.setWidth(2);
        edge->setPen(pen);
    }
}

void GraphWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GraphWidget::drawGraph() {
    clearGraph();

    int n = currentGraph.nodes().size();
    if (n == 0) return;

    int radius = 150;
    int cx = 0, cy = 0;
    double angleStep = 2 * M_PI / n;

    int i = 0;
    for (auto &node : currentGraph.nodes()) {
        double x = cx + radius * cos(i * angleStep);
        double y = cy + radius * sin(i * angleStep);
        drawNode(node, x, y);
        i++;
    }
    for (auto &edge : currentGraph.edges()) {
        drawEdge(edge);
    }
    fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GraphWidget::drawNode(const std::string &id, double x, double y) {
    int r = 20;
    auto *ellipse = scene->addEllipse(x - r, y - r, 2*r, 2*r,
                                      QPen(Qt::black, 2),
                                      QBrush(themeManager.nodeColor()));
    nodeItems[id] = ellipse;

    auto *label = scene->addText(QString::fromStdString(id));
    label->setDefaultTextColor(isDarkMode ? Qt::white : Qt::black);
    label->setPos(x - r/2, y - r/2);
    nodeLabels[id] = label;
}

void GraphWidget::drawEdge(const Edge &edge) {
    if (!nodeItems.count(edge.from) || !nodeItems.count(edge.to)) return;

    QPointF p1 = nodeItems[edge.from]->rect().center() + nodeItems[edge.from]->pos();
    QPointF p2 = nodeItems[edge.to]->rect().center() + nodeItems[edge.to]->pos();

    auto *line = scene->addLine(QLineF(p1, p2), QPen(themeManager.nodeColor(), 2));
    edgeItems.push_back(line);
}

void GraphWidget::showNextStep() {
    if (currentStep >= stepMessages.size()) {
        stepTimer.stop();
        return;
    }

    QString msg = QString::fromStdString(stepMessages[currentStep]);

    // ---- Detect MST step ----
    if (msg.startsWith("Edge added to MST")) {
        // Parse nodes: "Edge added to MST: u - v (weight X)"
        QStringList parts = msg.split(" ");
        if (parts.size() >= 7) {
            QString u = parts[4];
            QString v = parts[6];

            // highlight nodes
            if (nodeItems.count(u.toStdString()))
                nodeItems[u.toStdString()]->setBrush(QBrush(Qt::green));
            if (nodeItems.count(v.toStdString()))
                nodeItems[v.toStdString()]->setBrush(QBrush(Qt::green));

            // highlight edge
            for (auto *edge : edgeItems) {
                if ((edge->line().p1() == nodeItems[u.toStdString()]->rect().center() + nodeItems[u.toStdString()]->pos() &&
                     edge->line().p2() == nodeItems[v.toStdString()]->rect().center() + nodeItems[v.toStdString()]->pos()) ||
                    (edge->line().p1() == nodeItems[v.toStdString()]->rect().center() + nodeItems[v.toStdString()]->pos() &&
                     edge->line().p2() == nodeItems[u.toStdString()]->rect().center() + nodeItems[u.toStdString()]->pos())) {
                    QPen pen(Qt::green);
                    pen.setWidth(3);
                    edge->setPen(pen);
                }
            }
        }
    }

    currentStep++;
}
