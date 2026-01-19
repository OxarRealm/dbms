/**
 * @file session_manager.cpp
 * @brief 会话管理器实现
 */

#include "core/session_manager.h"

SessionManager::SessionManager() : m_currentUser("") {
}

SessionManager::~SessionManager() {
}

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

bool SessionManager::login(const std::string& userName) {
    m_currentUser = userName;
    return true;
}

void SessionManager::logout() {
    m_currentUser = "";
}

std::string SessionManager::getCurrentUser() const {
    return m_currentUser;
}

bool SessionManager::isLoggedIn() const {
    return !m_currentUser.empty();
}

void SessionManager::setCurrentUser(const std::string& userName) {
    m_currentUser = userName;
}




