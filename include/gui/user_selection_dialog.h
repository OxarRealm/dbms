#pragma once

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <string>
#include <vector>

/**
 * @file user_selection_dialog.h
 * @brief User Selection Dialog Class
 *
 * Dialog for selecting which user identity to use when opening a database
 */

/**
 * @brief User Selection Dialog Class
 */
class UserSelectionDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     * @param dbPath Database path (for loading users)
     */
    explicit UserSelectionDialog(QWidget *parent = nullptr, const std::string& dbPath = "");

    /**
     * @brief Destructor
     */
    ~UserSelectionDialog();

    /**
     * @brief Get selected username
     * @return Selected username string
     */
    std::string getSelectedUser() const;

private slots:
    /**
     * @brief Handle OK button click
     */
    void onOkClicked();

    /**
     * @brief Handle Cancel button click
     */
    void onCancelClicked();

private:
    /**
     * @brief Setup UI components
     */
    void setupUI();

    /**
     * @brief Load available users from database
     */
    void loadUsers();

    // UI Components
    QLabel *m_titleLabel;
    QLabel *m_userLabel;
    QComboBox *m_userComboBox;
    QLabel *m_passwordLabel;
    QLineEdit *m_passwordEdit;
    QPushButton *m_okBtn;
    QPushButton *m_cancelBtn;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;

    // Data
    std::string m_databasePath;
    std::string m_selectedUser;
};

