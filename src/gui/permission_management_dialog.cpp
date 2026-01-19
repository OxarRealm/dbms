/**
 * @file permission_management_dialog.cpp
 * @brief Permission Management Dialog Implementation
 */

#include "gui/permission_management_dialog.h"
#include "core/permission_manager.h"
#include "core/role_manager.h"
#include "core/table_manager.h"
#include <QFont>
#include <QHeaderView>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QTimer>
#include <set>

PermissionManagementDialog::PermissionManagementDialog(const std::string& userName,
                                                       const std::string& databasePath,
                                                       QWidget *parent)
    : QDialog(parent)
    , m_titleLabel(nullptr)
    , m_userLabel(nullptr)
    , m_permissionTable(nullptr)
    , m_grantBtn(nullptr)
    , m_revokeBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_closeBtn(nullptr)
    , m_mainLayout(nullptr)
    , m_buttonsLayout(nullptr)
    , m_actionButtonsLayout(nullptr)
    , m_objectTypeCombo(nullptr)
    , m_objectNameCombo(nullptr)
    , m_permissionList(nullptr)
    , m_grantOptionCheck(nullptr)
    , m_userName(userName)
    , m_databasePath(databasePath)
{
    setupUI();
    setWindowTitle("Manage Permissions");
    resize(800, 600);
    loadPermissions();
}

PermissionManagementDialog::~PermissionManagementDialog()
{
}

void PermissionManagementDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    m_titleLabel = new QLabel("Permission Management", this);
    m_titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // User label
    m_userLabel = new QLabel(QString("User: %1").arg(QString::fromStdString(m_userName)), this);
    m_userLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    m_mainLayout->addWidget(m_userLabel);

    // Permission table
    m_permissionTable = new QTableWidget(0, 4, this);
    m_permissionTable->setFont(QFont("Segoe UI", 9));
    m_permissionTable->setHorizontalHeaderLabels(QStringList() << "Object Type" << "Object Name" << "Permissions" << "Grant Option");
    m_permissionTable->horizontalHeader()->setStretchLastSection(true);
    m_permissionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_permissionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_permissionTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_mainLayout->addWidget(m_permissionTable, 1);

    // Action buttons
    m_actionButtonsLayout = new QHBoxLayout();
    
    m_grantBtn = new QPushButton("Grant Permission", this);
    m_grantBtn->setFont(QFont("Segoe UI", 9));
    connect(m_grantBtn, &QPushButton::clicked, this, &PermissionManagementDialog::onGrantPermission);
    m_actionButtonsLayout->addWidget(m_grantBtn);
    
    m_revokeBtn = new QPushButton("Revoke Permission", this);
    m_revokeBtn->setFont(QFont("Segoe UI", 9));
    connect(m_revokeBtn, &QPushButton::clicked, this, &PermissionManagementDialog::onRevokePermission);
    m_actionButtonsLayout->addWidget(m_revokeBtn);
    
    m_actionButtonsLayout->addStretch();
    
    m_mainLayout->addLayout(m_actionButtonsLayout);

    // Bottom buttons
    m_buttonsLayout = new QHBoxLayout();
    m_buttonsLayout->addStretch();
    
    m_refreshBtn = new QPushButton("Refresh", this);
    m_refreshBtn->setFont(QFont("Segoe UI", 9));
    connect(m_refreshBtn, &QPushButton::clicked, this, &PermissionManagementDialog::onRefresh);
    m_buttonsLayout->addWidget(m_refreshBtn);
    
    m_closeBtn = new QPushButton("Close", this);
    m_closeBtn->setFont(QFont("Segoe UI", 9));
    m_closeBtn->setDefault(true);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    m_buttonsLayout->addWidget(m_closeBtn);
    
    m_mainLayout->addLayout(m_buttonsLayout);
}

void PermissionManagementDialog::loadPermissions()
{
    m_permissionTable->setRowCount(0);
    
    PermissionManager permManager;
    permManager.setDatabasePath(m_databasePath);
    
    RoleManager roleManager;
    roleManager.setDatabasePath(m_databasePath);
    
    std::vector<std::string> userRoles;
    roleManager.getUserRoles(m_userName, userRoles);
    
    // Get all permissions for user (including role permissions)
    std::vector<PermissionInfo> permissions;
    if (permManager.getUserPermissions(m_userName, userRoles, permissions)) {
        // Group permissions by object
        std::map<std::pair<char, std::string>, std::vector<PermissionInfo>> grouped;
        for (const auto& perm : permissions) {
            std::pair<char, std::string> key(perm.objectType, perm.objectName);
            grouped[key].push_back(perm);
        }
        
        // Display grouped permissions
        for (const auto& pair : grouped) {
            int row = m_permissionTable->rowCount();
            m_permissionTable->insertRow(row);
            
            // Object Type
            QString objTypeStr = getObjectTypeString(pair.first.first);
            m_permissionTable->setItem(row, 0, new QTableWidgetItem(objTypeStr));
            
            // Object Name
            m_permissionTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(pair.first.second)));
            
            // Permissions (combine all permissions)
            QStringList permStrs;
            std::set<std::string> permSet;
            bool hasGrantOption = false;
            for (const auto& perm : pair.second) {
                // perm.permissionType is already a string
                permSet.insert(perm.permissionType);
                if (perm.withGrantOption) {
                    hasGrantOption = true;
                }
            }
            for (const auto& permStr : permSet) {
                permStrs << QString::fromStdString(permStr);
            }
            m_permissionTable->setItem(row, 2, new QTableWidgetItem(permStrs.join(", ")));
            
            // Grant Option
            m_permissionTable->setItem(row, 3, new QTableWidgetItem(hasGrantOption ? "Yes" : "No"));
        }
    }
}

void PermissionManagementDialog::onGrantPermission()
{
    // Create grant dialog
    QDialog grantDialog(this);
    grantDialog.setWindowTitle("Grant Permission");
    grantDialog.resize(500, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(&grantDialog);
    
    // Object Type
    QHBoxLayout *objTypeLayout = new QHBoxLayout();
    QLabel *objTypeLabel = new QLabel("Object Type:", &grantDialog);
    objTypeLabel->setFont(QFont("Segoe UI", 9));
    m_objectTypeCombo = new QComboBox(&grantDialog);
    m_objectTypeCombo->setFont(QFont("Segoe UI", 9));
    m_objectTypeCombo->addItem("Table", 1);
    m_objectTypeCombo->addItem("Database", 2);
    objTypeLayout->addWidget(objTypeLabel);
    objTypeLayout->addWidget(m_objectTypeCombo);
    layout->addLayout(objTypeLayout);
    
    // Object Name
    QHBoxLayout *objNameLayout = new QHBoxLayout();
    QLabel *objNameLabel = new QLabel("Object Name:", &grantDialog);
    objNameLabel->setFont(QFont("Segoe UI", 9));
    m_objectNameCombo = new QComboBox(&grantDialog);
    m_objectNameCombo->setFont(QFont("Segoe UI", 9));
    m_objectNameCombo->setEditable(true);
    objNameLayout->addWidget(objNameLabel);
    objNameLayout->addWidget(m_objectNameCombo);
    layout->addLayout(objNameLayout);
    
    // Load table names function
    auto loadObjectNames = [this]() {
        m_objectNameCombo->clear();
        int objectType = m_objectTypeCombo->currentData().toInt();
        if (objectType == 1) { // Table
            TableManager tableManager;
            tableManager.setDatabasePath(m_databasePath);
            std::vector<TableInfo> tables;
            if (tableManager.readAllTables(tables)) {
                for (const auto& table : tables) {
                    m_objectNameCombo->addItem(QString::fromStdString(table.tableName));
                }
            }
        } else if (objectType == 2) { // Database
            m_objectNameCombo->addItem(""); // Database doesn't need object name
        }
    };
    
    // Connect signal for when object type changes
    connect(m_objectTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), loadObjectNames);
    
    // Set initial index to Table (index 0)
    m_objectTypeCombo->setCurrentIndex(0);
    // Load table names immediately - use QTimer to ensure combo box is fully initialized
    QTimer::singleShot(0, [this, loadObjectNames]() {
        loadObjectNames();
    });
    
    // Permissions
    QLabel *permLabel = new QLabel("Permissions:", &grantDialog);
    permLabel->setFont(QFont("Segoe UI", 9));
    layout->addWidget(permLabel);
    
    m_permissionList = new QListWidget(&grantDialog);
    m_permissionList->setFont(QFont("Segoe UI", 9));
    m_permissionList->setSelectionMode(QAbstractItemView::MultiSelection);
    m_permissionList->addItem("SELECT");
    m_permissionList->addItem("INSERT");
    m_permissionList->addItem("UPDATE");
    m_permissionList->addItem("DELETE");
    m_permissionList->addItem("ALTER");
    m_permissionList->addItem("DROP");
    m_permissionList->addItem("CREATE_TABLE");
    m_permissionList->addItem("CREATE_INDEX");
    m_permissionList->addItem("ALL_PRIVILEGES");
    layout->addWidget(m_permissionList, 1);
    
    // Grant Option
    m_grantOptionCheck = new QCheckBox("WITH GRANT OPTION", &grantDialog);
    m_grantOptionCheck->setFont(QFont("Segoe UI", 9));
    layout->addWidget(m_grantOptionCheck);
    
    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &grantDialog);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Grant");
    buttonBox->button(QDialogButtonBox::Ok)->setFont(QFont("Segoe UI", 9));
    buttonBox->button(QDialogButtonBox::Cancel)->setFont(QFont("Segoe UI", 9));
    connect(buttonBox, &QDialogButtonBox::accepted, &grantDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &grantDialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (grantDialog.exec() == QDialog::Accepted) {
        char objectType = static_cast<char>(m_objectTypeCombo->currentData().toInt());
        std::string objectName = m_objectNameCombo->currentText().toStdString();
        
        // Get selected permissions
        std::vector<PermissionType> permissionTypes;
        for (int i = 0; i < m_permissionList->count(); ++i) {
            if (m_permissionList->item(i)->isSelected()) {
                QString permStr = m_permissionList->item(i)->text();
                PermissionType permType = PermissionManager::stringToPermissionType(permStr.toStdString());
                permissionTypes.push_back(permType);
            }
        }
        
        if (permissionTypes.empty()) {
            QMessageBox::warning(this, "No Selection", "Please select at least one permission.");
            return;
        }
        
        PermissionManager permManager;
        permManager.setDatabasePath(m_databasePath);
        
        bool withGrantOption = m_grantOptionCheck->isChecked();
        
        if (permManager.grantPermission(m_userName, objectType, objectName, permissionTypes, 
                                       withGrantOption, "SYSTEM")) {
            QMessageBox::information(this, "Success", "Permissions granted successfully.");
            loadPermissions();
        } else {
            QMessageBox::critical(this, "Error", "Failed to grant permissions.");
        }
    }
}

void PermissionManagementDialog::onRevokePermission()
{
    QTableWidgetItem *item = m_permissionTable->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a permission to revoke.");
        return;
    }
    
    int row = item->row();
    QString objTypeStr = m_permissionTable->item(row, 0)->text();
    QString objName = m_permissionTable->item(row, 1)->text();
    QString permsStr = m_permissionTable->item(row, 2)->text();
    
    char objectType = getObjectTypeFromString(objTypeStr);
    std::string objectName = objName.toStdString();
    
    // Parse permissions from string
    QStringList permStrs = permsStr.split(", ");
    std::vector<PermissionType> permissionTypes;
    for (const QString& permStr : permStrs) {
        PermissionType permType = PermissionManager::stringToPermissionType(permStr.toStdString());
        permissionTypes.push_back(permType);
    }
    
    if (permissionTypes.empty()) {
        QMessageBox::warning(this, "Error", "No permissions to revoke.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Revoke", 
                                    QString("Are you sure you want to revoke permissions on %1 '%2'?").arg(objTypeStr).arg(objName),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        PermissionManager permManager;
        permManager.setDatabasePath(m_databasePath);
        
        if (permManager.revokePermission(m_userName, objectType, objectName, permissionTypes)) {
            QMessageBox::information(this, "Success", "Permissions revoked successfully.");
            loadPermissions();
        } else {
            QMessageBox::critical(this, "Error", "Failed to revoke permissions.");
        }
    }
}

void PermissionManagementDialog::onRefresh()
{
    loadPermissions();
}

char PermissionManagementDialog::getObjectTypeFromString(const QString& str)
{
    if (str == "Table") {
        return OBJECT_TYPE_TABLE;
    } else if (str == "Database") {
        return OBJECT_TYPE_DATABASE;
    }
    return OBJECT_TYPE_TABLE;
}

QString PermissionManagementDialog::getObjectTypeString(char objectType)
{
    if (objectType == OBJECT_TYPE_TABLE) {
        return "Table";
    } else if (objectType == OBJECT_TYPE_DATABASE) {
        return "Database";
    }
    return "Unknown";
}

