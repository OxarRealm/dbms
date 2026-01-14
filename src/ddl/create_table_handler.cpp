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
        setError("SQL解析失败: " + parser.getLastError());
        return false;
    }
    
    // 转换为CreateTableNode
    CreateTableNode* createNode = dynamic_cast<CreateTableNode*>(node.get());
    if (createNode == nullptr) {
        setError("不是CREATE TABLE语句");
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
        setError("创建表失败: " + createNode->tableName);
        return false;
    }
    
    return true;
}

bool CreateTableHandler::validateTableStructure(CreateTableNode* node) {
    // 验证表名
    if (node->tableName.empty()) {
        setError("表名不能为空");
        return false;
    }
    
    if (node->tableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("表名过长（最大" + std::to_string(TABLE_NAME_LENGTH - 1) + "字符）");
        return false;
    }
    
    // 验证字段列表
    if (node->fields.empty()) {
        setError("表必须至少包含一个字段");
        return false;
    }
    
    // 验证每个字段
    for (size_t i = 0; i < node->fields.size(); i++) {
        const TableMode& field = node->fields[i];
        
        // 验证字段名
        if (strlen(field.sFieldName) == 0) {
            setError("字段 " + std::to_string(i + 1) + " 的字段名不能为空");
            return false;
        }
        
        // 验证数据类型
        if (strlen(field.sType) == 0) {
            setError("字段 " + std::string(field.sFieldName) + " 的数据类型不能为空");
            return false;
        }
        
        // 验证字段大小（对于char类型）
        if (strcmp(field.sType, "char") == 0 && field.iSize <= 0) {
            setError("字段 " + std::string(field.sFieldName) + " (char类型) 的大小必须大于0");
            return false;
        }
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("数据库文件名不能为空");
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

