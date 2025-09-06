#ifndef THEMEMANAGER_HPP
#define THEMEMANAGER_HPP

#include <QWidget>
#include <QPalette>
#include <QApplication>
#include <QColor>

class ThemeManager {
public:
    enum Mode { Light, Dark };

    ThemeManager() = default;

    void applyTheme(QWidget *widget, Mode mode);

    // Getters for GraphWidget colors
    QColor nodeColor() const;
    QColor edgeColor() const;
    QColor textColor() const;
    QColor highlightColor() const;
    QColor backgroundColor() const;
    QColor buttonColor() const;

private:
    Mode currentMode = Light;

    QColor lightNode   = QColor("#0df4af");
    QColor lightEdge   = QColor(Qt::darkGray);
    QColor lightText   = QColor(Qt::black);
    QColor lightHighlight = QColor(Qt::yellow);
    QColor lightBackground = QColor("#ffe4c4");  // bisque

    QColor lightButton = QColor("#0ca678");
    QColor lightButtonText = QColor("#000000");

    QColor darkNode    = QColor("#0ca678"); // tealish
    QColor darkEdge    = QColor(Qt::darkGray);
    QColor darkText    = QColor(Qt::white);
    QColor darkHighlight = QColor(Qt::yellow);
    QColor darkBackground = QColor("#183153"); // dark blue

    QColor darkButton = QColor("#056d4e");
    QColor darkButtonText = QColor("#ffffff");


};

#endif // THEMEMANAGER_HPP
