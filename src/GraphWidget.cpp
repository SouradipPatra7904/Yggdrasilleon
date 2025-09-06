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

    // start with light mode
    setTheme(false);
}

void GraphWidget::setTheme(bool darkMode) {
    isDarkMode = darkMode;
    themeManager.applyTheme(this, darkMode ? ThemeManager::Dark : ThemeManager::Light);
    scene->setBackgroundBrush(themeManager.backgroundColor());

    // Reapply node/edge colors if graph already exists
    for(auto &pair : nodeItems) pair.second->setBrush(themeManager.nodeColor());
    for(auto &line : edgeItems) line->setPen(QPen(themeManager.edgeColor(),2));
    for(auto &pair : nodeLabels) pair.second->setDefaultTextColor(themeManager.textColor());
}

void GraphWidget::setGraph(const Graph &graph) {
    currentGraph = graph;
    clearGraph();
    drawGraph();
}

void GraphWidget::reset() {
    clearGraph();
    stepMessages.clear();
    currentStep = 0;
}

void GraphWidget::clearGraph() {
    scene->clear();
    nodeItems.clear();
    nodeLabels.clear();
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

    scene->setSceneRect(scene->itemsBoundingRect().adjusted(-50,-50,50,50));
}

void GraphWidget::drawNode(const std::string &id, double x, double y) {
    double r = 30.0;
    auto ellipse = scene->addEllipse(x-r/2, y-r/2, r, r,
                                     QPen(Qt::black,2),
                                     QBrush(themeManager.nodeColor()));
    auto text = scene->addText(QString::fromStdString(id));
    QRectF bounds = text->boundingRect();
    text->setPos(x - bounds.width()/2, y - bounds.height()/2);
    text->setDefaultTextColor(themeManager.textColor());

    nodeItems[id] = ellipse;
    nodeLabels[id] = text;
}

void GraphWidget::drawEdge(const Edge &edge) {
    auto itFrom = nodeItems.find(edge.from);
    auto itTo = nodeItems.find(edge.to);
    if(itFrom==nodeItems.end() || itTo==nodeItems.end()) return;

    QPointF p1 = itFrom->second->rect().center() + itFrom->second->pos();
    QPointF p2 = itTo->second->rect().center() + itTo->second->pos();

    auto line = scene->addLine(QLineF(p1,p2), QPen(themeManager.edgeColor(),2));
    edgeItems.push_back(line);

    if(edge.weight) {
        QPointF mid = (p1+p2)/2;
        auto text = scene->addText(QString::number(*edge.weight));
        QRectF bounds = text->boundingRect();
        text->setPos(mid.x()-bounds.width()/2, mid.y()-bounds.height()/2);
        text->setDefaultTextColor(themeManager.textColor());
    }
}

void GraphWidget::animateSteps(const std::vector<std::string> &steps) {
    stepMessages = steps;
    currentStep = 0;
    stepTimer.start(800);
}

void GraphWidget::showNextStep() {
    if(currentStep >= stepMessages.size()) {
        stepTimer.stop();
        return;
    }

    QString msg = QString::fromStdString(stepMessages[currentStep]);

    // highlight node if mentioned
    for(auto &pair: nodeItems) {
        if(msg.contains(QString::fromStdString(pair.first))) {
            pair.second->setBrush(themeManager.highlightColor());
        }
    }

    // show step text
    double yOffset = 20 + 20*currentStep;
    auto textItem = scene->addText(msg);
    textItem->setDefaultTextColor(themeManager.textColor());
    textItem->setPos(scene->sceneRect().bottomLeft() + QPointF(10, -yOffset));

    currentStep++;
}
