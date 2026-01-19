#pragma once

#include "core/user_mode.h"
#include <string>
#include <vector>

/**
 * @file user_storage.h
 * @brief 用户权限存储管理器
 *
 * 负责将用户、角色、权限信息保存到.usr文件，并在数据库打开时加载
 */

/**
 * @brief 用户权限存储管理器类
 */
class UserStorageManager {
public:
    /**
     * @brief 获取用户权限文件路径
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径
     * @return 用户权限文件完整路径
     */
    static std::string getUserFilePath(const std::string& dbName, const std::string& dbPath);

    /**
     * @brief 获取角色文件路径
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径
     * @return 角色文件完整路径
     */
    static std::string getRoleFilePath(const std::string& dbName, const std::string& dbPath);

    /**
     * @brief 获取权限文件路径
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径
     * @return 权限文件完整路径
     */
    static std::string getPermissionFilePath(const std::string& dbName, const std::string& dbPath);
};




