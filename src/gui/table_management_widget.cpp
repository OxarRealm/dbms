/**
 * @file table_management_widget.cpp
 * @brief Table Management Widget Implementation
 */

// Disable strncpy warning - we ensure null termination manually
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

#include "gui/table_management_widget.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include "core/table_mode.h"
#include "core/constraint.h"
#include "core/constraint_registry.h"
#include "core/constraint_storage.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QFileInfo>
#include <cstring>
#include <algorithm>
#include <numeric>

// ==================== TableEditDialog Implementation ====================

TableEditDialog::TableEditDialog(QWidget *parent, const TableInfo* existingTable)
    : QDialog(parent)
    , m_tableNameEdit(nullptr)
    , m_databaseNameEdit(nullptr)
    , m_databaseNameLabel(nullptr)
    , m_tabWidget(nullptr)
    , m_fieldTable(nullptr)
    , m_addFieldBtn(nullptr)
    , m_removeFieldBtn(nullptr)
    , m_moveUpBtn(nullptr)
    , m_moveDownBtn(nullptr)
    , m_foreignKeyList(nullptr)
    , m_uniqueConstraintList(nullptr)
    , m_checkConstraintList(nullptr)
    , m_addForeignKeyBtn(nullptr)
    , m_editForeignKeyBtn(nullptr)
    , m_removeForeignKeyBtn(nullptr)
    , m_addUniqueBtn(nullptr)
    , m_editUniqueBtn(nullptr)
    , m_removeUniqueBtn(nullptr)
    , m_addCheckBtn(nullptr)
    , m_editCheckBtn(nullptr)
    , m_removeCheckBtn(nullptr)
    , m_buttonBox(nullptr)
{
    setupUI();
    
    if (existingTable) {
        loadTableInfo(*existingTable);
        // Load constraints from registry
        QString dbName = m_databaseNameEdit->text();
        if (!dbName.isEmpty()) {
            TableConstraints constraints;
            if (ConstraintRegistry::getInstance().getTableConstraints(
                    dbName.toStdString(), 
                    QString::fromLocal8Bit(existingTable->tableName).toStdString(),
                    constraints)) {
                loadConstraints(constraints);
            }
        }
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

    m_fieldTable = new QTableWidget(0, 8, this);
    m_fieldTable->setHorizontalHeaderLabels(QStringList() << "Field Name" << "Type" << "Size" << "KEY" << "NULL" << "VALID" << "UNIQUE" << "DEFAULT");
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

    // Create tab widget for Fields and Constraints
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setFont(QFont("Segoe UI", 9));
    
    // Fields tab
    m_tabWidget->addTab(fieldGroup, "Fields");
    
    // Constraints tab
    QWidget *constraintsTab = new QWidget(this);
    QVBoxLayout *constraintsLayout = new QVBoxLayout(constraintsTab);
    constraintsLayout->setSpacing(10);
    
    // Foreign Key Constraints
    QGroupBox *fkGroup = new QGroupBox("Foreign Key Constraints", this);
    QVBoxLayout *fkLayout = new QVBoxLayout(fkGroup);
    m_foreignKeyList = new QListWidget(this);
    m_foreignKeyList->setFont(QFont("Segoe UI", 9));
    m_foreignKeyList->setMinimumHeight(120);
    m_foreignKeyList->setMaximumHeight(200);
    fkLayout->addWidget(m_foreignKeyList);
    QHBoxLayout *fkBtnLayout = new QHBoxLayout();
    m_addForeignKeyBtn = new QPushButton("Add", this);
    m_editForeignKeyBtn = new QPushButton("Edit", this);
    m_removeForeignKeyBtn = new QPushButton("Remove", this);
    m_addForeignKeyBtn->setFont(QFont("Segoe UI", 9));
    m_editForeignKeyBtn->setFont(QFont("Segoe UI", 9));
    m_removeForeignKeyBtn->setFont(QFont("Segoe UI", 9));
    connect(m_addForeignKeyBtn, &QPushButton::clicked, this, &TableEditDialog::addForeignKey);
    connect(m_editForeignKeyBtn, &QPushButton::clicked, this, &TableEditDialog::editForeignKey);
    connect(m_removeForeignKeyBtn, &QPushButton::clicked, this, &TableEditDialog::removeForeignKey);
    fkBtnLayout->addWidget(m_addForeignKeyBtn);
    fkBtnLayout->addWidget(m_editForeignKeyBtn);
    fkBtnLayout->addWidget(m_removeForeignKeyBtn);
    fkBtnLayout->addStretch();
    fkLayout->addLayout(fkBtnLayout);
    constraintsLayout->addWidget(fkGroup);
    
    // Unique Constraints
    QGroupBox *uniqueGroup = new QGroupBox("Unique Constraints", this);
    QVBoxLayout *uniqueLayout = new QVBoxLayout(uniqueGroup);
    m_uniqueConstraintList = new QListWidget(this);
    m_uniqueConstraintList->setFont(QFont("Segoe UI", 9));
    m_uniqueConstraintList->setMinimumHeight(120);
    m_uniqueConstraintList->setMaximumHeight(200);
    uniqueLayout->addWidget(m_uniqueConstraintList);
    QHBoxLayout *uniqueBtnLayout = new QHBoxLayout();
    m_addUniqueBtn = new QPushButton("Add", this);
    m_editUniqueBtn = new QPushButton("Edit", this);
    m_removeUniqueBtn = new QPushButton("Remove", this);
    m_addUniqueBtn->setFont(QFont("Segoe UI", 9));
    m_editUniqueBtn->setFont(QFont("Segoe UI", 9));
    m_removeUniqueBtn->setFont(QFont("Segoe UI", 9));
    connect(m_addUniqueBtn, &QPushButton::clicked, this, &TableEditDialog::addUniqueConstraint);
    connect(m_editUniqueBtn, &QPushButton::clicked, this, &TableEditDialog::editUniqueConstraint);
    connect(m_removeUniqueBtn, &QPushButton::clicked, this, &TableEditDialog::removeUniqueConstraint);
    uniqueBtnLayout->addWidget(m_addUniqueBtn);
    uniqueBtnLayout->addWidget(m_editUniqueBtn);
    uniqueBtnLayout->addWidget(m_removeUniqueBtn);
    uniqueBtnLayout->addStretch();
    uniqueLayout->addLayout(uniqueBtnLayout);
    constraintsLayout->addWidget(uniqueGroup);
    
    // Check Constraints
    QGroupBox *checkGroup = new QGroupBox("Check Constraints", this);
    QVBoxLayout *checkLayout = new QVBoxLayout(checkGroup);
    m_checkConstraintList = new QListWidget(this);
    m_checkConstraintList->setFont(QFont("Segoe UI", 9));
    m_checkConstraintList->setMinimumHeight(120);
    m_checkConstraintList->setMaximumHeight(200);
    checkLayout->addWidget(m_checkConstraintList);
    QHBoxLayout *checkBtnLayout = new QHBoxLayout();
    m_addCheckBtn = new QPushButton("Add", this);
    m_editCheckBtn = new QPushButton("Edit", this);
    m_removeCheckBtn = new QPushButton("Remove", this);
    m_addCheckBtn->setFont(QFont("Segoe UI", 9));
    m_editCheckBtn->setFont(QFont("Segoe UI", 9));
    m_removeCheckBtn->setFont(QFont("Segoe UI", 9));
    connect(m_addCheckBtn, &QPushButton::clicked, this, &TableEditDialog::addCheckConstraint);
    connect(m_editCheckBtn, &QPushButton::clicked, this, &TableEditDialog::editCheckConstraint);
    connect(m_removeCheckBtn, &QPushButton::clicked, this, &TableEditDialog::removeCheckConstraint);
    checkBtnLayout->addWidget(m_addCheckBtn);
    checkBtnLayout->addWidget(m_editCheckBtn);
    checkBtnLayout->addWidget(m_removeCheckBtn);
    checkBtnLayout->addStretch();
    checkLayout->addLayout(checkBtnLayout);
    constraintsLayout->addWidget(checkGroup);
    
    constraintsLayout->addStretch();
    m_tabWidget->addTab(constraintsTab, "Constraints");
    
    mainLayout->addWidget(m_tabWidget);

    // Dialog buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    // Button text will be set in constructor based on mode
    m_buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    m_buttonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);

    setMinimumSize(900, 700);
    resize(1000, 800);
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
    
    // UNIQUE
    QComboBox *uniqueCombo = new QComboBox(this);
    uniqueCombo->setFont(QFont("Segoe UI", 9));
    uniqueCombo->addItems(QStringList() << "NOT_UNIQUE" << "UNIQUE");
    uniqueCombo->setCurrentIndex(0); // Default to NOT_UNIQUE
    m_fieldTable->setCellWidget(row, 6, uniqueCombo);
    
    // DEFAULT
    QLineEdit *defaultEdit = new QLineEdit(this);
    defaultEdit->setFont(QFont("Segoe UI", 9));
    defaultEdit->setPlaceholderText("Enter default value (optional)");
    m_fieldTable->setCellWidget(row, 7, defaultEdit);
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
        
        // UNIQUE
        QComboBox *uniqueCombo = new QComboBox(this);
        uniqueCombo->setFont(QFont("Segoe UI", 9));
        uniqueCombo->addItems(QStringList() << "NOT_UNIQUE" << "UNIQUE");
        uniqueCombo->setCurrentIndex(field.bUnique == FLAG_KEY ? 1 : 0);
        m_fieldTable->setCellWidget(row, 6, uniqueCombo);
        
        // DEFAULT
        QLineEdit *defaultEdit = new QLineEdit(this);
        defaultEdit->setFont(QFont("Segoe UI", 9));
        defaultEdit->setPlaceholderText("Enter default value (optional)");
        QString defaultValue = (field.sDefaultValue[0] != '\0') ? QString::fromLocal8Bit(field.sDefaultValue) : "";
        defaultEdit->setText(defaultValue);
        m_fieldTable->setCellWidget(row, 7, defaultEdit);
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

bool TableEditDialog::getTableInfo(TableInfo& tableInfo, TableConstraints& constraints)
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
        
        // UNIQUE
        QComboBox *uniqueCombo = qobject_cast<QComboBox*>(m_fieldTable->cellWidget(row, 6));
        if (uniqueCombo) {
            field.bUnique = (uniqueCombo->currentIndex() == 1) ? FLAG_KEY : 0;
        }
        
        // DEFAULT
        QLineEdit *defaultEdit = qobject_cast<QLineEdit*>(m_fieldTable->cellWidget(row, 7));
        if (defaultEdit) {
            QString defaultValue = defaultEdit->text().trimmed();
            if (!defaultValue.isEmpty()) {
                QByteArray defaultBytes = defaultValue.toLocal8Bit();
                strncpy(field.sDefaultValue, defaultBytes.constData(), 127);
                field.sDefaultValue[127] = '\0';
            } else {
                field.sDefaultValue[0] = '\0';
            }
        }

        tableInfo.fields.push_back(field);
    }
    
    // Collect constraints
    constraints.foreignKeys = m_foreignKeys;
    constraints.uniqueConstraints = m_uniqueConstraints;
    constraints.checkConstraints = m_checkConstraints;

    return true;
}

QStringList TableEditDialog::getFieldNames() const {
    QStringList fields;
    for (int row = 0; row < m_fieldTable->rowCount(); ++row) {
        QTableWidgetItem *item = m_fieldTable->item(row, 0);
        if (item && !item->text().trimmed().isEmpty()) {
            fields << item->text().trimmed();
        }
    }
    return fields;
}

QStringList TableEditDialog::getTableNames() const {
    // Get table names from the database
    QStringList tables;
    QString dbName = m_databaseNameEdit->text();
    if (!dbName.isEmpty()) {
        TableManager tableManager;
        tableManager.setDatabasePath(dbName.toStdString());
        std::vector<std::string> tableNames;
        if (tableManager.getAllTableNames(tableNames)) {
            for (const std::string& name : tableNames) {
                // Exclude current table name
                QString currentTableName = m_tableNameEdit->text().trimmed();
                if (QString::fromLocal8Bit(name.c_str()) != currentTableName) {
                    tables << QString::fromLocal8Bit(name.c_str());
                }
            }
        }
    }
    return tables;
}

void TableEditDialog::loadConstraints(const TableConstraints& constraints) {
    m_foreignKeys = constraints.foreignKeys;
    m_uniqueConstraints = constraints.uniqueConstraints;
    m_checkConstraints = constraints.checkConstraints;
    updateConstraintLists();
}

void TableEditDialog::updateConstraintLists() {
    // Update foreign key list
    m_foreignKeyList->clear();
    for (const auto& fk : m_foreignKeys) {
        QString text = QString("%1: %2 -> %3.%4 (ON DELETE %5, ON UPDATE %6)")
            .arg(fk.constraintName[0] != '\0' ? QString::fromLocal8Bit(fk.constraintName) : "Unnamed")
            .arg(QString::fromLocal8Bit(fk.fieldName))
            .arg(QString::fromLocal8Bit(fk.referencedTable))
            .arg(QString::fromLocal8Bit(fk.referencedField))
            .arg(QString::fromLocal8Bit(fk.onDeleteAction))
            .arg(QString::fromLocal8Bit(fk.onUpdateAction));
        m_foreignKeyList->addItem(text);
    }
    
    // Update unique constraint list
    m_uniqueConstraintList->clear();
    for (const auto& unique : m_uniqueConstraints) {
        QString fields = QString::fromStdString(
            std::accumulate(unique.fieldNames.begin(), unique.fieldNames.end(), std::string(),
                [](const std::string& a, const std::string& b) {
                    return a.empty() ? b : a + ", " + b;
                }));
        QString text = QString("%1: (%2)")
            .arg(unique.constraintName[0] != '\0' ? QString::fromLocal8Bit(unique.constraintName) : "Unnamed")
            .arg(fields);
        m_uniqueConstraintList->addItem(text);
    }
    
    // Update check constraint list
    m_checkConstraintList->clear();
    for (const auto& check : m_checkConstraints) {
        QString text = QString("%1: %2 %3")
            .arg(check.constraintName[0] != '\0' ? QString::fromLocal8Bit(check.constraintName) : "Unnamed")
            .arg(QString::fromLocal8Bit(check.fieldName))
            .arg(QString::fromStdString(check.expression));
        m_checkConstraintList->addItem(text);
    }
}

void TableEditDialog::addForeignKey() {
    // Create a custom dialog for foreign key input
    QStringList fields = getFieldNames();
    if (fields.isEmpty()) {
        QMessageBox::warning(this, "No Fields", "Please add fields first.");
        return;
    }
    
    QDialog dialog(this);
    dialog.setWindowTitle("Add Foreign Key Constraint");
    dialog.setMinimumSize(400, 300);
    dialog.resize(450, 350);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // Field selection
    QHBoxLayout *fieldLayout = new QHBoxLayout();
    fieldLayout->addWidget(new QLabel("Field:", &dialog));
    QComboBox *fieldCombo = new QComboBox(&dialog);
    fieldCombo->setFont(QFont("Segoe UI", 9));
    fieldCombo->addItems(fields);
    fieldLayout->addWidget(fieldCombo);
    layout->addLayout(fieldLayout);
    
    // Referenced table selection
    QStringList tables = getTableNames();
    if (tables.isEmpty()) {
        QMessageBox::warning(this, "No Tables", "No other tables found. Foreign key must reference another table.");
        return;
    }
    
    QHBoxLayout *refTableLayout = new QHBoxLayout();
    refTableLayout->addWidget(new QLabel("Referenced Table:", &dialog));
    QComboBox *refTableCombo = new QComboBox(&dialog);
    refTableCombo->setFont(QFont("Segoe UI", 9));
    refTableCombo->addItems(tables);
    refTableLayout->addWidget(refTableCombo);
    layout->addLayout(refTableLayout);
    
    // Referenced field input
    QHBoxLayout *refFieldLayout = new QHBoxLayout();
    refFieldLayout->addWidget(new QLabel("Referenced Field:", &dialog));
    QLineEdit *refFieldEdit = new QLineEdit(&dialog);
    refFieldEdit->setFont(QFont("Segoe UI", 9));
    refFieldLayout->addWidget(refFieldEdit);
    layout->addLayout(refFieldLayout);
    
    // ON DELETE action
    QHBoxLayout *onDeleteLayout = new QHBoxLayout();
    onDeleteLayout->addWidget(new QLabel("ON DELETE action:", &dialog));
    QComboBox *onDeleteCombo = new QComboBox(&dialog);
    onDeleteCombo->setFont(QFont("Segoe UI", 9));
    onDeleteCombo->addItems(QStringList() << "RESTRICT" << "CASCADE" << "SET NULL" << "NO ACTION");
    onDeleteLayout->addWidget(onDeleteCombo);
    layout->addLayout(onDeleteLayout);
    
    // ON UPDATE action
    QHBoxLayout *onUpdateLayout = new QHBoxLayout();
    onUpdateLayout->addWidget(new QLabel("ON UPDATE action:", &dialog));
    QComboBox *onUpdateCombo = new QComboBox(&dialog);
    onUpdateCombo->setFont(QFont("Segoe UI", 9));
    onUpdateCombo->addItems(QStringList() << "RESTRICT" << "CASCADE" << "SET NULL" << "NO ACTION");
    onUpdateLayout->addWidget(onUpdateCombo);
    layout->addLayout(onUpdateLayout);
    
    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    buttonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    QString fieldName = fieldCombo->currentText();
    QString refTable = refTableCombo->currentText();
    QString refField = refFieldEdit->text().trimmed();
    QString onDelete = onDeleteCombo->currentText();
    QString onUpdate = onUpdateCombo->currentText();
    
    if (refField.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Referenced field name cannot be empty.");
        return;
    }
    
    ForeignKeyConstraint fk;
    QByteArray fieldBytes = fieldName.toLocal8Bit();
    strncpy(fk.fieldName, fieldBytes.constData(), FIELD_NAME_LENGTH - 1);
    fk.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
    QByteArray refTableBytes = refTable.toLocal8Bit();
    strncpy(fk.referencedTable, refTableBytes.constData(), TABLE_NAME_LENGTH - 1);
    fk.referencedTable[TABLE_NAME_LENGTH - 1] = '\0';
    QByteArray refFieldBytes = refField.toLocal8Bit();
    strncpy(fk.referencedField, refFieldBytes.constData(), FIELD_NAME_LENGTH - 1);
    fk.referencedField[FIELD_NAME_LENGTH - 1] = '\0';
    QByteArray onDeleteBytes = onDelete.toLocal8Bit();
    strncpy(fk.onDeleteAction, onDeleteBytes.constData(), 15);
    fk.onDeleteAction[15] = '\0';
    QByteArray onUpdateBytes = onUpdate.toLocal8Bit();
    strncpy(fk.onUpdateAction, onUpdateBytes.constData(), 15);
    fk.onUpdateAction[15] = '\0';
    
    m_foreignKeys.push_back(fk);
    updateConstraintLists();
}

void TableEditDialog::editForeignKey() {
    int row = m_foreignKeyList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_foreignKeys.size())) {
        QMessageBox::information(this, "No Selection", "Please select a foreign key to edit.");
        return;
    }
    
    // For simplicity, remove and re-add (could create a proper dialog)
    m_foreignKeys.erase(m_foreignKeys.begin() + row);
    updateConstraintLists();
    addForeignKey();
}

void TableEditDialog::removeForeignKey() {
    int row = m_foreignKeyList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_foreignKeys.size())) {
        QMessageBox::information(this, "No Selection", "Please select a foreign key to remove.");
        return;
    }
    
    m_foreignKeys.erase(m_foreignKeys.begin() + row);
    updateConstraintLists();
}

void TableEditDialog::addUniqueConstraint() {
    QStringList fields = getFieldNames();
    if (fields.isEmpty()) {
        QMessageBox::warning(this, "No Fields", "Please add fields first.");
        return;
    }
    
    // For multi-field unique constraint, allow selecting multiple fields
    // Simplified: use comma-separated input
    bool ok;
    QString fieldNamesStr = QInputDialog::getText(this, "Unique Constraint", 
                                                  "Enter field names (comma-separated):",
                                                  QLineEdit::Normal, "", &ok);
    if (!ok || fieldNamesStr.isEmpty()) return;
    
    QStringList selectedFields = fieldNamesStr.split(',', Qt::SkipEmptyParts);
    for (QString& field : selectedFields) {
        field = field.trimmed();
    }
    
    // Validate fields exist
    for (const QString& field : selectedFields) {
        if (!fields.contains(field)) {
            QMessageBox::warning(this, "Invalid Field", 
                                QString("Field '%1' does not exist.").arg(field));
            return;
        }
    }
    
    UniqueConstraint unique;
    for (const QString& field : selectedFields) {
        unique.fieldNames.push_back(field.toStdString());
    }
    
    m_uniqueConstraints.push_back(unique);
    updateConstraintLists();
}

void TableEditDialog::editUniqueConstraint() {
    int row = m_uniqueConstraintList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_uniqueConstraints.size())) {
        QMessageBox::information(this, "No Selection", "Please select a unique constraint to edit.");
        return;
    }
    
    m_uniqueConstraints.erase(m_uniqueConstraints.begin() + row);
    updateConstraintLists();
    addUniqueConstraint();
}

void TableEditDialog::removeUniqueConstraint() {
    int row = m_uniqueConstraintList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_uniqueConstraints.size())) {
        QMessageBox::information(this, "No Selection", "Please select a unique constraint to remove.");
        return;
    }
    
    m_uniqueConstraints.erase(m_uniqueConstraints.begin() + row);
    updateConstraintLists();
}

void TableEditDialog::addCheckConstraint() {
    QStringList fields = getFieldNames();
    if (fields.isEmpty()) {
        QMessageBox::warning(this, "No Fields", "Please add fields first.");
        return;
    }
    
    bool ok;
    QString fieldName = QInputDialog::getItem(this, "Check Constraint", "Select field:",
                                              fields, 0, false, &ok);
    if (!ok || fieldName.isEmpty()) return;
    
    QDialog exprDialog(this);
    exprDialog.setWindowTitle("Add Check Constraint");
    exprDialog.setMinimumSize(400, 150);
    exprDialog.resize(450, 180);
    QVBoxLayout *exprLayout = new QVBoxLayout(&exprDialog);
    
    QHBoxLayout *exprInputLayout = new QHBoxLayout();
    exprInputLayout->addWidget(new QLabel("Expression:", &exprDialog));
    QLineEdit *exprEdit = new QLineEdit(&exprDialog);
    exprEdit->setFont(QFont("Segoe UI", 9));
    exprEdit->setPlaceholderText("e.g., > 0 AND < 100, >= 0");
    exprInputLayout->addWidget(exprEdit);
    exprLayout->addLayout(exprInputLayout);
    
    QLabel *hintLabel = new QLabel("Examples: > 0, < 100, >= 0 AND <= 150", &exprDialog);
    hintLabel->setFont(QFont("Segoe UI", 8));
    hintLabel->setStyleSheet("color: gray;");
    exprLayout->addWidget(hintLabel);
    
    QDialogButtonBox *exprButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &exprDialog);
    exprButtonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    exprButtonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(exprButtonBox, &QDialogButtonBox::accepted, &exprDialog, &QDialog::accept);
    connect(exprButtonBox, &QDialogButtonBox::rejected, &exprDialog, &QDialog::reject);
    exprLayout->addWidget(exprButtonBox);
    
    if (exprDialog.exec() != QDialog::Accepted) {
        return;
    }
    
    QString expression = exprEdit->text().trimmed();
    if (expression.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Expression cannot be empty.");
        return;
    }
    
    CheckConstraint check;
    QByteArray fieldBytes = fieldName.toLocal8Bit();
    strncpy(check.fieldName, fieldBytes.constData(), FIELD_NAME_LENGTH - 1);
    check.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
    check.expression = expression.toStdString();
    
    m_checkConstraints.push_back(check);
    updateConstraintLists();
}

void TableEditDialog::editCheckConstraint() {
    int row = m_checkConstraintList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_checkConstraints.size())) {
        QMessageBox::information(this, "No Selection", "Please select a check constraint to edit.");
        return;
    }
    
    m_checkConstraints.erase(m_checkConstraints.begin() + row);
    updateConstraintLists();
    addCheckConstraint();
}

void TableEditDialog::removeCheckConstraint() {
    int row = m_checkConstraintList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_checkConstraints.size())) {
        QMessageBox::information(this, "No Selection", "Please select a check constraint to remove.");
        return;
    }
    
    m_checkConstraints.erase(m_checkConstraints.begin() + row);
    updateConstraintLists();
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
    , m_dataManager(nullptr)
{
    m_tableManager = new TableManager();
    m_dataManager = new DataManager();
    setupUI();
}

TableManagementWidget::~TableManagementWidget()
{
    if (m_tableManager) {
        delete m_tableManager;
    }
    if (m_dataManager) {
        delete m_dataManager;
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
    
    // Add button to view constraints
    QPushButton *viewConstraintsBtn = new QPushButton("View Constraints", this);
    viewConstraintsBtn->setFont(QFont("Segoe UI", 9));
    connect(viewConstraintsBtn, &QPushButton::clicked, this, &TableManagementWidget::onViewConstraints);
    infoLayout->addWidget(viewConstraintsBtn);

    m_fieldInfoTable = new QTableWidget(0, 8, this);
    m_fieldInfoTable->setHorizontalHeaderLabels(QStringList() << "Field Name" << "Type" << "Size" << "KEY" << "NULL" << "VALID" << "UNIQUE" << "DEFAULT");
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
    if (m_dataManager) {
        m_dataManager->setDatabasePath(dbPath);
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
        // 扩展：显示约束信息
        m_fieldInfoTable->setItem(row, 6, new QTableWidgetItem(field.bUnique == FLAG_KEY ? "YES" : "NO"));
        QString defaultValue = (field.sDefaultValue[0] != '\0') ? QString::fromLocal8Bit(field.sDefaultValue) : "";
        m_fieldInfoTable->setItem(row, 7, new QTableWidgetItem(defaultValue));
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
        TableConstraints constraints;
        if (dialog.getTableInfo(tableInfo, constraints)) {
            if (m_tableManager->createTable(tableInfo)) {
                // Register constraints
                QFileInfo dbFileInfo(QString::fromLocal8Bit(m_databasePath.c_str()));
                QString dbName = dbFileInfo.baseName();
                if (dbName.isEmpty()) {
                    dbName = QString::fromLocal8Bit(m_databasePath.c_str());
                }
                ConstraintRegistry::getInstance().registerTableConstraints(
                    dbName.toStdString(),
                    QString::fromLocal8Bit(tableInfo.tableName).toStdString(),
                    constraints
                );
                
                // Save constraints to file
                ConstraintStorageManager::saveConstraints(dbName.toStdString(), m_databasePath);
                
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
    // 注意：必须在loadTableInfo之后设置，因为构造函数中会尝试加载约束
    QFileInfo dbFileInfo(QString::fromLocal8Bit(m_databasePath.c_str()));
    QString dbName = dbFileInfo.baseName();
    if (dbName.isEmpty()) {
        dbName = QString::fromLocal8Bit(m_databasePath.c_str());
    }
    dialog.m_databaseNameEdit->setText(dbName);
    dialog.m_databaseNameEdit->setEnabled(false);
    dialog.m_databaseNameEdit->setVisible(false);
    dialog.m_databaseNameLabel->setVisible(false);
    
    // 重新加载约束（因为databaseNameEdit现在已设置）
    TableConstraints constraints;
    if (ConstraintRegistry::getInstance().getTableConstraints(
            dbName.toStdString(),
            tableName.toStdString(),
            constraints)) {
        dialog.loadConstraints(constraints);
    }
    
    if (dialog.exec() == QDialog::Accepted) {
        TableInfo newTableInfo;
        TableConstraints constraints;
        if (dialog.getTableInfo(newTableInfo, constraints)) {
            if (m_tableManager->updateTable(tableName.toLocal8Bit().constData(), newTableInfo)) {
                // Register constraints
                QFileInfo dbFileInfo(QString::fromLocal8Bit(m_databasePath.c_str()));
                QString dbName = dbFileInfo.baseName();
                if (dbName.isEmpty()) {
                    dbName = QString::fromLocal8Bit(m_databasePath.c_str());
                }
                ConstraintRegistry::getInstance().registerTableConstraints(
                    dbName.toStdString(),
                    QString::fromLocal8Bit(newTableInfo.tableName).toStdString(),
                    constraints
                );
                
                // Save constraints to file
                ConstraintStorageManager::saveConstraints(dbName.toStdString(), m_databasePath);
                
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
        std::string tableNameStr = tableName.toLocal8Bit().constData();
        
        // 先删除表的数据（.dat文件中的记录）- 与SQL执行保持一致
        // 这是关键步骤：确保.dat文件中不再有任何该表的数据
        if (!m_dataManager->clearTable(tableNameStr)) {
            QMessageBox::warning(this, "Error", 
                QString("Failed to delete table data: %1").arg(tableName));
            return;
        }
        
        // 然后删除表结构（.dbf文件中的表定义）
        if (m_tableManager->deleteTable(tableNameStr)) {
            QMessageBox::information(this, "Success", "Table deleted successfully.");
            refreshTableList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete table structure.");
        }
    }
}

void TableManagementWidget::onRefresh()
{
    refreshTableList();
}

void TableManagementWidget::onViewConstraints()
{
    QListWidgetItem *item = m_tableList->currentItem();
    if (!item) {
        QMessageBox::information(this, "No Selection", "Please select a table first.");
        return;
    }
    
    QString tableName = item->text();
    QFileInfo dbFileInfo(QString::fromLocal8Bit(m_databasePath.c_str()));
    QString dbName = dbFileInfo.baseName();
    if (dbName.isEmpty()) {
        dbName = QString::fromLocal8Bit(m_databasePath.c_str());
    }
    
    TableConstraints constraints;
    if (!ConstraintRegistry::getInstance().getTableConstraints(
            dbName.toStdString(),
            tableName.toStdString(),
            constraints)) {
        QMessageBox::information(this, "No Constraints", 
            QString("Table '%1' has no constraints defined.").arg(tableName));
        return;
    }
    
    // Create a dialog to display constraints
    QDialog dialog(this);
    dialog.setWindowTitle(QString("Constraints for Table: %1").arg(tableName));
    dialog.setMinimumSize(600, 500);
    dialog.resize(700, 600);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // Foreign Key Constraints
    if (!constraints.foreignKeys.empty()) {
        QGroupBox *fkGroup = new QGroupBox("Foreign Key Constraints", &dialog);
        QVBoxLayout *fkLayout = new QVBoxLayout(fkGroup);
        QListWidget *fkList = new QListWidget(&dialog);
        fkList->setFont(QFont("Segoe UI", 9));
        fkList->setMinimumHeight(100);
        for (const auto& fk : constraints.foreignKeys) {
            QString text = QString("%1: %2 -> %3.%4 (ON DELETE %5, ON UPDATE %6)")
                .arg(fk.constraintName[0] != '\0' ? QString::fromLocal8Bit(fk.constraintName) : "Unnamed")
                .arg(QString::fromLocal8Bit(fk.fieldName))
                .arg(QString::fromLocal8Bit(fk.referencedTable))
                .arg(QString::fromLocal8Bit(fk.referencedField))
                .arg(QString::fromLocal8Bit(fk.onDeleteAction))
                .arg(QString::fromLocal8Bit(fk.onUpdateAction));
            fkList->addItem(text);
        }
        fkLayout->addWidget(fkList);
        layout->addWidget(fkGroup);
    }
    
    // Unique Constraints
    if (!constraints.uniqueConstraints.empty()) {
        QGroupBox *uniqueGroup = new QGroupBox("Unique Constraints", &dialog);
        QVBoxLayout *uniqueLayout = new QVBoxLayout(uniqueGroup);
        QListWidget *uniqueList = new QListWidget(&dialog);
        uniqueList->setFont(QFont("Segoe UI", 9));
        uniqueList->setMinimumHeight(100);
        for (const auto& unique : constraints.uniqueConstraints) {
            QString fields = QString::fromStdString(
                std::accumulate(unique.fieldNames.begin(), unique.fieldNames.end(), std::string(),
                    [](const std::string& a, const std::string& b) {
                        return a.empty() ? b : a + ", " + b;
                    }));
            QString text = QString("%1: (%2)")
                .arg(unique.constraintName[0] != '\0' ? QString::fromLocal8Bit(unique.constraintName) : "Unnamed")
                .arg(fields);
            uniqueList->addItem(text);
        }
        uniqueLayout->addWidget(uniqueList);
        layout->addWidget(uniqueGroup);
    }
    
    // Check Constraints
    if (!constraints.checkConstraints.empty()) {
        QGroupBox *checkGroup = new QGroupBox("Check Constraints", &dialog);
        QVBoxLayout *checkLayout = new QVBoxLayout(checkGroup);
        QListWidget *checkList = new QListWidget(&dialog);
        checkList->setFont(QFont("Segoe UI", 9));
        checkList->setMinimumHeight(100);
        for (const auto& check : constraints.checkConstraints) {
            QString text = QString("%1: %2 %3")
                .arg(check.constraintName[0] != '\0' ? QString::fromLocal8Bit(check.constraintName) : "Unnamed")
                .arg(QString::fromLocal8Bit(check.fieldName))
                .arg(QString::fromStdString(check.expression));
            checkList->addItem(text);
        }
        checkLayout->addWidget(checkList);
        layout->addWidget(checkGroup);
    }
    
    if (constraints.foreignKeys.empty() && 
        constraints.uniqueConstraints.empty() && 
        constraints.checkConstraints.empty()) {
        QLabel *noConstraintsLabel = new QLabel("No constraints defined for this table.", &dialog);
        noConstraintsLabel->setFont(QFont("Segoe UI", 9));
        noConstraintsLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(noConstraintsLabel);
    }
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttonBox);
    
    dialog.exec();
}

// Restore warning settings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

