#pragma once

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <string>
#include <vector>
#include "core/user_mode.h"

/**
 * @file permission_management_dialog.h
 * @brief Permission Management Dialog Class
 *
 * Dialog for managing permissions for a user
 */

/**
 * @brief Permission Management Dialog Class
 */
class PermissionManagementDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param userName Username to manage permissions for
     * @param databasePath Database path
     * @param parent Parent widget
     */
    explicit PermissionManagementDialog(const std::string& userName,
                                       const std::string& databasePath,
                                       QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PermissionManagementDialog();

private slots:
    /**
     * @brief Handle grant permission button click
     */
    void onGrantPermission();

    /**
     * @brief Handle revoke permission button click
     */
    void onRevokePermission();

    /**
     * @brief Handle refresh button click
     */
    void onRefresh();

private:
    /**
     * @brief Setup UI components
     */
    void setupUI();

    /**
     * @brief Load permission list
     */
    void loadPermissions();

    /**
     * @brief Get object type from string
     */
    char getObjectTypeFromString(const QString& str);

    /**
     * @brief Get object type string
     */
    QString getObjectTypeString(char objectType);

    // UI Components
    QLabel *m_titleLabel;
    QLabel *m_userLabel;
    QTableWidget *m_permissionTable;
    QPushButton *m_grantBtn;
    QPushButton *m_revokeBtn;
    QPushButton *m_refreshBtn;
    QPushButton *m_closeBtn;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonsLayout;
    QHBoxLayout *m_actionButtonsLayout;

    // Grant dialog components
    QComboBox *m_objectTypeCombo;
    QComboBox *m_objectNameCombo;
    QListWidget *m_permissionList;
    QCheckBox *m_grantOptionCheck;

    // Data
    std::string m_userName;
    std::string m_databasePath;
};

