#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QCloseEvent>
#include <QString>
#include <string>
#include <vector>

// Forward declarations
class TableManager;
class DataManager;

// Include table mode definitions (needed for TableInfo and Record)
#include "core/table_mode.h"

/**
 * @file data_operation_widget.h
 * @brief Data Operation Widget Class
 *
 * Widget for managing database records (insert, update, delete, view)
 */

/**
 * @brief Dialog for inserting/editing record
 */
class RecordEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit RecordEditDialog(QWidget *parent = nullptr, 
                             const TableInfo* tableInfo = nullptr,
                             const Record* existingRecord = nullptr);
    ~RecordEditDialog();

    /**
     * @brief Get the record data from dialog
     * @param record Output parameter
     * @return true if valid, false otherwise
     */
    bool getRecord(Record& record);

protected:
    /**
     * @brief Handle close event (X button)
     * @param event Close event
     */
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUI();
    void loadRecord(const Record& record);

    QVBoxLayout *m_mainLayout;
    std::vector<QLineEdit*> m_fieldEdits;
    std::vector<QLabel*> m_fieldLabels;
    const TableInfo* m_tableInfo;
    QDialogButtonBox *m_buttonBox;
};

/**
 * @brief Data Operation Widget Class
 */
class DataOperationWidget : public QWidget {
    Q_OBJECT

public:
    explicit DataOperationWidget(QWidget *parent = nullptr);
    ~DataOperationWidget();

    /**
     * @brief Set database path
     * @param dbPath Database file path (without extension)
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief Refresh data table
     */
    void refreshDataTable();

    /**
     * @brief Load table list (public for external refresh)
     */
    void loadTableList();

private slots:
    void onTableSelectionChanged(const QString& tableName);
    void onInsertRecord();
    void onEditRecord();
    void onDeleteRecord();
    void onRefresh();

private:
    void setupUI();
    void loadRecords(const std::string& tableName);
    void clearDataTable();
    int getSelectedRow() const;
    
    /**
     * @brief Check primary key uniqueness constraint
     * @param tableName Table name
     * @param tableInfo Table structure information
     * @param record Record to check
     * @param excludeIndex Record index to exclude from check (-1 for insert, >=0 for update)
     * @return true if unique, false otherwise
     */
    bool checkPrimaryKeyUnique(const std::string& tableName, const TableInfo& tableInfo, 
                               const Record& record, int excludeIndex);

    // UI Components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_topLayout;
    QComboBox *m_tableComboBox;
    QPushButton *m_insertBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_refreshBtn;
    QTableWidget *m_dataTable;
    QLabel *m_statusLabel;
    
    // Backend
    TableManager *m_tableManager;
    DataManager *m_dataManager;
    std::string m_databasePath;
    std::string m_currentTableName;
    TableInfo m_currentTableInfo;
    std::vector<Record> m_currentRecords;
    std::string m_lastError;  // Last error message
};

