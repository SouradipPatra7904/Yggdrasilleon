#include "ThemeManager.hpp"

void ThemeManager::applyTheme(QWidget *widget, Mode mode) {
    currentMode = mode;
    QPalette palette;

    if (mode == Dark) {
        palette.setColor(QPalette::Window, darkBackground);
        palette.setColor(QPalette::WindowText, darkText);
        palette.setColor(QPalette::Base, QColor("#072245"));
        palette.setColor(QPalette::AlternateBase, darkNode);
        palette.setColor(QPalette::ToolTipBase, darkText);
        palette.setColor(QPalette::ToolTipText, darkText);
        palette.setColor(QPalette::Text, darkText);

        palette.setColor(QPalette::Button, darkButton);
        palette.setColor(QPalette::ButtonText, darkButtonText);
    } else {
        palette.setColor(QPalette::Window, lightBackground);
        palette.setColor(QPalette::WindowText, lightText);
        palette.setColor(QPalette::Base, QColor(Qt::white));
        palette.setColor(QPalette::AlternateBase, lightNode);
        palette.setColor(QPalette::ToolTipBase, lightText);
        palette.setColor(QPalette::ToolTipText, lightText);
        palette.setColor(QPalette::Text, lightText);

        palette.setColor(QPalette::Button, lightButton);
        palette.setColor(QPalette::ButtonText, lightButtonText);
    }

    QApplication::setPalette(palette);
    widget->setPalette(palette);
}

// ---------- Getters ----------
QColor ThemeManager::nodeColor(bool isDarkMode) const {
    return (isDarkMode) ? darkNode : lightNode;
}

QColor ThemeManager::edgeColor(bool isDarkMode) const {
    return (isDarkMode) ? darkEdge : lightEdge;
}

QColor ThemeManager::textColor(bool isDarkMode) const {
    return (isDarkMode ? darkText : lightText);
}

QColor ThemeManager::backgroundColor(bool isDarkMode) const {
    return (isDarkMode ? darkBackground : lightBackground);
}

QColor ThemeManager::buttonColor(bool isDarkMode) const
{
    return (isDarkMode ? darkButton : lightButton);
}

QColor ThemeManager::highlightColor(bool isDarkMode) const {
    return (isDarkMode ? highlightColorDark : highlightColorLight);
    }


