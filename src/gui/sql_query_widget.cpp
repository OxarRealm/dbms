/**
 * @file sql_query_widget.cpp
 * @brief SQL Query Widget Implementation
 */

#include "gui/sql_query_widget.h"
#include "query/query_executor.h"
#include "query/select_handler.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFont>
#include <QScrollArea>
#include <QSplitter>
#include <QKeySequence>
#include <QStringList>
#include <QRegularExpression>

SQLQueryWidget::SQLQueryWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_splitter(nullptr)
    , m_sqlInputWidget(nullptr)
    , m_sqlInputLayout(nullptr)
    , m_sqlLabel(nullptr)
    , m_sqlInput(nullptr)
    , m_buttonLayout(nullptr)
    , m_executeBtn(nullptr)
    , m_clearSQLBtn(nullptr)
    , m_clearResultsBtn(nullptr)
    , m_resultWidget(nullptr)
    , m_resultLayout(nullptr)
    , m_resultLabel(nullptr)
    , m_resultTable(nullptr)
    , m_statusLabel(nullptr)
    , m_queryExecutor(nullptr)
    , m_databasePath("")
{
    m_queryExecutor = new QueryExecutor();
    setupUI();
}

SQLQueryWidget::~SQLQueryWidget()
{
    if (m_queryExecutor) {
        delete m_queryExecutor;
    }
}

void SQLQueryWidget::setDatabasePath(const std::string& dbPath)
{
    m_databasePath = dbPath;
    m_queryExecutor->setDatabasePath(m_databasePath);
}

void SQLQueryWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    // Create splitter for resizable panels
    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->setFont(QFont("Segoe UI", 9));

    // ========== SQL Input Panel (Top) ==========
    m_sqlInputWidget = new QWidget();
    m_sqlInputLayout = new QVBoxLayout(m_sqlInputWidget);
    m_sqlInputLayout->setSpacing(5);
    m_sqlInputLayout->setContentsMargins(0, 0, 0, 0);

    m_sqlLabel = new QLabel("SQL Statement:", m_sqlInputWidget);
    m_sqlLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_sqlInputLayout->addWidget(m_sqlLabel);

    m_sqlInput = new QTextEdit(m_sqlInputWidget);
    m_sqlInput->setFont(QFont("Consolas", 10));  // Use monospace font for SQL
    m_sqlInput->setPlaceholderText("Enter SQL statement here...\n\nExample:\nCREATE TABLE Users (UserID int KEY NO_NULL VALID, UserName char[50] NOT_KEY NO_NULL VALID) INTO MyDB;\n\nINSERT INTO Users VALUES (1, 'Alice') IN MyDB;\n\nSELECT * FROM Users;");
    m_sqlInput->setMinimumHeight(150);
    m_sqlInputLayout->addWidget(m_sqlInput);

    // Buttons
    m_buttonLayout = new QHBoxLayout();
    m_executeBtn = new QPushButton("Execute SQL", m_sqlInputWidget);
    m_executeBtn->setFont(QFont("Segoe UI", 9));
    m_executeBtn->setDefault(true);
    m_executeBtn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return));
    connect(m_executeBtn, &QPushButton::clicked, this, &SQLQueryWidget::onExecuteSQL);
    m_buttonLayout->addWidget(m_executeBtn);

    m_clearSQLBtn = new QPushButton("Clear SQL", m_sqlInputWidget);
    m_clearSQLBtn->setFont(QFont("Segoe UI", 9));
    connect(m_clearSQLBtn, &QPushButton::clicked, this, &SQLQueryWidget::onClearSQL);
    m_buttonLayout->addWidget(m_clearSQLBtn);

    m_buttonLayout->addStretch();
    m_sqlInputLayout->addLayout(m_buttonLayout);

    m_splitter->addWidget(m_sqlInputWidget);

    // ========== Result Panel (Bottom) ==========
    m_resultWidget = new QWidget();
    m_resultLayout = new QVBoxLayout(m_resultWidget);
    m_resultLayout->setSpacing(5);
    m_resultLayout->setContentsMargins(0, 0, 0, 0);

    m_resultLabel = new QLabel("Query Results:", m_resultWidget);
    m_resultLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_resultLayout->addWidget(m_resultLabel);

    m_resultTable = new QTableWidget(m_resultWidget);
    m_resultTable->setFont(QFont("Segoe UI", 9));
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    m_resultTable->setAlternatingRowColors(true);
    m_resultTable->setMinimumHeight(200);
    m_resultLayout->addWidget(m_resultTable);

    m_clearResultsBtn = new QPushButton("Clear Results", m_resultWidget);
    m_clearResultsBtn->setFont(QFont("Segoe UI", 9));
    connect(m_clearResultsBtn, &QPushButton::clicked, this, &SQLQueryWidget::onClearResults);
    m_resultLayout->addWidget(m_clearResultsBtn);

    m_statusLabel = new QLabel("Ready", m_resultWidget);
    m_statusLabel->setFont(QFont("Segoe UI", 9));
    m_statusLabel->setAlignment(Qt::AlignLeft);
    m_statusLabel->setStyleSheet("color: gray;");
    m_resultLayout->addWidget(m_statusLabel);

    m_splitter->addWidget(m_resultWidget);

    // Set splitter sizes (60% for SQL input, 40% for results)
    m_splitter->setSizes({600, 400});

    m_mainLayout->addWidget(m_splitter);
}

void SQLQueryWidget::onExecuteSQL()
{
    if (m_databasePath.empty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("No Database");
        msgBox.setText("Please create or open a database first.\n\nUse File -> Create Database or File -> Open Database.");
        msgBox.setFont(QFont("Segoe UI", 9));
        msgBox.exec();
        return;
    }

    QString sqlText = m_sqlInput->toPlainText().trimmed();
    if (sqlText.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Empty SQL");
        msgBox.setText("Please enter a SQL statement.");
        msgBox.setFont(QFont("Segoe UI", 9));
        msgBox.exec();
        return;
    }

    // Split SQL text into individual statements by semicolon
    // Handle both single statement and multiple statements
    QStringList statements = splitSQLStatements(sqlText);
    
    if (statements.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Empty SQL");
        msgBox.setText("Please enter a SQL statement.");
        msgBox.setFont(QFont("Segoe UI", 9));
        msgBox.exec();
        return;
    }

    // Execute statements
    if (statements.size() == 1) {
        // Single statement - execute directly
        std::string sql = statements[0].toStdString();
        ExecutionResult result = m_queryExecutor->execute(sql);
        
        // Display result
        if (result.type == ExecutionResultType::SUCCESS) {
            if (result.affectedRows > 0) {
                displaySuccess(result.formattedOutput, result.affectedRows);
            } else {
                displaySuccess(result.formattedOutput);
            }
        } else if (result.type == ExecutionResultType::QUERY_RESULT) {
            // Display query result directly from QueryResult structure
            displayQueryResultFromStruct(result.queryResult);
            // Show success message box for query
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("SQL Execution Success");
            msgBox.setText(QString("Query executed successfully. %1 row(s) returned.")
                .arg(static_cast<int>(result.queryResult.rows.size())));
            msgBox.setFont(QFont("Segoe UI", 9));
            msgBox.exec();
        } else {
            displayError(result.errorMessage);
        }
    } else {
        // Multiple statements - execute batch
        executeBatchStatements(statements);
    }
}

void SQLQueryWidget::onClearSQL()
{
    m_sqlInput->clear();
    m_sqlInput->setFocus();
}

void SQLQueryWidget::onClearResults()
{
    m_resultTable->clear();
    m_resultTable->setRowCount(0);
    m_resultTable->setColumnCount(0);
    m_statusLabel->setText("Ready");
    m_statusLabel->setStyleSheet("color: gray;");
}

void SQLQueryWidget::displayQueryResult(const std::string& result)
{
    // This method is kept for compatibility but not used
    // Use displayQueryResultFromStruct instead
    displayQueryResultFromStruct(QueryResult());
}

void SQLQueryWidget::displayQueryResultFromStruct(const QueryResult& result)
{
    m_resultTable->clear();
    
    if (result.columnNames.empty()) {
        m_resultTable->setRowCount(0);
        m_resultTable->setColumnCount(0);
        m_statusLabel->setText("Query executed successfully (no results)");
        m_statusLabel->setStyleSheet("color: green;");
        return;
    }

    // Set column headers
    QStringList headers;
    for (const auto& colName : result.columnNames) {
        headers << QString::fromStdString(colName);
    }
    m_resultTable->setColumnCount(headers.size());
    m_resultTable->setHorizontalHeaderLabels(headers);

    // Set data rows
    int rowCount = static_cast<int>(result.rows.size());
    m_resultTable->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        const auto& row = result.rows[i];
        for (int j = 0; j < headers.size(); ++j) {
            QString value = (j < static_cast<int>(row.size())) ? 
                QString::fromStdString(row[j]) : "";
            QTableWidgetItem *item = new QTableWidgetItem(value);
            item->setFont(QFont("Segoe UI", 9));
            m_resultTable->setItem(i, j, item);
        }
    }

    // Resize columns to content
    m_resultTable->resizeColumnsToContents();

    m_statusLabel->setText(QString("Query executed successfully. %1 row(s) returned.").arg(rowCount));
    m_statusLabel->setStyleSheet("color: green;");
}

void SQLQueryWidget::displayError(const std::string& error)
{
    m_resultTable->clear();
    m_resultTable->setRowCount(0);
    m_resultTable->setColumnCount(0);
    
    m_statusLabel->setText(QString::fromStdString("Error: " + error));
    m_statusLabel->setStyleSheet("color: red;");
    
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("SQL Execution Error");
    msgBox.setText(QString::fromStdString(error));
    msgBox.setFont(QFont("Segoe UI", 9));
    msgBox.exec();
}

void SQLQueryWidget::displaySuccess(const std::string& message, size_t affectedRows)
{
    m_resultTable->clear();
    m_resultTable->setRowCount(0);
    m_resultTable->setColumnCount(0);
    
    QString statusText = QString::fromStdString(message);
    if (affectedRows > 0) {
        statusText += QString(" (%1 row(s) affected)").arg(affectedRows);
    }
    
    m_statusLabel->setText(statusText);
    m_statusLabel->setStyleSheet("color: green;");

    // Show success message box
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("SQL Execution Success");
    msgBox.setText(statusText);
    msgBox.setFont(QFont("Segoe UI", 9));
    msgBox.exec();
}

QStringList SQLQueryWidget::splitSQLStatements(const QString& sqlText)
{
    QStringList statements;
    QString currentStatement;
    bool inString = false;
    char stringChar = '\0';
    
    for (int i = 0; i < sqlText.length(); ++i) {
        QChar ch = sqlText[i];
        
        // Handle string literals
        if (!inString && (ch == '\'' || ch == '"')) {
            inString = true;
            stringChar = ch.toLatin1();
            currentStatement += ch;
        } else if (inString && ch == stringChar) {
            // Check if it's escaped
            if (i > 0 && sqlText[i-1] != '\\') {
                inString = false;
                stringChar = '\0';
            }
            currentStatement += ch;
        } else if (!inString && ch == ';') {
            // End of statement - include semicolon
            currentStatement += ch;  // Include the semicolon
            currentStatement = currentStatement.trimmed();
            if (!currentStatement.isEmpty()) {
                statements.append(currentStatement);
            }
            currentStatement.clear();
        } else {
            currentStatement += ch;
        }
    }
    
    // Add remaining statement if any
    currentStatement = currentStatement.trimmed();
    if (!currentStatement.isEmpty()) {
        statements.append(currentStatement);
    }
    
    return statements;
}

void SQLQueryWidget::executeBatchStatements(const QStringList& statements)
{
    int successCount = 0;
    int errorCount = 0;
    size_t totalAffectedRows = 0;
    QStringList errorMessages;
    QueryResult lastQueryResult;
    bool hasQueryResult = false;
    
    // Execute each statement
    for (int i = 0; i < statements.size(); ++i) {
        std::string sql = statements[i].toStdString();
        ExecutionResult result = m_queryExecutor->execute(sql);
        
        if (result.type == ExecutionResultType::SUCCESS) {
            successCount++;
            totalAffectedRows += result.affectedRows;
        } else if (result.type == ExecutionResultType::QUERY_RESULT) {
            successCount++;
            lastQueryResult = result.queryResult;
            hasQueryResult = true;
        } else {
            errorCount++;
            errorMessages.append(QString("Statement %1: %2").arg(i + 1).arg(QString::fromStdString(result.errorMessage)));
        }
    }
    
    // Display results
    if (hasQueryResult) {
        // If there's a query result, display it
        displayQueryResultFromStruct(lastQueryResult);
    } else {
        // Clear result table for DML/DDL batch
        m_resultTable->clear();
        m_resultTable->setRowCount(0);
        m_resultTable->setColumnCount(0);
    }
    
    // Show summary message
    QString summary;
    if (errorCount == 0) {
        // All successful
        summary = QString("Batch execution completed successfully.\n\n"
                         "Total statements: %1\n"
                         "Successful: %2\n"
                         "Total rows affected: %3")
                  .arg(statements.size())
                  .arg(successCount)
                  .arg(totalAffectedRows);
        
        if (hasQueryResult) {
            summary += QString("\nLast query returned %1 row(s).")
                      .arg(static_cast<int>(lastQueryResult.rows.size()));
        }
        
        m_statusLabel->setText(QString("Batch execution: %1 statement(s) executed successfully, %2 row(s) affected")
                              .arg(successCount).arg(totalAffectedRows));
        m_statusLabel->setStyleSheet("color: green;");
        
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Batch Execution Success");
        msgBox.setText(summary);
        msgBox.setFont(QFont("Segoe UI", 9));
        msgBox.exec();
    } else {
        // Some errors
        summary = QString("Batch execution completed with errors.\n\n"
                         "Total statements: %1\n"
                         "Successful: %2\n"
                         "Failed: %3\n"
                         "Total rows affected: %4")
                  .arg(statements.size())
                  .arg(successCount)
                  .arg(errorCount)
                  .arg(totalAffectedRows);
        
        if (!errorMessages.isEmpty()) {
            summary += "\n\nErrors:\n";
            for (const QString& error : errorMessages) {
                summary += error + "\n";
            }
        }
        
        m_statusLabel->setText(QString("Batch execution: %1 succeeded, %2 failed")
                              .arg(successCount).arg(errorCount));
        m_statusLabel->setStyleSheet("color: orange;");
        
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Batch Execution Completed with Errors");
        msgBox.setText(summary);
        msgBox.setFont(QFont("Segoe UI", 9));
        msgBox.exec();
    }
}

