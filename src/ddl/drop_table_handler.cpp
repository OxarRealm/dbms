/**
 * @file drop_table_handler.cpp
 * @brief DROP TABLE语句处理器实现
 */

#include "ddl/drop_table_handler.h"
#include <iostream>
#include <cstring>
#include <filesystem>

DropTableHandler::DropTableHandler() {
}

DropTableHandler::~DropTableHandler() {
}

bool DropTableHandler::execute(const std::string& sql, const std::string& basePath) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 转换为DropTableNode
    DropTableNode* dropNode = dynamic_cast<DropTableNode*>(node.get());
    if (dropNode == nullptr) {
        setError("Not a DROP TABLE statement");
        return false;
    }
    
    // 验证删除参数
    if (!validateDropParameters(dropNode)) {
        return false;
    }
    
    // 解析数据库路径：如果提供了basePath，使用它来解析相对路径
    // 否则，假设databaseFileName是完整路径或当前目录下的文件名
    std::string dbPath = dropNode->databaseFileName;
    if (!basePath.empty()) {
        // 如果basePath是完整路径，提取目录部分
        std::string baseDir = basePath;
        size_t lastSlash = baseDir.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            baseDir = baseDir.substr(0, lastSlash + 1);
        } else {
            baseDir = "";  // 如果basePath没有路径分隔符，使用当前目录
        }
        // 组合完整路径
        if (!baseDir.empty()) {
            dbPath = baseDir + dropNode->databaseFileName;
        }
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(dbPath);
    m_dataManager.setDatabasePath(dbPath);
    
    // 删除表
    return dropTable(dropNode);
}

bool DropTableHandler::validateDropParameters(DropTableNode* node) {
    // 验证表名
    if (node->tableName.empty()) {
        setError("Table name cannot be empty");
        return false;
    }
    
    if (node->tableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("Table name is too long (maximum " + std::to_string(TABLE_NAME_LENGTH - 1) + " characters)");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("Database file name cannot be empty");
        return false;
    }
    
    return true;
}

bool DropTableHandler::dropTable(DropTableNode* node) {
    // 检查表是否存在
    if (!m_tableManager.tableExists(node->tableName)) {
        setError("Table does not exist: " + node->tableName);
        return false;
    }
    
    // 先删除表的数据（.dat文件中的记录）- 硬删除，完全删除所有大小写变体的数据
    // 这是关键步骤：确保.dat文件中不再有任何该表的数据
    if (!m_dataManager.clearTable(node->tableName)) {
        setError("Failed to delete table data: " + node->tableName);
        return false;
    }
    
    // 删除表结构（.dbf文件中的表定义）
    if (!m_tableManager.deleteTable(node->tableName)) {
        setError("Failed to delete table structure: " + node->tableName);
        return false;
    }
    
    return true;
}

std::string DropTableHandler::getLastError() const {
    return m_lastError;
}

void DropTableHandler::setError(const std::string& error) {
    m_lastError = error;
    // std::cerr << "DropTableHandler Error: " << error << std::endl; // Removed for GUI consistency
}

