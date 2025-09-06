#include "ThemeManager.hpp"

void ThemeManager::applyTheme(QWidget *widget, Mode mode) {
    QPalette palette;

    if (mode == Dark) {
        palette.setColor(QPalette::Window, QColor("#183153"));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor("#072245"));
        palette.setColor(QPalette::AlternateBase, QColor("#0ca678"));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor("#0ca678"));
        palette.setColor(QPalette::ButtonText, Qt::white);
    } else {
        palette.setColor(QPalette::Window, QColor("#ffe4c4"));
        palette.setColor(QPalette::WindowText, Qt::black);
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::AlternateBase, QColor("#63e6be"));
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Button, QColor("#63e6be"));
        palette.setColor(QPalette::ButtonText, Qt::black);
    }

    QApplication::setPalette(palette);
    widget->setPalette(palette);
}
