/**
 * @file user_manager.cpp
 * @brief 用户管理器实现
 */

#include "core/user_manager.h"
#include "core/user_storage.h"
#include <QtCore/QCryptographicHash>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <iomanip>

// 文件格式：
// [MAGIC_HEADER: "DBMS_USR_V1"]
// [用户数量: int]
// 对于每个用户：
//   [用户名长度: int] [用户名: char[]]
//   [密码哈希: char[PASSWORD_HASH_LENGTH]]
//   [创建时间: time_t]
//   [最后登录时间: time_t]
//   [状态: char (1=启用, 0=禁用)]

const char* USER_MAGIC_HEADER = "DBMS_USR_V1";

UserManager::UserManager() {
}

UserManager::~UserManager() {
}

void UserManager::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
    // 当设置数据库路径时，自动加载用户数据
    loadUsers();
}

bool UserManager::createUser(const std::string& userName, const std::string& password) {
    // 检查用户名是否为空
    if (userName.empty()) {
        std::cerr << "Error: User name cannot be empty" << std::endl;
        return false;
    }
    
    // 检查用户名长度
    if (userName.length() >= USER_NAME_LENGTH) {
        std::cerr << "Error: User name is too long (maximum " << (USER_NAME_LENGTH - 1) << " characters)" << std::endl;
        return false;
    }
    
    // 确保用户数据已加载
    if (m_users.empty() && !m_dbFilePath.empty()) {
        loadUsers();
    }
    
    // 检查用户是否已存在
    if (userExists(userName)) {
        std::cerr << "Error: User already exists: " << userName << std::endl;
        return false;
    }
    
    // 创建用户信息
    UserInfo userInfo(userName.c_str());
    
    // 计算密码哈希
    std::string passwordHash = hashPassword(password);
    strncpy(userInfo.passwordHash, passwordHash.c_str(), PASSWORD_HASH_LENGTH - 1);
    userInfo.passwordHash[PASSWORD_HASH_LENGTH - 1] = '\0';
    
    // 设置创建时间
    userInfo.createTime = std::time(nullptr);
    userInfo.lastLoginTime = 0;
    userInfo.status = USER_STATUS_ENABLED;
    
    // 添加到内存映射
    m_users[userName] = userInfo;
    
    // 保存到文件
    if (!saveUsers()) {
        // 如果保存失败，从内存中移除
        m_users.erase(userName);
        std::cerr << "Error: Failed to save user data" << std::endl;
        return false;
    }
    
    return true;
}

bool UserManager::changePassword(const std::string& userName, const std::string& newPassword) {
    // 检查用户是否存在
    if (!userExists(userName)) {
        std::cerr << "Error: User does not exist: " << userName << std::endl;
        return false;
    }
    
    // 计算新密码哈希
    std::string passwordHash = hashPassword(newPassword);
    
    // 更新用户密码哈希
    m_users[userName].passwordHash[0] = '\0';
    strncpy(m_users[userName].passwordHash, passwordHash.c_str(), PASSWORD_HASH_LENGTH - 1);
    m_users[userName].passwordHash[PASSWORD_HASH_LENGTH - 1] = '\0';
    
    // 保存到文件
    if (!saveUsers()) {
        std::cerr << "Error: Failed to save user data" << std::endl;
        return false;
    }
    
    return true;
}

bool UserManager::enableUser(const std::string& userName) {
    if (!userExists(userName)) {
        std::cerr << "Error: User does not exist: " << userName << std::endl;
        return false;
    }
    
    m_users[userName].setEnabled(true);
    
    if (!saveUsers()) {
        std::cerr << "Error: Failed to save user data" << std::endl;
        return false;
    }
    
    return true;
}

bool UserManager::disableUser(const std::string& userName) {
    if (!userExists(userName)) {
        std::cerr << "Error: User does not exist: " << userName << std::endl;
        return false;
    }
    
    m_users[userName].setEnabled(false);
    
    if (!saveUsers()) {
        std::cerr << "Error: Failed to save user data" << std::endl;
        return false;
    }
    
    return true;
}

bool UserManager::deleteUser(const std::string& userName) {
    if (!userExists(userName)) {
        std::cerr << "Error: User does not exist: " << userName << std::endl;
        return false;
    }
    
    // 从内存映射中移除
    m_users.erase(userName);
    
    // 保存到文件
    if (!saveUsers()) {
        std::cerr << "Error: Failed to save user data" << std::endl;
        return false;
    }
    
    return true;
}

bool UserManager::authenticate(const std::string& userName, const std::string& password) {
    // 检查用户是否存在
    if (!userExists(userName)) {
        return false;
    }
    
    // 检查用户是否启用
    if (!m_users[userName].isEnabled()) {
        std::cerr << "Error: User is disabled: " << userName << std::endl;
        return false;
    }
    
    // 验证密码
    std::string storedHash = m_users[userName].passwordHash;
    if (!verifyPassword(password, storedHash)) {
        return false;
    }
    
    // 更新最后登录时间
    updateLastLoginTime(userName);
    
    return true;
}

bool UserManager::userExists(const std::string& userName) const {
    return m_users.find(userName) != m_users.end();
}

bool UserManager::getUserInfo(const std::string& userName, UserInfo& userInfo) const {
    if (!userExists(userName)) {
        return false;
    }
    
    userInfo = m_users.at(userName);
    return true;
}

bool UserManager::getAllUserNames(std::vector<std::string>& userNames) const {
    userNames.clear();
    for (const auto& pair : m_users) {
        userNames.push_back(pair.first);
    }
    return true;
}

bool UserManager::updateLastLoginTime(const std::string& userName) {
    if (!userExists(userName)) {
        return false;
    }
    
    m_users[userName].lastLoginTime = std::time(nullptr);
    
    // 保存到文件（可选：为了性能，可以不每次都保存）
    // saveUsers();
    
    return true;
}

std::string UserManager::hashPassword(const std::string& password) {
    // 使用Qt的SHA256计算密码哈希
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.c_str(), static_cast<int>(password.length()));
    QByteArray hashBytes = hash.result();
    
    // 转换为十六进制字符串
    return hashBytes.toHex().toStdString();
}

bool UserManager::verifyPassword(const std::string& password, const std::string& hash) {
    std::string calculatedHash = hashPassword(password);
    return calculatedHash == hash;
}

bool UserManager::loadUsers() {
    if (m_dbFilePath.empty()) {
        // 如果没有设置数据库路径，清空内存数据
        m_users.clear();
        return true;
    }
    
    // 获取用户文件路径
    std::string filePath = UserStorageManager::getUserFilePath(m_dbFilePath, m_dbFilePath);
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在不算错误（可能是新数据库）
        m_users.clear();
        return true;
    }
    
    // 读取魔数头
    char header[16];
    file.read(header, strlen(USER_MAGIC_HEADER));
    if (file.gcount() != strlen(USER_MAGIC_HEADER) || 
        strncmp(header, USER_MAGIC_HEADER, strlen(USER_MAGIC_HEADER)) != 0) {
        std::cerr << "Error: Invalid user file format" << std::endl;
        file.close();
        return false;
    }
    
    // 读取用户数量
    int userCount;
    file.read(reinterpret_cast<char*>(&userCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read user count" << std::endl;
        file.close();
        return false;
    }
    
    m_users.clear();
    
    // 读取每个用户
    for (int i = 0; i < userCount; ++i) {
        UserInfo userInfo;
        
        // 读取用户名长度
        int nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
        if (file.gcount() != sizeof(int) || nameLen < 0 || nameLen >= USER_NAME_LENGTH) {
            std::cerr << "Error: Invalid user name length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取用户名
        file.read(userInfo.userName, nameLen);
        if (file.gcount() != nameLen) {
            std::cerr << "Error: Failed to read user name" << std::endl;
            file.close();
            return false;
        }
        userInfo.userName[nameLen] = '\0';
        
        // 读取密码哈希
        file.read(userInfo.passwordHash, PASSWORD_HASH_LENGTH);
        if (file.gcount() != PASSWORD_HASH_LENGTH) {
            std::cerr << "Error: Failed to read password hash" << std::endl;
            file.close();
            return false;
        }
        
        // 读取创建时间
        file.read(reinterpret_cast<char*>(&userInfo.createTime), sizeof(time_t));
        if (file.gcount() != sizeof(time_t)) {
            std::cerr << "Error: Failed to read create time" << std::endl;
            file.close();
            return false;
        }
        
        // 读取最后登录时间
        file.read(reinterpret_cast<char*>(&userInfo.lastLoginTime), sizeof(time_t));
        if (file.gcount() != sizeof(time_t)) {
            std::cerr << "Error: Failed to read last login time" << std::endl;
            file.close();
            return false;
        }
        
        // 读取状态
        file.read(reinterpret_cast<char*>(&userInfo.status), sizeof(char));
        if (file.gcount() != sizeof(char)) {
            std::cerr << "Error: Failed to read user status" << std::endl;
            file.close();
            return false;
        }
        
        // 添加到内存映射
        m_users[std::string(userInfo.userName)] = userInfo;
    }
    
    file.close();
    return true;
}

bool UserManager::saveUsers() const {
    if (m_dbFilePath.empty()) {
        std::cerr << "Error: Database path is not set" << std::endl;
        return false;
    }
    
    // 获取用户文件路径
    std::string filePath = UserStorageManager::getUserFilePath(m_dbFilePath, m_dbFilePath);
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create user file: " << filePath << std::endl;
        return false;
    }
    
    // 写入魔数头
    file.write(USER_MAGIC_HEADER, strlen(USER_MAGIC_HEADER));
    if (!file.good()) {
        std::cerr << "Error: Failed to write magic header" << std::endl;
        file.close();
        return false;
    }
    
    // 写入用户数量
    int userCount = static_cast<int>(m_users.size());
    file.write(reinterpret_cast<const char*>(&userCount), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write user count" << std::endl;
        file.close();
        return false;
    }
    
    // 写入每个用户
    for (const auto& pair : m_users) {
        const UserInfo& userInfo = pair.second;
        
        // 写入用户名长度和用户名
        int nameLen = static_cast<int>(strlen(userInfo.userName));
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write user name length" << std::endl;
            file.close();
            return false;
        }
        
        file.write(userInfo.userName, nameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write user name" << std::endl;
            file.close();
            return false;
        }
        
        // 写入密码哈希
        file.write(userInfo.passwordHash, PASSWORD_HASH_LENGTH);
        if (!file.good()) {
            std::cerr << "Error: Failed to write password hash" << std::endl;
            file.close();
            return false;
        }
        
        // 写入创建时间
        file.write(reinterpret_cast<const char*>(&userInfo.createTime), sizeof(time_t));
        if (!file.good()) {
            std::cerr << "Error: Failed to write create time" << std::endl;
            file.close();
            return false;
        }
        
        // 写入最后登录时间
        file.write(reinterpret_cast<const char*>(&userInfo.lastLoginTime), sizeof(time_t));
        if (!file.good()) {
            std::cerr << "Error: Failed to write last login time" << std::endl;
            file.close();
            return false;
        }
        
        // 写入状态
        file.write(reinterpret_cast<const char*>(&userInfo.status), sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write user status" << std::endl;
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

