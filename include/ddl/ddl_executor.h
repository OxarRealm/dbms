#pragma once

#include "ddl/create_table_handler.h"
#include "ddl/edit_table_handler.h"
#include "ddl/rename_table_handler.h"
#include "ddl/drop_table_handler.h"
#include "sql_parser/parser.h"
#include <string>
#include <memory>

/**
 * @file ddl_executor.h
 * @brief DDL执行器
 *
 * 统一的DDL语句执行器，根据SQL语句类型自动选择合适的处理器
 */

/**
 * @brief DDL执行器类
 *
 * 整合所有DDL处理器，提供统一的执行接口
 */
class DDLExecutor {
public:
    DDLExecutor();
    ~DDLExecutor();
    
    /**
     * @brief 执行DDL语句
     * @param sql DDL SQL语句
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
     * @brief 判断SQL语句是否为DDL语句
     * @param sql SQL语句
     * @return 是DDL语句返回true，否则返回false
     */
    static bool isDDLStatement(const std::string& sql);
    
private:
    CreateTableHandler m_createHandler;   // CREATE TABLE处理器
    EditTableHandler m_editHandler;        // EDIT TABLE处理器
    RenameTableHandler m_renameHandler;    // RENAME TABLE处理器
    DropTableHandler m_dropHandler;        // DROP TABLE处理器
    std::string m_lastError;               // 最后的错误信息
    std::string m_databasePath;             // 数据库路径
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 识别SQL语句类型
     * @param sql SQL语句
     * @return SQL语句类型（CREATE_TABLE, EDIT_TABLE, RENAME_TABLE, DROP_TABLE, UNKNOWN）
     */
    enum class StatementType {
        CREATE_TABLE,
        EDIT_TABLE,
        RENAME_TABLE,
        DROP_TABLE,
        UNKNOWN
    };
    StatementType identifyStatementType(const std::string& sql);
};

