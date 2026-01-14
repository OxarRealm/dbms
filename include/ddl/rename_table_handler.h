#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include <string>
#include <memory>

/**
 * @file rename_table_handler.h
 * @brief RENAME TABLE语句处理器
 *
 * 负责处理RENAME TABLE SQL语句，更新.dbf和.dat文件中的表名
 */

/**
 * @brief RENAME TABLE语句处理器类
 */
class RenameTableHandler {
public:
    RenameTableHandler();
    ~RenameTableHandler();
    
    /**
     * @brief 执行RENAME TABLE语句
     * @param sql RENAME TABLE SQL语句
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
     * @brief 验证重命名参数
     * @param node RENAME TABLE AST节点
     * @return 验证通过返回true，否则返回false
     */
    bool validateRenameParameters(RenameTableNode* node);
    
    /**
     * @brief 重命名表（更新.dbf和.dat文件）
     * @param node RENAME TABLE AST节点
     * @return 成功返回true，失败返回false
     */
    bool renameTable(RenameTableNode* node);
};

