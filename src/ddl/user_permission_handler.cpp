/**
 * @file user_permission_handler.cpp
 * @brief 用户权限相关SQL语句处理器实现
 */

#include "ddl/user_permission_handler.h"
#include <iostream>
#include <algorithm>
#include <cctype>

// ==================== CreateUserHandler ====================

CreateUserHandler::CreateUserHandler() {
}

CreateUserHandler::~CreateUserHandler() {
}

void CreateUserHandler::setDatabasePath(const std::string& dbPath) {
    m_dbFilePath = dbPath;
    m_userManager.setDatabasePath(dbPath);
}

bool CreateUserHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 关键修复：重新设置数据库路径以触发数据重新加载，确保与文件同步
    if (!m_dbFilePath.empty()) {
        m_userManager.setDatabasePath(m_dbFilePath);  // 这会触发loadUsers()
    }
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 转换为CreateUserNode
    CreateUserNode* createNode = dynamic_cast<CreateUserNode*>(node.get());
    if (createNode == nullptr) {
        setError("Not a CREATE USER statement");
        return false;
    }
    
    // 验证用户名
    if (createNode->userName.empty()) {
        setError("User name cannot be empty");
        return false;
    }
    
    // 检查用户是否已存在
    if (m_userManager.userExists(createNode->userName)) {
        setError("User already exists: " + createNode->userName);
        return false;
    }
    
    // 创建用户
    if (!m_userManager.createUser(createNode->userName, createNode->password)) {
        setError("Failed to create user: " + createNode->userName);
        return false;
    }
    
    return true;
}

std::string CreateUserHandler::getLastError() const {
    return m_lastError;
}

void CreateUserHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "CreateUserHandler错误: " << error << std::endl;
}

// ==================== AlterUserHandler ====================

AlterUserHandler::AlterUserHandler() {
}

AlterUserHandler::~AlterUserHandler() {
}

void AlterUserHandler::setDatabasePath(const std::string& dbPath) {
    m_dbFilePath = dbPath;
    m_userManager.setDatabasePath(dbPath);
}

bool AlterUserHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 关键修复：重新设置数据库路径以触发数据重新加载，确保与文件同步
    if (!m_dbFilePath.empty()) {
        m_userManager.setDatabasePath(m_dbFilePath);  // 这会触发loadUsers()
    }
    
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    AlterUserNode* alterNode = dynamic_cast<AlterUserNode*>(node.get());
    if (alterNode == nullptr) {
        setError("Not an ALTER USER statement");
        return false;
    }
    
    // 检查用户是否存在
    if (!m_userManager.userExists(alterNode->userName)) {
        setError("User does not exist: " + alterNode->userName);
        return false;
    }
    
    // 根据操作类型执行
    if (alterNode->isPasswordChange) {
        // 修改密码
        if (!m_userManager.changePassword(alterNode->userName, alterNode->newPassword)) {
            setError("Failed to change password for user: " + alterNode->userName);
            return false;
        }
    } else {
        // 启用/禁用用户
        if (alterNode->disable) {
            if (!m_userManager.disableUser(alterNode->userName)) {
                setError("Failed to disable user: " + alterNode->userName);
                return false;
            }
        } else {
            if (!m_userManager.enableUser(alterNode->userName)) {
                setError("Failed to enable user: " + alterNode->userName);
                return false;
            }
        }
    }
    
    return true;
}

std::string AlterUserHandler::getLastError() const {
    return m_lastError;
}

void AlterUserHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "AlterUserHandler错误: " << error << std::endl;
}

// ==================== DropUserHandler ====================

DropUserHandler::DropUserHandler() {
}

DropUserHandler::~DropUserHandler() {
}

void DropUserHandler::setDatabasePath(const std::string& dbPath) {
    m_dbFilePath = dbPath;
    m_userManager.setDatabasePath(dbPath);
}

bool DropUserHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 关键修复：重新设置数据库路径以触发数据重新加载，确保与文件同步
    if (!m_dbFilePath.empty()) {
        m_userManager.setDatabasePath(m_dbFilePath);  // 这会触发loadUsers()
    }
    
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    DropUserNode* dropNode = dynamic_cast<DropUserNode*>(node.get());
    if (dropNode == nullptr) {
        setError("Not a DROP USER statement");
        return false;
    }
    
    // 检查用户是否存在
    if (!m_userManager.userExists(dropNode->userName)) {
        setError("User does not exist: " + dropNode->userName);
        return false;
    }
    
    // 删除用户
    if (!m_userManager.deleteUser(dropNode->userName)) {
        setError("Failed to delete user: " + dropNode->userName);
        return false;
    }
    
    return true;
}

std::string DropUserHandler::getLastError() const {
    return m_lastError;
}

void DropUserHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "DropUserHandler错误: " << error << std::endl;
}

// ==================== CreateRoleHandler ====================

CreateRoleHandler::CreateRoleHandler() : m_dbFilePath("") {
}

CreateRoleHandler::~CreateRoleHandler() {
}

void CreateRoleHandler::setDatabasePath(const std::string& dbPath) {
    m_dbFilePath = dbPath;
    m_roleManager.setDatabasePath(dbPath);
}

bool CreateRoleHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 关键修复：重新设置数据库路径以触发数据重新加载，确保与文件同步
    if (!m_dbFilePath.empty()) {
        m_roleManager.setDatabasePath(m_dbFilePath);  // 这会触发loadRoles()
    }
    
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    CreateRoleNode* createNode = dynamic_cast<CreateRoleNode*>(node.get());
    if (createNode == nullptr) {
        setError("Not a CREATE ROLE statement");
        return false;
    }
    
    // 验证角色名
    if (createNode->roleName.empty()) {
        setError("Role name cannot be empty");
        return false;
    }
    
    // 检查角色是否已存在
    if (m_roleManager.roleExists(createNode->roleName)) {
        setError("Role already exists: " + createNode->roleName);
        return false;
    }
    
    // 创建角色
    if (!m_roleManager.createRole(createNode->roleName)) {
        setError("Failed to create role: " + createNode->roleName);
        return false;
    }
    
    return true;
}

std::string CreateRoleHandler::getLastError() const {
    return m_lastError;
}

void CreateRoleHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "CreateRoleHandler错误: " << error << std::endl;
}

// ==================== DropRoleHandler ====================

DropRoleHandler::DropRoleHandler() : m_dbFilePath("") {
}

DropRoleHandler::~DropRoleHandler() {
}

void DropRoleHandler::setDatabasePath(const std::string& dbPath) {
    m_dbFilePath = dbPath;
    m_roleManager.setDatabasePath(dbPath);
}

bool DropRoleHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    DropRoleNode* dropNode = dynamic_cast<DropRoleNode*>(node.get());
    if (dropNode == nullptr) {
        setError("Not a DROP ROLE statement");
        return false;
    }
    
    // 检查角色是否存在
    if (!m_roleManager.roleExists(dropNode->roleName)) {
        setError("Role does not exist: " + dropNode->roleName);
        return false;
    }
    
    // 删除角色
    if (!m_roleManager.deleteRole(dropNode->roleName)) {
        setError("Failed to delete role: " + dropNode->roleName);
        return false;
    }
    
    return true;
}

std::string DropRoleHandler::getLastError() const {
    return m_lastError;
}

void DropRoleHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "DropRoleHandler错误: " << error << std::endl;
}

// ==================== GrantHandler ====================

GrantHandler::GrantHandler() {
}

GrantHandler::~GrantHandler() {
}

void GrantHandler::setDatabasePath(const std::string& dbPath) {
    m_userManager.setDatabasePath(dbPath);
    m_roleManager.setDatabasePath(dbPath);
    m_permissionManager.setDatabasePath(dbPath);
}

void GrantHandler::setCurrentUser(const std::string& userName) {
    // 这个方法用于设置当前用户（授予权限时需要知道授予者）
}

bool GrantHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 关键修复：重新设置数据库路径以触发数据重新加载，确保与文件同步
    if (!m_dbFilePath.empty()) {
        m_userManager.setDatabasePath(m_dbFilePath);      // 这会触发loadUsers()
        m_roleManager.setDatabasePath(m_dbFilePath);      // 这会触发loadRoles()和loadUserRoles()
        m_permissionManager.setDatabasePath(m_dbFilePath); // 这会触发loadPermissions()
    }
    
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    GrantNode* grantNode = dynamic_cast<GrantNode*>(node.get());
    if (grantNode == nullptr) {
        setError("Not a GRANT statement");
        return false;
    }
    
    // 获取当前用户（授予者）
    std::string currentUser = SessionManager::getInstance().getCurrentUser();
    if (currentUser.empty()) {
        currentUser = "SYSTEM"; // 默认系统用户
    }
    
    if (grantNode->isRoleGrant) {
        // 授予角色给用户
        if (!m_roleManager.grantRoleToUser(grantNode->roleName, grantNode->grantee)) {
            setError("Failed to grant role to user");
            return false;
        }
    } else {
        // 授予权限给用户或角色
        // 转换权限类型字符串到PermissionType枚举
        std::vector<PermissionType> permissionTypes;
        for (const auto& privStr : grantNode->privilegeTypes) {
            PermissionType type = stringToPermissionType(privStr);
            permissionTypes.push_back(type);
        }
        
        // 转换对象类型
        char objectType = stringToObjectType(grantNode->objectType);
        
        // 判断是授予用户还是角色
        // 检查grantee是用户名还是角色名
        bool isRole = m_roleManager.roleExists(grantNode->grantee);
        bool isUser = m_userManager.userExists(grantNode->grantee);
        
        if (!isRole && !isUser) {
            setError("Grantee does not exist (neither user nor role): " + grantNode->grantee);
            return false;
        }
        
        if (isRole) {
            // 授予权限给角色
            if (!m_permissionManager.grantPermissionToRole(
                    grantNode->grantee, objectType, grantNode->objectName, 
                    permissionTypes, grantNode->withGrantOption, currentUser)) {
                setError("Failed to grant permissions to role");
                return false;
            }
        } else {
            // 授予权限给用户
            if (!m_permissionManager.grantPermission(
                    grantNode->grantee, objectType, grantNode->objectName,
                    permissionTypes, grantNode->withGrantOption, currentUser)) {
                setError("Failed to grant permissions to user");
                return false;
            }
        }
    }
    
    return true;
}

std::string GrantHandler::getLastError() const {
    return m_lastError;
}

void GrantHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "GrantHandler错误: " << error << std::endl;
}

PermissionType GrantHandler::stringToPermissionType(const std::string& str) const {
    return PermissionManager::stringToPermissionType(str);
}

char GrantHandler::stringToObjectType(const std::string& str) const {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    
    if (upperStr == "TABLE") {
        return OBJECT_TYPE_TABLE;
    } else if (upperStr == "DATABASE") {
        return OBJECT_TYPE_DATABASE;
    } else if (upperStr == "SYSTEM") {
        return OBJECT_TYPE_SYSTEM;
    }
    return 0;
}

// ==================== RevokeHandler ====================

RevokeHandler::RevokeHandler() : m_dbFilePath("") {
}

RevokeHandler::~RevokeHandler() {
}

void RevokeHandler::setDatabasePath(const std::string& dbPath) {
    m_dbFilePath = dbPath;
    m_userManager.setDatabasePath(dbPath);
    m_roleManager.setDatabasePath(dbPath);
    m_permissionManager.setDatabasePath(dbPath);
}

void RevokeHandler::setCurrentUser(const std::string& userName) {
    // 这个方法用于设置当前用户
}

bool RevokeHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 关键修复：重新设置数据库路径以触发数据重新加载，确保与文件同步
    if (!m_dbFilePath.empty()) {
        m_userManager.setDatabasePath(m_dbFilePath);      // 这会触发loadUsers()
        m_roleManager.setDatabasePath(m_dbFilePath);      // 这会触发loadRoles()和loadUserRoles()
        m_permissionManager.setDatabasePath(m_dbFilePath); // 这会触发loadPermissions()
    }
    
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    RevokeNode* revokeNode = dynamic_cast<RevokeNode*>(node.get());
    if (revokeNode == nullptr) {
        setError("Not a REVOKE statement");
        return false;
    }
    
    if (revokeNode->isRoleRevoke) {
        // 撤销角色
        if (!m_roleManager.revokeRoleFromUser(revokeNode->roleName, revokeNode->revokee)) {
            setError("Failed to revoke role from user");
            return false;
        }
    } else {
        // 撤销权限
        std::vector<PermissionType> permissionTypes;
        for (const auto& privStr : revokeNode->privilegeTypes) {
            permissionTypes.push_back(stringToPermissionType(privStr));
        }
        
        char objectType = stringToObjectType(revokeNode->objectType);
        
        // 判断是撤销用户还是角色的权限
        bool isRole = m_roleManager.roleExists(revokeNode->revokee);
        
        if (isRole) {
            if (!m_permissionManager.revokePermissionFromRole(
                    revokeNode->revokee, objectType, revokeNode->objectName, permissionTypes)) {
                setError("Failed to revoke permissions from role");
                return false;
            }
        } else {
            // 调用revokePermission（内部会重新加载权限并计算beforeCount/afterCount/removedCount）
            if (!m_permissionManager.revokePermission(
                    revokeNode->revokee, objectType, revokeNode->objectName, permissionTypes)) {
                setError("Failed to revoke permissions from user");
                return false;
            }
            
            // 使用PermissionManager内部的调试信息（它包含了beforeCount/afterCount/removedCount）
            std::string permDebugInfo = m_permissionManager.getLastDebugInfo();
            std::string debugInfo = "REVOKE executed. ";
            
            // 从PermissionManager的调试信息中提取关键数据
            // 格式：REVOKE DEBUG: Before=X, After=Y, Removed=Z, Remaining=...
            if (!permDebugInfo.empty() && permDebugInfo.find("REVOKE DEBUG:") == 0) {
                // 提取Before数量
                size_t beforePos = permDebugInfo.find("Before=");
                size_t afterPos = permDebugInfo.find(", After=");
                size_t removedPos = permDebugInfo.find(", Removed=");
                size_t remainingPos = permDebugInfo.find(", Remaining=");
                
                if (beforePos != std::string::npos && afterPos != std::string::npos) {
                    std::string beforeStr = permDebugInfo.substr(beforePos + 7, afterPos - beforePos - 7);
                    debugInfo += "Before: " + beforeStr + " permission(s) on " + revokeNode->objectName + ". ";
                }
                
                if (afterPos != std::string::npos && removedPos != std::string::npos) {
                    std::string afterStr = permDebugInfo.substr(afterPos + 8, removedPos - afterPos - 8);
                    debugInfo += "After: " + afterStr + " permission(s) remaining";
                    
                    // 提取Remaining信息
                    if (remainingPos != std::string::npos) {
                        std::string remainingStr = permDebugInfo.substr(remainingPos + 12);
                        if (remainingStr != "none" && !remainingStr.empty()) {
                            debugInfo += " (" + remainingStr + ")";
                        }
                    }
                    debugInfo += ". ";
                }
                
                // 提取Removed数量
                if (removedPos != std::string::npos) {
                    size_t commaPos = permDebugInfo.find(",", removedPos + 10);
                    std::string removedStr = permDebugInfo.substr(removedPos + 10, 
                                                                 commaPos != std::string::npos ? commaPos - removedPos - 10 : std::string::npos);
                    debugInfo += "Removed: " + removedStr + " permission(s).";
                }
            } else {
                // 回退方案：从PermissionManager获取afterCount
                std::vector<PermissionInfo> afterPerms;
                m_permissionManager.getUserPermissions(revokeNode->revokee, {}, afterPerms);
                size_t afterCount = 0;
                std::string remainingPerms = "";
                for (const auto& perm : afterPerms) {
                    if (perm.objectType == objectType && 
                        strcmp(perm.objectName, revokeNode->objectName.c_str()) == 0 &&
                        !perm.isRole) {
                        afterCount++;
                        if (!remainingPerms.empty()) remainingPerms += ", ";
                        remainingPerms += perm.permissionType;
                    }
                }
                debugInfo += "After: " + std::to_string(afterCount) + " permission(s) remaining";
                if (!remainingPerms.empty()) {
                    debugInfo += " (" + remainingPerms + ")";
                }
                debugInfo += ". ";
                debugInfo += "Removed: calculated from PermissionManager internal state.";
            }
            
            m_lastError = debugInfo;
            std::cerr << "RevokeHandler: " << debugInfo << std::endl;
        }
    }
    
    return true;
}

std::string RevokeHandler::getLastError() const {
    return m_lastError;
}

void RevokeHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "RevokeHandler错误: " << error << std::endl;
}

PermissionType RevokeHandler::stringToPermissionType(const std::string& str) const {
    return PermissionManager::stringToPermissionType(str);
}

char RevokeHandler::stringToObjectType(const std::string& str) const {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    
    if (upperStr == "TABLE") {
        return OBJECT_TYPE_TABLE;
    } else if (upperStr == "DATABASE") {
        return OBJECT_TYPE_DATABASE;
    } else if (upperStr == "SYSTEM") {
        return OBJECT_TYPE_SYSTEM;
    }
    return 0;
}

