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
    QColor nodeColor(bool isDarkMode) const;
    QColor edgeColor(bool isDarkMode) const;
    QColor textColor(bool isDarkMode) const;
    QColor highlightColor(bool isDarkMode) const;
    QColor backgroundColor(bool isDarkMode) const;
    QColor buttonColor(bool isDarkMode) const;

private:
    Mode currentMode = Light;

    QColor lightNode   = QColor("#0df4af");
    QColor lightEdge   = QColor(Qt::darkGray);
    QColor lightText   = QColor(Qt::black);
    QColor lightHighlight = QColor(Qt::yellow);
    QColor lightBackground = QColor("#ffe4c4");  // bisque

    QColor lightButton = QColor("#0ca678");
    QColor lightButtonText = QColor("#000000");

    QColor highlightColorLight = QColor("#f2ff00");

    QColor darkNode    = QColor("#0ca678"); // tealish
    QColor darkEdge    = QColor(Qt::darkGray);
    QColor darkText    = QColor(Qt::white);
    QColor darkHighlight = QColor(Qt::yellow);
    QColor darkBackground = QColor("#183153"); // dark blue

    QColor darkButton = QColor("#056d4e");
    QColor darkButtonText = QColor("#ffffff");

    QColor highlightColorDark = QColor("#ff6200");



};

#endif // THEMEMANAGER_HPP
