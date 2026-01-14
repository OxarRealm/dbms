/**
 * @file delete_handler.cpp
 * @brief DELETE语句处理器实现
 */

#include "dml/delete_handler.h"
#include "core/table_mode.h"
#include <algorithm>
#include <cstring>

DeleteHandler::DeleteHandler() : m_deletedCount(0) {
}

DeleteHandler::~DeleteHandler() {
}

bool DeleteHandler::execute(const std::string& sql) {
    m_lastError = "";
    m_deletedCount = 0;
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> ast = parser.parse();
    
    if (!ast) {
        setError("SQL解析失败: " + parser.getLastError());
        return false;
    }
    
    // 检查是否为DELETE节点
    DeleteNode* deleteNode = dynamic_cast<DeleteNode*>(ast.get());
    if (!deleteNode) {
        setError("不是DELETE语句");
        return false;
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(deleteNode->databaseFileName);
    m_dataManager.setDatabasePath(deleteNode->databaseFileName);
    
    // 读取表结构
    TableInfo tableInfo;
    if (!m_tableManager.readTable(deleteNode->tableName, tableInfo)) {
        setError("表不存在: " + deleteNode->tableName);
        return false;
    }
    
    // 检查条件字段是否存在
    int fieldIndex = findFieldIndex(tableInfo, deleteNode->conditionField);
    if (fieldIndex == -1) {
        setError("字段不存在: " + deleteNode->conditionField);
        return false;
    }
    
    // 读取所有记录
    std::vector<Record> records;
    if (!m_dataManager.readAllRecords(deleteNode->tableName, records)) {
        setError("读取记录失败");
        return false;
    }
    
    // 查找匹配的记录并删除
    std::vector<size_t> recordsToDelete;
    for (size_t i = 0; i < records.size(); ++i) {
        // 只处理有效记录
        if (records[i].validFlag == FLAG_VALID) {
            if (evaluateCondition(records[i], tableInfo, deleteNode->conditionField, deleteNode->conditionValue)) {
                recordsToDelete.push_back(i);
            }
        }
    }
    
    // 删除匹配的记录（从后往前删除，避免索引变化）
    for (auto it = recordsToDelete.rbegin(); it != recordsToDelete.rend(); ++it) {
        if (m_dataManager.deleteRecord(deleteNode->tableName, *it)) {
            m_deletedCount++;
        } else {
            setError("删除记录失败");
            return false;
        }
    }
    
    if (m_deletedCount == 0) {
        // 没有匹配的记录，不算错误，但给出提示
        // 这里不返回false，因为DELETE语句即使没有匹配的记录也是成功的
    }
    
    return true;
}

std::string DeleteHandler::getLastError() const {
    return m_lastError;
}

size_t DeleteHandler::getDeletedCount() const {
    return m_deletedCount;
}

void DeleteHandler::setError(const std::string& error) {
    m_lastError = error;
}

bool DeleteHandler::evaluateCondition(const Record& record, const TableInfo& tableInfo, 
                                      const std::string& conditionField, const std::string& conditionValue) {
    // 查找字段索引
    int fieldIndex = findFieldIndex(tableInfo, conditionField);
    if (fieldIndex == -1 || fieldIndex >= static_cast<int>(record.values.size())) {
        return false;
    }
    
    // 获取字段值
    const std::string& fieldValue = record.values[fieldIndex];
    
    // 简单相等比较（字符串比较）
    return fieldValue == conditionValue;
}

int DeleteHandler::findFieldIndex(const TableInfo& tableInfo, const std::string& fieldName) {
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (strcmp(tableInfo.fields[i].sFieldName, fieldName.c_str()) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

