/**
 * @file data_operation_widget.cpp
 * @brief Data Operation Widget Implementation
 */

#include "gui/data_operation_widget.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include "core/table_mode.h"
#include "dml/insert_handler.h"
#include "dml/update_handler.h"
#include "dml/delete_handler.h"
#include <QCloseEvent>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QComboBox>
#include <cstring>
#include <sstream>

// ==================== RecordEditDialog Implementation ====================

RecordEditDialog::RecordEditDialog(QWidget *parent, 
                                   const TableInfo* tableInfo,
                                   const Record* existingRecord)
    : QDialog(parent)
    , m_tableInfo(tableInfo)
    , m_buttonBox(nullptr)
{
    setupUI();
    
    // Set button text and window title based on mode
    if (existingRecord) {
        loadRecord(*existingRecord);
        setWindowTitle("Edit Record");
        if (m_buttonBox) {
            m_buttonBox->button(QDialogButtonBox::Ok)->setText("Save");
        }
    } else {
        setWindowTitle("Insert Record");
        if (m_buttonBox) {
            m_buttonBox->button(QDialogButtonBox::Ok)->setText("Insert");
        }
    }
}

RecordEditDialog::~RecordEditDialog()
{
}

void RecordEditDialog::setupUI()
{
    if (!m_tableInfo) {
        return;
    }

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create field inputs
    for (size_t i = 0; i < m_tableInfo->fields.size(); ++i) {
        const TableMode& field = m_tableInfo->fields[i];
        
        QHBoxLayout *fieldLayout = new QHBoxLayout();
        
        // Field label
        QString labelText = QString::fromLocal8Bit(field.sFieldName);
        if (field.bKey == FLAG_KEY) {
            labelText += " (KEY)";
        }
        if (field.bNullFlag == FLAG_NO_NULL) {
            labelText += " *";
        }
        QLabel *label = new QLabel(labelText + ":", this);
        label->setFont(QFont("Segoe UI", 9));
        label->setMinimumWidth(120);
        fieldLayout->addWidget(label);
        
        // Field input
        QLineEdit *edit = new QLineEdit(this);
        edit->setFont(QFont("Segoe UI", 9));
        edit->setPlaceholderText(QString("Enter %1 value").arg(QString::fromLocal8Bit(field.sType)));
        fieldLayout->addWidget(edit);
        
        m_fieldLabels.push_back(label);
        m_fieldEdits.push_back(edit);
        m_mainLayout->addLayout(fieldLayout);
    }

    // Dialog buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    m_buttonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    m_mainLayout->addWidget(m_buttonBox);

    setMinimumSize(500, 300);
    resize(600, 400);
}

void RecordEditDialog::loadRecord(const Record& record)
{
    for (size_t i = 0; i < m_fieldEdits.size() && i < record.getFieldCount(); ++i) {
        QString value = QString::fromStdString(record.getValue(i));
        m_fieldEdits[i]->setText(value);
    }
}

bool RecordEditDialog::getRecord(Record& record)
{
    if (!m_tableInfo) {
        return false;
    }

    record.values.clear();
    record.validFlag = FLAG_VALID;

    // Validate and collect field values
    for (size_t i = 0; i < m_fieldEdits.size() && i < m_tableInfo->fields.size(); ++i) {
        const TableMode& field = m_tableInfo->fields[i];
        QString value = m_fieldEdits[i]->text().trimmed();
        
        // Check NULL constraint
        if (value.isEmpty() && field.bNullFlag == FLAG_NO_NULL) {
            QMessageBox::warning(this, "Validation Error", 
                QString("Field '%1' cannot be empty (NOT NULL constraint)").arg(QString::fromLocal8Bit(field.sFieldName)));
            return false;
        }
        
        // Type validation (basic)
        if (!value.isEmpty()) {
            std::string fieldType = std::string(field.sType);
            if (fieldType == "int") {
                bool ok;
                value.toInt(&ok);
                if (!ok) {
                    QMessageBox::warning(this, "Validation Error", 
                        QString("Field '%1' must be an integer").arg(QString::fromLocal8Bit(field.sFieldName)));
                    return false;
                }
            } else if (fieldType == "float" || fieldType == "double") {
                bool ok;
                value.toDouble(&ok);
                if (!ok) {
                    QMessageBox::warning(this, "Validation Error", 
                        QString("Field '%1' must be a number").arg(QString::fromLocal8Bit(field.sFieldName)));
                    return false;
                }
            }
        }
        
        record.values.push_back(value.toStdString());
    }

    return true;
}

void RecordEditDialog::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(this, "Confirm Exit", 
        "Are you sure you want to exit?\n\nAll unsaved changes will be lost.",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

// ==================== DataOperationWidget Implementation ====================

DataOperationWidget::DataOperationWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_topLayout(nullptr)
    , m_tableComboBox(nullptr)
    , m_insertBtn(nullptr)
    , m_editBtn(nullptr)
    , m_deleteBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_dataTable(nullptr)
    , m_statusLabel(nullptr)
    , m_tableManager(nullptr)
    , m_dataManager(nullptr)
    , m_databasePath("")
    , m_currentTableName("")
{
    m_tableManager = new TableManager();
    m_dataManager = new DataManager();
    setupUI();
}

DataOperationWidget::~DataOperationWidget()
{
    delete m_tableManager;
    delete m_dataManager;
}

void DataOperationWidget::setDatabasePath(const std::string& dbPath)
{
    m_databasePath = dbPath;
    m_tableManager->setDatabasePath(m_databasePath);
    m_dataManager->setDatabasePath(m_databasePath);
    loadTableList();
}

void DataOperationWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    // Top layout: Table selection and buttons
    m_topLayout = new QHBoxLayout();
    
    QLabel *tableLabel = new QLabel("Table:", this);
    tableLabel->setFont(QFont("Segoe UI", 9));
    m_topLayout->addWidget(tableLabel);
    
    m_tableComboBox = new QComboBox(this);
    m_tableComboBox->setFont(QFont("Segoe UI", 9));
    m_tableComboBox->setMinimumWidth(200);
    connect(m_tableComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &DataOperationWidget::onTableSelectionChanged);
    m_topLayout->addWidget(m_tableComboBox);
    
    m_topLayout->addStretch();
    
    m_insertBtn = new QPushButton("Insert", this);
    m_insertBtn->setFont(QFont("Segoe UI", 9));
    connect(m_insertBtn, &QPushButton::clicked, this, &DataOperationWidget::onInsertRecord);
    m_topLayout->addWidget(m_insertBtn);
    
    m_editBtn = new QPushButton("Edit", this);
    m_editBtn->setFont(QFont("Segoe UI", 9));
    connect(m_editBtn, &QPushButton::clicked, this, &DataOperationWidget::onEditRecord);
    m_topLayout->addWidget(m_editBtn);
    
    m_deleteBtn = new QPushButton("Delete", this);
    m_deleteBtn->setFont(QFont("Segoe UI", 9));
    connect(m_deleteBtn, &QPushButton::clicked, this, &DataOperationWidget::onDeleteRecord);
    m_topLayout->addWidget(m_deleteBtn);
    
    m_refreshBtn = new QPushButton("Refresh", this);
    m_refreshBtn->setFont(QFont("Segoe UI", 9));
    connect(m_refreshBtn, &QPushButton::clicked, this, &DataOperationWidget::onRefresh);
    m_topLayout->addWidget(m_refreshBtn);
    
    m_mainLayout->addLayout(m_topLayout);

    // Data table
    m_dataTable = new QTableWidget(this);
    m_dataTable->setFont(QFont("Segoe UI", 9));
    m_dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_dataTable->horizontalHeader()->setStretchLastSection(true);
    m_dataTable->setAlternatingRowColors(true);
    m_mainLayout->addWidget(m_dataTable);

    // Status label
    m_statusLabel = new QLabel("No table selected", this);
    m_statusLabel->setFont(QFont("Segoe UI", 9));
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("color: gray;");
    m_mainLayout->addWidget(m_statusLabel);
}

void DataOperationWidget::loadTableList()
{
    m_tableComboBox->clear();
    
    if (m_databasePath.empty()) {
        return;
    }
    
    std::vector<std::string> tableNames;
    if (m_tableManager->getAllTableNames(tableNames)) {
        for (const auto& name : tableNames) {
            m_tableComboBox->addItem(QString::fromLocal8Bit(name.c_str()));
        }
    }
}

void DataOperationWidget::onTableSelectionChanged(const QString& tableName)
{
    if (tableName.isEmpty()) {
        clearDataTable();
        return;
    }
    
    m_currentTableName = tableName.toLocal8Bit().constData();
    loadRecords(m_currentTableName);
}

void DataOperationWidget::loadRecords(const std::string& tableName)
{
    // Load table structure
    if (!m_tableManager->readTable(tableName, m_currentTableInfo)) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to load table structure: %1").arg(QString::fromLocal8Bit(tableName.c_str())));
        clearDataTable();
        return;
    }
    
    // Load records
    m_currentRecords.clear();
    if (!m_dataManager->readValidRecords(tableName, m_currentRecords)) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to load records from table: %1").arg(QString::fromLocal8Bit(tableName.c_str())));
        clearDataTable();
        return;
    }
    
    // Setup table widget
    int fieldCount = static_cast<int>(m_currentTableInfo.fields.size());
    m_dataTable->setColumnCount(fieldCount);
    
    // Set headers
    QStringList headers;
    for (const auto& field : m_currentTableInfo.fields) {
        QString header = QString::fromLocal8Bit(field.sFieldName);
        if (field.bKey == FLAG_KEY) {
            header += " (KEY)";
        }
        headers << header;
    }
    m_dataTable->setHorizontalHeaderLabels(headers);
    
    // Populate data
    m_dataTable->setRowCount(static_cast<int>(m_currentRecords.size()));
    for (size_t i = 0; i < m_currentRecords.size(); ++i) {
        const Record& record = m_currentRecords[i];
        for (size_t j = 0; j < record.getFieldCount() && j < m_currentTableInfo.fields.size(); ++j) {
            QString value = QString::fromStdString(record.getValue(j));
            QTableWidgetItem *item = new QTableWidgetItem(value);
            item->setFont(QFont("Segoe UI", 9));
            m_dataTable->setItem(static_cast<int>(i), static_cast<int>(j), item);
        }
    }
    
    // Update status
    m_statusLabel->setText(QString("Table: %1 | Records: %2")
        .arg(QString::fromLocal8Bit(tableName.c_str()))
        .arg(m_currentRecords.size()));
    m_statusLabel->setStyleSheet("color: black;");
}

void DataOperationWidget::clearDataTable()
{
    m_dataTable->clear();
    m_dataTable->setRowCount(0);
    m_dataTable->setColumnCount(0);
    m_statusLabel->setText("No table selected");
    m_statusLabel->setStyleSheet("color: gray;");
    m_currentTableName = "";
    m_currentRecords.clear();
}

int DataOperationWidget::getSelectedRow() const
{
    QList<QTableWidgetItem*> selected = m_dataTable->selectedItems();
    if (selected.isEmpty()) {
        return -1;
    }
    return selected.first()->row();
}

void DataOperationWidget::onInsertRecord()
{
    if (m_databasePath.empty()) {
        QMessageBox::information(this, "No Database", 
            "Please create or open a database first.\n\nUse File -> Create Database or File -> Open Database.");
        return;
    }
    
    if (m_currentTableName.empty()) {
        QMessageBox::information(this, "No Table Selected", 
            "Please select a table first.");
        return;
    }
    
    RecordEditDialog dialog(this, &m_currentTableInfo, nullptr);
    if (dialog.exec() == QDialog::Accepted) {
        Record record;
        if (dialog.getRecord(record)) {
            // Build INSERT SQL statement to use InsertHandler (which handles constraints and defaults)
            std::ostringstream sql;
            sql << "INSERT INTO " << m_currentTableName << " VALUES (";
            
            for (size_t i = 0; i < record.values.size(); ++i) {
                if (i > 0) sql << ", ";
                // Escape string values
                std::string value = record.values[i];
                if (value.empty()) {
                    sql << "''";  // Empty string
                } else {
                    // Check if it's a string type (needs quotes)
                    bool isStringType = false;
                    if (i < m_currentTableInfo.fields.size()) {
                        std::string fieldType = std::string(m_currentTableInfo.fields[i].sType);
                        if (fieldType == "string" || fieldType == "char") {
                            isStringType = true;
                        }
                    }
                    
                    if (isStringType) {
                        // Escape single quotes
                        std::string escaped = value;
                        size_t pos = 0;
                        while ((pos = escaped.find("'", pos)) != std::string::npos) {
                            escaped.replace(pos, 1, "''");
                            pos += 2;
                        }
                        sql << "'" << escaped << "'";
                    } else {
                        sql << value;
                    }
                }
            }
            
            // Extract database filename from path (without path and extension)
            // m_databasePath is like "E:\Projects\VSCode\dbms\student_db"
            // SQL needs just "student_db"
            std::string dbFileName = m_databasePath;
            // Extract basename (filename without path)
            size_t lastSlash = dbFileName.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                dbFileName = dbFileName.substr(lastSlash + 1);
            }
            // Remove extension if present (shouldn't be, but just in case)
            size_t lastDot = dbFileName.find_last_of(".");
            if (lastDot != std::string::npos) {
                dbFileName = dbFileName.substr(0, lastDot);
            }
            
            sql << ") IN " << dbFileName << ";";
            
            // Use InsertHandler to execute INSERT (handles constraints and defaults)
            InsertHandler insertHandler;
            // Set the base path so InsertHandler can resolve the full path from filename
            // We'll modify InsertHandler to accept a base path, or use a workaround:
            // Store the full path and modify InsertHandler to use it.
            // Actually, let's modify InsertHandler::execute to accept an optional base path.
            // But that requires interface changes. For now, let's use a simpler approach:
            // Modify InsertHandler to resolve the full path from the filename using the current working directory
            // or store the base path in InsertHandler.
            
            // Workaround: Pass the full path through a modified execute method
            // But InsertHandler::execute only takes SQL string. Let's check if we can
            // modify it to also accept a base path, or use a different approach.
            
            // Actually, the real issue is that InsertHandler uses insertNode->databaseFileName
            // which is just the filename. We need to convert it to full path.
            // Let's modify InsertHandler::execute to resolve the full path.
            
            // For now, let's modify InsertHandler to accept a base path member or parameter.
            // But that's a bigger change. Let's use a simpler workaround:
            // We'll modify InsertHandler to store and use the base path.
            
            // Actually, looking at the code, InsertHandler sets the path from databaseFileName.
            // The issue is that databaseFileName is just the filename, not the full path.
            // We need to modify InsertHandler to resolve the full path.
            // Let's add a method to set the base path, or modify execute to accept it.
            
            // Simplest solution: Modify InsertHandler to accept a base path in constructor or setter.
            // But that requires interface changes. Let's use a workaround for now:
            // We'll modify InsertHandler::execute to resolve the path from the filename.
            
            // Actually, let's check the actual error. The error says "Expected identifier, but got: constraint"
            // This means the parser is expecting an identifier but got the CONSTRAINT keyword.
            // This could happen if:
            // 1. A value in VALUES is "constraint" and not quoted (but we quote string values)
            // 2. The database filename is "constraint" (unlikely but possible)
            // 3. There's a parsing error in the VALUES clause
            
            // Let's first fix the parser to handle keywords as identifiers in certain contexts,
            // and then fix the path resolution issue.
            
            if (insertHandler.execute(sql.str(), m_databasePath)) {
                QMessageBox::information(this, "Success", "Record inserted successfully.");
                refreshDataTable();
            } else {
                QMessageBox::warning(this, "Error", 
                    QString("Failed to insert record:\n%1").arg(QString::fromStdString(insertHandler.getLastError())));
            }
        }
    }
}

void DataOperationWidget::onEditRecord()
{
    if (m_databasePath.empty()) {
        QMessageBox::information(this, "No Database", 
            "Please create or open a database first.\n\nUse File -> Create Database or File -> Open Database.");
        return;
    }
    
    if (m_currentTableName.empty()) {
        QMessageBox::information(this, "No Table Selected", 
            "Please select a table first.");
        return;
    }
    
    int row = getSelectedRow();
    if (row < 0 || row >= static_cast<int>(m_currentRecords.size())) {
        QMessageBox::information(this, "No Selection", 
            "Please select a record to edit.");
        return;
    }
    
    RecordEditDialog dialog(this, &m_currentTableInfo, &m_currentRecords[row]);
    if (dialog.exec() == QDialog::Accepted) {
        Record newRecord;
        if (dialog.getRecord(newRecord)) {
            // Get the original record
            Record originalRecord = m_currentRecords[row];
            
            // Find primary key field for WHERE clause
            std::string pkFieldName;
            std::string pkValue;
            for (size_t i = 0; i < m_currentTableInfo.fields.size(); ++i) {
                if (m_currentTableInfo.fields[i].bKey == FLAG_KEY) {
                    pkFieldName = std::string(m_currentTableInfo.fields[i].sFieldName);
                    pkValue = originalRecord.getValue(i);
                    break;
                }
            }
            
            if (pkFieldName.empty()) {
                QMessageBox::warning(this, "Error", "Cannot update record: table has no primary key.");
                return;
            }
            
            // Extract database filename from path (without path and extension)
            std::string dbFileName = m_databasePath;
            size_t lastSlash = dbFileName.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                dbFileName = dbFileName.substr(lastSlash + 1);
            }
            size_t lastDot = dbFileName.find_last_of(".");
            if (lastDot != std::string::npos) {
                dbFileName = dbFileName.substr(0, lastDot);
            }
            
            // Build UPDATE SQL statements for each modified field
            // Since UPDATE syntax only supports one field at a time, we need to update each field separately
            UpdateHandler updateHandler;
            bool allSucceeded = true;
            std::string lastError;
            
            for (size_t i = 0; i < newRecord.values.size() && i < m_currentTableInfo.fields.size(); ++i) {
                std::string fieldName = std::string(m_currentTableInfo.fields[i].sFieldName);
                std::string newValue = newRecord.values[i];
                std::string oldValue = originalRecord.getValue(i);
                
                // Skip if value hasn't changed
                if (newValue == oldValue) {
                    continue;
                }
                
                // Build UPDATE SQL statement
                std::ostringstream sql;
                sql << "UPDATE " << m_currentTableName << " (SET " << fieldName << "=";
                
                // Escape string values
                std::string fieldType = std::string(m_currentTableInfo.fields[i].sType);
                if (fieldType == "string" || fieldType == "char") {
                    // Escape single quotes
                    std::string escaped = newValue;
                    size_t pos = 0;
                    while ((pos = escaped.find("'", pos)) != std::string::npos) {
                        escaped.replace(pos, 1, "''");
                        pos += 2;
                    }
                    sql << "'" << escaped << "'";
                } else {
                    sql << newValue;
                }
                
                sql << " WHERE " << pkFieldName << "=";
                
                // Escape primary key value if it's a string
                if (m_currentTableInfo.fields.size() > 0) {
                    // Find the primary key field type
                    std::string pkFieldType;
                    for (size_t j = 0; j < m_currentTableInfo.fields.size(); ++j) {
                        if (m_currentTableInfo.fields[j].bKey == FLAG_KEY) {
                            pkFieldType = std::string(m_currentTableInfo.fields[j].sType);
                            break;
                        }
                    }
                    
                    if (pkFieldType == "string" || pkFieldType == "char") {
                        // Escape single quotes
                        std::string escapedPk = pkValue;
                        size_t pos = 0;
                        while ((pos = escapedPk.find("'", pos)) != std::string::npos) {
                            escapedPk.replace(pos, 1, "''");
                            pos += 2;
                        }
                        sql << "'" << escapedPk << "'";
                    } else {
                        sql << pkValue;
                    }
                } else {
                    sql << pkValue;
                }
                
                sql << ") IN " << dbFileName << ";";
                
                // Execute UPDATE using UpdateHandler (which handles all constraints)
                if (!updateHandler.execute(sql.str(), m_databasePath)) {
                    allSucceeded = false;
                    lastError = updateHandler.getLastError();
                    break;  // Stop on first error
                }
            }
            
            if (allSucceeded) {
                QMessageBox::information(this, "Success", "Record updated successfully.");
                refreshDataTable();
            } else {
                QMessageBox::warning(this, "Error", 
                    QString("Failed to update record:\n%1").arg(QString::fromStdString(lastError)));
            }
        }
    }
}

void DataOperationWidget::onDeleteRecord()
{
    if (m_currentTableName.empty()) {
        QMessageBox::information(this, "No Table Selected", 
            "Please select a table first.");
        return;
    }
    
    int row = getSelectedRow();
    if (row < 0 || row >= static_cast<int>(m_currentRecords.size())) {
        QMessageBox::information(this, "No Selection", 
            "Please select a record to delete.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Delete", 
        "Are you sure you want to delete this record?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // Find the actual record index (considering all records, not just valid ones)
        std::vector<Record> allRecords;
        if (!m_dataManager->readAllRecords(m_currentTableName, allRecords)) {
            QMessageBox::warning(this, "Error", "Failed to load records for deletion.");
            return;
        }
        
        // Find the valid record index in all records
        size_t validIndex = 0;
        size_t targetIndex = 0;
        for (size_t i = 0; i < allRecords.size(); ++i) {
            if (allRecords[i].isValid()) {
                if (validIndex == static_cast<size_t>(row)) {
                    targetIndex = i;
                    break;
                }
                validIndex++;
            }
        }
        
        if (m_dataManager->deleteRecord(m_currentTableName, targetIndex)) {
            QMessageBox::information(this, "Success", "Record deleted successfully.");
            refreshDataTable();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete record.");
        }
    }
}

void DataOperationWidget::onRefresh()
{
    if (!m_currentTableName.empty()) {
        loadRecords(m_currentTableName);
    } else {
        loadTableList();
    }
}

void DataOperationWidget::refreshDataTable()
{
    if (!m_currentTableName.empty()) {
        loadRecords(m_currentTableName);
    }
}

bool DataOperationWidget::checkPrimaryKeyUnique(const std::string& tableName, const TableInfo& tableInfo, 
                                                 const Record& record, int excludeIndex)
{
    m_lastError = "";
    
    // Find all primary key fields
    std::vector<int> keyFieldIndices;
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (tableInfo.fields[i].bKey == FLAG_KEY) {
            keyFieldIndices.push_back(static_cast<int>(i));
        }
    }
    
    // If no primary key fields, no need to check
    if (keyFieldIndices.empty()) {
        return true;
    }
    
    // Read all records (including invalid ones to get correct indices)
    std::vector<Record> allRecords;
    if (!m_dataManager->readAllRecords(tableName, allRecords)) {
        // If read fails, assume no conflict (might be a new table)
        return true;
    }
    
    // Check if new record's primary key values conflict with existing valid records
    for (size_t i = 0; i < allRecords.size(); ++i) {
        // Skip the record being updated (if excludeIndex is valid)
        if (excludeIndex >= 0 && static_cast<size_t>(excludeIndex) == i) {
            continue;
        }
        
        // Only check valid records
        if (!allRecords[i].isValid()) {
            continue;
        }
        
        const Record& existingRecord = allRecords[i];
        bool allKeysMatch = true;
        for (int keyIndex : keyFieldIndices) {
            if (keyIndex >= 0 && keyIndex < static_cast<int>(record.values.size()) &&
                keyIndex >= 0 && keyIndex < static_cast<int>(existingRecord.values.size())) {
                if (record.values[keyIndex] != existingRecord.values[keyIndex]) {
                    allKeysMatch = false;
                    break;
                }
            } else {
                allKeysMatch = false;
                break;
            }
        }
        
        if (allKeysMatch) {
            // Build primary key field names list for error message
            std::string keyFields;
            for (size_t j = 0; j < keyFieldIndices.size(); ++j) {
                if (j > 0) keyFields += ", ";
                keyFields += std::string(tableInfo.fields[keyFieldIndices[j]].sFieldName);
            }
            m_lastError = "Primary key constraint violation: Field (" + keyFields + ") value already exists";
            return false;
        }
    }
    
    return true;
}

