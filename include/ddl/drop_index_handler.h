#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/index_manager.h"
#include <string>
#include <memory>

/**
 * @file drop_index_handler.h
 * @brief DROP INDEX语句处理器
 *
 * 负责处理DROP INDEX SQL语句，调用IndexManager删除索引
 */

/**
 * @brief DROP INDEX语句处理器类
 */
class DropIndexHandler {
public:
    DropIndexHandler();
    ~DropIndexHandler();
    
    /**
     * @brief 执行DROP INDEX语句
     * @param sql DROP INDEX SQL语句
     * @param basePath 基础数据库路径（用于解析SQL中的相对路径）
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql, const std::string& basePath = "");
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
private:
    IndexManager m_indexManager;   // 索引管理器
    std::string m_lastError;       // 最后的错误信息
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 验证索引删除参数
     * @param node DROP INDEX AST节点
     * @param dbPath 数据库路径（已解析的完整路径）
     * @return 验证通过返回true，否则返回false
     */
    bool validateIndexDeletion(DropIndexNode* node, const std::string& dbPath);
};



