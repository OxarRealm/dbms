/**
 * @file table_management_widget.cpp
 * @brief Table Management Widget Implementation
 */

#include "gui/table_management_widget.h"
#include "core/table_manager.h"
#include "core/table_mode.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QComboBox>
#include <cstring>
#include <algorithm>

// ==================== TableEditDialog Implementation ====================

TableEditDialog::TableEditDialog(QWidget *parent, const TableInfo* existingTable)
    : QDialog(parent)
    , m_tableNameEdit(nullptr)
    , m_databaseNameEdit(nullptr)
    , m_databaseNameLabel(nullptr)
    , m_fieldTable(nullptr)
    , m_addFieldBtn(nullptr)
    , m_removeFieldBtn(nullptr)
    , m_moveUpBtn(nullptr)
    , m_moveDownBtn(nullptr)
    , m_buttonBox(nullptr)
{
    setupUI();
    
    if (existingTable) {
        loadTableInfo(*existingTable);
        setWindowTitle("Edit Table");
        m_buttonBox->button(QDialogButtonBox::Ok)->setText("Edit");
    } else {
        setWindowTitle("Create Table");
        m_buttonBox->button(QDialogButtonBox::Ok)->setText("Create");
    }
}

TableEditDialog::~TableEditDialog()
{
}

void TableEditDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Database name input (hidden by default, shown only if needed)
    QHBoxLayout *dbLayout = new QHBoxLayout();
    m_databaseNameLabel = new QLabel("Database Name:", this);
    m_databaseNameLabel->setFont(QFont("Segoe UI", 9));
    dbLayout->addWidget(m_databaseNameLabel);
    m_databaseNameEdit = new QLineEdit(this);
    m_databaseNameEdit->setPlaceholderText("Enter database file name (without extension)");
    m_databaseNameEdit->setFont(QFont("Segoe UI", 9));
    dbLayout->addWidget(m_databaseNameEdit);
    mainLayout->addLayout(dbLayout);

    // Table name input
    QHBoxLayout *tableLayout = new QHBoxLayout();
    tableLayout->addWidget(new QLabel("Table Name:", this));
    m_tableNameEdit = new QLineEdit(this);
    m_tableNameEdit->setPlaceholderText("Enter table name");
    tableLayout->addWidget(m_tableNameEdit);
    mainLayout->addLayout(tableLayout);

    // Field definition table
    QGroupBox *fieldGroup = new QGroupBox("Fields", this);
    QVBoxLayout *fieldLayout = new QVBoxLayout(fieldGroup);

    m_fieldTable = new QTableWidget(0, 6, this);
    m_fieldTable->setHorizontalHeaderLabels(QStringList() << "Field Name" << "Type" << "Size" << "KEY" << "NULL" << "VALID");
    m_fieldTable->horizontalHeader()->setStretchLastSection(true);
    m_fieldTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fieldTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fieldTable->setFont(QFont("Segoe UI", 9));
    fieldLayout->addWidget(m_fieldTable);

    // Field buttons
    QHBoxLayout *fieldBtnLayout = new QHBoxLayout();
    m_addFieldBtn = new QPushButton("Add Field", this);
    m_removeFieldBtn = new QPushButton("Remove Field", this);
    m_moveUpBtn = new QPushButton("Move Up", this);
    m_moveDownBtn = new QPushButton("Move Down", this);
    
    m_addFieldBtn->setFont(QFont("Segoe UI", 9));
    m_removeFieldBtn->setFont(QFont("Segoe UI", 9));
    m_moveUpBtn->setFont(QFont("Segoe UI", 9));
    m_moveDownBtn->setFont(QFont("Segoe UI", 9));
    
    connect(m_addFieldBtn, &QPushButton::clicked, this, &TableEditDialog::addField);
    connect(m_removeFieldBtn, &QPushButton::clicked, this, &TableEditDialog::removeField);
    connect(m_moveUpBtn, &QPushButton::clicked, this, &TableEditDialog::moveFieldUp);
    connect(m_moveDownBtn, &QPushButton::clicked, this, &TableEditDialog::moveFieldDown);
    
    fieldBtnLayout->addWidget(m_addFieldBtn);
    fieldBtnLayout->addWidget(m_removeFieldBtn);
    fieldBtnLayout->addStretch();
    fieldBtnLayout->addWidget(m_moveUpBtn);
    fieldBtnLayout->addWidget(m_moveDownBtn);
    fieldLayout->addLayout(fieldBtnLayout);

    mainLayout->addWidget(fieldGroup);

    // Dialog buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    // Button text will be set in constructor based on mode
    m_buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    m_buttonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);

    setMinimumSize(700, 500);
    resize(800, 600);
}

void TableEditDialog::addField()
{
    int row = m_fieldTable->rowCount();
    m_fieldTable->insertRow(row);

    // Field name
    QTableWidgetItem *nameItem = new QTableWidgetItem("");
    m_fieldTable->setItem(row, 0, nameItem);

    // Type
    QComboBox *typeCombo = new QComboBox(this);
    typeCombo->setFont(QFont("Segoe UI", 9));
    typeCombo->addItems(QStringList() << "int" << "char" << "string" << "float" << "double");
    m_fieldTable->setCellWidget(row, 1, typeCombo);

    // Size
    QTableWidgetItem *sizeItem = new QTableWidgetItem("0");
    m_fieldTable->setItem(row, 2, sizeItem);

    // KEY
    QComboBox *keyCombo = new QComboBox(this);
    keyCombo->setFont(QFont("Segoe UI", 9));
    keyCombo->addItems(QStringList() << "NOT_KEY" << "KEY");
    m_fieldTable->setCellWidget(row, 3, keyCombo);

    // NULL
    QComboBox *nullCombo = new QComboBox(this);
    nullCombo->setFont(QFont("Segoe UI", 9));
    nullCombo->addItems(QStringList() << "NO_NULL" << "NULL");
    m_fieldTable->setCellWidget(row, 4, nullCombo);

    // VALID
    QComboBox *validCombo = new QComboBox(this);
    validCombo->setFont(QFont("Segoe UI", 9));
    validCombo->addItems(QStringList() << "INVALID" << "VALID");
    validCombo->setCurrentIndex(1); // Default to VALID
    m_fieldTable->setCellWidget(row, 5, validCombo);
}

void TableEditDialog::removeField()
{
    int row = m_fieldTable->currentRow();
    if (row >= 0) {
        m_fieldTable->removeRow(row);
    }
}

void TableEditDialog::moveFieldUp()
{
    int row = m_fieldTable->currentRow();
    if (row > 0) {
        // Swap rows
        for (int col = 0; col < m_fieldTable->columnCount(); ++col) {
            QTableWidgetItem *item1 = m_fieldTable->takeItem(row, col);
            QTableWidgetItem *item2 = m_fieldTable->takeItem(row - 1, col);
            m_fieldTable->setItem(row - 1, col, item1);
            m_fieldTable->setItem(row, col, item2);
            
            // Handle widgets
            QWidget *widget1 = m_fieldTable->cellWidget(row, col);
            QWidget *widget2 = m_fieldTable->cellWidget(row - 1, col);
            if (widget1 || widget2) {
                m_fieldTable->setCellWidget(row - 1, col, widget1);
                m_fieldTable->setCellWidget(row, col, widget2);
            }
        }
        m_fieldTable->selectRow(row - 1);
    }
}

void TableEditDialog::moveFieldDown()
{
    int row = m_fieldTable->currentRow();
    if (row >= 0 && row < m_fieldTable->rowCount() - 1) {
        // Swap rows
        for (int col = 0; col < m_fieldTable->columnCount(); ++col) {
            QTableWidgetItem *item1 = m_fieldTable->takeItem(row, col);
            QTableWidgetItem *item2 = m_fieldTable->takeItem(row + 1, col);
            m_fieldTable->setItem(row + 1, col, item1);
            m_fieldTable->setItem(row, col, item2);
            
            // Handle widgets
            QWidget *widget1 = m_fieldTable->cellWidget(row, col);
            QWidget *widget2 = m_fieldTable->cellWidget(row + 1, col);
            if (widget1 || widget2) {
                m_fieldTable->setCellWidget(row + 1, col, widget1);
                m_fieldTable->setCellWidget(row, col, widget2);
            }
        }
        m_fieldTable->selectRow(row + 1);
    }
}

void TableEditDialog::loadTableInfo(const TableInfo& tableInfo)
{
    m_tableNameEdit->setText(QString::fromLocal8Bit(tableInfo.tableName));
    
    m_fieldTable->setRowCount(0);
    for (const auto& field : tableInfo.fields) {
        int row = m_fieldTable->rowCount();
        m_fieldTable->insertRow(row);

        // Field name
        QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromLocal8Bit(field.sFieldName));
        m_fieldTable->setItem(row, 0, nameItem);

        // Type
        QComboBox *typeCombo = new QComboBox(this);
        typeCombo->setFont(QFont("Segoe UI", 9));
        typeCombo->addItems(QStringList() << "int" << "char" << "string" << "float" << "double");
        QString typeStr = QString::fromLocal8Bit(field.sType);
        int typeIndex = typeCombo->findText(typeStr);
        if (typeIndex >= 0) {
            typeCombo->setCurrentIndex(typeIndex);
        }
        m_fieldTable->setCellWidget(row, 1, typeCombo);

        // Size
        QTableWidgetItem *sizeItem = new QTableWidgetItem(QString::number(field.iSize));
        m_fieldTable->setItem(row, 2, sizeItem);

        // KEY
        QComboBox *keyCombo = new QComboBox(this);
        keyCombo->setFont(QFont("Segoe UI", 9));
        keyCombo->addItems(QStringList() << "NOT_KEY" << "KEY");
        keyCombo->setCurrentIndex(field.bKey == FLAG_KEY ? 1 : 0);
        m_fieldTable->setCellWidget(row, 3, keyCombo);

        // NULL
        QComboBox *nullCombo = new QComboBox(this);
        nullCombo->setFont(QFont("Segoe UI", 9));
        nullCombo->addItems(QStringList() << "NO_NULL" << "NULL");
        nullCombo->setCurrentIndex(field.bNullFlag == FLAG_NULL ? 1 : 0);
        m_fieldTable->setCellWidget(row, 4, nullCombo);

        // VALID
        QComboBox *validCombo = new QComboBox(this);
        validCombo->setFont(QFont("Segoe UI", 9));
        validCombo->addItems(QStringList() << "INVALID" << "VALID");
        validCombo->setCurrentIndex(field.bValidFlag == FLAG_VALID ? 1 : 0);
        m_fieldTable->setCellWidget(row, 5, validCombo);
    }
}

void TableEditDialog::closeEvent(QCloseEvent *event)
{
    // Handle close event (X button or Alt+F4)
    // Same behavior as Cancel button - ask for confirmation
    int ret = QMessageBox::question(this, "Confirm Exit", 
        "Are you sure you want to exit?\n\nAll unsaved changes will be lost.",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        event->accept(); // Close the dialog
    } else {
        event->ignore(); // Keep the dialog open
    }
}

bool TableEditDialog::getTableInfo(TableInfo& tableInfo)
{
    // Validate database name (if visible and enabled)
    if (m_databaseNameEdit->isVisible() && m_databaseNameEdit->isEnabled()) {
        QString dbName = m_databaseNameEdit->text().trimmed();
        if (dbName.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Database name cannot be empty.");
            return false;
        }
    }
    
    // Validate inputs
    QString tableName = m_tableNameEdit->text().trimmed();
    if (tableName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Table name cannot be empty.");
        return false;
    }

    if (m_fieldTable->rowCount() == 0) {
        QMessageBox::warning(this, "Validation Error", "Table must have at least one field.");
        return false;
    }

    // Set table name
    QByteArray tableNameBytes = tableName.toLocal8Bit();
    strncpy(tableInfo.tableName, tableNameBytes.constData(), TABLE_NAME_LENGTH - 1);
    tableInfo.tableName[TABLE_NAME_LENGTH - 1] = '\0';

    // Clear and populate fields
    tableInfo.fields.clear();
    for (int row = 0; row < m_fieldTable->rowCount(); ++row) {
        TableMode field;
        memset(&field, 0, sizeof(TableMode));

        // Field name
        QTableWidgetItem *nameItem = m_fieldTable->item(row, 0);
        if (!nameItem || nameItem->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", 
                QString("Field name at row %1 cannot be empty.").arg(row + 1));
            return false;
        }
        QByteArray nameBytes = nameItem->text().trimmed().toLocal8Bit();
        strncpy(field.sFieldName, nameBytes.constData(), FIELD_NAME_LENGTH - 1);
        field.sFieldName[FIELD_NAME_LENGTH - 1] = '\0';

        // Type
        QComboBox *typeCombo = qobject_cast<QComboBox*>(m_fieldTable->cellWidget(row, 1));
        if (typeCombo) {
            QByteArray typeBytes = typeCombo->currentText().toLocal8Bit();
            strncpy(field.sType, typeBytes.constData(), TYPE_NAME_LENGTH - 1);
            field.sType[TYPE_NAME_LENGTH - 1] = '\0';
        }

        // Size
        QTableWidgetItem *sizeItem = m_fieldTable->item(row, 2);
        if (sizeItem) {
            field.iSize = sizeItem->text().toInt();
        }

        // KEY
        QComboBox *keyCombo = qobject_cast<QComboBox*>(m_fieldTable->cellWidget(row, 3));
        if (keyCombo) {
            field.bKey = (keyCombo->currentIndex() == 1) ? FLAG_KEY : FLAG_NOT_KEY;
        }

        // NULL
        QComboBox *nullCombo = qobject_cast<QComboBox*>(m_fieldTable->cellWidget(row, 4));
        if (nullCombo) {
            field.bNullFlag = (nullCombo->currentIndex() == 1) ? FLAG_NULL : FLAG_NO_NULL;
        }

        // VALID
        QComboBox *validCombo = qobject_cast<QComboBox*>(m_fieldTable->cellWidget(row, 5));
        if (validCombo) {
            field.bValidFlag = (validCombo->currentIndex() == 1) ? FLAG_VALID : FLAG_INVALID;
        }

        tableInfo.fields.push_back(field);
    }

    return true;
}

// ==================== TableManagementWidget Implementation ====================

TableManagementWidget::TableManagementWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_leftLayout(nullptr)
    , m_rightLayout(nullptr)
    , m_tableListGroup(nullptr)
    , m_tableList(nullptr)
    , m_createBtn(nullptr)
    , m_editBtn(nullptr)
    , m_deleteBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_tableInfoGroup(nullptr)
    , m_tableNameLabel(nullptr)
    , m_fieldCountLabel(nullptr)
    , m_fieldInfoTable(nullptr)
    , m_noTableSelectedLabel(nullptr)
    , m_tableManager(nullptr)
{
    m_tableManager = new TableManager();
    setupUI();
}

TableManagementWidget::~TableManagementWidget()
{
    if (m_tableManager) {
        delete m_tableManager;
    }
}

void TableManagementWidget::setupUI()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    // Left panel: Table list
    m_leftLayout = new QVBoxLayout();
    m_tableListGroup = new QGroupBox("Tables", this);
    QVBoxLayout *listLayout = new QVBoxLayout(m_tableListGroup);

    m_tableList = new QListWidget(this);
    m_tableList->setFont(QFont("Segoe UI", 9));
    connect(m_tableList, &QListWidget::itemSelectionChanged, this, &TableManagementWidget::onTableSelectionChanged);
    listLayout->addWidget(m_tableList);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_createBtn = new QPushButton("Create", this);
    m_editBtn = new QPushButton("Edit", this);
    m_deleteBtn = new QPushButton("Delete", this);
    m_refreshBtn = new QPushButton("Refresh", this);

    m_createBtn->setFont(QFont("Segoe UI", 9));
    m_editBtn->setFont(QFont("Segoe UI", 9));
    m_deleteBtn->setFont(QFont("Segoe UI", 9));
    m_refreshBtn->setFont(QFont("Segoe UI", 9));

    connect(m_createBtn, &QPushButton::clicked, this, &TableManagementWidget::onCreateTable);
    connect(m_editBtn, &QPushButton::clicked, this, &TableManagementWidget::onEditTable);
    connect(m_deleteBtn, &QPushButton::clicked, this, &TableManagementWidget::onDeleteTable);
    connect(m_refreshBtn, &QPushButton::clicked, this, &TableManagementWidget::onRefresh);

    btnLayout->addWidget(m_createBtn);
    btnLayout->addWidget(m_editBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_refreshBtn);
    listLayout->addLayout(btnLayout);

    m_leftLayout->addWidget(m_tableListGroup);
    m_mainLayout->addLayout(m_leftLayout, 1);

    // Right panel: Table info
    m_rightLayout = new QVBoxLayout();
    m_tableInfoGroup = new QGroupBox("Table Information", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(m_tableInfoGroup);

    m_tableNameLabel = new QLabel("No table selected", this);
    m_tableNameLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    m_tableNameLabel->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget(m_tableNameLabel);

    m_fieldCountLabel = new QLabel("", this);
    m_fieldCountLabel->setFont(QFont("Segoe UI", 9));
    infoLayout->addWidget(m_fieldCountLabel);

    m_fieldInfoTable = new QTableWidget(0, 6, this);
    m_fieldInfoTable->setHorizontalHeaderLabels(QStringList() << "Field Name" << "Type" << "Size" << "KEY" << "NULL" << "VALID");
    m_fieldInfoTable->horizontalHeader()->setStretchLastSection(true);
    m_fieldInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fieldInfoTable->setFont(QFont("Segoe UI", 9));
    infoLayout->addWidget(m_fieldInfoTable);

    m_noTableSelectedLabel = new QLabel("Select a table to view its structure", this);
    m_noTableSelectedLabel->setFont(QFont("Segoe UI", 9));
    m_noTableSelectedLabel->setAlignment(Qt::AlignCenter);
    m_noTableSelectedLabel->setStyleSheet("color: gray;");
    infoLayout->addWidget(m_noTableSelectedLabel);

    m_rightLayout->addWidget(m_tableInfoGroup);
    m_mainLayout->addLayout(m_rightLayout, 2);
}

void TableManagementWidget::setDatabasePath(const std::string& dbPath)
{
    m_databasePath = dbPath;
    if (m_tableManager) {
        m_tableManager->setDatabasePath(dbPath);
    }
    refreshTableList();
}

void TableManagementWidget::refreshTableList()
{
    loadTableList();
}

void TableManagementWidget::loadTableList()
{
    m_tableList->clear();
    clearTableInfo();

    if (m_databasePath.empty()) {
        return;
    }

    std::vector<TableInfo> tableList;
    if (m_tableManager->readAllTables(tableList)) {
        for (const auto& table : tableList) {
            m_tableList->addItem(QString::fromLocal8Bit(table.tableName));
        }
    }
}

void TableManagementWidget::displayTableInfo(const TableInfo& tableInfo)
{
    m_tableNameLabel->setText(QString("Table: %1").arg(QString::fromLocal8Bit(tableInfo.tableName)));
    m_fieldCountLabel->setText(QString("Fields: %1").arg(tableInfo.fields.size()));
    m_noTableSelectedLabel->hide();

    m_fieldInfoTable->setRowCount(0);
    for (const auto& field : tableInfo.fields) {
        int row = m_fieldInfoTable->rowCount();
        m_fieldInfoTable->insertRow(row);

        m_fieldInfoTable->setItem(row, 0, new QTableWidgetItem(QString::fromLocal8Bit(field.sFieldName)));
        m_fieldInfoTable->setItem(row, 1, new QTableWidgetItem(QString::fromLocal8Bit(field.sType)));
        m_fieldInfoTable->setItem(row, 2, new QTableWidgetItem(QString::number(field.iSize)));
        m_fieldInfoTable->setItem(row, 3, new QTableWidgetItem(field.bKey == FLAG_KEY ? "KEY" : "NOT_KEY"));
        m_fieldInfoTable->setItem(row, 4, new QTableWidgetItem(field.bNullFlag == FLAG_NULL ? "NULL" : "NO_NULL"));
        m_fieldInfoTable->setItem(row, 5, new QTableWidgetItem(field.bValidFlag == FLAG_VALID ? "VALID" : "INVALID"));
    }
    m_fieldInfoTable->show();
}

void TableManagementWidget::clearTableInfo()
{
    m_tableNameLabel->setText("No table selected");
    m_fieldCountLabel->setText("");
    m_fieldInfoTable->setRowCount(0);
    m_fieldInfoTable->hide();
    m_noTableSelectedLabel->show();
}

void TableManagementWidget::onTableSelectionChanged()
{
    QListWidgetItem *item = m_tableList->currentItem();
    if (!item) {
        clearTableInfo();
        return;
    }

    QString tableName = item->text();
    TableInfo tableInfo;
    if (m_tableManager->readTable(tableName.toLocal8Bit().constData(), tableInfo)) {
        displayTableInfo(tableInfo);
    } else {
        clearTableInfo();
        QMessageBox::warning(this, "Error", QString("Failed to load table: %1").arg(tableName));
    }
}

void TableManagementWidget::onCreateTable()
{
    // Check if database is loaded
    if (m_databasePath.empty()) {
        QMessageBox::information(this, "No Database", 
            "Please create or open a database first.\n\nUse File -> Create Database or File -> Open Database.");
        return;
    }

    TableEditDialog dialog(this);
    // Hide database name input (it's set from main window)
    dialog.m_databaseNameEdit->setText(QString::fromLocal8Bit(m_databasePath.c_str()));
    dialog.m_databaseNameEdit->setEnabled(false);
    dialog.m_databaseNameEdit->setVisible(false);
    
    if (dialog.exec() == QDialog::Accepted) {
        TableInfo tableInfo;
        if (dialog.getTableInfo(tableInfo)) {
            if (m_tableManager->createTable(tableInfo)) {
                QMessageBox::information(this, "Success", "Table created successfully.");
                refreshTableList();
            } else {
                QMessageBox::warning(this, "Error", "Failed to create table.");
            }
        }
    }
}

void TableManagementWidget::onEditTable()
{
    QListWidgetItem *item = m_tableList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a table to edit.");
        return;
    }

    QString tableName = item->text();
    TableInfo tableInfo;
    if (!m_tableManager->readTable(tableName.toLocal8Bit().constData(), tableInfo)) {
        QMessageBox::warning(this, "Error", QString("Failed to load table: %1").arg(tableName));
        return;
    }

    TableEditDialog dialog(this, &tableInfo);
    // Hide database name input in edit mode (use current database)
    dialog.m_databaseNameEdit->setText(QString::fromLocal8Bit(m_databasePath.c_str()));
    dialog.m_databaseNameEdit->setEnabled(false);
    dialog.m_databaseNameEdit->setVisible(false);
    dialog.m_databaseNameLabel->setVisible(false);
    
    if (dialog.exec() == QDialog::Accepted) {
        TableInfo newTableInfo;
        if (dialog.getTableInfo(newTableInfo)) {
            if (m_tableManager->updateTable(tableName.toLocal8Bit().constData(), newTableInfo)) {
                QMessageBox::information(this, "Success", "Table updated successfully.");
                refreshTableList();
            } else {
                QMessageBox::warning(this, "Error", "Failed to update table.");
            }
        }
    }
}

void TableManagementWidget::onDeleteTable()
{
    QListWidgetItem *item = m_tableList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a table to delete.");
        return;
    }

    QString tableName = item->text();
    int ret = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete table '%1'?\n\nThis action cannot be undone.").arg(tableName),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (m_tableManager->deleteTable(tableName.toLocal8Bit().constData())) {
            QMessageBox::information(this, "Success", "Table deleted successfully.");
            refreshTableList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete table.");
        }
    }
}

void TableManagementWidget::onRefresh()
{
    refreshTableList();
}

