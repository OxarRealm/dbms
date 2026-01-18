/**
 * @file drop_index_handler.cpp
 * @brief DROP INDEX语句处理器实现
 */

#include "ddl/drop_index_handler.h"
#include "core/index_storage.h"
#include <iostream>

DropIndexHandler::DropIndexHandler() {
}

DropIndexHandler::~DropIndexHandler() {
}

bool DropIndexHandler::execute(const std::string& sql, const std::string& basePath) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 转换为DropIndexNode
    DropIndexNode* dropIndexNode = dynamic_cast<DropIndexNode*>(node.get());
    if (dropIndexNode == nullptr) {
        setError("Not a DROP INDEX statement");
        return false;
    }
    
    // 解析数据库路径：如果提供了basePath，使用它来解析相对路径
    // 否则，假设databaseFileName是完整路径或当前目录下的文件名
    std::string dbPath = dropIndexNode->databaseFileName;
    if (!basePath.empty()) {
        std::string baseDir = basePath;
        size_t lastSlash = baseDir.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            baseDir = baseDir.substr(0, lastSlash + 1);
        } else {
            baseDir = "";
        }
        if (!baseDir.empty()) {
            dbPath = baseDir + dropIndexNode->databaseFileName;
        }
    }
    
    // 验证索引删除参数
    if (!validateIndexDeletion(dropIndexNode, dbPath)) {
        return false;
    }
    
    // 设置数据库路径
    m_indexManager.setDatabasePath(dbPath);
    
    // 调用IndexManager删除索引
    if (!m_indexManager.dropIndex(dropIndexNode->tableName, 
                                   dropIndexNode->fieldName, 
                                   dropIndexNode->indexType)) {
        setError("Failed to drop index on " + dropIndexNode->tableName + 
                 "." + dropIndexNode->fieldName);
        return false;
    }
    
    // 保存索引信息到文件
    if (!IndexStorageManager::saveIndices(dropIndexNode->databaseFileName, 
                                          dbPath, 
                                          m_indexManager)) {
        setError("Failed to save index information");
        return false;
    }
    
    return true;
}

bool DropIndexHandler::validateIndexDeletion(DropIndexNode* node, const std::string& dbPath) {
    // 验证表名和字段名（至少需要指定表名和字段名，或者索引名）
    if (node->tableName.empty() && node->indexName.empty()) {
        setError("Table name or index name must be specified");
        return false;
    }
    
    // 如果指定了索引名，需要查找对应的表名和字段名
    // 这里简化处理：要求必须指定表名和字段名
    if (node->tableName.empty() || node->fieldName.empty()) {
        setError("Table name and field name must be specified");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("Database file name cannot be empty");
        return false;
    }
    
    // 检查索引是否存在（dbPath已经设置）
    if (!node->indexType.empty()) {
        if (!m_indexManager.hasIndex(node->tableName, node->fieldName, node->indexType)) {
            setError("Index does not exist on " + node->tableName + "." + node->fieldName + 
                     " with type " + node->indexType);
            return false;
        }
    } else {
        if (!m_indexManager.hasIndex(node->tableName, node->fieldName)) {
            setError("No index exists on " + node->tableName + "." + node->fieldName);
            return false;
        }
    }
    
    return true;
}

std::string DropIndexHandler::getLastError() const {
    return m_lastError;
}

void DropIndexHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "DropIndexHandler错误: " << error << std::endl;
}



