#include "GraphWidget.hpp"
#include <QResizeEvent>
#include <QBrush>
#include <QPen>
#include <QtMath>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)), currentStep(0) {
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);

    connect(&stepTimer, &QTimer::timeout, this, &GraphWidget::showNextStep);
}

void GraphWidget::setGraph(const Graph &graph) {
    currentGraph = graph;
    clearGraph();
    drawGraph();
}

void GraphWidget::animateSteps(const std::vector<std::string> &steps) {
    stepMessages = steps;
    currentStep = 0;
    stepTimer.start(800); // 0.8s per step
}

void GraphWidget::clearGraph() {
    scene->clear();
    nodeItems.clear();
    edgeItems.clear();
}

void GraphWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void GraphWidget::drawGraph() {
    auto nodes = currentGraph.nodes();
    int n = nodes.size();
    if (n == 0) return;

    double radius = qMin(width(), height()) / 2.5;
    double cx = width() / 2.0;
    double cy = height() / 2.0;

    for (int i = 0; i < n; i++) {
        double angle = (2 * M_PI * i) / n;
        double x = cx + radius * qCos(angle);
        double y = cy + radius * qSin(angle);
        drawNode(nodes[i], x, y);
    }

    for (auto &node : nodes) {
        for (auto &edge : currentGraph.neighbors(node)) {
            drawEdge(edge);
        }
    }

    scene->setSceneRect(scene->itemsBoundingRect().adjusted(-50, -50, 50, 50));
}

void GraphWidget::drawNode(const std::string &id, double x, double y) {
    double r = 30.0;
    auto ellipse = scene->addEllipse(x - r/2, y - r/2, r, r,
                                     QPen(Qt::black, 2),
                                     QBrush(Qt::white));
    auto text = scene->addText(QString::fromStdString(id));
    text->setPos(x - r/4, y - r/4);

    nodeItems[id] = ellipse;
}

void GraphWidget::drawEdge(const Edge &edge) {
    auto itFrom = nodeItems.find(edge.from);
    auto itTo = nodeItems.find(edge.to);
    if (itFrom == nodeItems.end() || itTo == nodeItems.end()) return;

    QPointF p1 = itFrom->second->rect().center() + itFrom->second->pos();
    QPointF p2 = itTo->second->rect().center() + itTo->second->pos();

    auto line = scene->addLine(QLineF(p1, p2), QPen(Qt::darkGray, 2));
    edgeItems.push_back(line);

    if (edge.weight) {
        QPointF mid = (p1 + p2) / 2;
        scene->addText(QString::number(*edge.weight))->setPos(mid);
    }
}

void GraphWidget::showNextStep() {
    if (currentStep >= stepMessages.size()) {
        stepTimer.stop();
        return;
    }

    QString msg = QString::fromStdString(stepMessages[currentStep]);

    // Highlight node if message mentions it
    for (auto &pair : nodeItems) {
        if (msg.contains(QString::fromStdString(pair.first))) {
            pair.second->setBrush(QBrush(Qt::yellow));
        }
    }

    // Show step as tooltip
    scene->addText(msg)->setPos(scene->sceneRect().bottomLeft() + QPointF(10, -20 - 15 * currentStep));

    currentStep++;
}
