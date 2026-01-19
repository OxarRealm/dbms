/**
 * @file user_management_widget.cpp
 * @brief User Management Widget Implementation
 */

#include "gui/user_management_widget.h"
#include "gui/role_management_dialog.h"
#include "gui/permission_management_dialog.h"
#include "core/user_manager.h"
#include "core/role_manager.h"
#include "core/permission_manager.h"
#include "core/user_mode.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QFont>
#include <map>
#include <set>
#include <iostream>

UserManagementWidget::UserManagementWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_leftLayout(nullptr)
    , m_rightLayout(nullptr)
    , m_userListGroup(nullptr)
    , m_userList(nullptr)
    , m_createUserBtn(nullptr)
    , m_deleteUserBtn(nullptr)
    , m_enableUserBtn(nullptr)
    , m_disableUserBtn(nullptr)
    , m_changePasswordBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_userInfoGroup(nullptr)
    , m_userNameLabel(nullptr)
    , m_userStatusLabel(nullptr)
    , m_roleTable(nullptr)
    , m_permissionTable(nullptr)
    , m_manageRolesBtn(nullptr)
    , m_managePermissionsBtn(nullptr)
    , m_noUserSelectedLabel(nullptr)
{
    setupUI();
}

UserManagementWidget::~UserManagementWidget()
{
}

void UserManagementWidget::setupUI()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    // Left panel: User list
    m_leftLayout = new QVBoxLayout();
    m_userListGroup = new QGroupBox("Users", this);
    QVBoxLayout *listLayout = new QVBoxLayout(m_userListGroup);

    m_userList = new QListWidget(this);
    m_userList->setFont(QFont("Segoe UI", 9));
    connect(m_userList, &QListWidget::itemSelectionChanged, this, &UserManagementWidget::onUserSelectionChanged);
    listLayout->addWidget(m_userList);

    // Buttons
    QVBoxLayout *btnLayout = new QVBoxLayout();
    QHBoxLayout *btnLayout1 = new QHBoxLayout();
    QHBoxLayout *btnLayout2 = new QHBoxLayout();
    
    m_createUserBtn = new QPushButton("Create User", this);
    m_deleteUserBtn = new QPushButton("Delete User", this);
    m_enableUserBtn = new QPushButton("Enable User", this);
    m_disableUserBtn = new QPushButton("Disable User", this);
    m_changePasswordBtn = new QPushButton("Change Password", this);
    m_refreshBtn = new QPushButton("Refresh", this);

    m_createUserBtn->setFont(QFont("Segoe UI", 9));
    m_deleteUserBtn->setFont(QFont("Segoe UI", 9));
    m_enableUserBtn->setFont(QFont("Segoe UI", 9));
    m_disableUserBtn->setFont(QFont("Segoe UI", 9));
    m_changePasswordBtn->setFont(QFont("Segoe UI", 9));
    m_refreshBtn->setFont(QFont("Segoe UI", 9));

    connect(m_createUserBtn, &QPushButton::clicked, this, &UserManagementWidget::onCreateUser);
    connect(m_deleteUserBtn, &QPushButton::clicked, this, &UserManagementWidget::onDeleteUser);
    connect(m_enableUserBtn, &QPushButton::clicked, this, &UserManagementWidget::onEnableUser);
    connect(m_disableUserBtn, &QPushButton::clicked, this, &UserManagementWidget::onDisableUser);
    connect(m_changePasswordBtn, &QPushButton::clicked, this, &UserManagementWidget::onChangePassword);
    connect(m_refreshBtn, &QPushButton::clicked, this, &UserManagementWidget::onRefresh);

    btnLayout1->addWidget(m_createUserBtn);
    btnLayout1->addWidget(m_deleteUserBtn);
    btnLayout2->addWidget(m_enableUserBtn);
    btnLayout2->addWidget(m_disableUserBtn);
    btnLayout->addLayout(btnLayout1);
    btnLayout->addLayout(btnLayout2);
    btnLayout->addWidget(m_changePasswordBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_refreshBtn);
    listLayout->addLayout(btnLayout);

    m_leftLayout->addWidget(m_userListGroup);
    m_mainLayout->addLayout(m_leftLayout, 1);

    // Right panel: User info
    m_rightLayout = new QVBoxLayout();
    m_userInfoGroup = new QGroupBox("User Information", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(m_userInfoGroup);

    m_userNameLabel = new QLabel("No user selected", this);
    m_userNameLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    infoLayout->addWidget(m_userNameLabel);

    m_userStatusLabel = new QLabel("", this);
    m_userStatusLabel->setFont(QFont("Segoe UI", 9));
    infoLayout->addWidget(m_userStatusLabel);

    // Roles table
    QLabel *roleLabel = new QLabel("Roles:", this);
    roleLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    infoLayout->addWidget(roleLabel);

    m_roleTable = new QTableWidget(0, 1, this);
    m_roleTable->setFont(QFont("Segoe UI", 9));
    m_roleTable->setHorizontalHeaderLabels(QStringList() << "Role");
    m_roleTable->horizontalHeader()->setStretchLastSection(true);
    m_roleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_roleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    infoLayout->addWidget(m_roleTable);

    m_manageRolesBtn = new QPushButton("Manage Roles", this);
    m_manageRolesBtn->setFont(QFont("Segoe UI", 9));
    connect(m_manageRolesBtn, &QPushButton::clicked, this, &UserManagementWidget::onManageRoles);
    infoLayout->addWidget(m_manageRolesBtn);

    // Permissions table
    QLabel *permLabel = new QLabel("Permissions:", this);
    permLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    infoLayout->addWidget(permLabel);

    m_permissionTable = new QTableWidget(0, 3, this);
    m_permissionTable->setFont(QFont("Segoe UI", 9));
    m_permissionTable->setHorizontalHeaderLabels(QStringList() << "Object Type" << "Object Name" << "Permission");
    m_permissionTable->horizontalHeader()->setStretchLastSection(true);
    m_permissionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_permissionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    infoLayout->addWidget(m_permissionTable);

    m_managePermissionsBtn = new QPushButton("Manage Permissions", this);
    m_managePermissionsBtn->setFont(QFont("Segoe UI", 9));
    connect(m_managePermissionsBtn, &QPushButton::clicked, this, &UserManagementWidget::onManagePermissions);
    infoLayout->addWidget(m_managePermissionsBtn);

    m_noUserSelectedLabel = new QLabel("Please select a user to view details.", this);
    m_noUserSelectedLabel->setFont(QFont("Segoe UI", 9));
    m_noUserSelectedLabel->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget(m_noUserSelectedLabel);

    m_rightLayout->addWidget(m_userInfoGroup);
    m_mainLayout->addLayout(m_rightLayout, 2);

    // Initial state
    clearUserInfo();
}

void UserManagementWidget::setDatabasePath(const std::string& dbPath)
{
    m_databasePath = dbPath;
    loadUserList();
}

void UserManagementWidget::loadUserList()
{
    m_userList->clear();
    
    if (m_databasePath.empty()) {
        return;
    }

    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    
    // Get all users
    std::vector<std::string> userNames;
    if (userManager.getAllUserNames(userNames)) {
        for (const auto& userName : userNames) {
            m_userList->addItem(QString::fromStdString(userName));
        }
    }
}

void UserManagementWidget::onCreateUser()
{
    if (m_databasePath.empty()) {
        QMessageBox::warning(this, "No Database", "Please open a database first.");
        return;
    }

    bool ok;
    QString username = QInputDialog::getText(this, "Create User", "Username:",
                                              QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) {
        return;
    }

    QString password = QInputDialog::getText(this, "Create User", "Password:",
                                             QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) {
        return;
    }

    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    
    if (userManager.createUser(username.toStdString(), password.toStdString())) {
        QMessageBox::information(this, "Success", "User created successfully.");
        loadUserList();
    } else {
        QMessageBox::critical(this, "Error", "Failed to create user. User may already exist.");
    }
}

void UserManagementWidget::onDeleteUser()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a user to delete.");
        return;
    }

    QString username = item->text();
    int ret = QMessageBox::question(this, "Confirm Delete", 
                                     QString("Are you sure you want to delete user '%1'?").arg(username),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        UserManager userManager;
        userManager.setDatabasePath(m_databasePath);
        
        if (userManager.deleteUser(username.toStdString())) {
            QMessageBox::information(this, "Success", "User deleted successfully.");
            loadUserList();
            clearUserInfo();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete user.");
        }
    }
}

void UserManagementWidget::onUserSelectionChanged()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        clearUserInfo();
        return;
    }

    QString username = item->text();
    displayUserInfo(username.toStdString());
}

void UserManagementWidget::onRefresh()
{
    loadUserList();
}

void UserManagementWidget::onManageRoles()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a user to manage roles.");
        return;
    }
    
    QString username = item->text();
    
    if (m_databasePath.empty()) {
        QMessageBox::warning(this, "No Database", "Please open a database first.");
        return;
    }
    
    RoleManagementDialog dialog(username.toStdString(), m_databasePath, this);
    dialog.exec();
    
    // Refresh user info after dialog closes
    displayUserInfo(username.toStdString());
}

void UserManagementWidget::onManagePermissions()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a user to manage permissions.");
        return;
    }
    
    QString username = item->text();
    
    if (m_databasePath.empty()) {
        QMessageBox::warning(this, "No Database", "Please open a database first.");
        return;
    }
    
    PermissionManagementDialog dialog(username.toStdString(), m_databasePath, this);
    dialog.exec();
    
    // Refresh user info after dialog closes
    displayUserInfo(username.toStdString());
}

void UserManagementWidget::onEnableUser()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a user to enable.");
        return;
    }
    
    QString username = item->text();
    
    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    
    if (userManager.enableUser(username.toStdString())) {
        QMessageBox::information(this, "Success", QString("User '%1' enabled successfully.").arg(username));
        displayUserInfo(username.toStdString());
    } else {
        QMessageBox::critical(this, "Error", "Failed to enable user.");
    }
}

void UserManagementWidget::onDisableUser()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a user to disable.");
        return;
    }
    
    QString username = item->text();
    
    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    
    if (userManager.disableUser(username.toStdString())) {
        QMessageBox::information(this, "Success", QString("User '%1' disabled successfully.").arg(username));
        displayUserInfo(username.toStdString());
    } else {
        QMessageBox::critical(this, "Error", "Failed to disable user.");
    }
}

void UserManagementWidget::onChangePassword()
{
    QListWidgetItem *item = m_userList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a user to change password.");
        return;
    }
    
    QString username = item->text();
    
    bool ok;
    QString newPassword = QInputDialog::getText(this, "Change Password", 
                                                QString("Enter new password for user '%1':").arg(username),
                                                QLineEdit::Password, "", &ok);
    if (!ok || newPassword.isEmpty()) {
        return;
    }
    
    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    
    if (userManager.changePassword(username.toStdString(), newPassword.toStdString())) {
        QMessageBox::information(this, "Success", QString("Password changed successfully for user '%1'.").arg(username));
    } else {
        QMessageBox::critical(this, "Error", "Failed to change password.");
    }
}

void UserManagementWidget::displayUserInfo(const std::string& username)
{
    // Hide "no user selected" label
    m_noUserSelectedLabel->setVisible(false);

    // Show user name
    m_userNameLabel->setText(QString("User: %1").arg(QString::fromStdString(username)));

    // Load user info from UserManager
    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    
    UserInfo userInfo;
    if (userManager.getUserInfo(username, userInfo)) {
        QString status = (userInfo.status == USER_STATUS_ENABLED) ? "Enabled" : "Disabled";
        m_userStatusLabel->setText(QString("Status: %1").arg(status));
    } else {
        m_userStatusLabel->setText("Status: Unknown");
    }

    // Load roles
    RoleManager roleManager;
    roleManager.setDatabasePath(m_databasePath);
    
    std::vector<std::string> roles;
    roleManager.getUserRoles(username, roles);
    
    m_roleTable->setRowCount(roles.size());
    for (size_t i = 0; i < roles.size(); ++i) {
        m_roleTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(roles[i])));
    }

    // Load permissions
    PermissionManager permManager;
    permManager.setDatabasePath(m_databasePath);
    
    std::vector<PermissionInfo> permissions;
    if (permManager.getUserPermissions(username, roles, permissions)) {
        // Group permissions by object
        std::map<std::pair<char, std::string>, std::vector<PermissionInfo>> grouped;
        for (const auto& perm : permissions) {
            std::pair<char, std::string> key(perm.objectType, perm.objectName);
            grouped[key].push_back(perm);
        }
        
        // Display grouped permissions
        m_permissionTable->setRowCount(grouped.size());
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
            m_permissionTable->setItem(row, 0, new QTableWidgetItem(objTypeStr));
            
            // Object Name
            m_permissionTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(pair.first.second)));
            
            // Permissions (combine all permissions)
            QStringList permStrs;
            std::set<std::string> permSet;
            for (const auto& perm : pair.second) {
                // perm.permissionType is already a string
                permSet.insert(perm.permissionType);
            }
            for (const auto& permStr : permSet) {
                permStrs << QString::fromStdString(permStr);
            }
            m_permissionTable->setItem(row, 2, new QTableWidgetItem(permStrs.join(", ")));
            
            row++;
        }
    } else {
        m_permissionTable->setRowCount(0);
    }
}

void UserManagementWidget::clearUserInfo()
{
    m_userNameLabel->setText("No user selected");
    m_userStatusLabel->setText("");
    m_roleTable->setRowCount(0);
    m_permissionTable->setRowCount(0);
    m_noUserSelectedLabel->setVisible(true);
}

void UserManagementWidget::refreshUserList()
{
    loadUserList();
}

