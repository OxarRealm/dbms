/**
 * @file index_manager.cpp
 * @brief 索引管理器实现
 */

#include "core/index_manager.h"
#include "core/table_manager.h"
#include <iostream>
#include <sstream>
#include <cstring>

IndexManager::IndexManager() {
}

IndexManager::~IndexManager() {
}

void IndexManager::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
    m_adjacentIndex.setDatabasePath(dbFilePath);
    m_hashIndex.setDatabasePath(dbFilePath);
    m_btreeIndex.setDatabasePath(dbFilePath);
}

std::string IndexManager::generateIndexName(const std::string& tableName, 
                                            const std::string& fieldName, 
                                            const std::string& indexType) const {
    return tableName + "_" + fieldName + "_" + indexType;
}

bool IndexManager::getFieldIndex(const std::string& tableName, const std::string& fieldName, 
                                 size_t& fieldIndex) const {
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        std::cerr << "错误: 无法读取表 " << tableName << " (数据库路径: " << m_dbFilePath << ")" << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < tableInfo.fields.size(); i++) {
        if (strcmp(tableInfo.fields[i].sFieldName, fieldName.c_str()) == 0) {
            fieldIndex = i;
            return true;
        }
    }
    
    std::cerr << "错误: 字段 " << fieldName << " 在表 " << tableName << " 中不存在" << std::endl;
    std::cerr << "可用字段: ";
    for (size_t i = 0; i < tableInfo.fields.size(); i++) {
        if (i > 0) std::cerr << ", ";
        std::cerr << tableInfo.fields[i].sFieldName;
    }
    std::cerr << std::endl;
    return false;
}

bool IndexManager::createIndex(const std::string& tableName, const std::string& fieldName, 
                               const std::string& indexType) {
    // 获取字段索引
    size_t fieldIndex;
    if (!getFieldIndex(tableName, fieldName, fieldIndex)) {
        std::cerr << "错误: 无法找到字段 " << fieldName << " 在表 " << tableName << std::endl;
        return false;
    }

    // 根据索引类型创建索引
    bool success = false;
    if (indexType == "adjacent") {
        success = m_adjacentIndex.buildIndex(tableName, fieldName, fieldIndex);
    } else if (indexType == "hash") {
        success = m_hashIndex.buildIndex(tableName, fieldName, fieldIndex);
    } else if (indexType == "btree") {
        success = m_btreeIndex.buildIndex(tableName, fieldName, fieldIndex);
    } else {
        std::cerr << "错误: 不支持的索引类型 " << indexType << std::endl;
        return false;
    }

    if (success) {
        // 保存索引信息
        IndexInfo info;
        info.indexName = generateIndexName(tableName, fieldName, indexType);
        info.tableName = tableName;
        info.fieldName = fieldName;
        info.indexType = indexType;
        info.fieldIndex = fieldIndex;
        info.isActive = true;
        
        m_indexInfo[tableName][fieldName][indexType] = info;
    }

    return success;
}

bool IndexManager::dropIndex(const std::string& tableName, const std::string& fieldName, 
                           const std::string& indexType) {
    if (indexType.empty()) {
        // 删除该字段的所有索引
        bool success = true;
        success = success && m_adjacentIndex.removeIndex(tableName, fieldName);
        success = success && m_hashIndex.removeIndex(tableName, fieldName);
        success = success && m_btreeIndex.removeIndex(tableName, fieldName);
        
        // 清除索引信息
        auto tableIt = m_indexInfo.find(tableName);
        if (tableIt != m_indexInfo.end()) {
            tableIt->second.erase(fieldName);
            if (tableIt->second.empty()) {
                m_indexInfo.erase(tableIt);
            }
        }
        
        return success;
    } else {
        // 删除指定类型的索引
        bool success = false;
        if (indexType == "adjacent") {
            success = m_adjacentIndex.removeIndex(tableName, fieldName);
        } else if (indexType == "hash") {
            success = m_hashIndex.removeIndex(tableName, fieldName);
        } else if (indexType == "btree") {
            success = m_btreeIndex.removeIndex(tableName, fieldName);
        } else {
            return false;
        }

        if (success) {
            // 清除索引信息
            auto tableIt = m_indexInfo.find(tableName);
            if (tableIt != m_indexInfo.end()) {
                auto fieldIt = tableIt->second.find(fieldName);
                if (fieldIt != tableIt->second.end()) {
                    fieldIt->second.erase(indexType);
                    if (fieldIt->second.empty()) {
                        tableIt->second.erase(fieldIt);
                    }
                }
            }
        }

        return success;
    }
}

bool IndexManager::hasIndex(const std::string& tableName, const std::string& fieldName, 
                           const std::string& indexType) const {
    if (indexType.empty()) {
        // 检查是否有任何类型的索引
        return m_adjacentIndex.hasIndex(tableName, fieldName) ||
               m_hashIndex.hasIndex(tableName, fieldName) ||
               m_btreeIndex.hasIndex(tableName, fieldName);
    } else {
        // 检查指定类型的索引
        if (indexType == "adjacent") {
            return m_adjacentIndex.hasIndex(tableName, fieldName);
        } else if (indexType == "hash") {
            return m_hashIndex.hasIndex(tableName, fieldName);
        } else if (indexType == "btree") {
            return m_btreeIndex.hasIndex(tableName, fieldName);
        }
    }
    
    return false;
}

bool IndexManager::getTableIndices(const std::string& tableName, 
                                   std::vector<IndexInfo>& indices) const {
    indices.clear();

    auto tableIt = m_indexInfo.find(tableName);
    if (tableIt == m_indexInfo.end()) {
        return true;  // 没有索引，返回空列表
    }

    for (const auto& fieldPair : tableIt->second) {
        for (const auto& typePair : fieldPair.second) {
            indices.push_back(typePair.second);
        }
    }

    return true;
}

bool IndexManager::getAllIndices(std::vector<IndexInfo>& indices) const {
    indices.clear();

    for (const auto& tablePair : m_indexInfo) {
        for (const auto& fieldPair : tablePair.second) {
            for (const auto& typePair : fieldPair.second) {
                indices.push_back(typePair.second);
            }
        }
    }

    return true;
}

bool IndexManager::updateIndex(const std::string& tableName, const std::string& fieldName) {
    if (fieldName.empty()) {
        // 更新该表的所有索引
        auto tableIt = m_indexInfo.find(tableName);
        if (tableIt == m_indexInfo.end()) {
            return true;  // 没有索引
        }

        bool success = true;
        for (const auto& fieldPair : tableIt->second) {
            for (const auto& typePair : fieldPair.second) {
                const IndexInfo& info = typePair.second;
                size_t fieldIndex;
                if (getFieldIndex(tableName, info.fieldName, fieldIndex)) {
                    if (info.indexType == "adjacent") {
                        success = success && m_adjacentIndex.updateIndex(tableName, info.fieldName, fieldIndex);
                    } else if (info.indexType == "hash") {
                        success = success && m_hashIndex.updateIndex(tableName, info.fieldName, fieldIndex);
                    } else if (info.indexType == "btree") {
                        success = success && m_btreeIndex.updateIndex(tableName, info.fieldName, fieldIndex);
                    }
                }
            }
        }
        return success;
    } else {
        // 更新指定字段的所有索引
        auto tableIt = m_indexInfo.find(tableName);
        if (tableIt == m_indexInfo.end()) {
            return true;
        }

        auto fieldIt = tableIt->second.find(fieldName);
        if (fieldIt == tableIt->second.end()) {
            return true;
        }

        bool success = true;
        size_t fieldIndex;
        if (getFieldIndex(tableName, fieldName, fieldIndex)) {
            for (const auto& typePair : fieldIt->second) {
                const IndexInfo& info = typePair.second;
                if (info.indexType == "adjacent") {
                    success = success && m_adjacentIndex.updateIndex(tableName, fieldName, fieldIndex);
                } else if (info.indexType == "hash") {
                    success = success && m_hashIndex.updateIndex(tableName, fieldName, fieldIndex);
                } else if (info.indexType == "btree") {
                    success = success && m_btreeIndex.updateIndex(tableName, fieldName, fieldIndex);
                }
            }
        }
        return success;
    }
}

void IndexManager::clear() {
    m_adjacentIndex.clear();
    m_hashIndex.clear();
    m_btreeIndex.clear();
    m_indexInfo.clear();
}

void IndexManager::clearTable(const std::string& tableName) {
    m_adjacentIndex.clearTable(tableName);
    m_hashIndex.clearTable(tableName);
    m_btreeIndex.clearTable(tableName);
    
    auto it = m_indexInfo.find(tableName);
    if (it != m_indexInfo.end()) {
        m_indexInfo.erase(it);
    }
}

bool IndexManager::getIndexStats(const std::string& tableName, const std::string& fieldName, 
                                 const std::string& indexType, 
                                 std::map<std::string, std::string>& stats) const {
    stats.clear();

    if (indexType == "adjacent") {
        // 相邻索引没有统计信息接口，返回基本信息
        stats["type"] = "Adjacent Index";
        stats["description"] = "Optimized for range queries";
        return true;
    } else if (indexType == "hash") {
        size_t bucketCount, totalItems, maxChainLength;
        if (m_hashIndex.getIndexStats(tableName, fieldName, bucketCount, totalItems, maxChainLength)) {
            stats["type"] = "Hash Index";
            stats["bucket_count"] = std::to_string(bucketCount);
            stats["total_items"] = std::to_string(totalItems);
            stats["max_chain_length"] = std::to_string(maxChainLength);
            stats["description"] = "O(1) average time complexity for point queries";
            return true;
        }
    } else if (indexType == "btree") {
        size_t nodeCount, leafCount, depth;
        if (m_btreeIndex.getIndexStats(tableName, fieldName, nodeCount, leafCount, depth)) {
            stats["type"] = "B+ Tree Index";
            stats["node_count"] = std::to_string(nodeCount);
            stats["leaf_count"] = std::to_string(leafCount);
            stats["depth"] = std::to_string(depth);
            stats["description"] = "General-purpose index for all query types";
            return true;
        }
    }

    return false;
}




