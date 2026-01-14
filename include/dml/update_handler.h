#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include <string>
#include <memory>

/**
 * @file update_handler.h
 * @brief UPDATE语句处理器
 *
 * 负责处理UPDATE SQL语句，调用DataManager更新记录
 */

/**
 * @brief UPDATE语句处理器类
 */
class UpdateHandler {
public:
    UpdateHandler();
    ~UpdateHandler();
    
    /**
     * @brief 执行UPDATE语句
     * @param sql UPDATE SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
    /**
     * @brief 获取更新的记录数量
     */
    size_t getUpdatedCount() const;
    
private:
    TableManager m_tableManager;   // 表管理器
    DataManager m_dataManager;      // 数据管理器
    std::string m_lastError;       // 最后的错误信息
    size_t m_updatedCount;         // 更新的记录数量
    
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
    
    /**
     * @brief 转换值类型（字符串到对应的数据类型）
     * @param value 字符串值
     * @param fieldType 字段类型
     * @return 转换后的值（字符串形式）
     */
    std::string convertValue(const std::string& value, const std::string& fieldType);
    
    /**
     * @brief 验证更新值
     * @param value 新值
     * @param field 字段定义
     * @return 验证通过返回true，否则返回false
     */
    bool validateUpdateValue(const std::string& value, const TableMode& field);
};

