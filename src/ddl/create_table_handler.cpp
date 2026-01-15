/**
 * @file create_table_handler.cpp
 * @brief CREATE TABLE语句处理器实现
 */

#include "ddl/create_table_handler.h"
#include <iostream>
#include <cstring>

CreateTableHandler::CreateTableHandler() {
}

CreateTableHandler::~CreateTableHandler() {
}

bool CreateTableHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 转换为CreateTableNode
    CreateTableNode* createNode = dynamic_cast<CreateTableNode*>(node.get());
    if (createNode == nullptr) {
        setError("Not a CREATE TABLE statement");
        return false;
    }
    
    // 验证表结构
    if (!validateTableStructure(createNode)) {
        return false;
    }
    
    // 创建表结构信息
    TableInfo tableInfo = createTableInfo(createNode);
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(createNode->databaseFileName);
    
    // 调用TableManager创建表
    if (!m_tableManager.createTable(tableInfo)) {
        setError("Failed to create table: " + createNode->tableName);
        return false;
    }
    
    return true;
}

bool CreateTableHandler::validateTableStructure(CreateTableNode* node) {
    // 验证表名
    if (node->tableName.empty()) {
        setError("Table name cannot be empty");
        return false;
    }
    
    if (node->tableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("Table name is too long (maximum " + std::to_string(TABLE_NAME_LENGTH - 1) + " characters)");
        return false;
    }
    
    // 验证字段列表
    if (node->fields.empty()) {
        setError("Table must contain at least one field");
        return false;
    }
    
    // 验证每个字段
    for (size_t i = 0; i < node->fields.size(); i++) {
        const TableMode& field = node->fields[i];
        
        // 验证字段名
        if (strlen(field.sFieldName) == 0) {
            setError("Field " + std::to_string(i + 1) + " name cannot be empty");
            return false;
        }
        
        // 验证数据类型
        if (strlen(field.sType) == 0) {
            setError("Field " + std::string(field.sFieldName) + " data type cannot be empty");
            return false;
        }
        
        // 验证字段大小（对于char类型）
        if (strcmp(field.sType, "char") == 0 && field.iSize <= 0) {
            setError("Field " + std::string(field.sFieldName) + " (char type) size must be greater than 0");
            return false;
        }
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("Database file name cannot be empty");
        return false;
    }
    
    return true;
}

TableInfo CreateTableHandler::createTableInfo(CreateTableNode* node) {
    TableInfo tableInfo(node->tableName.c_str());
    tableInfo.fields = node->fields;
    return tableInfo;
}

std::string CreateTableHandler::getLastError() const {
    return m_lastError;
}

void CreateTableHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "CreateTableHandler错误: " << error << std::endl;
}

