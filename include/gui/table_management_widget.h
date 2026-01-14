#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QCloseEvent>
#include <QString>
#include <string>
#include <vector>

// Forward declarations
class TableManager;
struct TableInfo;

/**
 * @file table_management_widget.h
 * @brief Table Management Widget Class
 *
 * Widget for managing database table structures
 */

/**
 * @brief Dialog for creating/editing table
 */
class TableEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit TableEditDialog(QWidget *parent = nullptr, const TableInfo* existingTable = nullptr);
    ~TableEditDialog();

    /**
     * @brief Get the table information from dialog
     * @param tableInfo Output parameter
     * @return true if valid, false otherwise
     */
    bool getTableInfo(TableInfo& tableInfo);

    // Make database name edit and label accessible for external setting
    QLineEdit *m_databaseNameEdit;
    QLabel *m_databaseNameLabel;

private slots:
    void addField();
    void removeField();
    void moveFieldUp();
    void moveFieldDown();

protected:
    /**
     * @brief Handle close event (X button)
     * @param event Close event
     */
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUI();
    void loadTableInfo(const TableInfo& tableInfo);
    void updateFieldTable();

    QLineEdit *m_tableNameEdit;
    QTableWidget *m_fieldTable;
    QPushButton *m_addFieldBtn;
    QPushButton *m_removeFieldBtn;
    QPushButton *m_moveUpBtn;
    QPushButton *m_moveDownBtn;
    QDialogButtonBox *m_buttonBox;
};

/**
 * @brief Table Management Widget Class
 */
class TableManagementWidget : public QWidget {
    Q_OBJECT

public:
    explicit TableManagementWidget(QWidget *parent = nullptr);
    ~TableManagementWidget();

    /**
     * @brief Set database path
     * @param dbPath Database file path (without extension)
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief Refresh table list
     */
    void refreshTableList();

private slots:
    void onCreateTable();
    void onEditTable();
    void onDeleteTable();
    void onTableSelectionChanged();
    void onRefresh();

private:
    void setupUI();
    void loadTableList();
    void displayTableInfo(const TableInfo& tableInfo);
    void clearTableInfo();

    // UI Components
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_leftLayout;
    QVBoxLayout *m_rightLayout;
    
    // Left panel: Table list
    QGroupBox *m_tableListGroup;
    QListWidget *m_tableList;
    QPushButton *m_createBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_refreshBtn;
    
    // Right panel: Table info
    QGroupBox *m_tableInfoGroup;
    QLabel *m_tableNameLabel;
    QLabel *m_fieldCountLabel;
    QTableWidget *m_fieldInfoTable;
    QLabel *m_noTableSelectedLabel;
    
    // Backend
    TableManager *m_tableManager;
    std::string m_databasePath;
};

