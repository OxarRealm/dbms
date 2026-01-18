#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include <string>
#include <memory>
#include <vector>

/**
 * @file select_handler.h
 * @brief SELECT查询处理器
 *
 * 负责处理SELECT SQL语句，执行查询并返回结果
 */

/**
 * @brief 查询结果结构
 */
struct QueryResult {
    std::vector<std::string> columnNames;     // 列名列表
    std::vector<std::vector<std::string>> rows;  // 行数据列表
    size_t rowCount;                          // 行数
    
    QueryResult() : rowCount(0) {}
};

/**
 * @brief SELECT语句处理器类
 */
class SelectHandler {
public:
    SelectHandler();
    ~SelectHandler();
    
    /**
     * @brief 执行SELECT语句
     * @param sql SELECT SQL语句
     * @param result 输出参数，查询结果
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql, QueryResult& result);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
    /**
     * @brief 设置数据库路径
     * @param dbPath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbPath);
    
private:
    TableManager m_tableManager;   // 表管理器
    DataManager m_dataManager;      // 数据管理器
    std::string m_lastError;       // 最后的错误信息
    
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
     * @brief 评估复杂WHERE条件（支持AND, OR, NOT）
     * @param record 记录
     * @param tableInfo 表结构信息
     * @param condition WHERE条件节点
     * @param outerRecord 外部查询的当前记录（用于关联子查询，可选）
     * @param outerTableInfo 外部查询的表结构信息（用于关联子查询，可选）
     * @param currentTableName 当前查询的表名（用于区分TableName.FieldName中的表名，可选）
     * @return 条件匹配返回true，否则返回false
     */
    bool evaluateWhereCondition(const Record& record, const TableInfo& tableInfo, 
                               const WhereCondition* condition,
                               const Record* outerRecord = nullptr,
                               const TableInfo* outerTableInfo = nullptr,
                               const std::string* currentTableName = nullptr);
    
    /**
     * @brief 查找字段索引
     * @param tableInfo 表结构信息
     * @param fieldName 字段名
     * @return 字段索引，如果不存在返回-1
     */
    int findFieldIndex(const TableInfo& tableInfo, const std::string& fieldName);
    
    /**
     * @brief 执行单表查询
     * @param node SELECT AST节点
     * @param result 输出参数，查询结果
     * @return 成功返回true，失败返回false
     */
    bool executeSingleTableQuery(SelectNode* node, QueryResult& result);
    
    /**
     * @brief 执行单表查询（支持外部查询上下文，用于关联子查询）
     * @param node SELECT AST节点
     * @param result 输出参数，查询结果
     * @param outerRecord 外部查询的当前记录（用于关联子查询，可选）
     * @param outerTableInfo 外部查询的表结构信息（用于关联子查询，可选）
     * @return 成功返回true，失败返回false
     */
    bool executeSingleTableQueryWithContext(SelectNode* node, QueryResult& result,
                                            const Record* outerRecord = nullptr,
                                            const TableInfo* outerTableInfo = nullptr);
    
    /**
     * @brief 执行多表查询（笛卡尔积）
     * @param node SELECT AST节点
     * @param result 输出参数，查询结果
     * @return 成功返回true，失败返回false
     */
    bool executeMultiTableQuery(SelectNode* node, QueryResult& result);
    
    /**
     * @brief 执行UNION查询
     * @param node SELECT AST节点（包含UNION查询）
     * @param result 输出参数，查询结果
     * @return 成功返回true，失败返回false
     */
    bool executeUnionQuery(SelectNode* node, QueryResult& result);
    
    /**
     * @brief 执行子查询
     * @param node SELECT AST节点（子查询）
     * @param result 输出参数，查询结果
     * @param outerRecord 外部查询的当前记录（用于关联子查询，可选）
     * @param outerTableInfo 外部查询的表结构信息（用于关联子查询，可选）
     * @return 成功返回true，失败返回false
     */
    bool executeSubquery(SelectNode* node, QueryResult& result,
                        const Record* outerRecord = nullptr,
                        const TableInfo* outerTableInfo = nullptr);
    
    /**
     * @brief 执行JOIN连接查询
     * @param node SELECT AST节点
     * @param result 输出参数，查询结果
     * @return 成功返回true，失败返回false
     */
    bool executeJoinQuery(SelectNode* node, QueryResult& result);
    
    /**
     * @brief 执行GROUP BY分组查询（包含聚合函数）
     * @param node SELECT AST节点
     * @param tableInfo 表结构信息
     * @param records 记录列表
     * @param result 输出参数，查询结果
     * @param outerRecord 外部查询的当前记录（用于关联子查询，可选）
     * @param outerTableInfo 外部查询的表结构信息（用于关联子查询，可选）
     * @param currentTableName 当前查询的表名（用于关联子查询，可选）
     * @return 成功返回true，失败返回false
     */
    bool executeGroupByQuery(SelectNode* node, const TableInfo& tableInfo,
                            const std::vector<Record>& records, QueryResult& result,
                            const Record* outerRecord = nullptr,
                            const TableInfo* outerTableInfo = nullptr,
                            const std::string* currentTableName = nullptr);
    
    /**
     * @brief 执行投影操作（选择字段）
     * @param record 记录
     * @param tableInfo 表结构信息
     * @param selectFields 要选择的字段列表（*表示所有字段）
     * @param row 输出参数，投影后的行数据
     * @return 成功返回true，失败返回false
     */
    bool projectFields(const Record& record, const TableInfo& tableInfo, 
                      const std::vector<std::string>& selectFields, 
                      std::vector<std::string>& row);
    
    /**
     * @brief 执行多表投影操作（从多个表的记录组合中选择字段）
     * @param combinedRecord 组合后的记录（多个表的字段值拼接）
     * @param tableInfos 表结构信息列表
     * @param selectFields 要选择的字段列表（格式：TableName.FieldName 或 FieldName）
     * @param row 输出参数，投影后的行数据
     * @return 成功返回true，失败返回false
     */
    bool projectMultiTableFields(const std::vector<std::string>& combinedRecord, 
                                 const std::vector<TableInfo>& tableInfos,
                                 const std::vector<std::string>& selectFields, 
                                 std::vector<std::string>& row);
    
    /**
     * @brief 评估多表WHERE条件
     * @param combinedRecord 组合后的记录
     * @param tableInfos 表结构信息列表
     * @param whereField 条件字段名（格式：TableName.FieldName 或 FieldName）
     * @param whereValue 条件值
     * @return 条件匹配返回true，否则返回false
     */
    bool evaluateMultiTableCondition(const std::vector<std::string>& combinedRecord, 
                                     const std::vector<TableInfo>& tableInfos,
                                     const std::string& whereField, 
                                     const std::string& whereValue);
    
    /**
     * @brief 查找字段索引（多表）
     * @param tableInfos 表结构信息列表
     * @param fieldName 字段名（格式：TableName.FieldName 或 FieldName）
     * @param tableIndex 输出参数，表索引
     * @param fieldIndex 输出参数，字段索引
     * @return 成功返回true，失败返回false
     */
    bool findMultiTableFieldIndex(const std::vector<TableInfo>& tableInfos, 
                                  const std::string& fieldName,
                                  size_t& tableIndex, int& fieldIndex);
    
    /**
     * @brief 生成笛卡尔积（递归辅助函数）
     * @param allTableRecords 所有表的记录列表
     * @param tableIndex 当前处理的表索引
     * @param currentCombination 当前记录组合
     * @param result 输出参数，所有可能的记录组合
     */
    void generateCartesianProduct(const std::vector<std::vector<Record>>& allTableRecords, 
                                 size_t tableIndex, 
                                 std::vector<Record> currentCombination,
                                 std::vector<std::vector<Record>>& result);
    
    /**
     * @brief 应用DISTINCT去重
     * @param rows 行数据列表（会被修改）
     */
    void applyDistinct(std::vector<std::vector<std::string>>& rows);
    
    /**
     * @brief 应用ORDER BY排序
     * @param rows 行数据列表（会被修改）
     * @param columnNames 列名列表
     * @param tableInfo 表结构信息（用于字段类型判断）
     * @param orderBy ORDER BY信息列表
     * @return 成功返回true，失败返回false
     */
    bool applyOrderBy(std::vector<std::vector<std::string>>& rows,
                     const std::vector<std::string>& columnNames,
                     const TableInfo& tableInfo,
                     const std::vector<OrderByInfo>& orderBy);
    
    /**
     * @brief 应用LIMIT限制
     * @param rows 行数据列表（会被修改）
     * @param limitCount 限制行数（-1表示无限制）
     */
    void applyLimit(std::vector<std::vector<std::string>>& rows, int limitCount);
    
    /**
     * @brief 应用GROUP BY分组
     * @param rows 行数据列表（会被修改）
     * @param columnNames 列名列表
     * @param tableInfo 表结构信息
     * @param groupByFields GROUP BY字段列表
     * @return 成功返回true，失败返回false
     */
    bool applyGroupBy(std::vector<std::vector<std::string>>& rows,
                     const std::vector<std::string>& columnNames,
                     const TableInfo& tableInfo,
                     const std::vector<std::string>& groupByFields);
    
    /**
     * @brief 从原始记录计算聚合函数值（新方法，推荐使用）
     * @param records 原始记录列表
     * @param tableInfo 表结构信息
     * @param aggregateFunc 聚合函数信息
     * @return 聚合函数计算结果（字符串形式）
     */
    std::string calculateAggregateFromRecords(const std::vector<Record>& records,
                                             const TableInfo& tableInfo,
                                             const AggregateFunction& aggregateFunc);
    
    /**
     * @brief 计算聚合函数值（旧方法，保留以保持兼容性）
     * @param rows 分组后的行数据列表
     * @param columnNames 列名列表
     * @param tableInfo 表结构信息
     * @param aggregateFunc 聚合函数信息
     * @return 聚合函数计算结果（字符串形式）
     */
    std::string calculateAggregate(const std::vector<std::vector<std::string>>& rows,
                                  const std::vector<std::string>& columnNames,
                                  const TableInfo& tableInfo,
                                  const AggregateFunction& aggregateFunc);
    
    /**
     * @brief 应用HAVING过滤
     * @param groupedRows 分组后的行数据列表（会被修改）
     * @param columnNames 列名列表
     * @param tableInfo 表结构信息
     * @param havingClause HAVING条件节点
     * @return 成功返回true，失败返回false
     */
    bool applyHaving(std::vector<std::vector<std::string>>& groupedRows,
                    const std::vector<std::string>& columnNames,
                    const TableInfo& tableInfo,
                    const WhereCondition* havingClause);
    
    /**
     * @brief 评估HAVING条件（辅助函数）
     * @param row 分组后的结果行
     * @param columnNames 列名列表
     * @param tableInfo 表结构信息
     * @param condition HAVING条件节点
     * @return 条件匹配返回true，否则返回false
     */
    bool evaluateHavingCondition(const std::vector<std::string>& row,
                                 const std::vector<std::string>& columnNames,
                                 const TableInfo& tableInfo,
                                 const WhereCondition* condition);
};

