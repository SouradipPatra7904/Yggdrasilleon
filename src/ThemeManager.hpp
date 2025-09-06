#ifndef THEMEMANAGER_HPP
#define THEMEMANAGER_HPP

#include <QWidget>
#include <QPalette>
#include <QApplication>

class ThemeManager {
public:
    enum Mode { Light, Dark };

    ThemeManager() = default; // no args constructor

    void applyTheme(QWidget *widget, Mode mode);
};

#endif // THEMEMANAGER_HPP
