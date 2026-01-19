#pragma once

#include "ddl/ddl_executor.h"
#include "dml/dml_executor.h"
#include "query/select_handler.h"
#include "sql_parser/parser.h"
#include "index/index_advisor.h"
#include "core/index_manager.h"
#include "core/permission_manager.h"
#include "core/role_manager.h"
#include "core/session_manager.h"
#include <string>
#include <memory>
#include <chrono>

/**
 * @file query_executor.h
 * @brief 查询执行器
 *
 * 统一的SQL语句执行器，自动识别SQL语句类型并路由到相应的处理器
 */

/**
 * @brief SQL执行结果类型
 */
enum class ExecutionResultType {
    SUCCESS,        // 执行成功
    ERROR,          // 执行失败
    QUERY_RESULT    // 查询结果
};

/**
 * @brief SQL执行结果结构
 */
struct ExecutionResult {
    ExecutionResultType type;              // 结果类型
    std::string errorMessage;              // 错误信息（如果失败）
    QueryResult queryResult;               // 查询结果（如果是查询语句）
    size_t affectedRows;                   // 影响的记录数（用于INSERT/DELETE/UPDATE）
    std::string formattedOutput;          // 格式化的输出字符串
    std::vector<std::string> usedIndices; // 使用的索引列表（格式：表名.字段名(索引类型)）
    
    ExecutionResult() : type(ExecutionResultType::SUCCESS), affectedRows(0) {}
};

/**
 * @brief 查询执行器类
 *
 * 整合DDL、DML和Query执行器，提供统一的执行接口
 */
class QueryExecutor {
public:
    QueryExecutor();
    ~QueryExecutor();
    
    /**
     * @brief 执行SQL语句
     * @param sql SQL语句
     * @return 执行结果
     */
    ExecutionResult execute(const std::string& sql);
    
    /**
     * @brief 设置数据库路径
     * @param dbPath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbPath);

    /**
     * @brief 获取索引建议器（用于查询日志记录和索引推荐）
     * @return IndexAdvisor引用
     */
    IndexAdvisor& getIndexAdvisor();
    
    /**
     * @brief 获取索引管理器（用于检查索引是否存在）
     * @return IndexManager指针
     */
    IndexManager* getIndexManager();
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
    /**
     * @brief 格式化查询结果输出
     * @param result 查询结果
     * @return 格式化后的字符串
     */
    static std::string formatQueryResult(const QueryResult& result);
    
    /**
     * @brief 判断SQL语句类型
     * @param sql SQL语句
     * @return 语句类型（DDL, DML, QUERY, UNKNOWN）
     */
    static std::string identifyStatementType(const std::string& sql);
    
    /**
     * @brief 从路径中提取数据库名（不含扩展名）
     * @param dbPath 数据库路径
     * @return 数据库名
     */
    static std::string extractDatabaseName(const std::string& dbPath);
    
private:
    DDLExecutor m_ddlExecutor;        // DDL执行器
    DMLExecutor m_dmlExecutor;        // DML执行器
    SelectHandler m_selectHandler;    // SELECT查询处理器
    IndexAdvisor m_indexAdvisor;      // 索引建议器
    IndexManager m_indexManager;      // 索引管理器（用于检测索引使用）
    std::string m_lastError;          // 最后的错误信息
    std::string m_databasePath;      // 数据库路径
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 执行DDL语句
     * @param sql DDL SQL语句
     * @param result 输出参数，执行结果
     * @return 成功返回true，失败返回false
     */
    bool executeDDL(const std::string& sql, ExecutionResult& result);
    
    /**
     * @brief 执行DML语句
     * @param sql DML SQL语句
     * @param result 输出参数，执行结果
     * @return 成功返回true，失败返回false
     */
    bool executeDML(const std::string& sql, ExecutionResult& result);
    
    /**
     * @brief 执行查询语句
     * @param sql SELECT SQL语句
     * @param result 输出参数，执行结果
     * @return 成功返回true，失败返回false
     */
    bool executeQuery(const std::string& sql, ExecutionResult& result);
    
    /**
     * @brief 从WHERE条件中提取字段名（辅助函数）
     * @param condition WHERE条件节点
     * @param fields 输出参数，字段名列表
     */
    void extractWhereFields(const WhereCondition* condition, std::vector<std::string>& fields);
    
    /**
     * @brief 检查权限
     * @param objectType 对象类型（1=表, 2=数据库, 3=系统）
     * @param objectName 对象名（表名或数据库名）
     * @param permissionType 权限类型
     * @return 有权限返回true，否则返回false
     */
    bool checkPermission(char objectType, const std::string& objectName, PermissionType permissionType);
    
    mutable PermissionManager m_permissionManager;  // 权限管理器（mutable以便在const方法中重新加载数据）
    mutable RoleManager m_roleManager;             // 角色管理器（mutable以便在const方法中重新加载数据）
};

