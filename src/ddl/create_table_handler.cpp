/**
 * @file create_table_handler.cpp
 * @brief CREATE TABLE语句处理器实现
 */

#include "ddl/create_table_handler.h"
#include "core/constraint.h"
#include "core/constraint_storage.h"
#include <iostream>
#include <cstring>
#include <algorithm>

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
    
    // 验证表级约束（外键引用的表是否存在等）
    if (!validateConstraints(createNode)) {
        return false;
    }
    
    // 创建表结构信息
    TableInfo tableInfo = createTableInfo(createNode);
    
    // 设置数据库路径
    // 注意：databaseFileName可能是相对路径或文件名，需要构建完整路径
    // 为了与加载时保持一致，我们需要确定数据库的完整路径
    // 但这里databaseFileName可能只是文件名，所以我们需要使用它作为路径
    // 实际上，TableManager会根据databaseFileName构建.dbf和.dat文件的路径
    m_tableManager.setDatabasePath(createNode->databaseFileName);
    
    // 调用TableManager创建表
    if (!m_tableManager.createTable(tableInfo)) {
        setError("Failed to create table: " + createNode->tableName);
        return false;
    }
    
    // 注册表级约束到约束注册表
    registerConstraints(createNode);
    
    // 保存约束到文件
    // 注意：databaseFileName作为数据库名（key），也作为路径（用于确定.cst文件位置）
    // 如果databaseFileName是相对路径，需要转换为绝对路径
    // 但为了简化，我们假设databaseFileName就是数据库文件名（不含路径）
    ConstraintStorageManager::saveConstraints(createNode->databaseFileName, createNode->databaseFileName);
    
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

bool CreateTableHandler::validateConstraints(CreateTableNode* node) {
    // 验证外键约束
    if (!validateForeignKeyConstraints(node)) {
        return false;
    }
    
    // 验证唯一约束（检查字段是否存在）
    for (const auto& uniqueConstraint : node->uniqueConstraints) {
        for (const std::string& fieldName : uniqueConstraint.fieldNames) {
            bool fieldExists = false;
            for (const auto& field : node->fields) {
                if (std::string(field.sFieldName) == fieldName) {
                    fieldExists = true;
                    break;
                }
            }
            if (!fieldExists) {
                setError("Unique constraint field '" + fieldName + "' does not exist in table '" + node->tableName + "'");
                return false;
            }
        }
    }
    
    // 验证检查约束（检查字段是否存在）
    for (const auto& checkConstraint : node->checkConstraints) {
        bool fieldExists = false;
        for (const auto& field : node->fields) {
            if (std::string(field.sFieldName) == std::string(checkConstraint.fieldName)) {
                fieldExists = true;
                break;
            }
        }
        if (!fieldExists) {
            setError("Check constraint field '" + std::string(checkConstraint.fieldName) + "' does not exist in table '" + node->tableName + "'");
            return false;
        }
    }
    
    return true;
}

bool CreateTableHandler::validateForeignKeyConstraints(CreateTableNode* node) {
    // 设置数据库路径以便读取其他表
    m_tableManager.setDatabasePath(node->databaseFileName);
    
    for (const auto& fk : node->foreignKeys) {
        // 检查引用表是否存在
        if (!m_tableManager.tableExists(fk.referencedTable)) {
            setError("Foreign key constraint references non-existent table: " + std::string(fk.referencedTable));
            return false;
        }
        
        // 检查本表字段是否存在
        bool fieldExists = false;
        for (const auto& field : node->fields) {
            if (std::string(field.sFieldName) == std::string(fk.fieldName)) {
                fieldExists = true;
                break;
            }
        }
        if (!fieldExists) {
            setError("Foreign key field '" + std::string(fk.fieldName) + "' does not exist in table '" + node->tableName + "'");
            return false;
        }
        
        // 检查引用表的引用字段是否存在
        TableInfo refTableInfo;
        if (!m_tableManager.readTable(fk.referencedTable, refTableInfo)) {
            setError("Failed to read referenced table structure: " + std::string(fk.referencedTable));
            return false;
        }
        
        bool refFieldExists = false;
        for (const auto& refField : refTableInfo.fields) {
            if (std::string(refField.sFieldName) == std::string(fk.referencedField)) {
                refFieldExists = true;
                // 检查引用字段是否为KEY（主键或唯一键）
                if (refField.bKey != FLAG_KEY) {
                    setError("Referenced field '" + std::string(fk.referencedField) + "' in table '" + 
                            std::string(fk.referencedTable) + "' must be a KEY (primary key)");
                    return false;
                }
                break;
            }
        }
        if (!refFieldExists) {
            setError("Referenced field '" + std::string(fk.referencedField) + "' does not exist in table '" + 
                    std::string(fk.referencedTable) + "'");
            return false;
        }
    }
    
    return true;
}

TableInfo CreateTableHandler::createTableInfo(CreateTableNode* node) {
    TableInfo tableInfo(node->tableName.c_str());
    tableInfo.fields = node->fields;
    return tableInfo;
}

void CreateTableHandler::registerConstraints(CreateTableNode* node) {
    // 创建约束信息结构
    TableConstraints constraints;
    constraints.foreignKeys = node->foreignKeys;
    constraints.uniqueConstraints = node->uniqueConstraints;
    constraints.checkConstraints = node->checkConstraints;
    
    // 注册到约束注册表
    ConstraintRegistry::getInstance().registerTableConstraints(
        node->databaseFileName,
        node->tableName,
        constraints
    );
}

std::string CreateTableHandler::getLastError() const {
    return m_lastError;
}

void CreateTableHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "CreateTableHandler错误: " << error << std::endl;
}

