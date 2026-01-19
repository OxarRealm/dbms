#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <ctime>

/**
 * @file user_mode.h
 * @brief 用户权限数据结构定义
 *
 * 定义用户、角色、权限等核心数据结构
 */

// 常量定义
#define USER_NAME_LENGTH 32           // 用户名最大长度
#define PASSWORD_HASH_LENGTH 64       // 密码哈希最大长度（SHA256）
#define ROLE_NAME_LENGTH 32           // 角色名最大长度
#define PERMISSION_TYPE_LENGTH 16     // 权限类型最大长度
#define OBJECT_NAME_LENGTH 64         // 对象名最大长度（表名或数据库名）

// 用户状态常量
#define USER_STATUS_ENABLED 1         // 用户启用
#define USER_STATUS_DISABLED 0        // 用户禁用

// 对象类型常量
#define OBJECT_TYPE_TABLE 1           // 表对象
#define OBJECT_TYPE_DATABASE 2        // 数据库对象
#define OBJECT_TYPE_SYSTEM 3          // 系统对象

/**
 * @brief 用户信息结构
 */
struct UserInfo {
    char userName[USER_NAME_LENGTH];          // 用户名
    char passwordHash[PASSWORD_HASH_LENGTH];  // 密码哈希（SHA256）
    time_t createTime;                        // 创建时间
    time_t lastLoginTime;                     // 最后登录时间
    char status;                              // 状态（1=启用, 0=禁用）
    
    UserInfo() : createTime(0), lastLoginTime(0), status(USER_STATUS_ENABLED) {
        userName[0] = '\0';
        passwordHash[0] = '\0';
    }
    
    UserInfo(const char* name) : createTime(std::time(nullptr)), 
                                   lastLoginTime(0), 
                                   status(USER_STATUS_ENABLED) {
        strncpy(userName, name, USER_NAME_LENGTH - 1);
        userName[USER_NAME_LENGTH - 1] = '\0';
        passwordHash[0] = '\0';
    }
    
    bool isEnabled() const {
        return status == USER_STATUS_ENABLED;
    }
    
    void setEnabled(bool enabled) {
        status = enabled ? USER_STATUS_ENABLED : USER_STATUS_DISABLED;
    }
};

/**
 * @brief 角色信息结构
 */
struct RoleInfo {
    char roleName[ROLE_NAME_LENGTH];  // 角色名
    std::string description;          // 角色描述
    time_t createTime;                // 创建时间
    
    RoleInfo() : createTime(0) {
        roleName[0] = '\0';
    }
    
    RoleInfo(const char* name) : createTime(std::time(nullptr)) {
        strncpy(roleName, name, ROLE_NAME_LENGTH - 1);
        roleName[ROLE_NAME_LENGTH - 1] = '\0';
    }
};

/**
 * @brief 权限信息结构
 */
struct PermissionInfo {
    char userName[USER_NAME_LENGTH];          // 用户名（如果直接授予用户）或角色名（如果授予角色）
    char objectType;                          // 对象类型（1=表, 2=数据库, 3=系统）
    char objectName[OBJECT_NAME_LENGTH];      // 对象名（表名或数据库名，系统权限为空）
    std::string permissionType;               // 权限类型（SELECT, INSERT, UPDATE, DELETE, ALTER, DROP, CREATE TABLE等）
    bool isRole;                              // 是否为角色权限（true=角色权限, false=用户权限）
    bool withGrantOption;                     // 是否有GRANT OPTION
    time_t grantTime;                         // 授予时间
    char grantedBy[USER_NAME_LENGTH];         // 授予者
    
    PermissionInfo() : objectType(0), isRole(false), withGrantOption(false), grantTime(0) {
        userName[0] = '\0';
        objectName[0] = '\0';
        grantedBy[0] = '\0';
    }
};

/**
 * @brief 用户角色关联结构
 */
struct UserRoleInfo {
    char userName[USER_NAME_LENGTH];  // 用户名
    char roleName[ROLE_NAME_LENGTH];  // 角色名
    time_t assignTime;                // 分配时间
    
    UserRoleInfo() : assignTime(0) {
        userName[0] = '\0';
        roleName[0] = '\0';
    }
};




