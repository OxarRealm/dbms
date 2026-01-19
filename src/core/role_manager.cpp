/**
 * @file role_manager.cpp
 * @brief 角色管理器实现
 */

#include "core/role_manager.h"
#include "core/user_storage.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

// 文件格式：
// [MAGIC_HEADER: "DBMS_ROL_V1"]
// [角色数量: int]
// 对于每个角色：
//   [角色名长度: int] [角色名: char[]]
//   [描述长度: int] [描述: char[]]
//   [创建时间: time_t]

// 用户角色关联文件格式：
// [MAGIC_HEADER: "DBMS_URR_V1"]
// [关联数量: int]
// 对于每个关联：
//   [用户名长度: int] [用户名: char[]]
//   [角色名长度: int] [角色名: char[]]
//   [分配时间: time_t]

const char* ROLE_MAGIC_HEADER = "DBMS_ROL_V1";
const char* USER_ROLE_MAGIC_HEADER = "DBMS_URR_V1";

RoleManager::RoleManager() {
}

RoleManager::~RoleManager() {
}

void RoleManager::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
    // 自动加载角色和用户角色关联数据
    loadRoles();
    loadUserRoles();
}

bool RoleManager::createRole(const std::string& roleName, const std::string& description) {
    // 检查角色名是否为空
    if (roleName.empty()) {
        std::cerr << "Error: Role name cannot be empty" << std::endl;
        return false;
    }
    
    // 检查角色名长度
    if (roleName.length() >= ROLE_NAME_LENGTH) {
        std::cerr << "Error: Role name is too long (maximum " << (ROLE_NAME_LENGTH - 1) << " characters)" << std::endl;
        return false;
    }
    
    // 关键修复：总是重新加载数据，确保与文件同步
    if (!m_dbFilePath.empty()) {
        loadRoles();
    }
    
    // 检查角色是否已存在
    if (roleExists(roleName)) {
        std::cerr << "Error: Role already exists: " << roleName << std::endl;
        return false;
    }
    
    // 创建角色信息
    RoleInfo roleInfo(roleName.c_str());
    roleInfo.description = description;
    roleInfo.createTime = std::time(nullptr);
    
    // 添加到内存映射
    m_roles[roleName] = roleInfo;
    
    // 保存到文件
    if (!saveRoles()) {
        // 如果保存失败，从内存中移除
        m_roles.erase(roleName);
        std::cerr << "Error: Failed to save role data" << std::endl;
        return false;
    }
    
    return true;
}

bool RoleManager::deleteRole(const std::string& roleName) {
    // 关键修复：总是重新加载数据，确保与文件同步
    if (!m_dbFilePath.empty()) {
        loadRoles();
        loadUserRoles();
    }
    
    if (!roleExists(roleName)) {
        std::cerr << "Error: Role does not exist: " << roleName << std::endl;
        return false;
    }
    
    // 检查是否有用户拥有此角色
    for (const auto& pair : m_userRoles) {
        const auto& roles = pair.second;
        if (std::find(roles.begin(), roles.end(), roleName) != roles.end()) {
            std::cerr << "Error: Cannot delete role, users still have this role: " << roleName << std::endl;
            return false;
        }
    }
    
    // 从内存映射中移除
    m_roles.erase(roleName);
    
    // 从所有用户的角色列表中移除（虽然检查过应该没有）
    for (auto& pair : m_userRoles) {
        auto& roles = pair.second;
        roles.erase(std::remove(roles.begin(), roles.end(), roleName), roles.end());
    }
    
    // 保存到文件
    if (!saveRoles() || !saveUserRoles()) {
        std::cerr << "Error: Failed to save role data" << std::endl;
        return false;
    }
    
    return true;
}

bool RoleManager::roleExists(const std::string& roleName) const {
    return m_roles.find(roleName) != m_roles.end();
}

bool RoleManager::getRoleInfo(const std::string& roleName, RoleInfo& roleInfo) const {
    if (!roleExists(roleName)) {
        return false;
    }
    
    roleInfo = m_roles.at(roleName);
    return true;
}

bool RoleManager::getAllRoleNames(std::vector<std::string>& roleNames) const {
    roleNames.clear();
    for (const auto& pair : m_roles) {
        roleNames.push_back(pair.first);
    }
    return true;
}

bool RoleManager::grantRoleToUser(const std::string& roleName, const std::string& userName) {
    // 关键修复：总是重新加载数据，确保与文件同步
    if (!m_dbFilePath.empty()) {
        loadRoles();
        loadUserRoles();
    }
    
    if (!roleExists(roleName)) {
        std::cerr << "Error: Role does not exist: " << roleName << std::endl;
        return false;
    }
    
    // 检查用户是否已经拥有此角色
    if (userHasRole(userName, roleName)) {
        // 已经拥有，不算错误
        return true;
    }
    
    // 添加到用户的角色列表
    m_userRoles[userName].push_back(roleName);
    
    // 保存到文件
    if (!saveUserRoles()) {
        // 如果保存失败，从内存中移除
        auto& roles = m_userRoles[userName];
        roles.erase(std::remove(roles.begin(), roles.end(), roleName), roles.end());
        if (roles.empty()) {
            m_userRoles.erase(userName);
        }
        std::cerr << "Error: Failed to save user role data" << std::endl;
        return false;
    }
    
    return true;
}

bool RoleManager::revokeRoleFromUser(const std::string& roleName, const std::string& userName) {
    // 关键修复：总是重新加载数据，确保与文件同步
    if (!m_dbFilePath.empty()) {
        loadUserRoles();
    }
    
    if (!userHasRole(userName, roleName)) {
        // 用户不拥有此角色，不算错误
        return true;
    }
    
    // 从用户的角色列表中移除
    auto& roles = m_userRoles[userName];
    roles.erase(std::remove(roles.begin(), roles.end(), roleName), roles.end());
    
    // 如果用户没有其他角色，从映射中移除
    if (roles.empty()) {
        m_userRoles.erase(userName);
    }
    
    // 保存到文件
    if (!saveUserRoles()) {
        std::cerr << "Error: Failed to save user role data" << std::endl;
        return false;
    }
    
    return true;
}

bool RoleManager::getUserRoles(const std::string& userName, std::vector<std::string>& roleNames) const {
    roleNames.clear();
    auto it = m_userRoles.find(userName);
    if (it != m_userRoles.end()) {
        roleNames = it->second;
    }
    return true;
}

bool RoleManager::userHasRole(const std::string& userName, const std::string& roleName) const {
    auto it = m_userRoles.find(userName);
    if (it == m_userRoles.end()) {
        return false;
    }
    
    const auto& roles = it->second;
    return std::find(roles.begin(), roles.end(), roleName) != roles.end();
}

bool RoleManager::loadRoles() {
    // 关键修复：总是先清空内存数据，确保从文件重新加载
    m_roles.clear();
    
    if (m_dbFilePath.empty()) {
        // 如果没有设置数据库路径，返回（已清空）
        return true;
    }
    
    std::string filePath = UserStorageManager::getRoleFilePath(m_dbFilePath, m_dbFilePath);
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在不算错误
        m_roles.clear();
        return true;
    }
    
    // 读取魔数头
    char header[16];
    file.read(header, strlen(ROLE_MAGIC_HEADER));
    if (file.gcount() != strlen(ROLE_MAGIC_HEADER) || 
        strncmp(header, ROLE_MAGIC_HEADER, strlen(ROLE_MAGIC_HEADER)) != 0) {
        std::cerr << "Error: Invalid role file format" << std::endl;
        file.close();
        return false;
    }
    
    // 读取角色数量
    int roleCount;
    file.read(reinterpret_cast<char*>(&roleCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read role count" << std::endl;
        file.close();
        return false;
    }
    
    m_roles.clear();
    
    // 读取每个角色
    for (int i = 0; i < roleCount; ++i) {
        RoleInfo roleInfo;
        
        // 读取角色名长度
        int nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
        if (file.gcount() != sizeof(int) || nameLen < 0 || nameLen >= ROLE_NAME_LENGTH) {
            std::cerr << "Error: Invalid role name length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取角色名
        file.read(roleInfo.roleName, nameLen);
        if (file.gcount() != nameLen) {
            std::cerr << "Error: Failed to read role name" << std::endl;
            file.close();
            return false;
        }
        roleInfo.roleName[nameLen] = '\0';
        
        // 读取描述长度
        int descLen;
        file.read(reinterpret_cast<char*>(&descLen), sizeof(int));
        if (file.gcount() != sizeof(int) || descLen < 0) {
            std::cerr << "Error: Invalid description length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取描述
        if (descLen > 0) {
            std::vector<char> descBuf(descLen + 1);
            file.read(descBuf.data(), descLen);
            if (file.gcount() != descLen) {
                std::cerr << "Error: Failed to read description" << std::endl;
                file.close();
                return false;
            }
            descBuf[descLen] = '\0';
            roleInfo.description = descBuf.data();
        }
        
        // 读取创建时间
        file.read(reinterpret_cast<char*>(&roleInfo.createTime), sizeof(time_t));
        if (file.gcount() != sizeof(time_t)) {
            std::cerr << "Error: Failed to read create time" << std::endl;
            file.close();
            return false;
        }
        
        // 添加到内存映射
        m_roles[std::string(roleInfo.roleName)] = roleInfo;
    }
    
    file.close();
    return true;
}

bool RoleManager::saveRoles() const {
    if (m_dbFilePath.empty()) {
        std::cerr << "Error: Database path is not set" << std::endl;
        return false;
    }
    
    std::string filePath = UserStorageManager::getRoleFilePath(m_dbFilePath, m_dbFilePath);
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create role file: " << filePath << std::endl;
        return false;
    }
    
    // 写入魔数头
    file.write(ROLE_MAGIC_HEADER, strlen(ROLE_MAGIC_HEADER));
    if (!file.good()) {
        std::cerr << "Error: Failed to write magic header" << std::endl;
        file.close();
        return false;
    }
    
    // 写入角色数量
    int roleCount = static_cast<int>(m_roles.size());
    file.write(reinterpret_cast<const char*>(&roleCount), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write role count" << std::endl;
        file.close();
        return false;
    }
    
    // 写入每个角色
    for (const auto& pair : m_roles) {
        const RoleInfo& roleInfo = pair.second;
        
        // 写入角色名长度和角色名
        int nameLen = static_cast<int>(strlen(roleInfo.roleName));
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write role name length" << std::endl;
            file.close();
            return false;
        }
        
        file.write(roleInfo.roleName, nameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write role name" << std::endl;
            file.close();
            return false;
        }
        
        // 写入描述长度和描述
        int descLen = static_cast<int>(roleInfo.description.length());
        file.write(reinterpret_cast<const char*>(&descLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write description length" << std::endl;
            file.close();
            return false;
        }
        
        if (descLen > 0) {
            file.write(roleInfo.description.c_str(), descLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write description" << std::endl;
                file.close();
                return false;
            }
        }
        
        // 写入创建时间
        file.write(reinterpret_cast<const char*>(&roleInfo.createTime), sizeof(time_t));
        if (!file.good()) {
            std::cerr << "Error: Failed to write create time" << std::endl;
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

bool RoleManager::loadUserRoles() {
    // 关键修复：总是先清空内存数据，确保从文件重新加载
    m_userRoles.clear();
    
    if (m_dbFilePath.empty()) {
        // 如果没有设置数据库路径，返回（已清空）
        return true;
    }
    
    // 用户角色关联文件路径：使用.usrr扩展名（User Role Relationship）
    std::string dirStr;
    size_t lastSlash = m_dbFilePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirStr = m_dbFilePath.substr(0, lastSlash);
    } else {
        dirStr = ".";
    }
    
    std::string filePath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        filePath += "\\";
        #else
        filePath += "/";
        #endif
    }
    
    // 从m_dbFilePath提取数据库名（去除路径）
    std::string dbName = m_dbFilePath;
    if (lastSlash != std::string::npos) {
        dbName = m_dbFilePath.substr(lastSlash + 1);
    }
    filePath += dbName + ".usrr";
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在不算错误
        m_userRoles.clear();
        return true;
    }
    
    // 读取魔数头
    char header[16];
    file.read(header, strlen(USER_ROLE_MAGIC_HEADER));
    if (file.gcount() != strlen(USER_ROLE_MAGIC_HEADER) || 
        strncmp(header, USER_ROLE_MAGIC_HEADER, strlen(USER_ROLE_MAGIC_HEADER)) != 0) {
        std::cerr << "Error: Invalid user role file format" << std::endl;
        file.close();
        return false;
    }
    
    // 读取关联数量
    int linkCount;
    file.read(reinterpret_cast<char*>(&linkCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read link count" << std::endl;
        file.close();
        return false;
    }
    
    m_userRoles.clear();
    
    // 读取每个关联
    for (int i = 0; i < linkCount; ++i) {
        std::string userName;
        std::string roleName;
        
        // 读取用户名长度
        int userNameLen;
        file.read(reinterpret_cast<char*>(&userNameLen), sizeof(int));
        if (file.gcount() != sizeof(int) || userNameLen < 0 || userNameLen >= USER_NAME_LENGTH) {
            std::cerr << "Error: Invalid user name length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取用户名
        std::vector<char> userNameBuf(userNameLen + 1);
        file.read(userNameBuf.data(), userNameLen);
        if (file.gcount() != userNameLen) {
            std::cerr << "Error: Failed to read user name" << std::endl;
            file.close();
            return false;
        }
        userNameBuf[userNameLen] = '\0';
        userName = userNameBuf.data();
        
        // 读取角色名长度
        int roleNameLen;
        file.read(reinterpret_cast<char*>(&roleNameLen), sizeof(int));
        if (file.gcount() != sizeof(int) || roleNameLen < 0 || roleNameLen >= ROLE_NAME_LENGTH) {
            std::cerr << "Error: Invalid role name length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取角色名
        std::vector<char> roleNameBuf(roleNameLen + 1);
        file.read(roleNameBuf.data(), roleNameLen);
        if (file.gcount() != roleNameLen) {
            std::cerr << "Error: Failed to read role name" << std::endl;
            file.close();
            return false;
        }
        roleNameBuf[roleNameLen] = '\0';
        roleName = roleNameBuf.data();
        
        // 添加到内存映射
        if (std::find(m_userRoles[userName].begin(), m_userRoles[userName].end(), roleName) == m_userRoles[userName].end()) {
            m_userRoles[userName].push_back(roleName);
        }
    }
    
    file.close();
    return true;
}

bool RoleManager::saveUserRoles() const {
    if (m_dbFilePath.empty()) {
        std::cerr << "Error: Database path is not set" << std::endl;
        return false;
    }
    
    // 用户角色关联文件路径
    std::string dirStr;
    size_t lastSlash = m_dbFilePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirStr = m_dbFilePath.substr(0, lastSlash);
    } else {
        dirStr = ".";
    }
    
    std::string filePath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        filePath += "\\";
        #else
        filePath += "/";
        #endif
    }
    
    std::string dbName = m_dbFilePath;
    if (lastSlash != std::string::npos) {
        dbName = m_dbFilePath.substr(lastSlash + 1);
    }
    filePath += dbName + ".usrr";
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create user role file: " << filePath << std::endl;
        return false;
    }
    
    // 写入魔数头
    file.write(USER_ROLE_MAGIC_HEADER, strlen(USER_ROLE_MAGIC_HEADER));
    if (!file.good()) {
        std::cerr << "Error: Failed to write magic header" << std::endl;
        file.close();
        return false;
    }
    
    // 计算总关联数量
    int totalLinks = 0;
    for (const auto& pair : m_userRoles) {
        totalLinks += static_cast<int>(pair.second.size());
    }
    
    // 写入关联数量
    file.write(reinterpret_cast<const char*>(&totalLinks), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write link count" << std::endl;
        file.close();
        return false;
    }
    
    // 写入每个关联
    for (const auto& pair : m_userRoles) {
        const std::string& userName = pair.first;
        const std::vector<std::string>& roles = pair.second;
        
        for (const auto& roleName : roles) {
            // 写入用户名长度和用户名
            int userNameLen = static_cast<int>(userName.length());
            file.write(reinterpret_cast<const char*>(&userNameLen), sizeof(int));
            if (!file.good()) {
                std::cerr << "Error: Failed to write user name length" << std::endl;
                file.close();
                return false;
            }
            
            file.write(userName.c_str(), userNameLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write user name" << std::endl;
                file.close();
                return false;
            }
            
            // 写入角色名长度和角色名
            int roleNameLen = static_cast<int>(roleName.length());
            file.write(reinterpret_cast<const char*>(&roleNameLen), sizeof(int));
            if (!file.good()) {
                std::cerr << "Error: Failed to write role name length" << std::endl;
                file.close();
                return false;
            }
            
            file.write(roleName.c_str(), roleNameLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write role name" << std::endl;
                file.close();
                return false;
            }
        }
    }
    
    file.close();
    return true;
}
