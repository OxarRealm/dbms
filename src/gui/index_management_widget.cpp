/**
 * @file index_management_widget.cpp
 * @brief Index Management Widget Implementation
 */

#include "gui/index_management_widget.h"
#include "core/index_manager.h"
#include "core/index_storage.h"
#include "core/table_manager.h"
#include "index/index_advisor.h"
#include "core/table_mode.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFont>
#include <QFileInfo>
#include <algorithm>
#include <map>
#include <set>
#include <cctype>
#include <locale>

// ==================== CreateIndexDialog Implementation ====================

CreateIndexDialog::CreateIndexDialog(QWidget *parent)
    : QDialog(parent)
    , m_tableCombo(nullptr)
    , m_fieldCombo(nullptr)
    , m_indexTypeCombo(nullptr)
    , m_buttonBox(nullptr)
{
    setupUI();
    setWindowTitle("Create Index");
    resize(400, 200);
}

CreateIndexDialog::~CreateIndexDialog()
{
}

void CreateIndexDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Table selection
    QHBoxLayout *tableLayout = new QHBoxLayout();
    QLabel *tableLabel = new QLabel("Table:", this);
    tableLabel->setFont(QFont("Segoe UI", 9));
    m_tableCombo = new QComboBox(this);
    m_tableCombo->setFont(QFont("Segoe UI", 9));
    tableLayout->addWidget(tableLabel);
    tableLayout->addWidget(m_tableCombo);
    mainLayout->addLayout(tableLayout);

    // Field selection
    QHBoxLayout *fieldLayout = new QHBoxLayout();
    QLabel *fieldLabel = new QLabel("Field:", this);
    fieldLabel->setFont(QFont("Segoe UI", 9));
    m_fieldCombo = new QComboBox(this);
    m_fieldCombo->setFont(QFont("Segoe UI", 9));
    fieldLayout->addWidget(fieldLabel);
    fieldLayout->addWidget(m_fieldCombo);
    mainLayout->addLayout(fieldLayout);

    // Index type selection
    QHBoxLayout *typeLayout = new QHBoxLayout();
    QLabel *typeLabel = new QLabel("Index Type:", this);
    typeLabel->setFont(QFont("Segoe UI", 9));
    m_indexTypeCombo = new QComboBox(this);
    m_indexTypeCombo->setFont(QFont("Segoe UI", 9));
    m_indexTypeCombo->addItem("Hash Index (Point Queries)", "hash");
    m_indexTypeCombo->addItem("Adjacent Index (Range Queries)", "adjacent");
    m_indexTypeCombo->addItem("B+ Tree Index (General Purpose)", "btree");
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(m_indexTypeCombo);
    mainLayout->addLayout(typeLayout);

    // Button box
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttonBox->button(QDialogButtonBox::Ok)->setText("Create");
    m_buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    m_buttonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);

    connect(m_tableCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CreateIndexDialog::tableChanged);
}

void CreateIndexDialog::setTables(const std::vector<std::string>& tables)
{
    m_tableCombo->clear();
    for (const auto& table : tables) {
        m_tableCombo->addItem(QString::fromStdString(table));
    }
    // If there are tables, select the first one and emit signal
    if (m_tableCombo->count() > 0) {
        m_tableCombo->setCurrentIndex(0);
        emit tableChanged(m_tableCombo->currentText());
    }
}


void CreateIndexDialog::setFields(const std::vector<std::string>& fields)
{
    m_fieldCombo->clear();
    for (const auto& field : fields) {
        m_fieldCombo->addItem(QString::fromStdString(field));
    }
}

void CreateIndexDialog::onTableChanged(const QString& tableName)
{
    // This will be handled by the parent widget
    emit tableChanged(tableName);
}

bool CreateIndexDialog::getIndexInfo(std::string& tableName, std::string& fieldName, std::string& indexType)
{
    if (m_tableCombo->currentIndex() < 0 || m_fieldCombo->currentIndex() < 0) {
        return false;
    }

    tableName = m_tableCombo->currentText().toStdString();
    fieldName = m_fieldCombo->currentText().toStdString();
    indexType = m_indexTypeCombo->currentData().toString().toStdString();

    return !tableName.empty() && !fieldName.empty() && !indexType.empty();
}

QString CreateIndexDialog::getCurrentTable() const
{
    if (m_tableCombo->currentIndex() >= 0) {
        return m_tableCombo->currentText();
    }
    return QString();
}

bool CreateIndexDialog::hasTables() const
{
    return m_tableCombo->count() > 0;
}

// ==================== IndexManagementWidget Implementation ====================

IndexManagementWidget::IndexManagementWidget(QWidget *parent)
    : QWidget(parent)
    , m_databasePath("")
    , m_indexAdvisor(nullptr)
    , m_indexManager(nullptr)
    , m_mainLayout(nullptr)
    , m_tableCombo(nullptr)
    , m_indexTable(nullptr)
    , m_createIndexBtn(nullptr)
    , m_deleteIndexBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_recommendationsGroup(nullptr)
    , m_recommendationsTable(nullptr)
    , m_viewRecommendationsBtn(nullptr)
    , m_createRecommendedBtn(nullptr)
    , m_detailsGroup(nullptr)
    , m_detailsText(nullptr)
{
    m_indexManager = new IndexManager();
    setupUI();
}

IndexManagementWidget::~IndexManagementWidget()
{
    if (m_indexManager) {
        delete m_indexManager;
        m_indexManager = nullptr;
    }
}

void IndexManagementWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    // Title
    QLabel *titleLabel = new QLabel("Index Management", this);
    titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
    m_mainLayout->addWidget(titleLabel);

    // Table selection
    QHBoxLayout *tableLayout = new QHBoxLayout();
    QLabel *tableLabel = new QLabel("Table:", this);
    tableLabel->setFont(QFont("Segoe UI", 9));
    m_tableCombo = new QComboBox(this);
    m_tableCombo->setFont(QFont("Segoe UI", 9));
    m_tableCombo->addItem("All Tables");
    connect(m_tableCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &IndexManagementWidget::onTableSelectionChanged);
    tableLayout->addWidget(tableLabel);
    tableLayout->addWidget(m_tableCombo);
    tableLayout->addStretch();
    m_mainLayout->addLayout(tableLayout);

    // Index list section
    QGroupBox *indexGroup = new QGroupBox("Indexes", this);
    indexGroup->setFont(QFont("Segoe UI", 9));
    QVBoxLayout *indexLayout = new QVBoxLayout(indexGroup);

    // Index table
    m_indexTable = new QTableWidget(this);
    m_indexTable->setFont(QFont("Segoe UI", 9));
    m_indexTable->setColumnCount(5);
    m_indexTable->setHorizontalHeaderLabels(QStringList() << "Index Name" << "Table" << "Field" << "Type" << "Status");
    m_indexTable->horizontalHeader()->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_indexTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_indexTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_indexTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_indexTable->horizontalHeader()->setStretchLastSection(true);
    connect(m_indexTable, &QTableWidget::itemSelectionChanged,
            this, &IndexManagementWidget::onIndexSelectionChanged);
    indexLayout->addWidget(m_indexTable);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_createIndexBtn = new QPushButton("Create Index", this);
    m_createIndexBtn->setFont(QFont("Segoe UI", 9));
    m_deleteIndexBtn = new QPushButton("Delete Index", this);
    m_deleteIndexBtn->setFont(QFont("Segoe UI", 9));
    m_refreshBtn = new QPushButton("Refresh", this);
    m_refreshBtn->setFont(QFont("Segoe UI", 9));
    m_deleteIndexBtn->setEnabled(false);
    
    connect(m_createIndexBtn, &QPushButton::clicked, this, &IndexManagementWidget::onCreateIndex);
    connect(m_deleteIndexBtn, &QPushButton::clicked, this, &IndexManagementWidget::onDeleteIndex);
    connect(m_refreshBtn, &QPushButton::clicked, this, &IndexManagementWidget::onRefresh);
    
    buttonLayout->addWidget(m_createIndexBtn);
    buttonLayout->addWidget(m_deleteIndexBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_refreshBtn);
    indexLayout->addLayout(buttonLayout);

    m_mainLayout->addWidget(indexGroup);

    // Recommendations section
    m_recommendationsGroup = new QGroupBox("Index Recommendations", this);
    m_recommendationsGroup->setFont(QFont("Segoe UI", 9));
    QVBoxLayout *recLayout = new QVBoxLayout(m_recommendationsGroup);

    m_recommendationsTable = new QTableWidget(this);
    m_recommendationsTable->setFont(QFont("Segoe UI", 9));
    m_recommendationsTable->setColumnCount(5);
    m_recommendationsTable->setHorizontalHeaderLabels(QStringList() << "Table" << "Field" << "Type" << "Expected Improvement" << "Reason");
    m_recommendationsTable->horizontalHeader()->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_recommendationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_recommendationsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_recommendationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recommendationsTable->horizontalHeader()->setStretchLastSection(true);
    recLayout->addWidget(m_recommendationsTable);

    QHBoxLayout *recButtonLayout = new QHBoxLayout();
    m_viewRecommendationsBtn = new QPushButton("View Recommendations", this);
    m_viewRecommendationsBtn->setFont(QFont("Segoe UI", 9));
    m_createRecommendedBtn = new QPushButton("Create Recommended Index", this);
    m_createRecommendedBtn->setFont(QFont("Segoe UI", 9));
    m_createRecommendedBtn->setEnabled(false);
    
    connect(m_viewRecommendationsBtn, &QPushButton::clicked, this, &IndexManagementWidget::onViewRecommendations);
    connect(m_createRecommendedBtn, &QPushButton::clicked, this, &IndexManagementWidget::onCreateRecommendedIndex);
    
    recButtonLayout->addWidget(m_viewRecommendationsBtn);
    recButtonLayout->addStretch();
    recButtonLayout->addWidget(m_createRecommendedBtn);
    recLayout->addLayout(recButtonLayout);

    m_mainLayout->addWidget(m_recommendationsGroup);

    // Index details section
    m_detailsGroup = new QGroupBox("Index Details", this);
    m_detailsGroup->setFont(QFont("Segoe UI", 9));
    QVBoxLayout *detailsLayout = new QVBoxLayout(m_detailsGroup);

    m_detailsText = new QTextEdit(this);
    m_detailsText->setFont(QFont("Segoe UI", 9));
    m_detailsText->setReadOnly(true);
    m_detailsText->setMaximumHeight(150);
    detailsLayout->addWidget(m_detailsText);

    m_mainLayout->addWidget(m_detailsGroup);
}

void IndexManagementWidget::setDatabasePath(const std::string& dbPath)
{
    m_databasePath = dbPath;
    
    if (m_indexManager) {
        m_indexManager->setDatabasePath(m_databasePath);
        
        // Load indices from .idx file
        if (!dbPath.empty()) {
            std::string dbName = getDatabaseName();
            if (!dbName.empty()) {
                IndexStorageManager::loadIndices(dbName, m_databasePath, *m_indexManager);
            }
        }
    }
    
    refreshIndexList();
    refreshRecommendations();
    
    // Update table combo
    m_tableCombo->clear();
    m_tableCombo->addItem("All Tables");
    std::vector<std::string> tables = getAvailableTables();
    for (const auto& table : tables) {
        m_tableCombo->addItem(QString::fromStdString(table));
    }
}

IndexManager* IndexManagementWidget::getIndexManager()
{
    return m_indexManager;
}

void IndexManagementWidget::refreshIndexList()
{
    // Reload indices from .idx file to ensure we have the latest data
    if (m_indexManager && !m_databasePath.empty()) {
        std::string dbName = getDatabaseName();
        if (!dbName.empty()) {
            IndexStorageManager::loadIndices(dbName, m_databasePath, *m_indexManager);
        }
    }
    
    updateIndexTable();
}

void IndexManagementWidget::refreshRecommendations()
{
    updateRecommendationsTable();
}

void IndexManagementWidget::updateIndexTable()
{
    m_indexTable->setRowCount(0);

    if (m_databasePath.empty() || !m_indexManager) {
        return;
    }

    std::vector<IndexInfo> indices;
    QString selectedTable = m_tableCombo->currentText();

    if (selectedTable == "All Tables") {
        m_indexManager->getAllIndices(indices);
    } else {
        m_indexManager->getTableIndices(selectedTable.toStdString(), indices);
    }

    for (const auto& index : indices) {
        int row = m_indexTable->rowCount();
        m_indexTable->insertRow(row);

        m_indexTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(index.indexName)));
        m_indexTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(index.tableName)));
        m_indexTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(index.fieldName)));
        
        QString typeStr;
        if (index.indexType == "hash") {
            typeStr = "Hash Index";
        } else if (index.indexType == "adjacent") {
            typeStr = "Adjacent Index";
        } else if (index.indexType == "btree") {
            typeStr = "B+ Tree Index";
        } else {
            typeStr = QString::fromStdString(index.indexType);
        }
        m_indexTable->setItem(row, 3, new QTableWidgetItem(typeStr));
        m_indexTable->setItem(row, 4, new QTableWidgetItem(index.isActive ? "Active" : "Inactive"));
    }

    m_indexTable->resizeColumnsToContents();
}

void IndexManagementWidget::updateRecommendationsTable()
{
    m_recommendationsTable->setRowCount(0);

    if (m_databasePath.empty()) {
        return;
    }

    // Use shared IndexAdvisor from SQLQueryWidget if available, otherwise create a new one
    IndexAdvisor* advisorToUse = m_indexAdvisor;
    bool needToDelete = false;
    
    if (!advisorToUse) {
        // Create a temporary advisor if no shared one is available
        advisorToUse = new IndexAdvisor();
        advisorToUse->setDatabasePath(m_databasePath);
        // Set IndexManager to the temporary advisor
        if (m_indexManager) {
            advisorToUse->setIndexManager(m_indexManager);
        }
        needToDelete = true;
    }

    // Ensure IndexManager has loaded indices from .idx file (same as updateIndexTable does)
    // This is critical - we must load indices before checking, just like refreshIndexList does
    std::string dbName = getDatabaseName();
    if (!dbName.empty() && m_indexManager) {
        IndexStorageManager::loadIndices(dbName, m_databasePath, *m_indexManager);
    }

    // Debug: Check log count
    size_t logCount = advisorToUse->getLogCount();
    
    // Debug: Get field stats to see why no recommendations
    std::vector<FieldUsageStats> stats;
    advisorToUse->analyzeQueryLogs(stats);

    // Override hasIndex check using IndexManager (which has loaded indices from .idx file)
    // IndexAdvisor's internal Index objects may not have loaded indices from .idx file
    // So we update stats with correct hasIndex values from IndexManager
    // Use the SAME method as updateIndexTable: getAllIndices from m_indexManager
    if (m_indexManager) {
        // Get all indices using the SAME method as updateIndexTable
        // This ensures we're checking against the same data source
        std::vector<IndexInfo> allIndices;
        m_indexManager->getAllIndices(allIndices);
        
        // Debug: Print loaded indices for troubleshooting
        // Create a lookup map for faster checking
        // Use case-insensitive comparison to handle potential case mismatches
        std::map<std::string, std::set<std::string>> indexMap;  // tableName -> set of fieldNames
        for (const auto& idx : allIndices) {
            indexMap[idx.tableName].insert(idx.fieldName);
        }
        
        // Now override hasIndex for each stat using the loaded indices
        for (auto& s : stats) {
            // First try exact match (case-sensitive)
            auto tableIt = indexMap.find(s.tableName);
            if (tableIt != indexMap.end()) {
                auto fieldIt = tableIt->second.find(s.fieldName);
                if (fieldIt != tableIt->second.end()) {
                    s.hasIndex = true;
                    continue;  // Found exact match, skip fallback
                }
            }
            
            // If no exact match, try case-insensitive search
            bool found = false;
            for (const auto& tablePair : indexMap) {
                // Case-insensitive table name comparison
                std::string tableLower = tablePair.first;
                std::transform(tableLower.begin(), tableLower.end(), tableLower.begin(), ::tolower);
                std::string statTableLower = s.tableName;
                std::transform(statTableLower.begin(), statTableLower.end(), statTableLower.begin(), ::tolower);
                
                if (tableLower == statTableLower) {
                    // Found matching table, check fields
                    for (const auto& fieldName : tablePair.second) {
                        std::string fieldLower = fieldName;
                        std::transform(fieldLower.begin(), fieldLower.end(), fieldLower.begin(), ::tolower);
                        std::string statFieldLower = s.fieldName;
                        std::transform(statFieldLower.begin(), statFieldLower.end(), statFieldLower.begin(), ::tolower);
                        
                        if (fieldLower == statFieldLower) {
                            s.hasIndex = true;
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                }
            }
            
            // If still not found, use IndexManager::hasIndex as final fallback
            if (!found) {
                s.hasIndex = m_indexManager->hasIndex(s.tableName, s.fieldName, "hash") ||
                             m_indexManager->hasIndex(s.tableName, s.fieldName, "btree") ||
                             m_indexManager->hasIndex(s.tableName, s.fieldName, "adjacent");
            }
        }
    }

    // Generate recommendations manually using corrected stats
    // generateRecommendations calls analyzeQueryLogs which uses IndexAdvisor's internal
    // Index objects that haven't loaded from .idx file, so hasIndex may be wrong.
    // Instead, we'll generate recommendations directly from the corrected stats.
    std::vector<IndexRecommendation> recommendations;
    
    for (const auto& fieldStats : stats) {
        // Skip if already has index (using corrected hasIndex from IndexManager)
        if (fieldStats.hasIndex) {
            continue;
        }
        
        // Skip if usage count is too low
        if (fieldStats.usageCount < 3) {
            continue;
        }
        
        // Skip if average execution time is too low
        if (fieldStats.avgExecutionTime < 0.1) {
            continue;
        }
        
        IndexRecommendation recommendation;
        recommendation.tableName = fieldStats.tableName;
        recommendation.fieldName = fieldStats.fieldName;
        recommendation.usageCount = fieldStats.usageCount;
        recommendation.avgExecutionTime = fieldStats.avgExecutionTime;
        
        // Determine index type by checking table structure
        // Check if field is primary key or integer type for hash index
        TableManager tableManager;
        tableManager.setDatabasePath(m_databasePath);
        TableInfo tableInfo;
        bool isPrimaryKey = false;
        bool isIntegerType = false;
        std::string fieldType = "";
        
        if (tableManager.readTable(fieldStats.tableName, tableInfo)) {
            for (const auto& field : tableInfo.fields) {
                if (strcmp(field.sFieldName, fieldStats.fieldName.c_str()) == 0) {
                    isPrimaryKey = (field.bKey == FLAG_KEY);
                    isIntegerType = (strcmp(field.sType, "int") == 0);
                    break;
                }
            }
        }
        
        // Determine index type based on field characteristics
        if (isPrimaryKey && isIntegerType) {
            recommendation.indexType = "hash";
            recommendation.expectedImprovement = 50.0;
            recommendation.reason = "Field suitable for hash index, optimizes point query performance (O(1) time complexity)";
        } else {
            recommendation.indexType = "btree";
            recommendation.expectedImprovement = 40.0;
            recommendation.reason = "Field suitable for B+ tree index, optimizes point queries, range queries, and sorting (general-purpose index)";
        }
        
        // Adjust improvement based on usage frequency and execution time
        double score = (std::min(fieldStats.usageCount * 5.0, 50.0) + 
                       std::min(fieldStats.avgExecutionTime / 2.0, 50.0));
        recommendation.expectedImprovement *= (1.0 + score / 100.0);
        
        recommendations.push_back(recommendation);
    }
    
    // Sort by score (usage count + execution time)
    std::sort(recommendations.begin(), recommendations.end(),
              [](const IndexRecommendation& a, const IndexRecommendation& b) {
                  return (a.usageCount * 5.0 + a.avgExecutionTime / 2.0) > 
                         (b.usageCount * 5.0 + b.avgExecutionTime / 2.0);
              });
    
    // Limit to 10 recommendations
    if (recommendations.size() > 10) {
        recommendations.resize(10);
    }
    
    bool generated = !recommendations.empty();
    
    // If no recommendations found, return early (message will be shown in onViewRecommendations)
    if (!generated || recommendations.empty()) {
        if (needToDelete) {
            delete advisorToUse;
        }
        return;
    }

    for (const auto& rec : recommendations) {
        int row = m_recommendationsTable->rowCount();
        m_recommendationsTable->insertRow(row);

        m_recommendationsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(rec.tableName)));
        m_recommendationsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(rec.fieldName)));
        
        QString typeStr;
        if (rec.indexType == "hash") {
            typeStr = "Hash Index";
        } else if (rec.indexType == "adjacent") {
            typeStr = "Adjacent Index";
        } else if (rec.indexType == "btree") {
            typeStr = "B+ Tree Index";
        } else {
            typeStr = QString::fromStdString(rec.indexType);
        }
        m_recommendationsTable->setItem(row, 2, new QTableWidgetItem(typeStr));
        
        QString improvementStr = QString::number(rec.expectedImprovement, 'f', 1) + "%";
        m_recommendationsTable->setItem(row, 3, new QTableWidgetItem(improvementStr));
        
        // Add index type explanation to reason
        QString reasonText = QString::fromStdString(rec.reason);
        if (rec.indexType == "hash") {
            reasonText += "\n(Hash: Best for point queries, O(1) time complexity)";
        } else if (rec.indexType == "btree") {
            reasonText += "\n(B+ Tree: General-purpose, supports point/range queries and sorting)";
        } else if (rec.indexType == "adjacent") {
            reasonText += "\n(Adjacent: Optimized for range queries)";
        }
        m_recommendationsTable->setItem(row, 4, new QTableWidgetItem(reasonText));
    }

    m_recommendationsTable->resizeColumnsToContents();
    
    if (needToDelete) {
        delete advisorToUse;
    }
}

void IndexManagementWidget::setIndexAdvisor(IndexAdvisor* advisor)
{
    m_indexAdvisor = advisor;
    
    // Set IndexManager to IndexAdvisor so it can check for existing indices
    if (m_indexAdvisor && m_indexManager) {
        m_indexAdvisor->setIndexManager(m_indexManager);
    }
}

std::vector<std::string> IndexManagementWidget::getAvailableTables() const
{
    std::vector<std::string> tables;

    if (m_databasePath.empty()) {
        return tables;
    }

    TableManager tableManager;
    tableManager.setDatabasePath(m_databasePath);
    tableManager.getAllTableNames(tables);

    return tables;
}

std::vector<std::string> IndexManagementWidget::getTableFields(const std::string& tableName) const
{
    std::vector<std::string> fields;

    if (m_databasePath.empty() || tableName.empty()) {
        return fields;
    }

    TableManager tableManager;
    tableManager.setDatabasePath(m_databasePath);

    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return fields;
    }

    for (const auto& field : tableInfo.fields) {
        fields.push_back(field.sFieldName);
    }

    return fields;
}

std::string IndexManagementWidget::getDatabaseName() const
{
    if (m_databasePath.empty()) {
        return "";
    }
    
    // 从路径中提取数据库名（不含扩展名）
    QFileInfo fileInfo(QString::fromLocal8Bit(m_databasePath.c_str()));
    QString dbName = fileInfo.baseName();
    if (dbName.isEmpty()) {
        // 如果没有路径分隔符，直接使用路径
        return m_databasePath;
    }
    return dbName.toStdString();
}

void IndexManagementWidget::onCreateIndex()
{
    if (m_databasePath.empty()) {
        QMessageBox::warning(this, "Error", "No database selected. Please open a database first.");
        return;
    }

    CreateIndexDialog dialog(this);
    
    // Connect table change signal BEFORE setTables, so the signal will be connected when setTables emits it
    connect(&dialog, &CreateIndexDialog::tableChanged,
            [&](const QString& tableName) {
                std::vector<std::string> fields = getTableFields(tableName.toStdString());
                dialog.setFields(fields);
            });

    // Now set tables, which will automatically select the first table and emit tableChanged signal
    // The lambda above will handle loading fields for the first table
    dialog.setTables(getAvailableTables());
    
    // Also manually load fields for the first table as a backup (in case signal wasn't emitted)
    QString firstTable = dialog.getCurrentTable();
    if (!firstTable.isEmpty()) {
        std::vector<std::string> fields = getTableFields(firstTable.toStdString());
        dialog.setFields(fields);
    }

    if (dialog.exec() == QDialog::Accepted) {
        std::string tableName, fieldName, indexType;
        if (dialog.getIndexInfo(tableName, fieldName, indexType)) {
            if (!m_indexManager) {
                QMessageBox::warning(this, "Error", "Index manager not initialized.");
                return;
            }

            if (m_indexManager->createIndex(tableName, fieldName, indexType)) {
                // 保存索引到文件
                std::string dbName = getDatabaseName();
                if (!dbName.empty()) {
                    IndexStorageManager::saveIndices(dbName, m_databasePath, *m_indexManager);
                }
                
                QMessageBox::information(this, "Success", 
                    QString("Index created successfully on %1.%2").arg(
                        QString::fromStdString(tableName), 
                        QString::fromStdString(fieldName)));
                refreshIndexList();
            } else {
                QMessageBox::warning(this, "Error", "Failed to create index. Please check the error message.");
            }
        }
    }
}

void IndexManagementWidget::onDeleteIndex()
{
    int currentRow = m_indexTable->currentRow();
    if (currentRow < 0) {
        return;
    }

    QTableWidgetItem *tableItem = m_indexTable->item(currentRow, 1);
    QTableWidgetItem *fieldItem = m_indexTable->item(currentRow, 2);
    QTableWidgetItem *typeItem = m_indexTable->item(currentRow, 3);

    if (!tableItem || !fieldItem || !typeItem) {
        return;
    }

    std::string tableName = tableItem->text().toStdString();
    std::string fieldName = fieldItem->text().toStdString();
    
    QString typeStr = typeItem->text();
    std::string indexType;
    if (typeStr == "Hash Index") {
        indexType = "hash";
    } else if (typeStr == "Adjacent Index") {
        indexType = "adjacent";
    } else if (typeStr == "B+ Tree Index") {
        indexType = "btree";
    } else {
        return;
    }

    int ret = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete the index on %1.%2?").arg(
            QString::fromStdString(tableName), 
            QString::fromStdString(fieldName)),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (!m_indexManager) {
            QMessageBox::warning(this, "Error", "Index manager not initialized.");
            return;
        }

        if (m_indexManager->dropIndex(tableName, fieldName, indexType)) {
            // 保存索引到文件（删除后更新）
            std::string dbName = getDatabaseName();
            if (!dbName.empty()) {
                IndexStorageManager::saveIndices(dbName, m_databasePath, *m_indexManager);
            }
            
            QMessageBox::information(this, "Success", "Index deleted successfully.");
            refreshIndexList();
            m_detailsText->clear();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete index.");
        }
    }
}

void IndexManagementWidget::onRefresh()
{
    refreshIndexList();
    refreshRecommendations();
}

void IndexManagementWidget::onViewRecommendations()
{
    refreshRecommendations();
    
    // Show informative message if no recommendations (only when user clicks the button)
    if (m_recommendationsTable->rowCount() == 0) {
        // Get advisor to check log count
        IndexAdvisor* advisorToUse = m_indexAdvisor;
        bool needToDelete = false;
        
        if (!advisorToUse) {
            advisorToUse = new IndexAdvisor();
            advisorToUse->setDatabasePath(m_databasePath);
            // Set IndexManager to the temporary advisor
            if (m_indexManager) {
                advisorToUse->setIndexManager(m_indexManager);
            }
            needToDelete = true;
        }
        
        size_t logCount = advisorToUse->getLogCount();
        std::vector<FieldUsageStats> stats;
        advisorToUse->analyzeQueryLogs(stats);
        
        QString msg;
        if (logCount == 0) {
            msg = "No query logs found.\n\n"
                  "Please execute some SELECT queries in the SQL Execution tab first.\n\n"
                  "Example:\n"
                  "  SELECT * FROM Students WHERE StudentID = 1;";
        } else {
            msg = QString("No recommendations generated.\n\n"
                          "Query logs: %1\n"
                          "Fields analyzed: %2\n\n").arg(logCount).arg(stats.size());
            if (stats.size() > 0) {
                msg += "Field statistics:\n";
                for (size_t i = 0; i < std::min(stats.size(), size_t(5)); i++) {
                    const auto& s = stats[i];
                    msg += QString("  - %1.%2: count=%3, avgTime=%4ms, hasIndex=%5\n")
                        .arg(QString::fromStdString(s.tableName))
                        .arg(QString::fromStdString(s.fieldName))
                        .arg(s.usageCount)
                        .arg(QString::number(s.avgExecutionTime, 'f', 2))
                        .arg(s.hasIndex ? "Yes" : "No");
                }
                // Debug: Show loaded indices for comparison
                if (m_indexManager) {
                    std::vector<IndexInfo> debugIndices;
                    m_indexManager->getAllIndices(debugIndices);
                    if (!debugIndices.empty()) {
                        msg += QString("\nLoaded indices (%1):\n").arg(debugIndices.size());
                        for (size_t i = 0; i < std::min(debugIndices.size(), size_t(5)); i++) {
                            const auto& idx = debugIndices[i];
                            msg += QString("  - %1.%2 (%3)\n")
                                .arg(QString::fromStdString(idx.tableName))
                                .arg(QString::fromStdString(idx.fieldName))
                                .arg(QString::fromStdString(idx.indexType));
                        }
                    } else {
                        msg += "\n(No indices loaded from .idx file)";
                    }
                }
                
                msg += "\nRecommendation conditions: count>=3, avgTime>=0.1ms, no existing index";
            }
        }
        
        if (needToDelete) {
            delete advisorToUse;
        }
        
        QMessageBox::information(this, "No Recommendations", msg);
    }
    
    m_createRecommendedBtn->setEnabled(m_recommendationsTable->rowCount() > 0);
}

void IndexManagementWidget::onCreateRecommendedIndex()
{
    int currentRow = m_recommendationsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please select a recommendation first.");
        return;
    }

    QTableWidgetItem *tableItem = m_recommendationsTable->item(currentRow, 0);
    QTableWidgetItem *fieldItem = m_recommendationsTable->item(currentRow, 1);
    QTableWidgetItem *typeItem = m_recommendationsTable->item(currentRow, 2);

    if (!tableItem || !fieldItem || !typeItem) {
        return;
    }

    std::string tableName = tableItem->text().toStdString();
    std::string fieldName = fieldItem->text().toStdString();
    
    QString typeStr = typeItem->text();
    std::string indexType;
    if (typeStr == "Hash Index") {
        indexType = "hash";
    } else if (typeStr == "Adjacent Index") {
        indexType = "adjacent";
    } else if (typeStr == "B+ Tree Index") {
        indexType = "btree";
    } else {
        return;
    }

    if (!m_indexManager) {
        QMessageBox::warning(this, "Error", "Index manager not initialized.");
        return;
    }

    if (m_indexManager->createIndex(tableName, fieldName, indexType)) {
        // 保存索引到文件
        std::string dbName = getDatabaseName();
        if (!dbName.empty()) {
            IndexStorageManager::saveIndices(dbName, m_databasePath, *m_indexManager);
        }
        
        QMessageBox::information(this, "Success", 
            QString("Recommended index created successfully on %1.%2").arg(
                QString::fromStdString(tableName), 
                QString::fromStdString(fieldName)));
        refreshIndexList();
        refreshRecommendations();
    } else {
        QMessageBox::warning(this, "Error", "Failed to create recommended index.");
    }
}

void IndexManagementWidget::onIndexSelectionChanged()
{
    int currentRow = m_indexTable->currentRow();
    m_deleteIndexBtn->setEnabled(currentRow >= 0);

    if (currentRow < 0) {
        m_detailsText->clear();
        return;
    }

    QTableWidgetItem *tableItem = m_indexTable->item(currentRow, 1);
    QTableWidgetItem *fieldItem = m_indexTable->item(currentRow, 2);
    QTableWidgetItem *typeItem = m_indexTable->item(currentRow, 3);

    if (!tableItem || !fieldItem || !typeItem) {
        return;
    }

    std::string tableName = tableItem->text().toStdString();
    std::string fieldName = fieldItem->text().toStdString();
    
    QString typeStr = typeItem->text();
    std::string indexType;
    if (typeStr == "Hash Index") {
        indexType = "hash";
    } else if (typeStr == "Adjacent Index") {
        indexType = "adjacent";
    } else if (typeStr == "B+ Tree Index") {
        indexType = "btree";
    } else {
        return;
    }

    if (!m_indexManager) {
        m_detailsText->setPlainText("Index manager not initialized.");
        return;
    }

    std::map<std::string, std::string> stats;
    if (m_indexManager->getIndexStats(tableName, fieldName, indexType, stats)) {
        QString details = QString("Index Statistics:\n\n");
        for (const auto& pair : stats) {
            details += QString("%1: %2\n").arg(
                QString::fromStdString(pair.first),
                QString::fromStdString(pair.second));
        }
        m_detailsText->setPlainText(details);
    } else {
        m_detailsText->setPlainText("No statistics available.");
    }
}

void IndexManagementWidget::onTableSelectionChanged(const QString& tableName)
{
    refreshIndexList();
}

