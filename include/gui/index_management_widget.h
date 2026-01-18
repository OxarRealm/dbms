#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QTextEdit>
#include <QString>
#include <string>
#include <vector>

// Forward declarations
class IndexManager;
class IndexAdvisor;
struct IndexInfo;
struct IndexRecommendation;

/**
 * @file index_management_widget.h
 * @brief Index Management Widget Class
 *
 * Widget for managing database indexes
 */

/**
 * @brief Dialog for creating index
 */
class CreateIndexDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateIndexDialog(QWidget *parent = nullptr);
    ~CreateIndexDialog();

    /**
     * @brief Get the index information from dialog
     * @param tableName Output parameter
     * @param fieldName Output parameter
     * @param indexType Output parameter
     * @return true if valid, false otherwise
     */
    bool getIndexInfo(std::string& tableName, std::string& fieldName, std::string& indexType);

    /**
     * @brief Set available tables
     * @param tables Table names
     */
    void setTables(const std::vector<std::string>& tables);

    /**
     * @brief Set available fields for a table
     * @param fields Field names
     */
    void setFields(const std::vector<std::string>& fields);

    /**
     * @brief Get current selected table name
     * @return Table name, empty if no table selected
     */
    QString getCurrentTable() const;

    /**
     * @brief Check if there are any tables available
     * @return true if tables are available, false otherwise
     */
    bool hasTables() const;

signals:
    void tableChanged(const QString& tableName);

private slots:
    void onTableChanged(const QString& tableName);

private:
    void setupUI();

    QComboBox *m_tableCombo;
    QComboBox *m_fieldCombo;
    QComboBox *m_indexTypeCombo;
    QDialogButtonBox *m_buttonBox;
};

/**
 * @brief Index Management Widget Class
 */
class IndexManagementWidget : public QWidget {
    Q_OBJECT

public:
    explicit IndexManagementWidget(QWidget *parent = nullptr);
    ~IndexManagementWidget();

    /**
     * @brief Set database path
     * @param dbPath Database file path (without extension)
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief Set IndexAdvisor reference (for sharing query logs from SQLQueryWidget)
     * @param advisor Pointer to IndexAdvisor instance (can be null)
     */
    void setIndexAdvisor(class IndexAdvisor* advisor);

    /**
     * @brief Refresh index list
     */
    void refreshIndexList();

    /**
     * @brief Refresh recommendations
     */
    void refreshRecommendations();

private slots:
    void onCreateIndex();
    void onDeleteIndex();
    void onRefresh();
    void onViewRecommendations();
    void onCreateRecommendedIndex();
    void onIndexSelectionChanged();
    void onTableSelectionChanged(const QString& tableName);

private:
    void setupUI();
    void updateIndexTable();
    void updateRecommendationsTable();
    std::vector<std::string> getAvailableTables() const;
    std::vector<std::string> getTableFields(const std::string& tableName) const;
    std::string getDatabaseName() const;
    IndexManager* getIndexManager();

    std::string m_databasePath;
    IndexManager* m_indexManager;  // 维护一个IndexManager实例
    class IndexAdvisor* m_indexAdvisor;  // 索引建议器引用（从SQLQueryWidget共享）
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QComboBox *m_tableCombo;
    QTableWidget *m_indexTable;
    QPushButton *m_createIndexBtn;
    QPushButton *m_deleteIndexBtn;
    QPushButton *m_refreshBtn;
    
    // Recommendations section
    QGroupBox *m_recommendationsGroup;
    QTableWidget *m_recommendationsTable;
    QPushButton *m_viewRecommendationsBtn;
    QPushButton *m_createRecommendedBtn;
    
    // Index details section
    QGroupBox *m_detailsGroup;
    QTextEdit *m_detailsText;
};

