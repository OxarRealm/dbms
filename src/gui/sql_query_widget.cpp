/**
 * @file sql_query_widget.cpp
 * @brief SQL Query Widget Implementation
 */

#include "gui/sql_query_widget.h"
#include "query/query_executor.h"
#include "query/select_handler.h"
#include "index/index_advisor.h"
#include "core/index_manager.h"
#include "core/index_storage.h"
#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFont>
#include <QScrollArea>
#include <QSplitter>
#include <QKeySequence>
#include <QStringList>
#include <QRegularExpression>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QLabel>
#include <functional>
#include <algorithm>

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
    , m_resultTableWidget(nullptr)
    , m_resultTableLayout(nullptr)
    , m_resultLabel(nullptr)
    , m_resultTable(nullptr)
    , m_statusLabel(nullptr)
    , m_advicePanel(nullptr)
    , m_adviceLayout(nullptr)
    , m_adviceTitle(nullptr)
    , m_adviceList(nullptr)
    , m_adviceEmptyLabel(nullptr)
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

IndexAdvisor* SQLQueryWidget::getIndexAdvisor()
{
    if (!m_queryExecutor) {
        return nullptr;
    }
    return &m_queryExecutor->getIndexAdvisor();
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
    m_resultLayout = new QHBoxLayout(m_resultWidget);  // Changed to horizontal
    m_resultLayout->setSpacing(10);
    m_resultLayout->setContentsMargins(0, 0, 0, 0);

    // Left side: Query Results
    m_resultTableWidget = new QWidget();
    m_resultTableLayout = new QVBoxLayout(m_resultTableWidget);
    m_resultTableLayout->setSpacing(5);
    m_resultTableLayout->setContentsMargins(0, 0, 0, 0);

    m_resultLabel = new QLabel("Query Results:", m_resultTableWidget);
    m_resultLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_resultTableLayout->addWidget(m_resultLabel);

    m_resultTable = new QTableWidget(m_resultTableWidget);
    m_resultTable->setFont(QFont("Segoe UI", 9));
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    m_resultTable->setAlternatingRowColors(true);
    m_resultTable->setMinimumHeight(200);
    m_resultTableLayout->addWidget(m_resultTable);

    m_clearResultsBtn = new QPushButton("Clear Results", m_resultTableWidget);
    m_clearResultsBtn->setFont(QFont("Segoe UI", 9));
    connect(m_clearResultsBtn, &QPushButton::clicked, this, &SQLQueryWidget::onClearResults);
    m_resultTableLayout->addWidget(m_clearResultsBtn);

    m_statusLabel = new QLabel("Ready", m_resultTableWidget);
    m_statusLabel->setFont(QFont("Segoe UI", 9));
    m_statusLabel->setAlignment(Qt::AlignLeft);
    m_statusLabel->setStyleSheet("color: gray;");
    m_resultTableLayout->addWidget(m_statusLabel);

    m_resultLayout->addWidget(m_resultTableWidget, 2);  // 2/3 width

    // Right side: Smart Advice Panel
    m_advicePanel = new QGroupBox("Smart Recommendations", m_resultWidget);
    m_advicePanel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_adviceLayout = new QVBoxLayout(m_advicePanel);
    m_adviceLayout->setSpacing(5);
    m_adviceLayout->setContentsMargins(10, 15, 10, 10);

    m_adviceTitle = new QLabel("Query Analysis & Suggestions", m_advicePanel);
    m_adviceTitle->setFont(QFont("Segoe UI", 8));
    m_adviceTitle->setStyleSheet("color: #666;");
    m_adviceLayout->addWidget(m_adviceTitle);

    m_adviceList = new QListWidget(m_advicePanel);
    m_adviceList->setFont(QFont("Segoe UI", 8));
    m_adviceList->setWordWrap(true);
    m_adviceList->setMinimumWidth(300);
    m_adviceList->setMaximumWidth(400);
    m_adviceLayout->addWidget(m_adviceList);

    m_adviceEmptyLabel = new QLabel("Execute a query to see recommendations", m_advicePanel);
    m_adviceEmptyLabel->setFont(QFont("Segoe UI", 8));
    m_adviceEmptyLabel->setStyleSheet("color: #999; padding: 20px;");
    m_adviceEmptyLabel->setAlignment(Qt::AlignCenter);
    m_adviceEmptyLabel->setWordWrap(true);
    m_adviceLayout->addWidget(m_adviceEmptyLabel);

    m_resultLayout->addWidget(m_advicePanel, 1);  // 1/3 width

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
            
            // Extract query information for advice analysis
            std::string tableName = "";
            std::vector<std::string> whereFields;
            double executionTime = 1.0;  // Default
            size_t resultCount = result.queryResult.rows.size();
            
            // Parse SQL to extract table name and WHERE fields
            Parser parser(sql);
            std::unique_ptr<ASTNode> ast = parser.parse();
            if (ast) {
                SelectNode* selectNode = dynamic_cast<SelectNode*>(ast.get());
                if (selectNode && !selectNode->fromTables.empty()) {
                    tableName = selectNode->fromTables[0];
                    if (!selectNode->whereField.empty()) {
                        whereFields.push_back(selectNode->whereField);
                    } else if (selectNode->whereClause) {
                        // Extract WHERE fields from AST recursively
                        std::function<void(const WhereCondition*)> extractFields = 
                            [&](const WhereCondition* cond) {
                                if (!cond) return;
                                
                                // Extract field name (may contain table name like "Table.Field")
                                if (!cond->fieldName.empty()) {
                                    std::string fieldName = cond->fieldName;
                                    // Remove table prefix if present
                                    size_t dotPos = fieldName.find('.');
                                    if (dotPos != std::string::npos) {
                                        fieldName = fieldName.substr(dotPos + 1);
                                    }
                                    // Avoid duplicates
                                    if (std::find(whereFields.begin(), whereFields.end(), fieldName) == whereFields.end()) {
                                        whereFields.push_back(fieldName);
                                    }
                                }
                                
                                // Recursively process left and right conditions
                                if (cond->left) {
                                    extractFields(cond->left.get());
                                }
                                if (cond->right) {
                                    extractFields(cond->right.get());
                                }
                            };
                        
                        extractFields(selectNode->whereClause.get());
                    }
                }
            }
            
            // Try to get execution time from query logs
            IndexAdvisor* advisor = getIndexAdvisor();
            if (advisor && advisor->getLogCount() > 0) {
                std::vector<QueryLogEntry> allQueries;
                if (advisor->identifySlowQueries(allQueries, 0.0)) {
                    if (!allQueries.empty()) {
                        // Get the last query's execution time
                        executionTime = allQueries.back().executionTime;
                    }
                }
            }
            
            // Update advice panel
            updateAdvicePanel(sql, tableName, whereFields, executionTime, resultCount);
            
            // Show success message box for query
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("SQL Execution Success");
            
            // 构建消息文本（包含索引使用信息）
            QString message = QString("Query executed successfully. %1 row(s) returned.")
                .arg(static_cast<int>(result.queryResult.rows.size()));
            
            // 如果使用了索引，添加索引使用提示
            if (!result.usedIndices.empty()) {
                message += "\n\nIndex(es) used: ";
                QStringList indexList;
                for (const auto& index : result.usedIndices) {
                    indexList.append(QString::fromStdString(index));
                }
                message += indexList.join(", ");
            }
            
            msgBox.setText(message);
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
    
    // Clear advice panel
    if (m_adviceList) {
        m_adviceList->clear();
    }
    if (m_adviceEmptyLabel) {
        m_adviceEmptyLabel->show();
    }
}

void SQLQueryWidget::updateAdvicePanel(const std::string& sql, const std::string& tableName,
                                       const std::vector<std::string>& whereFields,
                                       double executionTime, size_t resultCount)
{
    if (!m_queryExecutor || !m_adviceList) {
        return;
    }
    
    IndexAdvisor* advisor = getIndexAdvisor();
    if (!advisor) {
        return;
    }
    
    // Get IndexManager from QueryExecutor to check if indices exist
    IndexManager* indexManager = m_queryExecutor->getIndexManager();
    
    // Analyze current query and get advice
    std::vector<QueryAdvice> adviceList;
    advisor->analyzeCurrentQuery(sql, tableName, whereFields, executionTime, resultCount, adviceList);
    
    // Fix hasIndex check: IndexAdvisor uses internal Index objects that may not have loaded from .idx file
    // So we need to override the check using IndexManager which has loaded indices
    if (indexManager) {
        // Reload indices from .idx file to ensure IndexManager has latest data
        std::string dbName = QueryExecutor::extractDatabaseName(m_databasePath);
        if (!dbName.empty()) {
            IndexStorageManager::loadIndices(dbName, m_databasePath, *indexManager);
        }
        
        // Filter out recommendations for fields that already have indices
        std::vector<QueryAdvice> filteredAdviceList;
        for (auto& advice : adviceList) {
            if (advice.type == QueryAdviceType::INDEX_RECOMMEND) {
                // Extract field name from suggestion (format: "CREATE INDEX TableName_FieldName_idx ON TableName(FieldName)...")
                // Or check using IndexManager
                // For now, we'll check if the suggestion contains a field name and verify it doesn't have an index
                // But we need to extract table and field from the advice message or suggestion
                // Actually, we should check the WHERE fields directly
                bool hasIndex = false;
                for (const auto& fieldName : whereFields) {
                    if (indexManager->hasIndex(tableName, fieldName)) {
                        hasIndex = true;
                        break;
                    }
                }
                if (!hasIndex) {
                    filteredAdviceList.push_back(advice);
                }
            } else {
                // Keep non-index recommendations
                filteredAdviceList.push_back(advice);
            }
        }
        adviceList = filteredAdviceList;
    }
    
    // Clear previous advice
    m_adviceList->clear();
    
    if (adviceList.empty()) {
        if (m_adviceEmptyLabel) {
            m_adviceEmptyLabel->show();
        }
        return;
    }
    
    if (m_adviceEmptyLabel) {
        m_adviceEmptyLabel->hide();
    }
    
    // Add advice items to list
    for (const auto& advice : adviceList) {
        // Create a container widget for the advice item
        QWidget* itemWidget = new QWidget(m_adviceList);
        QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 8, 10, 8);
        itemLayout->setSpacing(5);
        
        // Set background color based on severity
        QString bgColor;
        if (advice.severity == "warning") {
            bgColor = "background-color: rgb(255, 248, 220);";  // Light yellow
        } else if (advice.severity == "error") {
            bgColor = "background-color: rgb(255, 235, 238);";  // Light red
        } else {
            bgColor = "background-color: rgb(240, 248, 255);";  // Light blue
        }
        itemWidget->setStyleSheet(bgColor + "border-radius: 4px;");
        
        // Title label (bold, colored)
        QLabel* titleLabel = new QLabel(itemWidget);
        QString titleColor;
        if (advice.severity == "warning") {
            titleColor = "#d97706";
        } else if (advice.severity == "error") {
            titleColor = "#dc2626";
        } else {
            titleColor = "#2563eb";
        }
        titleLabel->setText(QString("<b style='color: %1;'>%2</b>")
                           .arg(titleColor)
                           .arg(QString::fromStdString(advice.title)));
        titleLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
        titleLabel->setWordWrap(true);
        itemLayout->addWidget(titleLabel);
        
        // Message label
        QLabel* messageLabel = new QLabel(itemWidget);
        messageLabel->setText(QString::fromStdString(advice.message));
        messageLabel->setFont(QFont("Segoe UI", 8));
        messageLabel->setWordWrap(true);
        messageLabel->setStyleSheet("color: #333;");
        itemLayout->addWidget(messageLabel);
        
        // Suggestion label (if exists)
        if (!advice.suggestion.empty()) {
            QLabel* suggestionLabel = new QLabel(itemWidget);
            QString suggestionText = QString::fromStdString(advice.suggestion);
            
            // Add index type explanation for index recommendations
            if (advice.type == QueryAdviceType::INDEX_RECOMMEND) {
                // Extract index type from suggestion and add explanation
                if (suggestionText.contains("USING hash")) {
                    suggestionText += "\n\n<i style='color: #666; font-size: 7pt;'>Hash Index: Best for point queries (WHERE field = value). O(1) average time complexity. Suitable for primary keys and integer fields.</i>";
                } else if (suggestionText.contains("USING btree")) {
                    suggestionText += "\n\n<i style='color: #666; font-size: 7pt;'>B+ Tree Index: General-purpose index. Supports point queries, range queries (>, <, BETWEEN), and sorting (ORDER BY). Recommended for most use cases.</i>";
                } else if (suggestionText.contains("USING adjacent")) {
                    suggestionText += "\n\n<i style='color: #666; font-size: 7pt;'>Adjacent Index: Optimized for range queries (WHERE field >= value AND field <= value). Suitable for numeric and ordered fields.</i>";
                }
            }
            
            suggestionLabel->setText(QString("<i style='color: #0066cc;'>💡 %1</i>")
                                    .arg(suggestionText));
            suggestionLabel->setFont(QFont("Segoe UI", 8));
            suggestionLabel->setWordWrap(true);
            suggestionLabel->setStyleSheet("color: #0066cc;");
            itemLayout->addWidget(suggestionLabel);
        }
        
        // Create list item and set widget
        QListWidgetItem* item = new QListWidgetItem(m_adviceList);
        item->setSizeHint(itemWidget->sizeHint());
        item->setToolTip(QString::fromStdString(advice.suggestion));
        m_adviceList->setItemWidget(item, itemWidget);
    }
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
    bool inComment = false;
    
    for (int i = 0; i < sqlText.length(); ++i) {
        QChar ch = sqlText[i];
        
        // Handle single-line comments (--)
        if (!inString && !inComment && i < sqlText.length() - 1 && 
            ch == '-' && sqlText[i+1] == '-') {
            // Skip the rest of the line (comment)
            while (i < sqlText.length() && sqlText[i] != '\n') {
                i++;
            }
            // If we have accumulated a statement, add it before the comment
            if (!currentStatement.trimmed().isEmpty()) {
                currentStatement = currentStatement.trimmed();
                if (!currentStatement.isEmpty()) {
                    statements.append(currentStatement);
                }
                currentStatement.clear();
            }
            continue;
        }
        
        // Handle string literals
        if (!inString && !inComment && (ch == '\'' || ch == '"')) {
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
        } else if (!inString && !inComment && ch == ';') {
            // End of statement - include semicolon
            currentStatement += ch;  // Include the semicolon
            currentStatement = currentStatement.trimmed();
            if (!currentStatement.isEmpty()) {
                statements.append(currentStatement);
            }
            currentStatement.clear();
        } else if (!inString && !inComment) {
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

