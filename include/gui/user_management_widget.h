#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <string>
#include <vector>

/**
 * @file user_management_widget.h
 * @brief User Management Widget Class
 *
 * Widget for managing users, roles, and permissions
 */

/**
 * @brief User Management Widget Class
 */
class UserManagementWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit UserManagementWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~UserManagementWidget();

    /**
     * @brief Set database path
     * @param dbPath Database file path (without extension)
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief Refresh user list
     */
    void refreshUserList();

private slots:
    /**
     * @brief Handle create user button click
     */
    void onCreateUser();

    /**
     * @brief Handle delete user button click
     */
    void onDeleteUser();

    /**
     * @brief Handle user selection change
     */
    void onUserSelectionChanged();

    /**
     * @brief Handle refresh button click
     */
    void onRefresh();

    /**
     * @brief Handle manage roles button click
     */
    void onManageRoles();

    /**
     * @brief Handle manage permissions button click
     */
    void onManagePermissions();

    /**
     * @brief Handle enable user button click
     */
    void onEnableUser();

    /**
     * @brief Handle disable user button click
     */
    void onDisableUser();

    /**
     * @brief Handle change password button click
     */
    void onChangePassword();

private:
    /**
     * @brief Setup UI components
     */
    void setupUI();

    /**
     * @brief Load user list from database
     */
    void loadUserList();

    /**
     * @brief Display user information
     * @param username Username to display
     */
    void displayUserInfo(const std::string& username);

    /**
     * @brief Clear user information display
     */
    void clearUserInfo();

    // UI Components
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_leftLayout;
    QVBoxLayout *m_rightLayout;
    
    // Left panel: User list
    QGroupBox *m_userListGroup;
    QListWidget *m_userList;
    QPushButton *m_createUserBtn;
    QPushButton *m_deleteUserBtn;
    QPushButton *m_enableUserBtn;
    QPushButton *m_disableUserBtn;
    QPushButton *m_changePasswordBtn;
    QPushButton *m_refreshBtn;
    
    // Right panel: User info
    QGroupBox *m_userInfoGroup;
    QLabel *m_userNameLabel;
    QLabel *m_userStatusLabel;
    QTableWidget *m_roleTable;
    QTableWidget *m_permissionTable;
    QPushButton *m_manageRolesBtn;
    QPushButton *m_managePermissionsBtn;
    QLabel *m_noUserSelectedLabel;
    
    // Backend
    std::string m_databasePath;
};

