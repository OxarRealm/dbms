/**
 * @file rename_table_handler.cpp
 * @brief RENAME TABLE语句处理器实现
 */

#include "ddl/rename_table_handler.h"
#include <iostream>
#include <cstring>

RenameTableHandler::RenameTableHandler() {
}

RenameTableHandler::~RenameTableHandler() {
}

bool RenameTableHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 转换为RenameTableNode
    RenameTableNode* renameNode = dynamic_cast<RenameTableNode*>(node.get());
    if (renameNode == nullptr) {
        setError("Not a RENAME TABLE statement");
        return false;
    }
    
    // 验证重命名参数
    if (!validateRenameParameters(renameNode)) {
        return false;
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(renameNode->databaseFileName);
    m_dataManager.setDatabasePath(renameNode->databaseFileName);
    
    // 重命名表
    return renameTable(renameNode);
}

bool RenameTableHandler::validateRenameParameters(RenameTableNode* node) {
    // 验证旧表名
    if (node->oldTableName.empty()) {
        setError("Old table name cannot be empty");
        return false;
    }
    
    if (node->oldTableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("Old table name is too long (maximum " + std::to_string(TABLE_NAME_LENGTH - 1) + " characters)");
        return false;
    }
    
    // 验证新表名
    if (node->newTableName.empty()) {
        setError("New table name cannot be empty");
        return false;
    }
    
    if (node->newTableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("New table name is too long (maximum " + std::to_string(TABLE_NAME_LENGTH - 1) + " characters)");
        return false;
    }
    
    // 验证新表名和旧表名不同
    if (node->oldTableName == node->newTableName) {
        setError("New table name and old table name are the same");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("Database file name cannot be empty");
        return false;
    }
    
    return true;
}

bool RenameTableHandler::renameTable(RenameTableNode* node) {
    // 检查旧表是否存在
    if (!m_tableManager.tableExists(node->oldTableName)) {
        setError("Table does not exist: " + node->oldTableName);
        return false;
    }
    
    // 检查新表名是否已存在
    if (m_tableManager.tableExists(node->newTableName)) {
        setError("Table already exists: " + node->newTableName);
        return false;
    }
    
    // 先读取旧表的所有记录（在重命名表结构之前，此时.dbf和.dat都还是旧表名）
    std::vector<Record> records;
    bool hasData = m_dataManager.readAllRecords(node->oldTableName, records);
    
    // 如果旧表有数据，需要先迁移到新表名（在更新.dbf之前）
    // 这样可以确保数据迁移时，.dat文件中还是旧表名，可以正确读取
    if (hasData && !records.empty()) {
        // 将记录写入新表名下
        if (!m_dataManager.insertRecords(node->newTableName, records)) {
            setError("Failed to migrate data to new table name");
            return false;
        }
        
        // 清空旧表名的数据（硬删除）
        if (!m_dataManager.clearTable(node->oldTableName)) {
            setError("Failed to clear old table data");
            // 尝试恢复：删除新表名的数据
            m_dataManager.clearTable(node->newTableName);
            return false;
        }
    }
    
    // 使用TableManager重命名表（会更新.dbf文件）
    // 注意：此时.dat文件已经更新为新表名，.dbf文件也要更新为新表名
    if (!m_tableManager.renameTable(node->oldTableName, node->newTableName)) {
        setError("Failed to rename table: " + node->oldTableName);
        // 如果.dbf更新失败，尝试恢复.dat文件
        if (hasData && !records.empty()) {
            // 删除新表名的数据
            m_dataManager.clearTable(node->newTableName);
            // 恢复旧表名的数据
            m_dataManager.insertRecords(node->oldTableName, records);
        }
        return false;
    }
    
    return true;
}

std::string RenameTableHandler::getLastError() const {
    return m_lastError;
}

void RenameTableHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "RenameTableHandler错误: " << error << std::endl;
}

