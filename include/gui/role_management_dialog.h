#pragma once

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <string>
#include <vector>

/**
 * @file role_management_dialog.h
 * @brief Role Management Dialog Class
 *
 * Dialog for managing roles for a user
 */

/**
 * @brief Role Management Dialog Class
 */
class RoleManagementDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param userName Username to manage roles for
     * @param databasePath Database path
     * @param parent Parent widget
     */
    explicit RoleManagementDialog(const std::string& userName, 
                                  const std::string& databasePath,
                                  QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~RoleManagementDialog();

private slots:
    /**
     * @brief Handle grant role button click
     */
    void onGrantRole();

    /**
     * @brief Handle revoke role button click
     */
    void onRevokeRole();

    /**
     * @brief Handle refresh button click
     */
    void onRefresh();

    /**
     * @brief Handle view permissions button click
     */
    void onViewPermissions();

private:
    /**
     * @brief Setup UI components
     */
    void setupUI();

    /**
     * @brief Load role list
     */
    void loadRoleList();

    /**
     * @brief Load user roles
     */
    void loadUserRoles();

    // UI Components
    QLabel *m_titleLabel;
    QLabel *m_userLabel;
    QListWidget *m_availableRolesList;
    QListWidget *m_userRolesList;
    QPushButton *m_grantBtn;
    QPushButton *m_revokeBtn;
    QPushButton *m_viewPermissionsBtn;
    QPushButton *m_refreshBtn;
    QPushButton *m_closeBtn;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_listsLayout;
    QHBoxLayout *m_buttonsLayout;
    QHBoxLayout *m_actionButtonsLayout;

    // Data
    std::string m_userName;
    std::string m_databasePath;
};

