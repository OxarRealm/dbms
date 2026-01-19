#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <string>

/**
 * @file login_dialog.h
 * @brief Login Dialog Class
 *
 * Dialog for user login authentication
 */

/**
 * @brief Login Dialog Class
 */
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit LoginDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~LoginDialog();

    /**
     * @brief Get entered username
     * @return Username string
     */
    std::string getUsername() const;

    /**
     * @brief Get entered password
     * @return Password string
     */
    std::string getPassword() const;

private slots:
    /**
     * @brief Handle login button click
     */
    void onLoginClicked();

    /**
     * @brief Handle cancel button click
     */
    void onCancelClicked();

private:
    /**
     * @brief Setup UI components
     */
    void setupUI();

    /**
     * @brief Verify user credentials
     * @param username Username
     * @param password Password
     * @return True if credentials are valid, false otherwise
     */
    bool verifyCredentials(const std::string& username, const std::string& password);

    // UI Components
    QLabel *m_titleLabel;
    QLabel *m_usernameLabel;
    QLineEdit *m_usernameEdit;
    QLabel *m_passwordLabel;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_cancelBtn;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;

    // Result
    std::string m_username;
    std::string m_password;
    bool m_loginAccepted;
};

