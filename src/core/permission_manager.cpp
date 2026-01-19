/**
 * @file permission_manager.cpp
 * @brief 权限管理器实现
 */

#include "core/permission_manager.h"
#include "core/user_storage.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <sstream>

// 文件格式：
// [MAGIC_HEADER: "DBMS_PER_V1"]
// [权限数量: int]
// 对于每个权限：
//   [用户名/角色名长度: int] [用户名/角色名: char[]]
//   [对象类型: char]
//   [对象名长度: int] [对象名: char[]]
//   [权限类型长度: int] [权限类型: char[]]
//   [是否为角色权限: char] (1=角色, 0=用户)
//   [是否有GRANT OPTION: char] (1=有, 0=无)
//   [授予时间: time_t]
//   [授予者长度: int] [授予者: char[]]

const char* PERMISSION_MAGIC_HEADER = "DBMS_PER_V1";

PermissionManager::PermissionManager() {
}

PermissionManager::~PermissionManager() {
}

void PermissionManager::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
    // 自动加载权限数据
    loadPermissions();
}

bool PermissionManager::grantPermission(const std::string& userName, char objectType,
                                       const std::string& objectName,
                                       const std::vector<PermissionType>& permissionTypes,
                                       bool withGrantOption,
                                       const std::string& grantedBy) {
    if (userName.empty()) {
        std::cerr << "Error: User name cannot be empty" << std::endl;
        return false;
    }
    
    if (permissionTypes.empty()) {
        std::cerr << "Error: Permission types cannot be empty" << std::endl;
        return false;
    }
    
    // 关键修复：在操作前重新加载权限，确保使用最新的文件数据
    if (!m_dbFilePath.empty()) {
        loadPermissions();
    }
    
    // 为每个权限类型创建或更新权限记录
    for (PermissionType permType : permissionTypes) {
        std::string permTypeStr = permissionTypeToString(permType);
        
        // 检查是否已存在相同的权限记录
        bool found = false;
        for (auto& perm : m_userPermissions) {
            if (strcmp(perm.userName, userName.c_str()) == 0 &&
                perm.objectType == objectType &&
                strcmp(perm.objectName, objectName.c_str()) == 0 &&
                perm.permissionType == permTypeStr &&
                !perm.isRole) {
                // 更新现有权限（更新GRANT OPTION和授予信息）
                perm.withGrantOption = withGrantOption;
                perm.grantTime = std::time(nullptr);
                if (!grantedBy.empty()) {
                    strncpy(perm.grantedBy, grantedBy.c_str(), USER_NAME_LENGTH - 1);
                    perm.grantedBy[USER_NAME_LENGTH - 1] = '\0';
                }
                found = true;
                break;
            }
        }
        
        if (!found) {
            // 创建新权限记录
            PermissionInfo permInfo;
            strncpy(permInfo.userName, userName.c_str(), USER_NAME_LENGTH - 1);
            permInfo.userName[USER_NAME_LENGTH - 1] = '\0';
            permInfo.objectType = objectType;
            strncpy(permInfo.objectName, objectName.c_str(), OBJECT_NAME_LENGTH - 1);
            permInfo.objectName[OBJECT_NAME_LENGTH - 1] = '\0';
            permInfo.permissionType = permTypeStr;
            permInfo.isRole = false;
            permInfo.withGrantOption = withGrantOption;
            permInfo.grantTime = std::time(nullptr);
            if (!grantedBy.empty()) {
                strncpy(permInfo.grantedBy, grantedBy.c_str(), USER_NAME_LENGTH - 1);
                permInfo.grantedBy[USER_NAME_LENGTH - 1] = '\0';
            } else {
                permInfo.grantedBy[0] = '\0';
            }
            
            m_userPermissions.push_back(permInfo);
        }
    }
    
    // 保存到文件
    if (!savePermissions()) {
        std::cerr << "Error: Failed to save permission data" << std::endl;
        return false;
    }
    
    return true;
}

bool PermissionManager::grantPermissionToRole(const std::string& roleName, char objectType,
                                              const std::string& objectName,
                                              const std::vector<PermissionType>& permissionTypes,
                                              bool withGrantOption,
                                              const std::string& grantedBy) {
    if (roleName.empty()) {
        std::cerr << "Error: Role name cannot be empty" << std::endl;
        return false;
    }
    
    if (permissionTypes.empty()) {
        std::cerr << "Error: Permission types cannot be empty" << std::endl;
        return false;
    }
    
    // 关键修复：在操作前重新加载权限，确保使用最新的文件数据
    if (!m_dbFilePath.empty()) {
        loadPermissions();
    }
    
    // 为每个权限类型创建或更新权限记录
    for (PermissionType permType : permissionTypes) {
        std::string permTypeStr = permissionTypeToString(permType);
        
        // 检查是否已存在相同的权限记录
        bool found = false;
        for (auto& perm : m_rolePermissions) {
            if (strcmp(perm.userName, roleName.c_str()) == 0 &&
                perm.objectType == objectType &&
                strcmp(perm.objectName, objectName.c_str()) == 0 &&
                perm.permissionType == permTypeStr &&
                perm.isRole) {
                // 更新现有权限
                perm.withGrantOption = withGrantOption;
                perm.grantTime = std::time(nullptr);
                if (!grantedBy.empty()) {
                    strncpy(perm.grantedBy, grantedBy.c_str(), USER_NAME_LENGTH - 1);
                    perm.grantedBy[USER_NAME_LENGTH - 1] = '\0';
                }
                found = true;
                break;
            }
        }
        
        if (!found) {
            // 创建新权限记录
            PermissionInfo permInfo;
            strncpy(permInfo.userName, roleName.c_str(), USER_NAME_LENGTH - 1);
            permInfo.userName[USER_NAME_LENGTH - 1] = '\0';
            permInfo.objectType = objectType;
            strncpy(permInfo.objectName, objectName.c_str(), OBJECT_NAME_LENGTH - 1);
            permInfo.objectName[OBJECT_NAME_LENGTH - 1] = '\0';
            permInfo.permissionType = permTypeStr;
            permInfo.isRole = true;
            permInfo.withGrantOption = withGrantOption;
            permInfo.grantTime = std::time(nullptr);
            if (!grantedBy.empty()) {
                strncpy(permInfo.grantedBy, grantedBy.c_str(), USER_NAME_LENGTH - 1);
                permInfo.grantedBy[USER_NAME_LENGTH - 1] = '\0';
            } else {
                permInfo.grantedBy[0] = '\0';
            }
            
            m_rolePermissions.push_back(permInfo);
        }
    }
    
    // 保存到文件
    if (!savePermissions()) {
        std::cerr << "Error: Failed to save permission data" << std::endl;
        return false;
    }
    
    return true;
}

bool PermissionManager::revokePermission(const std::string& userName, char objectType,
                                         const std::string& objectName,
                                         const std::vector<PermissionType>& permissionTypes) {
    if (userName.empty()) {
        std::cerr << "Error: User name cannot be empty" << std::endl;
        return false;
    }
    
    // 关键修复：在操作前重新加载权限，确保使用最新的文件数据
    // 这样可以避免内存和文件不一致的问题
    if (!m_dbFilePath.empty()) {
        loadPermissions();
    }
    
    // 调试信息：输出撤销前的状态
    std::cerr << "=== REVOKE PERMISSION DEBUG ===" << std::endl;
    std::cerr << "User: " << userName << std::endl;
    std::cerr << "Object Type: " << (int)objectType << " (1=TABLE, 2=DATABASE, 3=SYSTEM)" << std::endl;
    std::cerr << "Object Name: " << objectName << std::endl;
    std::cerr << "Permissions to revoke: ";
    for (const auto& permType : permissionTypes) {
        std::cerr << permissionTypeToString(permType) << " ";
    }
    std::cerr << std::endl;
    
    // 调试：输出所有用户权限（不筛选）
    std::cerr << "Total user permissions in memory (after reload): " << m_userPermissions.size() << std::endl;
    for (size_t i = 0; i < m_userPermissions.size(); ++i) {
        const auto& perm = m_userPermissions[i];
        std::cerr << "  All perm[" << i << "]: User=" << perm.userName 
                  << ", ObjectType=" << (int)perm.objectType 
                  << ", Object=" << perm.objectName 
                  << ", Type=" << perm.permissionType 
                  << ", isRole=" << (perm.isRole ? "true" : "false") << std::endl;
    }
    
    // 统计撤销前的权限数量
    size_t beforeCount = 0;
    for (const auto& perm : m_userPermissions) {
        bool userMatch = (strcmp(perm.userName, userName.c_str()) == 0);
        bool objectTypeMatch = (perm.objectType == objectType);
        bool objectNameMatch = (strcmp(perm.objectName, objectName.c_str()) == 0);
        bool isUserPerm = !perm.isRole;
        
        if (userMatch && objectTypeMatch && objectNameMatch && isUserPerm) {
            beforeCount++;
            std::cerr << "  Before: User=" << perm.userName 
                      << ", Object=" << perm.objectName 
                      << ", Type=" << perm.permissionType 
                      << ", isRole=" << (perm.isRole ? "true" : "false") << std::endl;
        } else {
            // 调试：输出为什么不匹配
            if (userMatch && objectTypeMatch && isUserPerm) {
                std::cerr << "  Skipped (object name mismatch): User=" << perm.userName 
                          << ", Object=" << perm.objectName << " (expected: " << objectName << ")"
                          << ", Type=" << perm.permissionType << std::endl;
            } else if (userMatch && isUserPerm) {
                std::cerr << "  Skipped (object type mismatch): User=" << perm.userName 
                          << ", ObjectType=" << (int)perm.objectType << " (expected: " << (int)objectType << ")"
                          << ", Object=" << perm.objectName << std::endl;
            } else if (isUserPerm) {
                std::cerr << "  Skipped (user mismatch): User=" << perm.userName 
                          << " (expected: " << userName << ")"
                          << ", Object=" << perm.objectName << std::endl;
            }
        }
    }
    std::cerr << "Total matching permissions before: " << beforeCount << std::endl;
    
    // 移除指定的权限
    // 注意：如果权限是ALL_PRIVILEGES，撤销任何权限时都应该匹配
    // 使用更安全的方式：先收集要删除的权限类型字符串，然后一次性删除
    std::vector<std::string> permTypeStrs;
    for (PermissionType permType : permissionTypes) {
        std::string permTypeStr = permissionTypeToString(permType);
        std::string upperPermTypeStr = permTypeStr;
        std::transform(upperPermTypeStr.begin(), upperPermTypeStr.end(), 
                      upperPermTypeStr.begin(), ::toupper);
        permTypeStrs.push_back(upperPermTypeStr);
        std::cerr << "  Target permission type (upper): " << upperPermTypeStr << std::endl;
    }
    
    // 使用更安全的方式：先收集要删除的迭代器，然后删除
    // 这样可以避免remove_if的lambda捕获问题
    std::vector<size_t> indicesToRemove;
    for (size_t i = 0; i < m_userPermissions.size(); ++i) {
        const PermissionInfo& perm = m_userPermissions[i];
        
        // 检查用户名、对象类型、对象名是否匹配
        bool userMatch = (strcmp(perm.userName, userName.c_str()) == 0);
        bool objectTypeMatch = (perm.objectType == objectType);
        bool objectNameMatch = (strcmp(perm.objectName, objectName.c_str()) == 0);
        bool isUserPerm = !perm.isRole;
        
        if (!userMatch || !objectTypeMatch || !objectNameMatch || !isUserPerm) {
            continue;  // 不匹配，跳过
        }
        
        // 调试：输出所有检查的权限
        std::cerr << "  Checking permission[" << i << "]: User=" << perm.userName 
                  << ", Object=" << perm.objectName 
                  << ", Type=" << perm.permissionType 
                  << ", isRole=" << (perm.isRole ? "true" : "false") << std::endl;
        
        // 检查权限类型是否匹配
        // 如果权限是ALL_PRIVILEGES，撤销任何权限时都应该匹配
        // 否则需要精确匹配（大小写不敏感）
        std::string upperPermType = perm.permissionType;
        std::transform(upperPermType.begin(), upperPermType.end(), 
                      upperPermType.begin(), ::toupper);
        
        bool shouldRemove = false;
        if (upperPermType == "ALL_PRIVILEGES" || upperPermType == "ALL PRIVILEGES" || upperPermType == "ALL") {
            // 如果权限是ALL_PRIVILEGES，撤销任何权限时都应该匹配
            shouldRemove = true;
            std::cerr << "    -> Matched (ALL_PRIVILEGES)" << std::endl;
        } else {
            // 否则需要精确匹配（大小写不敏感）
            // 检查是否在要撤销的权限类型列表中
            for (const auto& upperPermTypeStr : permTypeStrs) {
                if (upperPermType == upperPermTypeStr) {
                    shouldRemove = true;
                    std::cerr << "    -> Matched (exact: " << upperPermType << " == " << upperPermTypeStr << ")" << std::endl;
                    break;
                }
            }
            if (!shouldRemove) {
                std::cerr << "    -> Not matched (exact: " << upperPermType << " not in target list)" << std::endl;
            }
        }
        
        if (shouldRemove) {
            indicesToRemove.push_back(i);
        }
    }
    
    // 从后往前删除，避免索引变化
    size_t removedCount = indicesToRemove.size();
    for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
        m_userPermissions.erase(m_userPermissions.begin() + *it);
    }
    
    // 调试信息：输出撤销后的状态
    std::cerr << "Removed " << removedCount << " permission(s)" << std::endl;
    size_t afterCount = 0;
    std::string remainingPerms = "";
    for (const auto& perm : m_userPermissions) {
        if (strcmp(perm.userName, userName.c_str()) == 0 && 
            perm.objectType == objectType && 
            strcmp(perm.objectName, objectName.c_str()) == 0 &&
            !perm.isRole) {
            afterCount++;
            if (!remainingPerms.empty()) remainingPerms += ", ";
            remainingPerms += perm.permissionType;
            std::cerr << "  After: User=" << perm.userName 
                      << ", Object=" << perm.objectName 
                      << ", Type=" << perm.permissionType 
                      << ", isRole=" << (perm.isRole ? "true" : "false") << std::endl;
        }
    }
    std::cerr << "Total matching permissions after: " << afterCount << std::endl;
    std::cerr << "=== END REVOKE DEBUG ===" << std::endl;
    
    // 保存调试信息到m_lastDebugInfo
    m_lastDebugInfo = "REVOKE DEBUG: Before=" + std::to_string(beforeCount) + 
                      ", After=" + std::to_string(afterCount) + 
                      ", Removed=" + std::to_string(removedCount) +
                      ", Remaining=" + (remainingPerms.empty() ? "none" : remainingPerms);
    
    // 保存到文件
    if (!savePermissions()) {
        std::cerr << "Error: Failed to save permission data" << std::endl;
        return false;
    }
    
    return true;
}

bool PermissionManager::revokePermissionFromRole(const std::string& roleName, char objectType,
                                                  const std::string& objectName,
                                                  const std::vector<PermissionType>& permissionTypes) {
    if (roleName.empty()) {
        std::cerr << "Error: Role name cannot be empty" << std::endl;
        return false;
    }
    
    // 关键修复：在操作前重新加载权限，确保使用最新的文件数据
    if (!m_dbFilePath.empty()) {
        loadPermissions();
    }
    
    // 移除指定的权限
    // 注意：如果权限是ALL_PRIVILEGES，撤销任何权限时都应该匹配
    // 使用更安全的方式：先收集要删除的权限类型字符串，然后一次性删除
    std::vector<std::string> permTypeStrs;
    for (PermissionType permType : permissionTypes) {
        std::string permTypeStr = permissionTypeToString(permType);
        std::string upperPermTypeStr = permTypeStr;
        std::transform(upperPermTypeStr.begin(), upperPermTypeStr.end(), 
                      upperPermTypeStr.begin(), ::toupper);
        permTypeStrs.push_back(upperPermTypeStr);
    }
    
    // 使用 erase-remove idiom，但确保只删除精确匹配的权限
    m_rolePermissions.erase(
        std::remove_if(m_rolePermissions.begin(), m_rolePermissions.end(),
            [&](const PermissionInfo& perm) {
                // 检查角色名、对象类型、对象名是否匹配
                bool roleMatch = (strcmp(perm.userName, roleName.c_str()) == 0);
                bool objectTypeMatch = (perm.objectType == objectType);
                bool objectNameMatch = (strcmp(perm.objectName, objectName.c_str()) == 0);
                bool isRolePerm = perm.isRole;
                
                if (!roleMatch || !objectTypeMatch || !objectNameMatch || !isRolePerm) {
                    return false;
                }
                
                // 检查权限类型是否匹配
                // 如果权限是ALL_PRIVILEGES，撤销任何权限时都应该匹配
                // 否则需要精确匹配（大小写不敏感）
                std::string upperPermType = perm.permissionType;
                std::transform(upperPermType.begin(), upperPermType.end(), 
                              upperPermType.begin(), ::toupper);
                
                if (upperPermType == "ALL_PRIVILEGES" || upperPermType == "ALL PRIVILEGES" || upperPermType == "ALL") {
                    // 如果权限是ALL_PRIVILEGES，撤销任何权限时都应该匹配
                    return true;
                } else {
                    // 否则需要精确匹配（大小写不敏感）
                    // 检查是否在要撤销的权限类型列表中
                    for (const auto& upperPermTypeStr : permTypeStrs) {
                        if (upperPermType == upperPermTypeStr) {
                            return true;
                        }
                    }
                    return false;
                }
            }),
        m_rolePermissions.end()
    );
    
    // 保存到文件
    if (!savePermissions()) {
        std::cerr << "Error: Failed to save permission data" << std::endl;
        return false;
    }
    
    return true;
}

bool PermissionManager::hasPermission(const std::string& userName, char objectType,
                                      const std::string& objectName, PermissionType permissionType,
                                      const std::vector<std::string>& userRoles) const {
    // 首先检查用户直接拥有的权限
    std::string targetPermStr = permissionTypeToString(permissionType);
    
    for (const auto& perm : m_userPermissions) {
        if (strcmp(perm.userName, userName.c_str()) == 0 &&
            perm.objectType == objectType &&
            strcmp(perm.objectName, objectName.c_str()) == 0 &&
            !perm.isRole) {
            // 检查权限类型是否匹配（考虑ALL_PRIVILEGES）
            if (permissionIncludes(perm.permissionType, permissionType)) {
                return true;
            }
        }
    }
    
    // 然后检查用户通过角色继承的权限
    for (const std::string& roleName : userRoles) {
        for (const auto& perm : m_rolePermissions) {
            if (strcmp(perm.userName, roleName.c_str()) == 0 &&
                perm.objectType == objectType &&
                strcmp(perm.objectName, objectName.c_str()) == 0 &&
                perm.isRole) {
                if (permissionIncludes(perm.permissionType, permissionType)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool PermissionManager::getUserPermissions(const std::string& userName,
                                           const std::vector<std::string>& userRoles,
                                           std::vector<PermissionInfo>& permissions) const {
    permissions.clear();
    
    // 获取用户直接拥有的权限
    for (const auto& perm : m_userPermissions) {
        if (strcmp(perm.userName, userName.c_str()) == 0 && !perm.isRole) {
            permissions.push_back(perm);
        }
    }
    
    // 获取用户通过角色继承的权限
    for (const std::string& roleName : userRoles) {
        for (const auto& perm : m_rolePermissions) {
            if (strcmp(perm.userName, roleName.c_str()) == 0 && perm.isRole) {
                permissions.push_back(perm);
            }
        }
    }
    
    return true;
}

bool PermissionManager::getObjectPermissions(char objectType, const std::string& objectName,
                                            std::vector<PermissionInfo>& permissions) const {
    permissions.clear();
    
    // 获取用户权限
    for (const auto& perm : m_userPermissions) {
        if (perm.objectType == objectType &&
            strcmp(perm.objectName, objectName.c_str()) == 0) {
            permissions.push_back(perm);
        }
    }
    
    // 获取角色权限
    for (const auto& perm : m_rolePermissions) {
        if (perm.objectType == objectType &&
            strcmp(perm.objectName, objectName.c_str()) == 0) {
            permissions.push_back(perm);
        }
    }
    
    return true;
}

bool PermissionManager::getRolePermissions(const std::string& roleName,
                                          std::vector<PermissionInfo>& permissions) const {
    permissions.clear();
    
    // 获取角色的所有权限
    for (const auto& perm : m_rolePermissions) {
        if (strcmp(perm.userName, roleName.c_str()) == 0 && perm.isRole) {
            permissions.push_back(perm);
        }
    }
    
    return true;
}

std::string PermissionManager::permissionTypeToString(PermissionType type) {
    switch (type) {
        case PermissionType::SELECT:
            return "SELECT";
        case PermissionType::INSERT:
            return "INSERT";
        case PermissionType::UPDATE:
            return "UPDATE";
        case PermissionType::DELETE:
            return "DELETE";
        case PermissionType::ALTER:
            return "ALTER";
        case PermissionType::DROP:
            return "DROP";
        case PermissionType::CREATE_TABLE:
            return "CREATE_TABLE";
        case PermissionType::DROP_DATABASE:
            return "DROP_DATABASE";
        case PermissionType::CREATE_INDEX:
            return "CREATE_INDEX";
        case PermissionType::CREATE_USER:
            return "CREATE_USER";
        case PermissionType::GRANT_OPTION:
            return "GRANT_OPTION";
        case PermissionType::ALL_PRIVILEGES:
            return "ALL_PRIVILEGES";
        default:
            return "UNKNOWN";
    }
}

PermissionType PermissionManager::stringToPermissionType(const std::string& str) {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    
    if (upperStr == "SELECT") {
        return PermissionType::SELECT;
    } else if (upperStr == "INSERT") {
        return PermissionType::INSERT;
    } else if (upperStr == "UPDATE") {
        return PermissionType::UPDATE;
    } else if (upperStr == "DELETE") {
        return PermissionType::DELETE;
    } else if (upperStr == "ALTER") {
        return PermissionType::ALTER;
    } else if (upperStr == "DROP") {
        return PermissionType::DROP;
    } else if (upperStr == "CREATE_TABLE" || upperStr == "CREATE TABLE") {
        return PermissionType::CREATE_TABLE;
    } else if (upperStr == "DROP_DATABASE" || upperStr == "DROP DATABASE") {
        return PermissionType::DROP_DATABASE;
    } else if (upperStr == "CREATE_INDEX" || upperStr == "CREATE INDEX") {
        return PermissionType::CREATE_INDEX;
    } else if (upperStr == "CREATE_USER" || upperStr == "CREATE USER") {
        return PermissionType::CREATE_USER;
    } else if (upperStr == "GRANT_OPTION" || upperStr == "GRANT OPTION") {
        return PermissionType::GRANT_OPTION;
    } else if (upperStr == "ALL_PRIVILEGES" || upperStr == "ALL PRIVILEGES" || upperStr == "ALL") {
        return PermissionType::ALL_PRIVILEGES;
    }
    
    return PermissionType::SELECT; // 默认返回SELECT
}

std::vector<PermissionType> PermissionManager::parsePermissionTypes(const std::string& str) {
    std::vector<PermissionType> result;
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    
    // 处理ALL PRIVILEGES
    if (upperStr.find("ALL") != std::string::npos || upperStr.find("ALL_PRIVILEGES") != std::string::npos) {
        result.push_back(PermissionType::ALL_PRIVILEGES);
        return result;
    }
    
    // 分割字符串（支持逗号和空格分隔）
    std::istringstream iss(upperStr);
    std::string token;
    while (std::getline(iss, token, ',')) {
        // 去除前后空格
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        
        if (!token.empty()) {
            PermissionType type = stringToPermissionType(token);
            result.push_back(type);
        }
    }
    
    return result;
}

bool PermissionManager::permissionIncludes(const std::string& permissionType, PermissionType targetType) {
    // 如果权限类型是ALL_PRIVILEGES，则包含所有权限
    std::string upperPermType = permissionType;
    std::transform(upperPermType.begin(), upperPermType.end(), upperPermType.begin(), ::toupper);
    
    if (upperPermType == "ALL_PRIVILEGES" || upperPermType == "ALL PRIVILEGES" || upperPermType == "ALL") {
        return true;
    }
    
    // 否则检查精确匹配
    std::string targetPermStr = permissionTypeToString(targetType);
    return upperPermType == targetPermStr;
}

bool PermissionManager::loadPermissions() {
    if (m_dbFilePath.empty()) {
        m_userPermissions.clear();
        m_rolePermissions.clear();
        return true;
    }
    
    std::string filePath = UserStorageManager::getPermissionFilePath(m_dbFilePath, m_dbFilePath);
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在不算错误
        m_userPermissions.clear();
        m_rolePermissions.clear();
        return true;
    }
    
    // 读取魔数头
    char header[16];
    file.read(header, strlen(PERMISSION_MAGIC_HEADER));
    if (file.gcount() != strlen(PERMISSION_MAGIC_HEADER) || 
        strncmp(header, PERMISSION_MAGIC_HEADER, strlen(PERMISSION_MAGIC_HEADER)) != 0) {
        std::cerr << "Error: Invalid permission file format" << std::endl;
        file.close();
        return false;
    }
    
    // 读取权限数量
    int permCount;
    file.read(reinterpret_cast<char*>(&permCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read permission count" << std::endl;
        file.close();
        return false;
    }
    
    m_userPermissions.clear();
    m_rolePermissions.clear();
    
    // 读取每个权限
    for (int i = 0; i < permCount; ++i) {
        PermissionInfo permInfo;
        
        // 读取用户名/角色名长度
        int nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
        if (file.gcount() != sizeof(int) || nameLen < 0 || nameLen >= USER_NAME_LENGTH) {
            std::cerr << "Error: Invalid name length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取用户名/角色名
        file.read(permInfo.userName, nameLen);
        if (file.gcount() != nameLen) {
            std::cerr << "Error: Failed to read user/role name" << std::endl;
            file.close();
            return false;
        }
        permInfo.userName[nameLen] = '\0';
        
        // 读取对象类型
        file.read(reinterpret_cast<char*>(&permInfo.objectType), sizeof(char));
        if (file.gcount() != sizeof(char)) {
            std::cerr << "Error: Failed to read object type" << std::endl;
            file.close();
            return false;
        }
        
        // 读取对象名长度
        int objNameLen;
        file.read(reinterpret_cast<char*>(&objNameLen), sizeof(int));
        if (file.gcount() != sizeof(int) || objNameLen < 0 || objNameLen >= OBJECT_NAME_LENGTH) {
            std::cerr << "Error: Invalid object name length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取对象名
        file.read(permInfo.objectName, objNameLen);
        if (file.gcount() != objNameLen) {
            std::cerr << "Error: Failed to read object name" << std::endl;
            file.close();
            return false;
        }
        permInfo.objectName[objNameLen] = '\0';
        
        // 读取权限类型长度
        int permTypeLen;
        file.read(reinterpret_cast<char*>(&permTypeLen), sizeof(int));
        if (file.gcount() != sizeof(int) || permTypeLen < 0) {
            std::cerr << "Error: Invalid permission type length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取权限类型
        if (permTypeLen > 0) {
            std::vector<char> permTypeBuf(permTypeLen + 1);
            file.read(permTypeBuf.data(), permTypeLen);
            if (file.gcount() != permTypeLen) {
                std::cerr << "Error: Failed to read permission type" << std::endl;
                file.close();
                return false;
            }
            permTypeBuf[permTypeLen] = '\0';
            permInfo.permissionType = permTypeBuf.data();
        }
        
        // 读取是否为角色权限
        char isRoleChar;
        file.read(&isRoleChar, sizeof(char));
        if (file.gcount() != sizeof(char)) {
            std::cerr << "Error: Failed to read isRole flag" << std::endl;
            file.close();
            return false;
        }
        permInfo.isRole = (isRoleChar == 1);
        
        // 读取是否有GRANT OPTION
        char grantOptionChar;
        file.read(&grantOptionChar, sizeof(char));
        if (file.gcount() != sizeof(char)) {
            std::cerr << "Error: Failed to read grant option flag" << std::endl;
            file.close();
            return false;
        }
        permInfo.withGrantOption = (grantOptionChar == 1);
        
        // 读取授予时间
        file.read(reinterpret_cast<char*>(&permInfo.grantTime), sizeof(time_t));
        if (file.gcount() != sizeof(time_t)) {
            std::cerr << "Error: Failed to read grant time" << std::endl;
            file.close();
            return false;
        }
        
        // 读取授予者长度
        int grantedByLen;
        file.read(reinterpret_cast<char*>(&grantedByLen), sizeof(int));
        if (file.gcount() != sizeof(int) || grantedByLen < 0 || grantedByLen >= USER_NAME_LENGTH) {
            std::cerr << "Error: Invalid granted by length" << std::endl;
            file.close();
            return false;
        }
        
        // 读取授予者
        if (grantedByLen > 0) {
            file.read(permInfo.grantedBy, grantedByLen);
            if (file.gcount() != grantedByLen) {
                std::cerr << "Error: Failed to read granted by" << std::endl;
                file.close();
                return false;
            }
            permInfo.grantedBy[grantedByLen] = '\0';
        } else {
            permInfo.grantedBy[0] = '\0';
        }
        
        // 根据isRole添加到对应的列表
        if (permInfo.isRole) {
            m_rolePermissions.push_back(permInfo);
        } else {
            m_userPermissions.push_back(permInfo);
        }
    }
    
    file.close();
    return true;
}

bool PermissionManager::savePermissions() const {
    if (m_dbFilePath.empty()) {
        std::cerr << "Error: Database path is not set" << std::endl;
        return false;
    }
    
    std::string filePath = UserStorageManager::getPermissionFilePath(m_dbFilePath, m_dbFilePath);
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create permission file: " << filePath << std::endl;
        return false;
    }
    
    // 写入魔数头
    file.write(PERMISSION_MAGIC_HEADER, strlen(PERMISSION_MAGIC_HEADER));
    if (!file.good()) {
        std::cerr << "Error: Failed to write magic header" << std::endl;
        file.close();
        return false;
    }
    
    // 计算总权限数量
    int totalPerms = static_cast<int>(m_userPermissions.size() + m_rolePermissions.size());
    
    // 写入权限数量
    file.write(reinterpret_cast<const char*>(&totalPerms), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write permission count" << std::endl;
        file.close();
        return false;
    }
    
    // 写入用户权限
    for (const auto& permInfo : m_userPermissions) {
        // 写入用户名长度和用户名
        int nameLen = static_cast<int>(strlen(permInfo.userName));
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write user name length" << std::endl;
            file.close();
            return false;
        }
        
        file.write(permInfo.userName, nameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write user name" << std::endl;
            file.close();
            return false;
        }
        
        // 写入对象类型
        file.write(reinterpret_cast<const char*>(&permInfo.objectType), sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write object type" << std::endl;
            file.close();
            return false;
        }
        
        // 写入对象名长度和对象名
        int objNameLen = static_cast<int>(strlen(permInfo.objectName));
        file.write(reinterpret_cast<const char*>(&objNameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write object name length" << std::endl;
            file.close();
            return false;
        }
        
        file.write(permInfo.objectName, objNameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write object name" << std::endl;
            file.close();
            return false;
        }
        
        // 写入权限类型长度和权限类型
        int permTypeLen = static_cast<int>(permInfo.permissionType.length());
        file.write(reinterpret_cast<const char*>(&permTypeLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write permission type length" << std::endl;
            file.close();
            return false;
        }
        
        if (permTypeLen > 0) {
            file.write(permInfo.permissionType.c_str(), permTypeLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write permission type" << std::endl;
                file.close();
                return false;
            }
        }
        
        // 写入是否为角色权限
        char isRoleChar = permInfo.isRole ? 1 : 0;
        file.write(&isRoleChar, sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write isRole flag" << std::endl;
            file.close();
            return false;
        }
        
        // 写入是否有GRANT OPTION
        char grantOptionChar = permInfo.withGrantOption ? 1 : 0;
        file.write(&grantOptionChar, sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write grant option flag" << std::endl;
            file.close();
            return false;
        }
        
        // 写入授予时间
        file.write(reinterpret_cast<const char*>(&permInfo.grantTime), sizeof(time_t));
        if (!file.good()) {
            std::cerr << "Error: Failed to write grant time" << std::endl;
            file.close();
            return false;
        }
        
        // 写入授予者长度和授予者
        int grantedByLen = static_cast<int>(strlen(permInfo.grantedBy));
        file.write(reinterpret_cast<const char*>(&grantedByLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write granted by length" << std::endl;
            file.close();
            return false;
        }
        
        if (grantedByLen > 0) {
            file.write(permInfo.grantedBy, grantedByLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write granted by" << std::endl;
                file.close();
                return false;
            }
        }
    }
    
    // 写入角色权限（格式相同）
    for (const auto& permInfo : m_rolePermissions) {
        int nameLen = static_cast<int>(strlen(permInfo.userName));
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write role name length" << std::endl;
            file.close();
            return false;
        }
        
        file.write(permInfo.userName, nameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write role name" << std::endl;
            file.close();
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(&permInfo.objectType), sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write object type" << std::endl;
            file.close();
            return false;
        }
        
        int objNameLen = static_cast<int>(strlen(permInfo.objectName));
        file.write(reinterpret_cast<const char*>(&objNameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write object name length" << std::endl;
            file.close();
            return false;
        }
        
        file.write(permInfo.objectName, objNameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write object name" << std::endl;
            file.close();
            return false;
        }
        
        int permTypeLen = static_cast<int>(permInfo.permissionType.length());
        file.write(reinterpret_cast<const char*>(&permTypeLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write permission type length" << std::endl;
            file.close();
            return false;
        }
        
        if (permTypeLen > 0) {
            file.write(permInfo.permissionType.c_str(), permTypeLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write permission type" << std::endl;
                file.close();
                return false;
            }
        }
        
        char isRoleChar = permInfo.isRole ? 1 : 0;
        file.write(&isRoleChar, sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write isRole flag" << std::endl;
            file.close();
            return false;
        }
        
        char grantOptionChar = permInfo.withGrantOption ? 1 : 0;
        file.write(&grantOptionChar, sizeof(char));
        if (!file.good()) {
            std::cerr << "Error: Failed to write grant option flag" << std::endl;
            file.close();
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(&permInfo.grantTime), sizeof(time_t));
        if (!file.good()) {
            std::cerr << "Error: Failed to write grant time" << std::endl;
            file.close();
            return false;
        }
        
        int grantedByLen = static_cast<int>(strlen(permInfo.grantedBy));
        file.write(reinterpret_cast<const char*>(&grantedByLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write granted by length" << std::endl;
            file.close();
            return false;
        }
        
        if (grantedByLen > 0) {
            file.write(permInfo.grantedBy, grantedByLen);
            if (!file.good()) {
                std::cerr << "Error: Failed to write granted by" << std::endl;
                file.close();
                return false;
            }
        }
    }
    
    file.close();
    return true;
}

std::string PermissionManager::getLastDebugInfo() const {
    return m_lastDebugInfo;
}
