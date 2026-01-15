#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include <string>
#include <memory>

/**
 * @file insert_handler.h
 * @brief INSERT语句处理器
 *
 * 负责处理INSERT SQL语句，调用DataManager插入记录
 */

/**
 * @brief INSERT语句处理器类
 */
class InsertHandler {
public:
    InsertHandler();
    ~InsertHandler();
    
    /**
     * @brief 执行INSERT语句
     * @param sql INSERT SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
private:
    TableManager m_tableManager;   // 表管理器
    DataManager m_dataManager;      // 数据管理器
    std::string m_lastError;       // 最后的错误信息
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 验证插入的数据
     * @param node INSERT AST节点
     * @param tableInfo 表结构信息
     * @return 验证通过返回true，否则返回false
     */
    bool validateInsertData(InsertNode* node, const TableInfo& tableInfo);
    
    /**
     * @brief 转换值类型（字符串到对应的数据类型）
     * @param value 字符串值
     * @param fieldType 字段类型
     * @return 转换后的值（字符串形式）
     */
    std::string convertValue(const std::string& value, const std::string& fieldType);
    
    /**
     * @brief 创建记录
     * @param node INSERT AST节点
     * @param tableInfo 表结构信息
     * @return Record结构
     */
    Record createRecord(InsertNode* node, const TableInfo& tableInfo);
    
    /**
     * @brief 检查主键唯一性约束
     * @param tableName 表名
     * @param tableInfo 表结构信息
     * @param record 要插入的记录
     * @return 主键唯一返回true，否则返回false
     */
    bool checkPrimaryKeyUnique(const std::string& tableName, const TableInfo& tableInfo, const Record& record);
};

