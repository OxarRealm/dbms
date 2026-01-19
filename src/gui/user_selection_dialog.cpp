/**
 * @file user_selection_dialog.cpp
 * @brief User Selection Dialog Implementation
 */

#include "gui/user_selection_dialog.h"
#include "core/user_manager.h"
#include <QFont>
#include <QMessageBox>
#include <iostream>

UserSelectionDialog::UserSelectionDialog(QWidget *parent, const std::string& dbPath)
    : QDialog(parent)
    , m_titleLabel(nullptr)
    , m_userLabel(nullptr)
    , m_userComboBox(nullptr)
    , m_passwordLabel(nullptr)
    , m_passwordEdit(nullptr)
    , m_okBtn(nullptr)
    , m_cancelBtn(nullptr)
    , m_mainLayout(nullptr)
    , m_buttonLayout(nullptr)
    , m_databasePath(dbPath)
    , m_selectedUser("")
{
    setupUI();
    loadUsers();
    setWindowTitle("Database Login");
    setModal(true);
    resize(350, 200);
    setMinimumWidth(300);
}

UserSelectionDialog::~UserSelectionDialog()
{
}

void UserSelectionDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title label
    m_titleLabel = new QLabel("Select user identity for this database:", this);
    m_titleLabel->setFont(QFont("Segoe UI", 9));
    m_mainLayout->addWidget(m_titleLabel);

    // User selection
    QHBoxLayout *userLayout = new QHBoxLayout();
    m_userLabel = new QLabel("User:", this);
    m_userLabel->setFont(QFont("Segoe UI", 9));
    userLayout->addWidget(m_userLabel);

    m_userComboBox = new QComboBox(this);
    m_userComboBox->setFont(QFont("Segoe UI", 9));
    m_userComboBox->setMinimumWidth(200);
    // "admin" is always available (default)
    m_userComboBox->addItem("admin");
    m_userComboBox->setCurrentText("admin");
    userLayout->addWidget(m_userComboBox);
    userLayout->addStretch();
    m_mainLayout->addLayout(userLayout);

    // Password input
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    m_passwordLabel = new QLabel("Password:", this);
    m_passwordLabel->setFont(QFont("Segoe UI", 9));
    passwordLayout->addWidget(m_passwordLabel);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setFont(QFont("Segoe UI", 9));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumWidth(200);
    passwordLayout->addWidget(m_passwordEdit);
    passwordLayout->addStretch();
    m_mainLayout->addLayout(passwordLayout);

    // Buttons
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->addStretch();

    m_okBtn = new QPushButton("OK", this);
    m_okBtn->setFont(QFont("Segoe UI", 9));
    m_okBtn->setDefault(true);
    connect(m_okBtn, &QPushButton::clicked, this, &UserSelectionDialog::onOkClicked);

    m_cancelBtn = new QPushButton("Cancel", this);
    m_cancelBtn->setFont(QFont("Segoe UI", 9));
    connect(m_cancelBtn, &QPushButton::clicked, this, &UserSelectionDialog::onCancelClicked);

    m_buttonLayout->addWidget(m_okBtn);
    m_buttonLayout->addWidget(m_cancelBtn);
    m_mainLayout->addLayout(m_buttonLayout);

    // Set focus to password edit
    m_passwordEdit->setFocus();
}

void UserSelectionDialog::loadUsers()
{
    if (m_databasePath.empty()) {
        return;
    }

    // Load users from database
    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);

    std::vector<std::string> userNames;
    if (userManager.getAllUserNames(userNames)) {
        // Add users to combo box (skip "admin" if it's already there)
        for (const auto& userName : userNames) {
            // Check if user already exists in combo box
            bool exists = false;
            for (int i = 0; i < m_userComboBox->count(); ++i) {
                if (m_userComboBox->itemText(i).toStdString() == userName) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                m_userComboBox->addItem(QString::fromStdString(userName));
            }
        }
    }
}

void UserSelectionDialog::onOkClicked()
{
    QString selectedUser = m_userComboBox->currentText().trimmed();
    if (selectedUser.isEmpty()) {
        QMessageBox::warning(this, "Invalid Selection", "Please select a user.");
        return;
    }

    QString password = m_passwordEdit->text();
    
    // For "admin" user, check if password is "admin" (default)
    if (selectedUser == "admin") {
        if (password == "admin") {
            m_selectedUser = selectedUser.toStdString();
            accept();
            return;
        } else {
            QMessageBox::warning(this, "Login Failed", "Invalid password for admin user.");
            m_passwordEdit->clear();
            m_passwordEdit->setFocus();
            return;
        }
    }
    
    // For other users, verify using UserManager
    UserManager userManager;
    userManager.setDatabasePath(m_databasePath);
    bool authResult = userManager.authenticate(selectedUser.toStdString(), password.toStdString());
    
    if (authResult) {
        m_selectedUser = selectedUser.toStdString();
        accept();
        return;
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
        return;
    }
}

void UserSelectionDialog::onCancelClicked()
{
    reject();
}

std::string UserSelectionDialog::getSelectedUser() const
{
    return m_selectedUser;
}

