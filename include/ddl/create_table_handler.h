#pragma once

#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/table_manager.h"
#include "core/constraint_registry.h"
#include <string>
#include <memory>

/**
 * @file create_table_handler.h
 * @brief CREATE TABLE语句处理器
 *
 * 负责处理CREATE TABLE SQL语句，调用TableManager创建表
 */

/**
 * @brief CREATE TABLE语句处理器类
 */
class CreateTableHandler {
public:
    CreateTableHandler();
    ~CreateTableHandler();
    
    /**
     * @brief 执行CREATE TABLE语句
     * @param sql CREATE TABLE SQL语句
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
     * @brief 验证表结构
     * @param node CREATE TABLE AST节点
     * @return 验证通过返回true，否则返回false
     */
    bool validateTableStructure(CreateTableNode* node);
    
    /**
     * @brief 验证表级约束
     * @param node CREATE TABLE AST节点
     * @return 验证通过返回true，否则返回false
     */
    bool validateConstraints(CreateTableNode* node);
    
    /**
     * @brief 验证外键约束
     * @param node CREATE TABLE AST节点
     * @return 验证通过返回true，否则返回false
     */
    bool validateForeignKeyConstraints(CreateTableNode* node);
    
    /**
     * @brief 创建表结构信息
     * @param node CREATE TABLE AST节点
     * @return TableInfo结构
     */
    TableInfo createTableInfo(CreateTableNode* node);
    
    /**
     * @brief 注册表级约束到约束注册表
     * @param node CREATE TABLE AST节点
     */
    void registerConstraints(CreateTableNode* node);
};

