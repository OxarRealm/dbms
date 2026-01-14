#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include <string>
#include <memory>

/**
 * @file delete_handler.h
 * @brief DELETE语句处理器
 *
 * 负责处理DELETE SQL语句，调用DataManager删除记录
 */

/**
 * @brief DELETE语句处理器类
 */
class DeleteHandler {
public:
    DeleteHandler();
    ~DeleteHandler();
    
    /**
     * @brief 执行DELETE语句
     * @param sql DELETE SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
    /**
     * @brief 获取删除的记录数量
     */
    size_t getDeletedCount() const;
    
private:
    TableManager m_tableManager;   // 表管理器
    DataManager m_dataManager;      // 数据管理器
    std::string m_lastError;       // 最后的错误信息
    size_t m_deletedCount;         // 删除的记录数量
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 评估条件（WHERE子句）
     * @param record 记录
     * @param tableInfo 表结构信息
     * @param conditionField 条件字段名
     * @param conditionValue 条件值
     * @return 条件匹配返回true，否则返回false
     */
    bool evaluateCondition(const Record& record, const TableInfo& tableInfo, 
                          const std::string& conditionField, const std::string& conditionValue);
    
    /**
     * @brief 查找字段索引
     * @param tableInfo 表结构信息
     * @param fieldName 字段名
     * @return 字段索引，如果不存在返回-1
     */
    int findFieldIndex(const TableInfo& tableInfo, const std::string& fieldName);
};

