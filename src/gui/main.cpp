/**
 * @file main.cpp
 * @brief 应用程序入口
 */

#include "gui/main_window.h"
#include <QApplication>
#include <QFont>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application information
    app.setApplicationName("Database Management System");
    app.setApplicationVersion("0.6.1");
    app.setOrganizationName("Database New Technology Practice");

    // Set global font (Segoe UI for all text)
    QFont segoeFont("Segoe UI", 9);
    segoeFont.setStyleHint(QFont::SansSerif);
    app.setFont(segoeFont);

    // Set application style (Fusion style for modern look)
    app.setStyle(QStyleFactory::create("Fusion"));

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}

