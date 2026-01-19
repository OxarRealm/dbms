#pragma once

#include "core/user_mode.h"
#include <string>
#include <vector>
#include <map>

/**
 * @file user_manager.h
 * @brief 用户管理器
 *
 * 负责用户的创建、修改、删除、认证等功能
 */

/**
 * @brief 用户管理器类
 */
class UserManager {
public:
    UserManager();
    ~UserManager();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 创建用户
     * @param userName 用户名
     * @param password 密码（明文）
     * @return 成功返回true，失败返回false
     */
    bool createUser(const std::string& userName, const std::string& password);

    /**
     * @brief 修改用户密码
     * @param userName 用户名
     * @param newPassword 新密码（明文）
     * @return 成功返回true，失败返回false
     */
    bool changePassword(const std::string& userName, const std::string& newPassword);

    /**
     * @brief 启用用户
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool enableUser(const std::string& userName);

    /**
     * @brief 禁用用户
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool disableUser(const std::string& userName);

    /**
     * @brief 删除用户
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool deleteUser(const std::string& userName);

    /**
     * @brief 用户认证（验证用户名和密码）
     * @param userName 用户名
     * @param password 密码（明文）
     * @return 认证成功返回true，失败返回false
     */
    bool authenticate(const std::string& userName, const std::string& password);

    /**
     * @brief 检查用户是否存在
     * @param userName 用户名
     * @return 存在返回true，不存在返回false
     */
    bool userExists(const std::string& userName) const;

    /**
     * @brief 获取用户信息
     * @param userName 用户名
     * @param userInfo 输出参数，用户信息
     * @return 成功返回true，失败返回false
     */
    bool getUserInfo(const std::string& userName, UserInfo& userInfo) const;

    /**
     * @brief 获取所有用户名
     * @param userNames 输出参数，用户名列表
     * @return 成功返回true，失败返回false
     */
    bool getAllUserNames(std::vector<std::string>& userNames) const;

    /**
     * @brief 更新用户最后登录时间
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool updateLastLoginTime(const std::string& userName);

    /**
     * @brief 计算密码哈希值（SHA256）
     * @param password 密码（明文）
     * @return 密码哈希值（十六进制字符串）
     */
    static std::string hashPassword(const std::string& password);

    /**
     * @brief 验证密码哈希值
     * @param password 密码（明文）
     * @param hash 密码哈希值
     * @return 匹配返回true，不匹配返回false
     */
    static bool verifyPassword(const std::string& password, const std::string& hash);

private:
    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）
    std::map<std::string, UserInfo> m_users;  // 用户名 -> 用户信息映射

    /**
     * @brief 加载用户数据
     * @return 成功返回true，失败返回false
     */
    bool loadUsers();

    /**
     * @brief 保存用户数据
     * @return 成功返回true，失败返回false
     */
    bool saveUsers() const;
};




