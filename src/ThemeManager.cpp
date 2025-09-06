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
QColor ThemeManager::nodeColor() const {
    return (currentMode == Dark) ? darkNode : lightNode;
}

QColor ThemeManager::edgeColor() const {
    return (currentMode == Dark) ? darkEdge : lightEdge;
}

QColor ThemeManager::textColor() const {
    return (currentMode == Dark) ? darkText : lightText;
}

QColor ThemeManager::highlightColor() const {
    return (currentMode == Dark) ? darkHighlight : lightHighlight;
}

QColor ThemeManager::backgroundColor() const {
    return (currentMode == Dark) ? darkBackground : lightBackground;
}

QColor ThemeManager::buttonColor() const
{
    return (currentMode == Dark ? darkButton : lightButton);
}
