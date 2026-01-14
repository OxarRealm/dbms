#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include <string>
#include <memory>

/**
 * @file edit_table_handler.h
 * @brief EDIT TABLE语句处理器
 *
 * 负责处理EDIT TABLE SQL语句，调用TableManager更新表结构
 */

/**
 * @brief EDIT TABLE语句处理器类
 */
class EditTableHandler {
public:
    EditTableHandler();
    ~EditTableHandler();
    
    /**
     * @brief 执行EDIT TABLE语句
     * @param sql EDIT TABLE SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
private:
    TableManager m_tableManager;   // 表管理器
    std::string m_lastError;       // 最后的错误信息
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 验证字段定义
     * @param node EDIT TABLE AST节点
     * @return 验证通过返回true，否则返回false
     */
    bool validateFieldDefinition(EditTableNode* node);
    
    /**
     * @brief 更新表结构（添加或修改字段）
     * @param node EDIT TABLE AST节点
     * @return 成功返回true，失败返回false
     */
    bool updateTableStructure(EditTableNode* node);
};

