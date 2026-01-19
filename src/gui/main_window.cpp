/**
 * @file main_window.cpp
 * @brief Main Window Implementation
 */

#include "gui/main_window.h"
#include "gui/table_management_widget.h"
#include "gui/data_operation_widget.h"
#include "gui/sql_query_widget.h"
#include "gui/index_management_widget.h"
#include "gui/user_management_widget.h"
#include "gui/user_selection_dialog.h"
#include "core/constraint_storage.h"
#include "core/session_manager.h"
#include "core/constraint_registry.h"
#include "core/index_storage.h"
#include "core/index_manager.h"
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QTimer>
#include <QDateTime>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QScrollArea>
#include <QFrame>
#include <QIODevice>
#include <QFileDialog>
#include <QInputDialog>
#include <QFileInfo>
#include <QDir>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_statusLabel(nullptr)
    , m_timer(nullptr)
    , m_currentDatabase("")
    , m_databasePath("")
    , m_tableManagementWidget(nullptr)
    , m_dataOperationWidget(nullptr)
    , m_sqlQueryWidget(nullptr)
    , m_userManagementWidget(nullptr)
{
    setupFonts();
    
    // No login required at startup - login will be required when opening a database
    setupUI();
    createMenuBar();
    createStatusBar();
    setupShortcuts();
    
    // Initialize timer for status bar updates
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    m_timer->start(1000); // Update every second
    updateStatusBar(); // Initial update
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Create main layout
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create tab widget
    createTabWidget();
    m_mainLayout->addWidget(m_tabWidget);

    // Set window properties with version
    setWindowTitle("Database Management System v0.6.1");
    resize(1200, 800);
    setMinimumSize(800, 600);
}

void MainWindow::setupFonts()
{
    // Set Segoe UI font for all text
    QFont segoeFont("Segoe UI", 9);
    segoeFont.setStyleHint(QFont::SansSerif);

    // Set global font for all widgets
    QApplication::setFont(segoeFont);
}

void MainWindow::createTabWidget()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setFont(QFont("Segoe UI", 9));

    // Create tabs in the specified order
    m_tabWidget->addTab(createTableManagementTab(), "Table Management");
    m_tabWidget->addTab(createIndexManagementTab(), "Index Management");
    m_tabWidget->addTab(createDataOperationTab(), "Data Operation");
    m_tabWidget->addTab(createSQLExecutionTab(), "SQL Execution");
    m_tabWidget->addTab(createUserManagementTab(), "User Management");
    m_tabWidget->addTab(createGuideTab(), "Guide");

    // Connect tab change signal
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // Set tab style
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setTabsClosable(false);
    m_tabWidget->setMovable(false);
}

QWidget* MainWindow::createTableManagementTab()
{
    m_tableManagementWidget = new TableManagementWidget(this);
    // If database is already loaded, set it
    if (!m_databasePath.empty()) {
        m_tableManagementWidget->setDatabasePath(m_databasePath);
    }
    return m_tableManagementWidget;
}

QWidget* MainWindow::createDataOperationTab()
{
    m_dataOperationWidget = new DataOperationWidget(this);
    // If database is already loaded, set it
    if (!m_databasePath.empty()) {
        m_dataOperationWidget->setDatabasePath(m_databasePath);
    }
    return m_dataOperationWidget;
}

QWidget* MainWindow::createSQLExecutionTab()
{
    m_sqlQueryWidget = new SQLQueryWidget(this);
    // If database is already loaded, set it
    if (!m_databasePath.empty()) {
        m_sqlQueryWidget->setDatabasePath(m_databasePath);
    }
    return m_sqlQueryWidget;
}

QWidget* MainWindow::createGuideTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create scroll area for guide content
    QScrollArea *scrollArea = new QScrollArea(tab);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    // Create text edit for guide content
    QTextEdit *guideText = new QTextEdit(scrollArea);
    guideText->setReadOnly(true);
    guideText->setFont(QFont("Segoe UI", 9));
    guideText->setFrameShape(QFrame::NoFrame);

    // Load USER_GUIDE.md content
    QFile guideFile("USER_GUIDE.md");
    if (guideFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&guideFile);
        QString content = in.readAll();
        
        // Convert Markdown to plain text (simple conversion)
        // For now, just display as plain text
        // TODO: Implement proper Markdown rendering if needed
        guideText->setPlainText(content);
    } else {
        // If file not found, show default content
        guideText->setPlainText(
            "User Guide\n\n"
            "This guide provides instructions for using the Database Management System.\n\n"
            "Please refer to USER_GUIDE.md in the project root directory for the complete guide.\n\n"
            "The guide will be automatically loaded when USER_GUIDE.md is available."
        );
    }

    scrollArea->setWidget(guideText);
    layout->addWidget(scrollArea);

    return tab;
}

QWidget* MainWindow::createIndexManagementTab()
{
    m_indexManagementWidget = new IndexManagementWidget(this);
    // Share IndexAdvisor from SQLQueryWidget (must create SQL tab first)
    // This is done in setCurrentDatabase to ensure both widgets exist
    // If database is already loaded, set it
    if (!m_databasePath.empty()) {
        m_indexManagementWidget->setDatabasePath(m_databasePath);
        // Connect IndexAdvisor after SQLQueryWidget is created
        if (m_sqlQueryWidget) {
            m_indexManagementWidget->setIndexAdvisor(m_sqlQueryWidget->getIndexAdvisor());
        }
    }
    return m_indexManagementWidget;
}

QWidget* MainWindow::createUserManagementTab()
{
    m_userManagementWidget = new UserManagementWidget(this);
    // If database is already loaded, set it
    if (!m_databasePath.empty()) {
        m_userManagementWidget->setDatabasePath(m_databasePath);
    }
    return m_userManagementWidget;
}

void MainWindow::createMenuBar()
{
    // Create File menu
    QMenu *fileMenu = menuBar()->addMenu("File");
    
    QAction *createDbAction = fileMenu->addAction("Create Database");
    createDbAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(createDbAction, &QAction::triggered, this, &MainWindow::onCreateDatabase);
    
    QAction *openDbAction = fileMenu->addAction("Open Database");
    openDbAction->setShortcut(QKeySequence::Open);
    connect(openDbAction, &QAction::triggered, this, &MainWindow::onOpenDatabase);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("Exit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Create Help menu
    QMenu *helpMenu = menuBar()->addMenu("Help");
    
    QAction *aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Ready");
    
    // Create label for database name and time (right side of status bar)
    m_statusLabel = new QLabel(this);
    m_statusLabel->setFont(QFont("Segoe UI", 9));
    statusBar()->addPermanentWidget(m_statusLabel);
    
    // Initial update
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    // Get current time
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timeStr = currentTime.toString("yyyy/MM/dd hh:mm");
    
    // Format: "database_name  yyyy/MM/dd hh:mm" or just time if no database
    QString statusText;
    if (m_currentDatabase.isEmpty()) {
        statusText = timeStr;
    } else {
        statusText = QString("%1  %2").arg(m_currentDatabase).arg(timeStr);
    }
    
    m_statusLabel->setText(statusText);
}

void MainWindow::onCreateDatabase()
{
    // Use file dialog to get database name and location
    QString fileName = QFileDialog::getSaveFileName(this, 
        "Create Database", 
        "", 
        "Database Files (*.dbf);;All Files (*.*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Ensure .dbf extension
    QFileInfo fileInfo(fileName);
    QString dbName = fileInfo.baseName();
    QString dbPath = fileInfo.absolutePath() + QDir::separator() + dbName;
    QString dbfPath = dbPath + ".dbf";
    QString datPath = dbPath + ".dat";
    
    // Check if file already exists
    if (QFile::exists(dbfPath) || QFile::exists(datPath)) {
        int ret = QMessageBox::question(this, "File Exists", 
            QString("Database file '%1' already exists.\n\nDo you want to open it instead?").arg(dbName),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        
        if (ret == QMessageBox::Yes) {
            // Open existing database - show user selection dialog
            UserSelectionDialog userDialog(this, dbPath.toLocal8Bit().constData());
            
            if (userDialog.exec() != QDialog::Accepted) {
                // User cancelled login - don't open database
                QMessageBox::information(this, "Cancelled", 
                    QString("Database '%1' was not opened.").arg(dbName));
                return;
            }
            
            // Set selected user in session
            std::string selectedUser = userDialog.getSelectedUser();
            SessionManager::getInstance().login(selectedUser);
            
            // Set current database
            setCurrentDatabase(dbPath.toLocal8Bit().constData());
            
            QMessageBox::information(this, "Success", 
                QString("Database '%1' opened successfully as user '%2'.").arg(dbName).arg(QString::fromStdString(selectedUser)));
            return;
        } else {
            return; // User cancelled
        }
    }
    
    // Create empty database files
    // Create .dbf file (empty file is valid - no tables yet)
    std::ofstream dbfFile(dbfPath.toLocal8Bit().constData(), std::ios::binary);
    if (!dbfFile.is_open()) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to create database file: %1").arg(dbfPath));
        return;
    }
    dbfFile.close();
    
    // Create .dat file (empty file is valid - no records yet)
    std::ofstream datFile(datPath.toLocal8Bit().constData(), std::ios::binary);
    if (!datFile.is_open()) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to create data file: %1").arg(datPath));
        // Try to clean up .dbf file
        QFile::remove(dbfPath);
        return;
    }
    datFile.close();
    
    // Set current database
    setCurrentDatabase(dbPath.toLocal8Bit().constData());
    
    // For new database, default to admin user
    SessionManager::getInstance().login("admin");
    
    QMessageBox::information(this, "Success", 
        QString("Database '%1' created successfully. Logged in as 'admin'.").arg(dbName));
}

void MainWindow::onOpenDatabase()
{
    // Open file dialog to select .dbf file
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Open Database", 
        "", 
        "Database Files (*.dbf);;All Files (*.*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Extract database name and path (without extension)
    QFileInfo fileInfo(fileName);
    QString dbName = fileInfo.baseName();
    QString dbPath = fileInfo.absolutePath() + QDir::separator() + dbName;
    
    // Check if .dbf file exists
    if (!QFile::exists(fileName)) {
        QMessageBox::warning(this, "Error", 
            QString("Database file not found: %1").arg(fileName));
        return;
    }
    
    // Show user selection dialog for this database
    // admin/admin is always available (default)
    // Other users can be selected if they exist in the database
    UserSelectionDialog userDialog(this, dbPath.toLocal8Bit().constData());
    
    if (userDialog.exec() != QDialog::Accepted) {
        // User cancelled login - don't open database
        QMessageBox::information(this, "Cancelled", 
            QString("Database '%1' was not opened.").arg(dbName));
        return;
    }
    
    // Set selected user in session
    std::string selectedUser = userDialog.getSelectedUser();
    SessionManager::getInstance().login(selectedUser);
    
    // Set current database
    setCurrentDatabase(dbPath.toLocal8Bit().constData());
    
    QMessageBox::information(this, "Success", 
        QString("Database '%1' opened successfully as user '%2'.").arg(dbName).arg(QString::fromStdString(selectedUser)));
}

void MainWindow::setCurrentDatabase(const std::string& dbPath)
{
    m_databasePath = dbPath;
    
    // Extract database name for display (just the filename without path and extension)
    QFileInfo fileInfo(QString::fromLocal8Bit(dbPath.c_str()));
    QString dbName = fileInfo.baseName();
    if (dbName.isEmpty()) {
        // If no path, use the dbPath directly
        dbName = QString::fromLocal8Bit(dbPath.c_str());
    }
    m_currentDatabase = dbName;
    
    // Load constraints from .cst file
    // 注意：数据库名应该是文件名（不含路径），路径用于确定.cst文件位置
    std::string dbNameStr = dbName.toStdString();
    ConstraintRegistry::getInstance().clearDatabase(dbNameStr);
    ConstraintStorageManager::loadConstraints(dbNameStr, dbPath);
    
    // Update status bar
    updateStatusBar();
    
    // Update table management widget if it exists
    if (m_tableManagementWidget) {
        m_tableManagementWidget->setDatabasePath(m_databasePath);
        m_tableManagementWidget->refreshTableList();
    }
    
    // Update index management widget if it exists
    // setDatabasePath will automatically load indices from .idx file
    if (m_indexManagementWidget) {
        m_indexManagementWidget->setDatabasePath(m_databasePath);
        m_indexManagementWidget->refreshIndexList();
        // Share IndexAdvisor from SQLQueryWidget so recommendations can access query logs
        if (m_sqlQueryWidget) {
            m_indexManagementWidget->setIndexAdvisor(m_sqlQueryWidget->getIndexAdvisor());
        }
    }
    
    // Update data operation widget if it exists
    if (m_dataOperationWidget) {
        m_dataOperationWidget->setDatabasePath(m_databasePath);
        m_dataOperationWidget->loadTableList();  // Refresh table list
    }
    
    // Update SQL query widget if it exists
    if (m_sqlQueryWidget) {
        m_sqlQueryWidget->setDatabasePath(m_databasePath);
    }
    
    // Update user management widget if it exists
    if (m_userManagementWidget) {
        m_userManagementWidget->setDatabasePath(m_databasePath);
    }
    
    // Update status bar message
    statusBar()->showMessage(QString("Database: %1").arg(m_currentDatabase), 3000);
}

std::string MainWindow::getCurrentDatabasePath() const
{
    return m_databasePath;
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About", 
        "Database Management System\n\n"
        "Version: 0.6.1\n"
        "Database New Technology Practice Course Project\n\n"
        "Features:\n"
        "- Table Structure Management\n"
        "- Data Operations\n"
        "- SQL Query Execution\n"
        "- Index Management\n"
        "- Index Optimization\n\n"
        "Keyboard Shortcuts:\n"
        "- Ctrl+Q: Exit\n"
        "- F1: Help\n"
        "- Ctrl+Tab: Next tab\n"
        "- Ctrl+Shift+Tab: Previous tab");
}

void MainWindow::setupShortcuts()
{
    // General shortcuts
    QAction *exitAction = new QAction(this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    addAction(exitAction);

    // Help shortcut
    QAction *helpAction = new QAction(this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showAbout);
    addAction(helpAction);

    // Tab navigation shortcuts
    QAction *nextTabAction = new QAction(this);
    nextTabAction->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(nextTabAction, &QAction::triggered, this, [this]() {
        int currentIndex = m_tabWidget->currentIndex();
        int nextIndex = (currentIndex + 1) % m_tabWidget->count();
        m_tabWidget->setCurrentIndex(nextIndex);
    });
    addAction(nextTabAction);

    QAction *prevTabAction = new QAction(this);
    prevTabAction->setShortcut(QKeySequence("Ctrl+Shift+Tab"));
    connect(prevTabAction, &QAction::triggered, this, [this]() {
        int currentIndex = m_tabWidget->currentIndex();
        int prevIndex = (currentIndex - 1 + m_tabWidget->count()) % m_tabWidget->count();
        m_tabWidget->setCurrentIndex(prevIndex);
    });
    addAction(prevTabAction);

    // Note: Other shortcuts (Ctrl+N, Ctrl+I, etc.) will be added when corresponding features are implemented
}

void MainWindow::onTabChanged(int index)
{
    QString tabName = m_tabWidget->tabText(index);
    statusBar()->showMessage(QString("Switched to: %1").arg(tabName), 2000);
    
    // Refresh Data Operation tab when switching to it
    if (tabName == "Data Operation" && m_dataOperationWidget) {
        m_dataOperationWidget->loadTableList();
    }
}

