#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QFont>
#include <QTimer>
#include <QDateTime>
#include <QTextEdit>
#include <QString>

/**
 * @file main_window.h
 * @brief Main Window Class
 *
 * Main window of the Intelligent Music Playback Management System
 */

/**
 * @brief Main Window Class
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MainWindow();

private slots:
    /**
     * @brief Show about dialog
     */
    void showAbout();

    /**
     * @brief Handle tab change
     * @param index Tab index
     */
    void onTabChanged(int index);

    /**
     * @brief Update status bar with database name and current time
     */
    void updateStatusBar();

    /**
     * @brief Create a new database
     */
    void onCreateDatabase();

    /**
     * @brief Open an existing database
     */
    void onOpenDatabase();

private:
    /**
     * @brief Initialize UI
     */
    void setupUI();

    /**
     * @brief Create menu bar
     */
    void createMenuBar();

    /**
     * @brief Create status bar
     */
    void createStatusBar();

    /**
     * @brief Setup fonts (Segoe UI for all text)
     */
    void setupFonts();

    /**
     * @brief Create tab widget with all tabs
     */
    void createTabWidget();

    /**
     * @brief Create table management tab
     * @return Widget for table management
     */
    QWidget* createTableManagementTab();

    /**
     * @brief Create data operation tab
     * @return Widget for data operation
     */
    QWidget* createDataOperationTab();

    /**
     * @brief Create SQL execution tab
     * @return Widget for SQL execution
     */
    QWidget* createSQLExecutionTab();

    /**
     * @brief Create guide tab
     * @return Widget for user guide
     */
    QWidget* createGuideTab();

    /**
     * @brief Create index management tab
     * @return Widget for index management
     */
    QWidget* createIndexManagementTab();

    /**
     * @brief Setup keyboard shortcuts
     */
    void setupShortcuts();

    /**
     * @brief Set current database and update all components
     * @param dbPath Database file path (without extension)
     */
    void setCurrentDatabase(const std::string& dbPath);

    /**
     * @brief Get current database path
     * @return Database file path (without extension), empty if no database loaded
     */
    std::string getCurrentDatabasePath() const;

    // UI Components
    QWidget *m_centralWidget;      // Central widget
    QVBoxLayout *m_mainLayout;     // Main layout
    QTabWidget *m_tabWidget;       // Tab widget for main workspace
    QLabel *m_statusLabel;         // Status bar label for database name and time
    QTimer *m_timer;               // Timer for updating time display
    QString m_currentDatabase;    // Current database name (empty if no database loaded)
    std::string m_databasePath;    // Current database file path (without extension)
    
    // Tab widgets (for updating database path)
    class TableManagementWidget *m_tableManagementWidget;  // Table management widget
    class DataOperationWidget *m_dataOperationWidget;     // Data operation widget
};

