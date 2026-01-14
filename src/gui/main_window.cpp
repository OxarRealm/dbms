/**
 * @file main_window.cpp
 * @brief 主窗口实现
 */

#include "gui/main_window.h"
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_welcomeLabel(nullptr)
    , m_testButton(nullptr)
{
    setupUI();
    setupFonts();
    createMenuBar();
    createStatusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 创建中央部件
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);

    // 创建欢迎标签
    m_welcomeLabel = new QLabel("欢迎使用智能音乐播放管理系统", this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_welcomeLabel);

    // 创建测试按钮
    m_testButton = new QPushButton("测试按钮", this);
    m_testButton->setMinimumHeight(40);
    connect(m_testButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "测试", "按钮点击成功！\nGUI程序运行正常。");
    });
    m_mainLayout->addWidget(m_testButton);

    // 添加弹性空间
    m_mainLayout->addStretch();

    // 设置窗口属性
    setWindowTitle("智能音乐播放管理系统");
    resize(800, 600);
}

void MainWindow::setupFonts()
{
    // 设置英文字体为Segoe UI
    QFont segoeFont("Segoe UI", 9);
    segoeFont.setStyleHint(QFont::SansSerif);

    // 为所有英文文本设置字体
    // 注意：中文文本保持默认字体，只有英文使用Segoe UI
    m_testButton->setFont(segoeFont);
    
    // 设置窗口标题等系统文本的字体（通过应用程序全局设置）
    QApplication::setFont(segoeFont, "QPushButton");
    QApplication::setFont(segoeFont, "QMenuBar");
    QApplication::setFont(segoeFont, "QMenu");
    QApplication::setFont(segoeFont, "QStatusBar");
}

void MainWindow::createMenuBar()
{
    // 创建文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    QAction *exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 创建帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    
    QAction *aboutAction = helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("就绪");
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于", 
        "智能音乐播放管理系统\n\n"
        "版本：0.4.0\n"
        "数据库新技术实践课程设计项目\n\n"
        "功能：\n"
        "- 表结构管理\n"
        "- 数据操作\n"
        "- SQL查询\n"
        "- 智能推荐\n"
        "- 索引优化");
}

