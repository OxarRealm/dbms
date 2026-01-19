/**
 * @file login_dialog.cpp
 * @brief Login Dialog Implementation
 */

#include "gui/login_dialog.h"
#include "core/user_manager.h"
#include "core/session_manager.h"
#include <QFont>
#include <iostream>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , m_titleLabel(nullptr)
    , m_usernameLabel(nullptr)
    , m_usernameEdit(nullptr)
    , m_passwordLabel(nullptr)
    , m_passwordEdit(nullptr)
    , m_loginBtn(nullptr)
    , m_cancelBtn(nullptr)
    , m_mainLayout(nullptr)
    , m_buttonLayout(nullptr)
    , m_loginAccepted(false)
{
    setupUI();
    setWindowTitle("Login");
    setModal(true);
    resize(350, 180);
    setMinimumWidth(300);
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    m_titleLabel = new QLabel("User Login", this);
    m_titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // Username
    m_usernameLabel = new QLabel("Username:", this);
    m_usernameLabel->setFont(QFont("Segoe UI", 9));
    m_mainLayout->addWidget(m_usernameLabel);

    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setFont(QFont("Segoe UI", 9));
    m_usernameEdit->setPlaceholderText("Enter username");
    m_mainLayout->addWidget(m_usernameEdit);

    // Password
    m_passwordLabel = new QLabel("Password:", this);
    m_passwordLabel->setFont(QFont("Segoe UI", 9));
    m_mainLayout->addWidget(m_passwordLabel);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setFont(QFont("Segoe UI", 9));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Enter password");
    m_mainLayout->addWidget(m_passwordEdit);

    // Buttons
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->addStretch();

    m_loginBtn = new QPushButton("Login", this);
    m_loginBtn->setFont(QFont("Segoe UI", 9));
    // Don't set as default button, so Enter key behavior is controlled by line edits
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    m_buttonLayout->addWidget(m_loginBtn);

    m_cancelBtn = new QPushButton("Cancel", this);
    m_cancelBtn->setFont(QFont("Segoe UI", 9));
    connect(m_cancelBtn, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    m_buttonLayout->addWidget(m_cancelBtn);

    m_mainLayout->addLayout(m_buttonLayout);

    // Set focus to username edit
    m_usernameEdit->setFocus();

    // Connect Enter key: username -> move to password, password -> login
    connect(m_usernameEdit, &QLineEdit::returnPressed, [this]() {
        // Always move to password field when Enter is pressed in username field
        m_passwordEdit->setFocus();
        m_passwordEdit->selectAll(); // Select all text for easy replacement
    });
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::onLoginClicked()
{
    std::string username = m_usernameEdit->text().toStdString();
    std::string password = m_passwordEdit->text().toStdString();

    if (username.empty()) {
        QMessageBox::warning(this, "Login Failed", "Please enter username.");
        m_usernameEdit->setFocus();
        return;
    }

    // If password is empty and we're called from username field, move to password field instead
    if (password.empty() && m_usernameEdit->hasFocus()) {
        m_passwordEdit->setFocus();
        return;
    }

    if (password.empty()) {
        QMessageBox::warning(this, "Login Failed", "Please enter password.");
        m_passwordEdit->setFocus();
        return;
    }

    // For now, if database is not loaded, allow any login
    // In production, we should check against user system
    if (verifyCredentials(username, password)) {
        m_username = username;
        m_password = password;
        m_loginAccepted = true;
        accept();
    } else {
        QMessageBox::critical(this, "Login Failed", "Invalid username or password.");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}

void LoginDialog::onCancelClicked()
{
    m_loginAccepted = false;
    reject();
}

bool LoginDialog::verifyCredentials(const std::string& username, const std::string& password)
{
    // If database is not loaded, check if admin user exists in system
    // For now, we'll allow login if username/password match or if user doesn't exist yet (first time setup)
    
    // Try to authenticate using UserManager
    // Note: UserManager needs database path, but for login we might not have database yet
    // For initial setup, allow default admin login
    if (username == "admin" && password == "admin") {
        return true;  // Default admin for first time setup
    }

    // TODO: Implement proper authentication using SessionManager
    // SessionManager should handle authentication and session management
    // For now, accept any non-empty credentials for development
    
    return !username.empty() && !password.empty();
}

std::string LoginDialog::getUsername() const
{
    return m_username;
}

std::string LoginDialog::getPassword() const
{
    return m_password;
}

