#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/user_manager.h"
#include "core/role_manager.h"
#include "core/permission_manager.h"
#include "core/session_manager.h"
#include <string>
#include <memory>

/**
 * @file user_permission_handler.h
 * @brief 用户权限相关SQL语句处理器
 *
 * 负责处理CREATE USER、ALTER USER、DROP USER、CREATE ROLE、
 * DROP ROLE、GRANT、REVOKE等SQL语句
 */

/**
 * @brief CREATE USER语句处理器
 */
class CreateUserHandler {
public:
    CreateUserHandler();
    ~CreateUserHandler();

    /**
     * @brief 设置数据库文件路径
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief 执行CREATE USER语句
     * @param sql CREATE USER SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);

    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;

private:
    UserManager m_userManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
};

/**
 * @brief ALTER USER语句处理器
 */
class AlterUserHandler {
public:
    AlterUserHandler();
    ~AlterUserHandler();

    void setDatabasePath(const std::string& dbPath);
    bool execute(const std::string& sql);
    std::string getLastError() const;

private:
    UserManager m_userManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
};

/**
 * @brief DROP USER语句处理器
 */
class DropUserHandler {
public:
    DropUserHandler();
    ~DropUserHandler();

    void setDatabasePath(const std::string& dbPath);
    bool execute(const std::string& sql);
    std::string getLastError() const;

private:
    UserManager m_userManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
};

/**
 * @brief CREATE ROLE语句处理器
 */
class CreateRoleHandler {
public:
    CreateRoleHandler();
    ~CreateRoleHandler();

    void setDatabasePath(const std::string& dbPath);
    bool execute(const std::string& sql);
    std::string getLastError() const;

private:
    RoleManager m_roleManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
};

/**
 * @brief DROP ROLE语句处理器
 */
class DropRoleHandler {
public:
    DropRoleHandler();
    ~DropRoleHandler();

    void setDatabasePath(const std::string& dbPath);
    bool execute(const std::string& sql);
    std::string getLastError() const;

private:
    RoleManager m_roleManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
};

/**
 * @brief GRANT语句处理器
 */
class GrantHandler {
public:
    GrantHandler();
    ~GrantHandler();

    void setDatabasePath(const std::string& dbPath);
    void setCurrentUser(const std::string& userName);
    bool execute(const std::string& sql);
    std::string getLastError() const;

private:
    UserManager m_userManager;
    RoleManager m_roleManager;
    PermissionManager m_permissionManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
    
    /**
     * @brief 转换字符串权限类型到PermissionType枚举
     */
    PermissionType stringToPermissionType(const std::string& str) const;
    
    /**
     * @brief 转换对象类型字符串到char类型码
     */
    char stringToObjectType(const std::string& str) const;
};

/**
 * @brief REVOKE语句处理器
 */
class RevokeHandler {
public:
    RevokeHandler();
    ~RevokeHandler();

    void setDatabasePath(const std::string& dbPath);
    void setCurrentUser(const std::string& userName);
    bool execute(const std::string& sql);
    std::string getLastError() const;

private:
    UserManager m_userManager;
    RoleManager m_roleManager;
    PermissionManager m_permissionManager;
    std::string m_dbFilePath;  // 保存数据库路径，用于重新加载数据
    std::string m_lastError;
    void setError(const std::string& error);
    
    PermissionType stringToPermissionType(const std::string& str) const;
    char stringToObjectType(const std::string& str) const;
};

