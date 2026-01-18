/**
 * @file create_index_handler.cpp
 * @brief CREATE INDEX语句处理器实现
 */

#include "ddl/create_index_handler.h"
#include "core/index_storage.h"
#include <iostream>

CreateIndexHandler::CreateIndexHandler() {
}

CreateIndexHandler::~CreateIndexHandler() {
}

bool CreateIndexHandler::execute(const std::string& sql, const std::string& basePath) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 转换为CreateIndexNode
    CreateIndexNode* createIndexNode = dynamic_cast<CreateIndexNode*>(node.get());
    if (createIndexNode == nullptr) {
        setError("Not a CREATE INDEX statement");
        return false;
    }
    
    // 验证索引创建参数
    if (!validateIndexCreation(createIndexNode, basePath)) {
        return false;
    }
    
    // 解析数据库路径：如果提供了basePath，使用它来解析相对路径
    // 否则，假设databaseFileName是完整路径或当前目录下的文件名
    std::string dbPath = createIndexNode->databaseFileName;
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
            dbPath = baseDir + createIndexNode->databaseFileName;
        }
    }
    
    // 设置数据库路径
    m_indexManager.setDatabasePath(dbPath);
    
    // 调用IndexManager创建索引
    if (!m_indexManager.createIndex(createIndexNode->tableName, 
                                     createIndexNode->fieldName, 
                                     createIndexNode->indexType)) {
        setError("Failed to create index on " + createIndexNode->tableName + 
                 "." + createIndexNode->fieldName);
        return false;
    }
    
    // 保存索引信息到文件
    if (!IndexStorageManager::saveIndices(createIndexNode->databaseFileName, 
                                          dbPath, 
                                          m_indexManager)) {
        setError("Failed to save index information");
        return false;
    }
    
    return true;
}

bool CreateIndexHandler::validateIndexCreation(CreateIndexNode* node, const std::string& basePath) {
    // 验证表名
    if (node->tableName.empty()) {
        setError("Table name cannot be empty");
        return false;
    }
    
    // 验证字段名
    if (node->fieldName.empty()) {
        setError("Field name cannot be empty");
        return false;
    }
    
    // 验证索引类型
    if (node->indexType != "hash" && node->indexType != "adjacent" && node->indexType != "btree") {
        setError("Invalid index type. Supported types: hash, adjacent, btree");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("Database file name cannot be empty");
        return false;
    }
    
    // 解析数据库路径（用于验证）
    std::string dbPath = node->databaseFileName;
    if (!basePath.empty()) {
        std::string baseDir = basePath;
        size_t lastSlash = baseDir.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            baseDir = baseDir.substr(0, lastSlash + 1);
        } else {
            baseDir = "";
        }
        if (!baseDir.empty()) {
            dbPath = baseDir + node->databaseFileName;
        }
    }
    
    // 检查索引是否已存在
    m_indexManager.setDatabasePath(dbPath);
    if (m_indexManager.hasIndex(node->tableName, node->fieldName, node->indexType)) {
        setError("Index already exists on " + node->tableName + "." + node->fieldName + 
                 " with type " + node->indexType);
        return false;
    }
    
    return true;
}

std::string CreateIndexHandler::getLastError() const {
    return m_lastError;
}

void CreateIndexHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "CreateIndexHandler错误: " << error << std::endl;
}



