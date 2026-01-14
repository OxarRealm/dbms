#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include <string>
#include <memory>

/**
 * @file drop_table_handler.h
 * @brief DROP TABLE语句处理器
 *
 * 负责处理DROP TABLE SQL语句，删除表结构和数据
 */

/**
 * @brief DROP TABLE语句处理器类
 */
class DropTableHandler {
public:
    DropTableHandler();
    ~DropTableHandler();
    
    /**
     * @brief 执行DROP TABLE语句
     * @param sql DROP TABLE SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
private:
    TableManager m_tableManager;   // 表管理器
    DataManager m_dataManager;     // 数据管理器
    std::string m_lastError;       // 最后的错误信息
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 验证删除参数
     * @param node DROP TABLE AST节点
     * @return 验证通过返回true，否则返回false
     */
    bool validateDropParameters(DropTableNode* node);
    
    /**
     * @brief 删除表（删除.dbf和.dat文件中的表）
     * @param node DROP TABLE AST节点
     * @return 成功返回true，失败返回false
     */
    bool dropTable(DropTableNode* node);
};

