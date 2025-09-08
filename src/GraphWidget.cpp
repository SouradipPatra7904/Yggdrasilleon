// GraphWidget.cpp
#include "GraphWidget.hpp"
#include <QGraphicsTextItem>
#include <QResizeEvent>
#include <QPen>
#include <QBrush>
#include <QtMath>

// small helper to compare points with tolerance
static bool pointsNear(const QPointF &a, const QPointF &b, double tol = 1.5) {
    return qAbs(a.x() - b.x()) <= tol && qAbs(a.y() - b.y()) <= tol;
}

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)) {

    setScene(scene);
    setRenderHint(QPainter::Antialiasing);

    // Initialize theme-aware visuals
    setTheme(isDarkMode);

    // note: MainWindow drives timing; GraphWidget processes messages immediately
}

void GraphWidget::setGraph(const Graph &graph) {
    currentGraph = graph;
    drawGraph();
}

void GraphWidget::animateSteps(const std::vector<std::string> &steps) {
    // Process messages immediately (MainWindow controls timing)
    for (const auto &s : steps) {
        processMessage(QString::fromStdString(s));
    }
}

void GraphWidget::clearGraph() {
    // Remove all items from the scene, then clear our maps/vectors
    scene->clear();

    nodeItems.clear();
    nodeLabels.clear();
    edgeItems.clear();
}

void GraphWidget::reset() {
    clearGraph();
    Messages.clear();
    currentStep = 0;
}

void GraphWidget::setTheme(bool darkMode) {
    isDarkMode = darkMode;

    // Background
    scene->setBackgroundBrush(themeManager.backgroundColor(isDarkMode));

    // Recolor nodes & labels & edges
    for (auto &p : nodeItems) {
        if (p.second) p.second->setBrush(themeManager.nodeColor(isDarkMode));
    }
    for (auto &p : nodeLabels) {
        if (p.second) p.second->setDefaultTextColor(themeManager.textColor(isDarkMode));
    }
    for (auto &ve : edgeItems) {
        if (ve.line) ve.line->setPen(QPen(themeManager.edgeColor(isDarkMode), 2));
        if (ve.arrow) {
            ve.arrow->setBrush(themeManager.edgeColor(isDarkMode));
            ve.arrow->setPen(QPen(themeManager.edgeColor(isDarkMode)));
        }
    }
}

void GraphWidget::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    if (!scene->items().isEmpty()) {
        fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void GraphWidget::drawGraph() {
    clearGraph();

    auto nodes = currentGraph.nodes();
    int n = (int)nodes.size();
    if (n == 0) return;

    // circle layout
    double radius = qMin(width(), height()) / 2.7;
    double cx = width() / 2.0;
    double cy = height() / 2.0;
    double angleStep = 2.0 * M_PI / n;

    int i = 0;
    for (const auto &node : nodes) {
        double angle = i * angleStep;
        double x = cx + radius * qCos(angle);
        double y = cy + radius * qSin(angle);
        drawNode(node, x, y);
        ++i;
    }

    for (const auto &edge : currentGraph.edges()) {
        drawEdge(edge);
    }

    // ensure everything fits
    scene->setSceneRect(scene->itemsBoundingRect().adjusted(-40, -40, 40, 40));
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void GraphWidget::drawNode(const std::string &id, double x, double y) {
    const int r = 26;

    // ellipse
    auto *ellipse = scene->addEllipse(x - r, y - r, 2 * r, 2 * r,
                                      QPen(Qt::black, 2),
                                      QBrush(themeManager.nodeColor(isDarkMode)));
    // set z so it's above edges
    ellipse->setZValue(2.0);

    nodeItems[id] = ellipse;

    // label centered
    auto *label = scene->addText(QString::fromStdString(id));
    QRectF bounds = label->boundingRect();
    label->setPos(x - bounds.width() / 2.0, y - bounds.height() / 2.0);
    label->setDefaultTextColor(themeManager.textColor(isDarkMode));
    label->setZValue(3.0);

    nodeLabels[id] = label;
}

void GraphWidget::drawEdge(const Edge &edge) {
    if (!nodeItems.count(edge.from) || !nodeItems.count(edge.to)) return;

    QPointF p1 = nodeItems[edge.from]->rect().center() + nodeItems[edge.from]->pos();
    QPointF p2 = nodeItems[edge.to]->rect().center() + nodeItems[edge.to]->pos();

    // line
    QPen pen(themeManager.edgeColor(isDarkMode), 2);
    auto *line = scene->addLine(QLineF(p1, p2), pen);
    line->setZValue(0.0); // behind nodes

    VisualEdge ve;
    ve.line = line;
    ve.from = edge.from;
    ve.to = edge.to;
    ve.directed = edge.directed;
    ve.arrow = nullptr;

    // add arrow polygon for directed edges
    if (edge.directed) {
        // calculate arrow triangle at p2
        const double arrowSize = 12.0;
        QPointF vec = p1 - p2; // pointing from tip to base
        double len = std::hypot(vec.x(), vec.y());
        if (len < 1.0) len = 1.0;
        double ux = vec.x() / len;
        double uy = vec.y() / len;

        // perpendicular
        double px = -uy;
        double py = ux;

        QPointF tip = p2;
        QPointF base1 = p2 + QPointF(ux * arrowSize + px * (arrowSize * 0.6),
                                     uy * arrowSize + py * (arrowSize * 0.6));
        QPointF base2 = p2 + QPointF(ux * arrowSize - px * (arrowSize * 0.6),
                                     uy * arrowSize - py * (arrowSize * 0.6));

        QPolygonF poly;
        poly << tip << base1 << base2;
        auto *arrow = scene->addPolygon(poly, QPen(themeManager.edgeColor(isDarkMode)), QBrush(themeManager.edgeColor(isDarkMode)));
        // arrow behind nodes but above line so set z slightly higher than line
        arrow->setZValue(0.5);
        ve.arrow = arrow;
    }

    edgeItems.push_back(ve);
}

GraphWidget::VisualEdge* GraphWidget::findVisualEdge(const std::string &from, const std::string &to, bool directed) {
    for (auto &ve : edgeItems) {
        if (directed) {
            if (ve.directed && ve.from == from && ve.to == to) return &ve;
        } else {
            // undirected match either orientation
            if (!ve.directed && ((ve.from == from && ve.to == to) || (ve.from == to && ve.to == from))) return &ve;
            // also allow matching a directed visual when searching undirected pairs
            if (ve.directed && ve.from == from && ve.to == to) return &ve;
            if (ve.directed && ve.from == to && ve.to == from) return &ve;
        }
    }
    return nullptr;
}

void GraphWidget::highlightEdgeVisual(VisualEdge *ve, const QColor &color, int width) {
    if (!ve) return;
    if (ve->line) {
        ve->line->setPen(QPen(color, width));
    }
    if (ve->arrow) {
        ve->arrow->setBrush(QBrush(color));
        ve->arrow->setPen(QPen(color));
    }
}


void GraphWidget::processMessage(const QString &msg) {

    // ---- Internal control to RESET colors (used when negative-cycle detected) ----
    if (msg == "RESET_COLORS") {
        // revert nodes to normal
        for (auto &pair : nodeItems) {
            pair.second->setBrush(themeManager.nodeColor(isDarkMode));
        }
        // revert edges to normal
        for (auto &ve : edgeItems) {
            if (ve.line) ve.line->setPen(QPen(themeManager.edgeColor(isDarkMode), 2));
            if (ve.arrow) {
                ve.arrow->setBrush(themeManager.edgeColor(isDarkMode));
                ve.arrow->setPen(QPen(themeManager.edgeColor(isDarkMode)));
            }
        }
        // revert labels
        for (auto &pair : nodeLabels) {
            pair.second->setDefaultTextColor(themeManager.textColor(isDarkMode));
        }
        return;
    }

    // MST edge step (existing)
    if (msg.startsWith("Edge added to MST")) {
        QStringList parts = msg.split(" ");
        if (parts.size() >= 7) {
            QString u = parts[4];
            QString v = parts[6];

            // theme-aware MST highlight
            QColor mstColor = themeManager.highlightColor(isDarkMode);

            if (nodeItems.count(u.toStdString()))
                nodeItems[u.toStdString()]->setBrush(QBrush(mstColor));
            if (nodeItems.count(v.toStdString()))
                nodeItems[v.toStdString()]->setBrush(QBrush(mstColor));

            // find and highlight the edge visual
            VisualEdge *ve = findVisualEdge(u.toStdString(), v.toStdString(), false);
            if (ve) highlightEdgeVisual(ve, mstColor, 3);
        }
        return;
    }

    // Cycles - expected exact formats:
    // "Cycles detected (undirected): A -> B -> C -> D -> A"
    // "Cycles detected (directed): A -> B -> C -> A"
    if (msg.startsWith("Cycles detected (undirected):") || msg.startsWith("Cycles detected (directed):")) {
        bool isDirected = msg.startsWith("Cycles detected (directed):");

        // path string after the colon
        QString pathStr = msg.section(":", 1).trimmed();

        // split on "->"
        QStringList nodes = pathStr.split("->", Qt::SkipEmptyParts);
        for (QString &n : nodes) n = n.trimmed();

        // choose cycle color theme aware (fallback local shades)
        QColor cycleColor = isDarkMode ? QColor("#ff8a80") : QColor("#d32f2f");

        // highlight nodes
        for (const QString &n : nodes) {
            auto key = n.toStdString();
            if (nodeItems.count(key)) {
                nodeItems[key]->setBrush(QBrush(cycleColor));
            }
        }

        // highlight edges between consecutive nodes
        for (int i = 0; i < nodes.size() - 1; ++i) {
            QString u = nodes[i];
            QString v = nodes[i + 1];
            VisualEdge *ve = findVisualEdge(u.toStdString(), v.toStdString(), isDirected);
            if (!ve) {
                // fallback: try opposite orientation (for undirected visuals)
                ve = findVisualEdge(v.toStdString(), u.toStdString(), isDirected);
            }
            if (ve) highlightEdgeVisual(ve, cycleColor, 3);
        }

        // if undirected and path not closed explicitly by arrow list, connect last->first
        if (!isDirected && nodes.size() > 1) {
            QString u = nodes.last();
            QString v = nodes.first();
            VisualEdge *ve = findVisualEdge(u.toStdString(), v.toStdString(), false);
            if (!ve) ve = findVisualEdge(v.toStdString(), u.toStdString(), false);
            if (ve) highlightEdgeVisual(ve, cycleColor, 3);
        }

        return;
    }

    // Completed message - do nothing graphical
    if (msg.startsWith("Cycle Detection Completed") || msg.startsWith("Cycle Detection Completed !")) {
        return;
    }

    // ---- Floyd-Warshall structured update (FW_UPDATE u v k value) ----
    if (msg.startsWith("FW_UPDATE")) {
        // Format: "FW_UPDATE u v k <value>"
        QStringList parts = msg.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 4) {
            QString u = parts[1];
            QString v = parts[2];
            // highlight nodes with theme highlight color
            if (nodeItems.count(u.toStdString()))
                nodeItems[u.toStdString()]->setBrush(QBrush(themeManager.highlightColor(isDarkMode)));
            if (nodeItems.count(v.toStdString()))
                nodeItems[v.toStdString()]->setBrush(QBrush(themeManager.highlightColor(isDarkMode)));

            // highlight matching visual edge if exists
            VisualEdge *ve = findVisualEdge(u.toStdString(), v.toStdString(), false);
            if (!ve) ve = findVisualEdge(v.toStdString(), u.toStdString(), false);
            if (ve) highlightEdgeVisual(ve, themeManager.highlightColor(isDarkMode), 3);
        }
        return;
    }

    // Legacy / human-readable variants that GUI previously handled:
    // "Updated distance: A -> B = 42"
    if (msg.startsWith("Updated distance:")) {
        QString after = msg.section(':', 1).trimmed(); // "A -> B = 42"
        QStringList parts = after.split("->", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            QString u = parts[0].trimmed();
            QString right = parts[1].trimmed(); // "B = 42"
            QString v = right.split('=')[0].trimmed();

            if (nodeItems.count(u.toStdString()))
                nodeItems[u.toStdString()]->setBrush(QBrush(themeManager.highlightColor(isDarkMode)));
            if (nodeItems.count(v.toStdString()))
                nodeItems[v.toStdString()]->setBrush(QBrush(themeManager.highlightColor(isDarkMode)));

            // attempt to find and highlight an edge between u and v (if present)
            VisualEdge *ve = findVisualEdge(u.toStdString(), v.toStdString(), false);
            if (!ve) ve = findVisualEdge(v.toStdString(), u.toStdString(), false);
            if (ve) highlightEdgeVisual(ve, themeManager.highlightColor(isDarkMode), 3);
        }
        return;
    }

    // Another legacy form: "Edge update highlight: ...", keep compatibility
    if (msg.startsWith("Edge update highlight:")) {
        // format used elsewhere: "Edge update highlight: from A to B" or similar
        // try to extract capitalized tokens
        QStringList parts = msg.split(" ", Qt::SkipEmptyParts);
        // naive extraction: find tokens that are node names (present in nodeItems)
        QString u, v;
        for (int i = 0; i < parts.size(); ++i) {
            QString p = parts[i];
            if (nodeItems.count(p.toStdString())) {
                if (u.isEmpty()) u = p;
                else if (v.isEmpty()) { v = p; break; }
            }
        }
        if (!u.isEmpty() && !v.isEmpty()) {
            if (nodeItems.count(u.toStdString()))
                nodeItems[u.toStdString()]->setBrush(QBrush(themeManager.highlightColor(isDarkMode)));
            if (nodeItems.count(v.toStdString()))
                nodeItems[v.toStdString()]->setBrush(QBrush(themeManager.highlightColor(isDarkMode)));

            VisualEdge *ve = findVisualEdge(u.toStdString(), v.toStdString(), false);
            if (!ve) ve = findVisualEdge(v.toStdString(), u.toStdString(), false);
            if (ve) highlightEdgeVisual(ve, themeManager.highlightColor(isDarkMode), 3);
        }
        return;
    }

    // Fallback: other messages (DFS/BFS/Dijkstra steps) may mention nodes.
    // We highlight any node names mentioned by the message (e.g. "Visited: A")
    for (const auto &pair : nodeItems) {
        const QString nodeName = QString::fromStdString(pair.first);
        if (msg.contains(nodeName)) {
            // small persistent highlight (use theme highlight color)
            QColor col = themeManager.highlightColor(isDarkMode);
            pair.second->setBrush(QBrush(col));
        }
    }
}
