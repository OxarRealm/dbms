#pragma once

#include "dml/insert_handler.h"
#include "dml/delete_handler.h"
#include "dml/update_handler.h"
#include "sql_parser/parser.h"
#include <string>
#include <memory>

/**
 * @file dml_executor.h
 * @brief DML执行器
 *
 * 统一的DML语句执行器，根据SQL语句类型自动选择合适的处理器
 */

/**
 * @brief DML执行器类
 *
 * 整合所有DML处理器，提供统一的执行接口
 */
class DMLExecutor {
public:
    DMLExecutor();
    ~DMLExecutor();
    
    /**
     * @brief 执行DML语句
     * @param sql DML SQL语句
     * @return 成功返回true，失败返回false
     */
    bool execute(const std::string& sql);
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
    /**
     * @brief 设置数据库路径
     * @param dbPath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbPath);
    
    /**
     * @brief 判断SQL语句是否为DML语句
     * @param sql SQL语句
     * @return 是DML语句返回true，否则返回false
     */
    static bool isDMLStatement(const std::string& sql);
    
    /**
     * @brief 获取最后操作影响的记录数（用于INSERT/DELETE/UPDATE）
     */
    size_t getAffectedRows() const;
    
private:
    InsertHandler m_insertHandler;   // INSERT处理器
    DeleteHandler m_deleteHandler;    // DELETE处理器
    UpdateHandler m_updateHandler;   // UPDATE处理器
    std::string m_lastError;         // 最后的错误信息
    std::string m_databasePath;      // 数据库路径
    size_t m_affectedRows;           // 影响的记录数
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 识别SQL语句类型
     * @param sql SQL语句
     * @return SQL语句类型（INSERT, DELETE, UPDATE, UNKNOWN）
     */
    enum class StatementType {
        INSERT,
        DELETE,
        UPDATE,
        UNKNOWN
    };
    StatementType identifyStatementType(const std::string& sql);
};

