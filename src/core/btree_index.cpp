/**
 * @file btree_index.cpp
 * @brief B+树索引实现
 */

#include "core/btree_index.h"
#include "core/data_manager.h"
#include "core/table_manager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>

BTreeIndex::BTreeIndex() {
}

BTreeIndex::~BTreeIndex() {
}

void BTreeIndex::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
}

int BTreeIndex::compareValues(const std::string& value1, const std::string& value2, 
                             const std::string& fieldType) const {
    // 根据字段类型进行比较
    if (fieldType == "int") {
        try {
            int v1 = std::stoi(value1);
            int v2 = std::stoi(value2);
            if (v1 < v2) return -1;
            if (v1 > v2) return 1;
            return 0;
        } catch (...) {
            // 转换失败，使用字符串比较
            if (value1 < value2) return -1;
            if (value1 > value2) return 1;
            return 0;
        }
    } else if (fieldType == "float" || fieldType == "double") {
        try {
            double v1 = std::stod(value1);
            double v2 = std::stod(value2);
            if (v1 < v2) return -1;
            if (v1 > v2) return 1;
            return 0;
        } catch (...) {
            // 转换失败，使用字符串比较
            if (value1 < value2) return -1;
            if (value1 > value2) return 1;
            return 0;
        }
    } else {
        // 字符串类型，直接比较
        if (value1 < value2) return -1;
        if (value1 > value2) return 1;
        return 0;
    }
}

size_t BTreeIndex::findKeyPosition(const std::shared_ptr<BTreeNode>& node, 
                                  const std::string& key, const std::string& fieldType) const {
    // 二分查找
    size_t left = 0;
    size_t right = node->keys.size();
    
    while (left < right) {
        size_t mid = (left + right) / 2;
        int cmp = compareValues(node->keys[mid], key, fieldType);
        if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    return left;
}

size_t BTreeIndex::findKeyInLeaf(const std::shared_ptr<BTreeNode>& node, 
                                 const std::string& key, const std::string& fieldType) const {
    size_t pos = findKeyPosition(node, key, fieldType);
    if (pos < node->keys.size() && 
        compareValues(node->keys[pos], key, fieldType) == 0) {
        return pos;
    }
    return SIZE_MAX;
}

bool BTreeIndex::buildIndex(const std::string& tableName, const std::string& fieldName, 
                           size_t fieldIndex) {
    // 读取表结构
    std::vector<TableMode> fields;
    if (!readTableFields(tableName, fields)) {
        std::cerr << "错误: 无法读取表结构 " << tableName << std::endl;
        return false;
    }

    if (fieldIndex >= fields.size()) {
        std::cerr << "错误: 字段索引超出范围" << std::endl;
        return false;
    }

    // 读取所有记录
    std::vector<Record> records;
    if (!readRecords(tableName, records)) {
        std::cerr << "错误: 无法读取记录" << std::endl;
        return false;
    }

    if (records.empty()) {
        // 空表，创建空索引（只有根节点）
        auto root = std::make_shared<BTreeNode>(BTreeNodeType::LEAF);
        root->isRoot = true;
        m_indices[tableName][fieldName] = root;
        return true;
    }

    // 获取字段类型
    std::string fieldType = fields[fieldIndex].sType;

    // 创建键值对列表（键值 -> 记录索引）
    std::vector<std::pair<std::string, size_t>> keyValuePairs;
    for (size_t i = 0; i < records.size(); i++) {
        if (records[i].isValid()) {
            std::string keyValue = records[i].getValue(fieldIndex);
            keyValuePairs.push_back({keyValue, i});
        }
    }

    // 按键值排序
    std::sort(keyValuePairs.begin(), keyValuePairs.end(),
              [this, &fieldType](const std::pair<std::string, size_t>& a, 
                                const std::pair<std::string, size_t>& b) {
                  return compareValues(a.first, b.first, fieldType) < 0;
              });

    // 构建B+树（简化版：直接构建叶子节点，然后构建内部节点）
    // 创建叶子节点列表
    std::vector<std::shared_ptr<BTreeNode>> leafNodes;
    std::shared_ptr<BTreeNode> currentLeaf = nullptr;
    std::shared_ptr<BTreeNode> prevLeaf = nullptr;

    for (const auto& pair : keyValuePairs) {
        if (currentLeaf == nullptr || currentLeaf->keys.size() >= MAX_KEYS) {
            // 创建新叶子节点
            currentLeaf = std::make_shared<BTreeNode>(BTreeNodeType::LEAF);
            if (prevLeaf != nullptr) {
                prevLeaf->next = currentLeaf;
                currentLeaf->prev = prevLeaf;
            }
            leafNodes.push_back(currentLeaf);
            prevLeaf = currentLeaf;
        }
        
        currentLeaf->keys.push_back(pair.first);
        currentLeaf->values.push_back(pair.second);
    }

    // 如果只有一个叶子节点，直接作为根节点
    if (leafNodes.size() == 1) {
        leafNodes[0]->isRoot = true;
        m_indices[tableName][fieldName] = leafNodes[0];
        return true;
    }

    // 构建内部节点（自底向上）
    std::vector<std::shared_ptr<BTreeNode>> currentLevel = leafNodes;
    
    while (currentLevel.size() > 1) {
        std::vector<std::shared_ptr<BTreeNode>> nextLevel;
        std::shared_ptr<BTreeNode> currentInternal = nullptr;

        for (size_t i = 0; i < currentLevel.size(); i++) {
            if (currentInternal == nullptr || currentInternal->keys.size() >= MAX_KEYS) {
                // 创建新内部节点
                currentInternal = std::make_shared<BTreeNode>(BTreeNodeType::INTERNAL);
                nextLevel.push_back(currentInternal);
            }

            auto child = currentLevel[i];
            child->parent = currentInternal;
            child->parentIndex = currentInternal->children.size();
            currentInternal->children.push_back(child);

            // 添加键值（使用子节点的最小键值）
            if (!child->keys.empty()) {
                std::string minKey = child->keys[0];
                if (currentInternal->keys.empty() || 
                    compareValues(minKey, currentInternal->keys.back(), fieldType) > 0) {
                    currentInternal->keys.push_back(minKey);
                    currentInternal->values.push_back(i);
                }
            }
        }

        // 设置根节点标志
        if (nextLevel.size() == 1) {
            nextLevel[0]->isRoot = true;
            m_indices[tableName][fieldName] = nextLevel[0];
            return true;
        }

        currentLevel = nextLevel;
    }

    return true;
}

bool BTreeIndex::updateIndex(const std::string& tableName, const std::string& fieldName, 
                             size_t fieldIndex) {
    // 重新构建索引
    return buildIndex(tableName, fieldName, fieldIndex);
}

bool BTreeIndex::removeIndex(const std::string& tableName, const std::string& fieldName) {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    tableIt->second.erase(fieldIt);
    
    if (tableIt->second.empty()) {
        m_indices.erase(tableIt);
    }

    return true;
}

bool BTreeIndex::hasIndex(const std::string& tableName, const std::string& fieldName) const {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    return tableIt->second.find(fieldName) != tableIt->second.end();
}

bool BTreeIndex::pointQuery(const std::string& tableName, const std::string& fieldName,
                           const std::string& keyValue, std::vector<size_t>& resultIndices) const {
    resultIndices.clear();

    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    auto root = fieldIt->second;
    if (root == nullptr) {
        return false;
    }

    // 读取字段类型
    std::vector<TableMode> fields;
    if (!readTableFields(tableName, fields)) {
        return false;
    }

    size_t fieldIndex = SIZE_MAX;
    for (size_t i = 0; i < fields.size(); i++) {
        if (fields[i].sFieldName == fieldName) {
            fieldIndex = i;
            break;
        }
    }

    if (fieldIndex == SIZE_MAX) {
        return false;
    }

    std::string fieldType = fields[fieldIndex].sType;

    // 从根节点开始查找
    auto currentNode = root;
    while (currentNode->type == BTreeNodeType::INTERNAL) {
        size_t pos = findKeyPosition(currentNode, keyValue, fieldType);
        if (pos > 0) pos--;
        if (pos >= currentNode->children.size()) {
            return false;
        }
        currentNode = currentNode->children[pos];
    }

    // 在叶子节点中查找
    size_t pos = findKeyInLeaf(currentNode, keyValue, fieldType);
    if (pos != SIZE_MAX) {
        resultIndices.push_back(currentNode->values[pos]);
    }

    return true;
}

bool BTreeIndex::rangeQuery(const std::string& tableName, const std::string& fieldName,
                           const std::string& startValue, const std::string& endValue,
                           std::vector<size_t>& resultIndices) const {
    resultIndices.clear();

    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    auto root = fieldIt->second;
    if (root == nullptr) {
        return false;
    }

    // 读取字段类型
    std::vector<TableMode> fields;
    if (!readTableFields(tableName, fields)) {
        return false;
    }

    size_t fieldIndex = SIZE_MAX;
    for (size_t i = 0; i < fields.size(); i++) {
        if (fields[i].sFieldName == fieldName) {
            fieldIndex = i;
            break;
        }
    }

    if (fieldIndex == SIZE_MAX) {
        return false;
    }

    std::string fieldType = fields[fieldIndex].sType;

    // 从根节点开始查找起始值
    auto currentNode = root;
    while (currentNode->type == BTreeNodeType::INTERNAL) {
        size_t pos = findKeyPosition(currentNode, startValue, fieldType);
        if (pos > 0) pos--;
        if (pos >= currentNode->children.size()) {
            return false;
        }
        currentNode = currentNode->children[pos];
    }

    // 在叶子节点中找到起始位置
    size_t startPos = findKeyPosition(currentNode, startValue, fieldType);
    
    // 遍历叶子节点，收集范围内的记录
    while (currentNode != nullptr) {
        for (size_t i = startPos; i < currentNode->keys.size(); i++) {
            int cmpEnd = compareValues(currentNode->keys[i], endValue, fieldType);
            if (cmpEnd > 0) {
                // 超过结束值，停止
                return true;
            }
            
            resultIndices.push_back(currentNode->values[i]);
        }

        // 移动到下一个叶子节点
        currentNode = currentNode->next;
        startPos = 0;
    }

    return true;
}

bool BTreeIndex::sequentialScan(const std::string& tableName, const std::string& fieldName,
                               std::vector<size_t>& resultIndices) const {
    resultIndices.clear();

    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    auto root = fieldIt->second;
    if (root == nullptr) {
        return false;
    }

    // 找到最左边的叶子节点
    auto currentNode = root;
    while (currentNode->type == BTreeNodeType::INTERNAL) {
        if (currentNode->children.empty()) {
            return false;
        }
        currentNode = currentNode->children[0];
    }

    // 遍历所有叶子节点
    while (currentNode != nullptr) {
        for (size_t i = 0; i < currentNode->values.size(); i++) {
            resultIndices.push_back(currentNode->values[i]);
        }
        currentNode = currentNode->next;
    }

    return true;
}

void BTreeIndex::clear() {
    m_indices.clear();
}

void BTreeIndex::clearTable(const std::string& tableName) {
    auto it = m_indices.find(tableName);
    if (it != m_indices.end()) {
        m_indices.erase(it);
    }
}

bool BTreeIndex::getIndexStats(const std::string& tableName, const std::string& fieldName,
                               size_t& nodeCount, size_t& leafCount, size_t& depth) const {
    nodeCount = 0;
    leafCount = 0;
    depth = 0;

    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    auto root = fieldIt->second;
    if (root == nullptr) {
        return false;
    }

    size_t maxDepth = 0;
    countNodes(root, nodeCount, leafCount, 1, maxDepth);
    depth = maxDepth;

    return true;
}

void BTreeIndex::countNodes(const std::shared_ptr<BTreeNode>& node, 
                            size_t& nodeCount, size_t& leafCount, 
                            size_t depth, size_t& maxDepth) const {
    if (node == nullptr) {
        return;
    }

    nodeCount++;
    if (node->type == BTreeNodeType::LEAF) {
        leafCount++;
    }

    if (depth > maxDepth) {
        maxDepth = depth;
    }

    // 递归统计子节点
    for (const auto& child : node->children) {
        countNodes(child, nodeCount, leafCount, depth + 1, maxDepth);
    }
}

bool BTreeIndex::readRecords(const std::string& tableName, std::vector<Record>& records) const {
    DataManager dataManager;
    dataManager.setDatabasePath(m_dbFilePath);
    return dataManager.readAllRecords(tableName, records);
}

bool BTreeIndex::readTableFields(const std::string& tableName, std::vector<TableMode>& fields) const {
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return false;
    }
    
    fields = tableInfo.fields;
    return true;
}












