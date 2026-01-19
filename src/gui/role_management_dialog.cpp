/**
 * @file role_management_dialog.cpp
 * @brief Role Management Dialog Implementation
 */

#include "gui/role_management_dialog.h"
#include "core/role_manager.h"
#include "core/permission_manager.h"
#include "core/user_mode.h"
#include <QFont>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <set>
#include <map>
#include <vector>

RoleManagementDialog::RoleManagementDialog(const std::string& userName,
                                           const std::string& databasePath,
                                           QWidget *parent)
    : QDialog(parent)
    , m_titleLabel(nullptr)
    , m_userLabel(nullptr)
    , m_availableRolesList(nullptr)
    , m_userRolesList(nullptr)
    , m_grantBtn(nullptr)
    , m_revokeBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_closeBtn(nullptr)
    , m_mainLayout(nullptr)
    , m_listsLayout(nullptr)
    , m_buttonsLayout(nullptr)
    , m_actionButtonsLayout(nullptr)
    , m_userName(userName)
    , m_databasePath(databasePath)
{
    setupUI();
    setWindowTitle("Manage Roles");
    resize(600, 500);
    loadRoleList();
    loadUserRoles();
}

RoleManagementDialog::~RoleManagementDialog()
{
}

void RoleManagementDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    m_titleLabel = new QLabel("Role Management", this);
    m_titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // User label
    m_userLabel = new QLabel(QString("User: %1").arg(QString::fromStdString(m_userName)), this);
    m_userLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    m_mainLayout->addWidget(m_userLabel);

    // Lists layout
    m_listsLayout = new QHBoxLayout();
    m_listsLayout->setSpacing(15);

    // Available roles
    QGroupBox *availableGroup = new QGroupBox("Available Roles", this);
    availableGroup->setFont(QFont("Segoe UI", 9));
    QVBoxLayout *availableLayout = new QVBoxLayout(availableGroup);
    
    m_availableRolesList = new QListWidget(availableGroup);
    m_availableRolesList->setFont(QFont("Segoe UI", 9));
    availableLayout->addWidget(m_availableRolesList);
    
    m_listsLayout->addWidget(availableGroup, 1);

    // User roles
    QGroupBox *userRolesGroup = new QGroupBox("User Roles", this);
    userRolesGroup->setFont(QFont("Segoe UI", 9));
    QVBoxLayout *userRolesLayout = new QVBoxLayout(userRolesGroup);
    
    m_userRolesList = new QListWidget(userRolesGroup);
    m_userRolesList->setFont(QFont("Segoe UI", 9));
    userRolesLayout->addWidget(m_userRolesList);
    
    m_listsLayout->addWidget(userRolesGroup, 1);

    m_mainLayout->addLayout(m_listsLayout, 1);

    // Action buttons
    m_actionButtonsLayout = new QHBoxLayout();
    
    m_grantBtn = new QPushButton("Grant Role →", this);
    m_grantBtn->setFont(QFont("Segoe UI", 9));
    connect(m_grantBtn, &QPushButton::clicked, this, &RoleManagementDialog::onGrantRole);
    m_actionButtonsLayout->addWidget(m_grantBtn);
    
    m_revokeBtn = new QPushButton("← Revoke Role", this);
    m_revokeBtn->setFont(QFont("Segoe UI", 9));
    connect(m_revokeBtn, &QPushButton::clicked, this, &RoleManagementDialog::onRevokeRole);
    m_actionButtonsLayout->addWidget(m_revokeBtn);
    
    m_viewPermissionsBtn = new QPushButton("View Permissions", this);
    m_viewPermissionsBtn->setFont(QFont("Segoe UI", 9));
    connect(m_viewPermissionsBtn, &QPushButton::clicked, this, &RoleManagementDialog::onViewPermissions);
    m_actionButtonsLayout->addWidget(m_viewPermissionsBtn);
    
    m_actionButtonsLayout->addStretch();
    
    m_mainLayout->addLayout(m_actionButtonsLayout);

    // Bottom buttons
    m_buttonsLayout = new QHBoxLayout();
    m_buttonsLayout->addStretch();
    
    m_refreshBtn = new QPushButton("Refresh", this);
    m_refreshBtn->setFont(QFont("Segoe UI", 9));
    connect(m_refreshBtn, &QPushButton::clicked, this, &RoleManagementDialog::onRefresh);
    m_buttonsLayout->addWidget(m_refreshBtn);
    
    m_closeBtn = new QPushButton("Close", this);
    m_closeBtn->setFont(QFont("Segoe UI", 9));
    m_closeBtn->setDefault(true);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    m_buttonsLayout->addWidget(m_closeBtn);
    
    m_mainLayout->addLayout(m_buttonsLayout);
}

void RoleManagementDialog::loadRoleList()
{
    m_availableRolesList->clear();
    
    RoleManager roleManager;
    roleManager.setDatabasePath(m_databasePath);
    
    std::vector<std::string> allRoles;
    if (roleManager.getAllRoleNames(allRoles)) {
        // Get user's current roles
        std::vector<std::string> userRoles;
        roleManager.getUserRoles(m_userName, userRoles);
        
        std::set<std::string> userRolesSet(userRoles.begin(), userRoles.end());
        
        // Add only roles that user doesn't have
        for (const auto& role : allRoles) {
            if (userRolesSet.find(role) == userRolesSet.end()) {
                m_availableRolesList->addItem(QString::fromStdString(role));
            }
        }
    }
}

void RoleManagementDialog::loadUserRoles()
{
    m_userRolesList->clear();
    
    RoleManager roleManager;
    roleManager.setDatabasePath(m_databasePath);
    
    std::vector<std::string> userRoles;
    if (roleManager.getUserRoles(m_userName, userRoles)) {
        for (const auto& role : userRoles) {
            m_userRolesList->addItem(QString::fromStdString(role));
        }
    }
}

void RoleManagementDialog::onGrantRole()
{
    QListWidgetItem *item = m_availableRolesList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a role to grant.");
        return;
    }

    QString roleName = item->text();
    
    RoleManager roleManager;
    roleManager.setDatabasePath(m_databasePath);
    
    if (roleManager.grantRoleToUser(roleName.toStdString(), m_userName)) {
        QMessageBox::information(this, "Success", QString("Role '%1' granted successfully.").arg(roleName));
        loadRoleList();
        loadUserRoles();
    } else {
        QMessageBox::critical(this, "Error", "Failed to grant role.");
    }
}

void RoleManagementDialog::onRevokeRole()
{
    QListWidgetItem *item = m_userRolesList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a role to revoke.");
        return;
    }

    QString roleName = item->text();
    
    RoleManager roleManager;
    roleManager.setDatabasePath(m_databasePath);
    
    if (roleManager.revokeRoleFromUser(roleName.toStdString(), m_userName)) {
        QMessageBox::information(this, "Success", QString("Role '%1' revoked successfully.").arg(roleName));
        loadRoleList();
        loadUserRoles();
    } else {
        QMessageBox::critical(this, "Error", "Failed to revoke role.");
    }
}

void RoleManagementDialog::onRefresh()
{
    loadRoleList();
    loadUserRoles();
}

void RoleManagementDialog::onViewPermissions()
{
    // Get selected role from user roles list
    QListWidgetItem *item = m_userRolesList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a role to view permissions.");
        return;
    }
    
    QString roleName = item->text();
    
    // Get role permissions
    PermissionManager permManager;
    permManager.setDatabasePath(m_databasePath);
    // setDatabasePath会自动调用loadPermissions()，确保权限数据已加载
    
    std::vector<PermissionInfo> permissions;
    if (!permManager.getRolePermissions(roleName.toStdString(), permissions)) {
        QMessageBox::warning(this, "Error", "Failed to load role permissions.");
        return;
    }
    
    // 如果权限列表为空，显示提示信息
    if (permissions.empty()) {
        QMessageBox::information(this, "No Permissions", 
            QString("Role '%1' has no permissions assigned.").arg(roleName));
        return;
    }
    
    // Create dialog to display permissions
    QDialog permDialog(this);
    permDialog.setWindowTitle(QString("Permissions for Role: %1").arg(roleName));
    permDialog.resize(700, 500);
    
    QVBoxLayout *layout = new QVBoxLayout(&permDialog);
    
    // Title
    QLabel *titleLabel = new QLabel(QString("Permissions for Role: <b>%1</b>").arg(roleName), &permDialog);
    titleLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    layout->addWidget(titleLabel);
    
    // Permissions table
    QTableWidget *permTable = new QTableWidget(&permDialog);
    permTable->setFont(QFont("Segoe UI", 9));
    permTable->setColumnCount(4);
    permTable->setHorizontalHeaderLabels(QStringList() << "Object Type" << "Object Name" << "Permissions" << "Grant Option");
    permTable->horizontalHeader()->setStretchLastSection(true);
    permTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    permTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Group permissions by object
    std::map<std::pair<char, std::string>, std::vector<PermissionInfo>> grouped;
    for (const auto& perm : permissions) {
        std::pair<char, std::string> key(perm.objectType, perm.objectName);
        grouped[key].push_back(perm);
    }
    
    // Display grouped permissions
    permTable->setRowCount(grouped.size());
    int row = 0;
    for (const auto& pair : grouped) {
        // Object Type
        QString objTypeStr;
        if (pair.first.first == OBJECT_TYPE_TABLE) {
            objTypeStr = "Table";
        } else if (pair.first.first == OBJECT_TYPE_DATABASE) {
            objTypeStr = "Database";
        } else {
            objTypeStr = "Unknown";
        }
        permTable->setItem(row, 0, new QTableWidgetItem(objTypeStr));
        
        // Object Name
        permTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(pair.first.second)));
        
        // Permissions (combine all permissions)
        QStringList permStrs;
        std::set<std::string> permSet;
        bool hasGrantOption = false;
        for (const auto& perm : pair.second) {
            permSet.insert(perm.permissionType);
            if (perm.withGrantOption) {
                hasGrantOption = true;
            }
        }
        for (const auto& permStr : permSet) {
            permStrs << QString::fromStdString(permStr);
        }
        permTable->setItem(row, 2, new QTableWidgetItem(permStrs.join(", ")));
        
        // Grant Option
        permTable->setItem(row, 3, new QTableWidgetItem(hasGrantOption ? "Yes" : "No"));
        
        row++;
    }
    
    layout->addWidget(permTable);
    
    // Close button
    QPushButton *closeBtn = new QPushButton("Close", &permDialog);
    closeBtn->setFont(QFont("Segoe UI", 9));
    closeBtn->setDefault(true);
    connect(closeBtn, &QPushButton::clicked, &permDialog, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);
    
    permDialog.exec();
}

