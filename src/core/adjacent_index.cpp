/**
 * @file adjacent_index.cpp
 * @brief 相邻索引实现
 */

#include "core/adjacent_index.h"
#include "core/data_manager.h"
#include "core/table_manager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

AdjacentIndex::AdjacentIndex() {
}

AdjacentIndex::~AdjacentIndex() {
}

void AdjacentIndex::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
}

bool AdjacentIndex::buildIndex(const std::string& tableName, const std::string& fieldName, 
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
        // 空表，创建空索引
        m_indices[tableName][fieldName] = std::vector<IndexItem>();
        return true;
    }

    // 创建索引项列表（初始化为记录数量）
    std::vector<IndexItem> indexItems(records.size());

    // 创建排序索引（记录索引 -> 排序后的位置）
    std::vector<size_t> sortIndices(records.size());
    for (size_t i = 0; i < records.size(); i++) {
        sortIndices[i] = i;
    }

    // 获取字段类型
    const std::string& fieldType = fields[fieldIndex].sType;

    // 对记录索引进行排序（根据字段值）
    std::sort(sortIndices.begin(), sortIndices.end(), 
              [&](size_t i1, size_t i2) {
                  const std::string& v1 = records[i1].getValue(fieldIndex);
                  const std::string& v2 = records[i2].getValue(fieldIndex);
                  return compareValues(v1, v2, fieldType);
              });

    // 建立相邻关系
    for (size_t i = 0; i < sortIndices.size(); i++) {
        size_t currentRecordIndex = sortIndices[i];
        
        // 设置下一个相邻记录
        if (i + 1 < sortIndices.size()) {
            size_t nextRecordIndex = sortIndices[i + 1];
            indexItems[currentRecordIndex].nextIndex = nextRecordIndex;
        } else {
            indexItems[currentRecordIndex].nextIndex = SIZE_MAX;  // 最后一个记录
        }

        // 设置上一个相邻记录
        if (i > 0) {
            size_t prevRecordIndex = sortIndices[i - 1];
            indexItems[currentRecordIndex].prevIndex = prevRecordIndex;
        } else {
            indexItems[currentRecordIndex].prevIndex = SIZE_MAX;  // 第一个记录
        }
    }

    // 保存索引
    m_indices[tableName][fieldName] = indexItems;

    return true;
}

bool AdjacentIndex::updateIndex(const std::string& tableName, const std::string& fieldName, 
                                size_t fieldIndex) {
    // 重新构建索引
    return buildIndex(tableName, fieldName, fieldIndex);
}

bool AdjacentIndex::removeIndex(const std::string& tableName, const std::string& fieldName) {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    tableIt->second.erase(fieldIt);

    // 如果表没有其他索引，删除表的条目
    if (tableIt->second.empty()) {
        m_indices.erase(tableIt);
    }

    return true;
}

bool AdjacentIndex::hasIndex(const std::string& tableName, const std::string& fieldName) const {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    return tableIt->second.find(fieldName) != tableIt->second.end();
}

bool AdjacentIndex::getNextIndex(const std::string& tableName, const std::string& fieldName,
                                 size_t recordIndex, size_t& nextIndex) const {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    const std::vector<IndexItem>& indexItems = fieldIt->second;
    if (recordIndex >= indexItems.size()) {
        return false;
    }

    const IndexItem& item = indexItems[recordIndex];
    if (item.nextIndex == SIZE_MAX) {
        nextIndex = SIZE_MAX;
        return false;  // 没有下一个记录
    }

    nextIndex = item.nextIndex;
    return true;
}

bool AdjacentIndex::getPrevIndex(const std::string& tableName, const std::string& fieldName,
                                 size_t recordIndex, size_t& prevIndex) const {
    auto tableIt = m_indices.find(tableName);
    if (tableIt == m_indices.end()) {
        return false;
    }

    auto fieldIt = tableIt->second.find(fieldName);
    if (fieldIt == tableIt->second.end()) {
        return false;
    }

    const std::vector<IndexItem>& indexItems = fieldIt->second;
    if (recordIndex >= indexItems.size()) {
        return false;
    }

    const IndexItem& item = indexItems[recordIndex];
    if (item.prevIndex == SIZE_MAX) {
        prevIndex = SIZE_MAX;
        return false;  // 没有上一个记录
    }

    prevIndex = item.prevIndex;
    return true;
}

bool AdjacentIndex::rangeQuery(const std::string& tableName, const std::string& fieldName,
                               const std::string& startValue, const std::string& endValue,
                               std::vector<size_t>& resultIndices) const {
    resultIndices.clear();

    // 检查索引是否存在
    if (!hasIndex(tableName, fieldName)) {
        return false;
    }

    // 读取表结构和记录
    std::vector<TableMode> fields;
    if (!readTableFields(tableName, fields)) {
        return false;
    }

    std::vector<Record> records;
    if (!readRecords(tableName, records)) {
        return false;
    }

    // 找到字段索引和类型
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

    // 使用顺序扫描获取所有排序后的记录索引
    std::vector<size_t> sortedIndices;
    if (!sequentialScan(tableName, fieldName, sortedIndices)) {
        return false;
    }

    // 遍历排序后的索引，找到范围内的记录
    bool inRange = false;
    for (size_t recordIdx : sortedIndices) {
        if (!records[recordIdx].isValid()) {
            continue;
        }

        const std::string& value = records[recordIdx].getValue(fieldIndex);
        
        // 检查是否进入范围（value >= startValue）
        if (!inRange) {
            if (value == startValue || !compareValues(value, startValue, fieldType)) {
                inRange = true;
            } else {
                continue;  // 还没到起始值
            }
        }

        // 检查是否超出范围（value > endValue）
        if (compareValues(endValue, value, fieldType)) {
            break;  // 超出范围，停止遍历
        }

        // 在范围内，添加到结果
        resultIndices.push_back(recordIdx);
    }

    return true;
}

bool AdjacentIndex::sequentialScan(const std::string& tableName, const std::string& fieldName,
                                  std::vector<size_t>& resultIndices) const {
    resultIndices.clear();

    // 检查索引是否存在
    if (!hasIndex(tableName, fieldName)) {
        return false;
    }

    // 读取记录
    std::vector<Record> records;
    if (!readRecords(tableName, records)) {
        return false;
    }

    if (records.empty()) {
        return true;
    }

    // 找到第一个记录（prevIndex == SIZE_MAX 的记录）
    const auto& indexItems = m_indices.at(tableName).at(fieldName);
    size_t firstIndex = SIZE_MAX;

    for (size_t i = 0; i < indexItems.size(); i++) {
        if (indexItems[i].prevIndex == SIZE_MAX) {
            firstIndex = i;
            break;
        }
    }

    if (firstIndex == SIZE_MAX) {
        return true;  // 没有找到起始记录
    }

    // 从第一个记录开始，沿着相邻索引遍历所有记录
    size_t currentIndex = firstIndex;
    while (currentIndex != SIZE_MAX) {
        if (records[currentIndex].isValid()) {
            resultIndices.push_back(currentIndex);
        }

        // 移动到下一个相邻记录
        size_t nextIndex;
        if (!getNextIndex(tableName, fieldName, currentIndex, nextIndex) || 
            nextIndex == SIZE_MAX) {
            break;
        }
        currentIndex = nextIndex;
    }

    return true;
}

void AdjacentIndex::clear() {
    m_indices.clear();
}

void AdjacentIndex::clearTable(const std::string& tableName) {
    m_indices.erase(tableName);
}

bool AdjacentIndex::compareValues(const std::string& value1, const std::string& value2,
                                  const std::string& fieldType) const {
    // 根据字段类型进行比较
    if (fieldType == "int") {
        int v1 = std::stoi(value1);
        int v2 = std::stoi(value2);
        return v1 < v2;
    } else if (fieldType == "float") {
        float v1 = std::stof(value1);
        float v2 = std::stof(value2);
        return v1 < v2;
    } else if (fieldType == "double") {
        double v1 = std::stod(value1);
        double v2 = std::stod(value2);
        return v1 < v2;
    } else {
        // 字符串类型，直接比较
        return value1 < value2;
    }
}

bool AdjacentIndex::readRecords(const std::string& tableName, std::vector<Record>& records) const {
    DataManager dataManager;
    dataManager.setDatabasePath(m_dbFilePath);
    return dataManager.readAllRecords(tableName, records);
}

bool AdjacentIndex::readTableFields(const std::string& tableName, 
                                    std::vector<TableMode>& fields) const {
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return false;
    }
    fields = tableInfo.fields;
    return true;
}

