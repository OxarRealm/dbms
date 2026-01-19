#pragma once

#include "core/user_mode.h"
#include <string>
#include <vector>
#include <map>
#include <set>

/**
 * @file permission_manager.h
 * @brief 权限管理器
 *
 * 负责权限的授予、撤销、检查等功能
 */

/**
 * @brief 权限类型枚举
 */
enum class PermissionType {
    SELECT,           // SELECT权限
    INSERT,           // INSERT权限
    UPDATE,           // UPDATE权限
    DELETE,           // DELETE权限
    ALTER,            // ALTER权限
    DROP,             // DROP权限
    CREATE_TABLE,     // CREATE TABLE权限
    DROP_DATABASE,    // DROP DATABASE权限
    CREATE_INDEX,     // CREATE INDEX权限
    CREATE_USER,      // CREATE USER权限
    GRANT_OPTION,     // GRANT OPTION权限
    ALL_PRIVILEGES    // ALL PRIVILEGES（所有权限）
};

/**
 * @brief 权限管理器类
 */
class PermissionManager {
public:
    PermissionManager();
    ~PermissionManager();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 授予权限给用户
     * @param userName 用户名
     * @param objectType 对象类型（1=表, 2=数据库, 3=系统）
     * @param objectName 对象名（表名或数据库名，系统权限为空）
     * @param permissionTypes 权限类型列表
     * @param withGrantOption 是否有GRANT OPTION
     * @param grantedBy 授予者（默认当前用户）
     * @return 成功返回true，失败返回false
     */
    bool grantPermission(const std::string& userName, char objectType, 
                        const std::string& objectName, 
                        const std::vector<PermissionType>& permissionTypes,
                        bool withGrantOption = false,
                        const std::string& grantedBy = "");

    /**
     * @brief 授予权限给角色
     * @param roleName 角色名
     * @param objectType 对象类型
     * @param objectName 对象名
     * @param permissionTypes 权限类型列表
     * @param withGrantOption 是否有GRANT OPTION
     * @param grantedBy 授予者
     * @return 成功返回true，失败返回false
     */
    bool grantPermissionToRole(const std::string& roleName, char objectType,
                               const std::string& objectName,
                               const std::vector<PermissionType>& permissionTypes,
                               bool withGrantOption = false,
                               const std::string& grantedBy = "");

    /**
     * @brief 撤销用户权限
     * @param userName 用户名
     * @param objectType 对象类型
     * @param objectName 对象名
     * @param permissionTypes 权限类型列表
     * @return 成功返回true，失败返回false
     */
    bool revokePermission(const std::string& userName, char objectType,
                         const std::string& objectName,
                         const std::vector<PermissionType>& permissionTypes);

    /**
     * @brief 撤销角色权限
     * @param roleName 角色名
     * @param objectType 对象类型
     * @param objectName 对象名
     * @param permissionTypes 权限类型列表
     * @return 成功返回true，失败返回false
     */
    bool revokePermissionFromRole(const std::string& roleName, char objectType,
                                  const std::string& objectName,
                                  const std::vector<PermissionType>& permissionTypes);

    /**
     * @brief 检查用户是否有权限
     * @param userName 用户名
     * @param objectType 对象类型
     * @param objectName 对象名
     * @param permissionType 权限类型
     * @param userRoles 用户角色列表（可选，用于检查角色权限）
     * @return 有权限返回true，否则返回false
     */
    bool hasPermission(const std::string& userName, char objectType,
                      const std::string& objectName, PermissionType permissionType,
                      const std::vector<std::string>& userRoles = std::vector<std::string>()) const;

    /**
     * @brief 获取用户的所有权限
     * @param userName 用户名
     * @param userRoles 用户角色列表（可选）
     * @param permissions 输出参数，权限信息列表
     * @return 成功返回true，失败返回false
     */
    bool getUserPermissions(const std::string& userName,
                           const std::vector<std::string>& userRoles,
                           std::vector<PermissionInfo>& permissions) const;

    /**
     * @brief 获取对象的所有权限
     * @param objectType 对象类型
     * @param objectName 对象名
     * @param permissions 输出参数，权限信息列表
     * @return 成功返回true，失败返回false
     */
    bool getObjectPermissions(char objectType, const std::string& objectName,
                             std::vector<PermissionInfo>& permissions) const;

    /**
     * @brief 获取角色的所有权限
     * @param roleName 角色名
     * @param permissions 输出参数，权限信息列表
     * @return 成功返回true，失败返回false
     */
    bool getRolePermissions(const std::string& roleName,
                           std::vector<PermissionInfo>& permissions) const;

    /**
     * @brief 权限类型转字符串
     * @param type 权限类型
     * @return 权限类型字符串
     */
    static std::string permissionTypeToString(PermissionType type);

    /**
     * @brief 字符串转权限类型
     * @param str 权限类型字符串
     * @return 权限类型
     */
    static PermissionType stringToPermissionType(const std::string& str);

    /**
     * @brief 解析权限类型列表（如"SELECT,INSERT,UPDATE"）
     * @param str 权限类型字符串
     * @return 权限类型列表
     */
    static std::vector<PermissionType> parsePermissionTypes(const std::string& str);

    /**
     * @brief 获取最后的调试信息（用于REVOKE操作的调试）
     * @return 调试信息字符串
     */
    std::string getLastDebugInfo() const;

private:
    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）
    std::vector<PermissionInfo> m_userPermissions;  // 用户权限列表
    std::vector<PermissionInfo> m_rolePermissions;  // 角色权限列表
    std::string m_lastDebugInfo;  // 最后的调试信息（用于REVOKE操作）

    /**
     * @brief 加载权限数据
     * @return 成功返回true，失败返回false
     */
    bool loadPermissions();

    /**
     * @brief 保存权限数据
     * @return 成功返回true，失败返回false
     */
    bool savePermissions() const;

    /**
     * @brief 检查权限是否包含指定类型（考虑ALL_PRIVILEGES）
     * @param permissionType 权限类型字符串
     * @param targetType 目标权限类型
     * @return 包含返回true，否则返回false
     */
    static bool permissionIncludes(const std::string& permissionType, PermissionType targetType);
};




