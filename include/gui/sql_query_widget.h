#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QListWidget>
#include <QGroupBox>
#include <QScrollArea>
#include <string>
#include "query/select_handler.h"
#include "index/index_advisor.h"

// Forward declarations
class QueryExecutor;

/**
 * @file sql_query_widget.h
 * @brief SQL Query Widget Class
 *
 * Widget for executing SQL statements and displaying query results
 */

/**
 * @brief SQL Query Widget Class
 */
class SQLQueryWidget : public QWidget {
    Q_OBJECT

public:
    explicit SQLQueryWidget(QWidget *parent = nullptr);
    ~SQLQueryWidget();

    /**
     * @brief Set database path
     * @param dbPath Database file path (without extension)
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief Get the IndexAdvisor instance (for sharing with IndexManagementWidget)
     * @return Reference to the IndexAdvisor
     */
    class IndexAdvisor* getIndexAdvisor();

private slots:
    void onExecuteSQL();
    void onClearSQL();
    void onClearResults();

private:
    void setupUI();
    void displayQueryResult(const std::string& result);
    void displayQueryResultFromStruct(const QueryResult& result);
    void displayError(const std::string& error);
    void displaySuccess(const std::string& message, size_t affectedRows = 0);
    QStringList splitSQLStatements(const QString& sqlText);
    void executeBatchStatements(const QStringList& statements);
    void updateAdvicePanel(const std::string& sql, const std::string& tableName,
                          const std::vector<std::string>& whereFields,
                          double executionTime, size_t resultCount);

    // UI Components
    QVBoxLayout *m_mainLayout;
    QSplitter *m_splitter;
    QWidget *m_sqlInputWidget;
    QVBoxLayout *m_sqlInputLayout;
    QLabel *m_sqlLabel;
    QTextEdit *m_sqlInput;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_executeBtn;
    QPushButton *m_clearSQLBtn;
    QPushButton *m_clearResultsBtn;
    
    QWidget *m_resultWidget;
    QHBoxLayout *m_resultLayout;  // Changed to horizontal layout
    QWidget *m_resultTableWidget;
    QVBoxLayout *m_resultTableLayout;
    QLabel *m_resultLabel;
    QTableWidget *m_resultTable;
    QLabel *m_statusLabel;
    
    // Smart Advice Panel (right side)
    QGroupBox *m_advicePanel;
    QVBoxLayout *m_adviceLayout;
    QLabel *m_adviceTitle;
    QListWidget *m_adviceList;
    QLabel *m_adviceEmptyLabel;
    
    // Backend
    QueryExecutor *m_queryExecutor;
    std::string m_databasePath;
};

