#pragma once

#include "core/user_mode.h"
#include <string>
#include <vector>
#include <map>

/**
 * @file role_manager.h
 * @brief 角色管理器
 *
 * 负责角色的创建、删除、分配等功能
 */

/**
 * @brief 角色管理器类
 */
class RoleManager {
public:
    RoleManager();
    ~RoleManager();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 创建角色
     * @param roleName 角色名
     * @param description 角色描述（可选）
     * @return 成功返回true，失败返回false
     */
    bool createRole(const std::string& roleName, const std::string& description = "");

    /**
     * @brief 删除角色
     * @param roleName 角色名
     * @return 成功返回true，失败返回false
     */
    bool deleteRole(const std::string& roleName);

    /**
     * @brief 检查角色是否存在
     * @param roleName 角色名
     * @return 存在返回true，不存在返回false
     */
    bool roleExists(const std::string& roleName) const;

    /**
     * @brief 获取角色信息
     * @param roleName 角色名
     * @param roleInfo 输出参数，角色信息
     * @return 成功返回true，失败返回false
     */
    bool getRoleInfo(const std::string& roleName, RoleInfo& roleInfo) const;

    /**
     * @brief 获取所有角色名
     * @param roleNames 输出参数，角色名列表
     * @return 成功返回true，失败返回false
     */
    bool getAllRoleNames(std::vector<std::string>& roleNames) const;

    /**
     * @brief 授予角色给用户
     * @param roleName 角色名
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool grantRoleToUser(const std::string& roleName, const std::string& userName);

    /**
     * @brief 从用户撤销角色
     * @param roleName 角色名
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool revokeRoleFromUser(const std::string& roleName, const std::string& userName);

    /**
     * @brief 获取用户的所有角色
     * @param userName 用户名
     * @param roleNames 输出参数，角色名列表
     * @return 成功返回true，失败返回false
     */
    bool getUserRoles(const std::string& userName, std::vector<std::string>& roleNames) const;

    /**
     * @brief 检查用户是否拥有角色
     * @param userName 用户名
     * @param roleName 角色名
     * @return 拥有返回true，否则返回false
     */
    bool userHasRole(const std::string& userName, const std::string& roleName) const;

private:
    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）
    std::map<std::string, RoleInfo> m_roles;  // 角色名 -> 角色信息映射
    std::map<std::string, std::vector<std::string>> m_userRoles;  // 用户名 -> 角色名列表映射

    /**
     * @brief 加载角色数据
     * @return 成功返回true，失败返回false
     */
    bool loadRoles();

    /**
     * @brief 保存角色数据
     * @return 成功返回true，失败返回false
     */
    bool saveRoles() const;

    /**
     * @brief 加载用户角色关联数据
     * @return 成功返回true，失败返回false
     */
    bool loadUserRoles();

    /**
     * @brief 保存用户角色关联数据
     * @return 成功返回true，失败返回false
     */
    bool saveUserRoles() const;
};




