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

    // 设置应用程序信息
    app.setApplicationName("智能音乐播放管理系统");
    app.setApplicationVersion("0.4.0");
    app.setOrganizationName("数据库新技术实践");

    // 设置全局字体（英文使用Segoe UI）
    QFont segoeFont("Segoe UI", 9);
    segoeFont.setStyleHint(QFont::SansSerif);
    app.setFont(segoeFont);

    // 设置应用程序样式（使用系统默认样式）
    app.setStyle(QStyleFactory::create("Fusion"));

    // 创建并显示主窗口
    MainWindow window;
    window.show();

    return app.exec();
}

