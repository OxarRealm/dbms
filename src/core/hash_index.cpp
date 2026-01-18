/**
 * @file hash_index.cpp
 * @brief 哈希索引实现
 */

#include "core/hash_index.h"
#include "core/data_manager.h"
#include "core/table_manager.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

HashIndex::HashIndex() {
}

HashIndex::~HashIndex() {
}

void HashIndex::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
}

size_t HashIndex::hashFunction(const std::string& str, size_t tableSize) const {
    // 使用djb2哈希算法
    size_t hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    return hash % tableSize;
}

size_t HashIndex::hashInt(const std::string& value, size_t tableSize) const {
    try {
        int intValue = std::stoi(value);
        // 使用简单的取模哈希
        return (static_cast<size_t>(intValue) % tableSize + tableSize) % tableSize;
    } catch (...) {
        // 如果转换失败，使用字符串哈希
        return hashFunction(value, tableSize);
    }
}

size_t HashIndex::hashFloat(const std::string& value, size_t tableSize) const {
    try {
        double floatValue = std::stod(value);
        // 将浮点数转换为整数后取模
        long long intValue = static_cast<long long>(floatValue * 1000);  // 保留3位小数精度
        return (static_cast<size_t>(intValue) % tableSize + tableSize) % tableSize;
    } catch (...) {
        // 如果转换失败，使用字符串哈希
        return hashFunction(value, tableSize);
    }
}

bool HashIndex::readRecords(const std::string& tableName, std::vector<Record>& records) const {
    DataManager dataManager;
    dataManager.setDatabasePath(m_dbFilePath);
    return dataManager.readAllRecords(tableName, records);
}

bool HashIndex::readTableFields(const std::string& tableName, std::vector<TableMode>& fields) const {
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return false;
    }
    fields = tableInfo.fields;
    return true;
}

bool HashIndex::buildIndex(const std::string& tableName, const std::string& fieldName, 
                          size_t fieldIndex) {
    // 读取表结构
    std::vector<TableMode> fields;
    if (!readTableFields(tableName, fields)) {
        std::cerr << "Error: Cannot read table structure " << tableName << std::endl;
        return false;
    }

    if (fieldIndex >= fields.size()) {
        std::cerr << "Error: Field index out of range" << std::endl;
        return false;
    }

    // 读取所有记录
    std::vector<Record> records;
    if (!readRecords(tableName, records)) {
        std::cerr << "Error: Cannot read records" << std::endl;
        return false;
    }

    // 创建哈希表
    HashTable& hashTable = m_indices[tableName][fieldName];
    hashTable = HashTable(DEFAULT_HASH_TABLE_SIZE);

    // 获取字段类型
    const std::string& fieldType = fields[fieldIndex].sType;

    // 构建索引：遍历所有记录，将键值插入哈希表
    for (size_t i = 0; i < records.size(); i++) {
        if (!records[i].isValid()) {
            continue;  // 跳过无效记录
        }

        const std::string& keyValue = records[i].getValue(fieldIndex);
        
        // 计算哈希值
        size_t hashValue;
        if (fieldType == "int") {
            hashValue = hashInt(keyValue, hashTable.size);
        } else if (fieldType == "float" || fieldType == "double") {
            hashValue = hashFloat(keyValue, hashTable.size);
        } else {
            // 字符串类型或其他类型，使用通用哈希函数
            hashValue = hashFunction(keyValue, hashTable.size);
        }

        // 在对应的哈希桶中查找是否已存在该键值
        std::list<HashIndexItem>& bucket = hashTable.buckets[hashValue];
        bool found = false;
        
        for (auto& item : bucket) {
            if (item.keyValue == keyValue) {
                // 键值已存在，添加记录索引
                item.recordIndices.push_back(i);
                found = true;
                break;
            }
        }

        if (!found) {
            // 键值不存在，创建新的索引项
            HashIndexItem newItem(keyValue);
            newItem.recordIndices.push_back(i);
            bucket.push_back(newItem);
        }
    }

    return true;
}

bool HashIndex::updateIndex(const std::string& tableName, const std::string& fieldName, 
                           size_t fieldIndex) {
    // 重新构建索引
    return buildIndex(tableName, fieldName, fieldIndex);
}

bool HashIndex::removeIndex(const std::string& tableName, const std::string& fieldName) {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    tableIt->second.erase(fieldIt);
    
    // 如果表没有其他索引了，删除表的条目
    if (tableIt->second.empty()) {
        m_indices.erase(tableIt);
    }

    return true;
}

bool HashIndex::hasIndex(const std::string& tableName, const std::string& fieldName) const {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    return fieldIt != tableIt->second.end();
}

bool HashIndex::pointQuery(const std::string& tableName, const std::string& fieldName,
                          const std::string& keyValue, std::vector<size_t>& resultIndices) const {
    resultIndices.clear();

    // 检查索引是否存在
    if (!hasIndex(tableName, fieldName)) {
        return false;
    }

    // 读取表结构以获取字段类型
    std::vector<TableMode> fields;
    if (!readTableFields(tableName, fields)) {
        return false;
    }

    // 找到字段索引
    size_t fieldIndex = SIZE_MAX;
    std::string fieldType;
    for (size_t i = 0; i < fields.size(); i++) {
        if (fields[i].sFieldName == fieldName) {
            fieldIndex = i;
            fieldType = fields[i].sType;
            break;
        }
    }

    if (fieldIndex == SIZE_MAX) {
        return false;
    }

    // 获取哈希表
    const HashTable& hashTable = m_indices.at(tableName).at(fieldName);

    // 计算哈希值
    size_t hashValue;
    if (fieldType == "int") {
        hashValue = hashInt(keyValue, hashTable.size);
    } else if (fieldType == "float" || fieldType == "double") {
        hashValue = hashFloat(keyValue, hashTable.size);
    } else {
        hashValue = hashFunction(keyValue, hashTable.size);
    }

    // 在对应的哈希桶中查找
    const std::list<HashIndexItem>& bucket = hashTable.buckets[hashValue];
    
    for (const auto& item : bucket) {
        if (item.keyValue == keyValue) {
            // 找到匹配的键值，返回所有记录索引
            resultIndices = item.recordIndices;
            
            // 验证记录是否仍然有效（读取记录验证）
            std::vector<Record> records;
            if (readRecords(tableName, records)) {
                std::vector<size_t> validIndices;
                for (size_t idx : resultIndices) {
                    if (idx < records.size() && records[idx].isValid()) {
                        validIndices.push_back(idx);
                    }
                }
                resultIndices = validIndices;
            }
            
            return true;
        }
    }

    // 未找到匹配的键值
    return true;  // 返回true但resultIndices为空
}

void HashIndex::clear() {
    m_indices.clear();
}

void HashIndex::clearTable(const std::string& tableName) {
    auto it = m_indices.find(tableName);
    if (it != m_indices.end()) {
        m_indices.erase(it);
    }
}

bool HashIndex::getIndexStats(const std::string& tableName, const std::string& fieldName,
                             size_t& bucketCount, size_t& totalItems, size_t& maxChainLength) const {
    if (!hasIndex(tableName, fieldName)) {
        return false;
    }

    const HashTable& hashTable = m_indices.at(tableName).at(fieldName);
    bucketCount = hashTable.size;
    totalItems = 0;
    maxChainLength = 0;

    for (const auto& bucket : hashTable.buckets) {
        size_t chainLength = bucket.size();
        totalItems += chainLength;
        if (chainLength > maxChainLength) {
            maxChainLength = chainLength;
        }
    }

    return true;
}

