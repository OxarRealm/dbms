#pragma once

#include "core/user_mode.h"
#include <string>

/**
 * @file session_manager.h
 * @brief 会话管理器
 *
 * 负责管理当前登录用户的会话状态
 */

/**
 * @brief 会话管理器类（单例模式）
 */
class SessionManager {
public:
    /**
     * @brief 获取单例实例
     * @return SessionManager引用
     */
    static SessionManager& getInstance();

    /**
     * @brief 登录（创建会话）
     * @param userName 用户名
     * @return 成功返回true，失败返回false
     */
    bool login(const std::string& userName);

    /**
     * @brief 登出（结束会话）
     */
    void logout();

    /**
     * @brief 获取当前登录用户名
     * @return 当前登录用户名（未登录返回空字符串）
     */
    std::string getCurrentUser() const;

    /**
     * @brief 检查是否已登录
     * @return 已登录返回true，否则返回false
     */
    bool isLoggedIn() const;

    /**
     * @brief 设置当前用户（用于测试，一般不直接调用）
     * @param userName 用户名
     */
    void setCurrentUser(const std::string& userName);

private:
    std::string m_currentUser;  // 当前登录用户名
    
    // 私有构造函数（单例模式）
    SessionManager();
    ~SessionManager();
    
    // 禁用拷贝构造和赋值（单例模式）
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
};




