#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QFont>

/**
 * @file main_window.h
 * @brief 主窗口类
 *
 * 智能音乐播放管理系统的主窗口
 */

/**
 * @brief 主窗口类
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow();

private slots:
    /**
     * @brief 关于对话框
     */
    void showAbout();

private:
    /**
     * @brief 初始化界面
     */
    void setupUI();

    /**
     * @brief 创建菜单栏
     */
    void createMenuBar();

    /**
     * @brief 创建状态栏
     */
    void createStatusBar();

    /**
     * @brief 设置字体（英文使用Segoe UI）
     */
    void setupFonts();

    // UI组件
    QWidget *m_centralWidget;      // 中央部件
    QVBoxLayout *m_mainLayout;     // 主布局
    QLabel *m_welcomeLabel;        // 欢迎标签
    QPushButton *m_testButton;     // 测试按钮
};

