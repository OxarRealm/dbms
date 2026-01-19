/**
 * @file delete_handler.cpp
 * @brief DELETE语句处理器实现
 */

#include "dml/delete_handler.h"
#include "core/constraint_registry.h"
#include "core/table_manager.h"
#include "core/table_mode.h"
#include "core/constraint.h"
#include <algorithm>
#include <cstring>
#include <cstddef>
#include <climits>
#include <filesystem>

DeleteHandler::DeleteHandler() : m_constraintManager(&m_dataManager), m_deletedCount(0) {
}

DeleteHandler::~DeleteHandler() {
}

bool DeleteHandler::execute(const std::string& sql, const std::string& basePath) {
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
    
    // 解析数据库路径：如果提供了basePath，使用它来解析相对路径
    // 否则，假设databaseFileName是完整路径或当前目录下的文件名
    std::string dbPath = deleteNode->databaseFileName;
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
            dbPath = baseDir + deleteNode->databaseFileName;
        }
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(dbPath);
    m_dataManager.setDatabasePath(dbPath);
    
    // 提取数据库名（baseName）用于约束查询
    // 约束注册时使用baseName，所以这里也需要提取baseName
    // 注意：这里使用deleteNode->databaseFileName（SQL中的数据库名），而不是dbPath（完整路径）
    std::string dbNameForConstraints = deleteNode->databaseFileName;
    try {
        // 尝试从路径中提取文件名（不含扩展名）
        std::filesystem::path dbPathObj(deleteNode->databaseFileName);
        std::string fileName = dbPathObj.filename().string();
        // 移除扩展名（如果有）
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos) {
            fileName = fileName.substr(0, dotPos);
        }
        if (!fileName.empty()) {
            dbNameForConstraints = fileName;
        }
    } catch (...) {
        // 如果filesystem操作失败，使用原始值
        // 尝试手动提取
        std::string dbPathStr = deleteNode->databaseFileName;
        size_t lastSlash = dbPathStr.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            dbPathStr = dbPathStr.substr(lastSlash + 1);
        }
        size_t dotPos = dbPathStr.find_last_of('.');
        if (dotPos != std::string::npos) {
            dbPathStr = dbPathStr.substr(0, dotPos);
        }
        if (!dbPathStr.empty()) {
            dbNameForConstraints = dbPathStr;
        }
    }
    
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
    
    // 先检查外键约束
    for (size_t recordIndex : recordsToDelete) {
        if (!checkForeignKeyConstraints(deleteNode->tableName, tableInfo, records[recordIndex], dbNameForConstraints)) {
            return false;
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

bool DeleteHandler::checkForeignKeyConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                                const Record& record, const std::string& dbName) {
    // 获取引用此表的所有外键约束
    std::vector<std::pair<std::string, ForeignKeyConstraint>> referencingConstraints = 
        ConstraintRegistry::getInstance().getReferencingConstraints(dbName, tableName);
    
    if (referencingConstraints.empty()) {
        return true;  // 没有其他表引用此表，可以删除
    }
    
    // 找到主键字段的值（被引用的值）
    std::string primaryKeyValue;
    for (size_t i = 0; i < tableInfo.fields.size() && i < record.values.size(); ++i) {
        if (tableInfo.fields[i].bKey == FLAG_KEY) {
            primaryKeyValue = record.values[i];
            break;
        }
    }
    
    if (primaryKeyValue.empty()) {
        return true;  // 没有主键，无法检查外键约束
    }
    
    // 检查每个引用此表的外键约束
    for (const auto& pair : referencingConstraints) {
        const std::string& referencingTableName = pair.first;
        const ForeignKeyConstraint& fk = pair.second;
        
        // 检查引用表中是否有记录引用此值
        std::vector<Record> referencingRecords;
        if (!m_dataManager.readAllRecords(referencingTableName, referencingRecords)) {
            continue;  // 无法读取引用表，跳过
        }
        
        // 获取引用表的结构
        TableManager tableManager;
        tableManager.setDatabasePath(dbName);
        TableInfo refTableInfo;
        if (!tableManager.readTable(referencingTableName, refTableInfo)) {
            continue;  // 无法读取引用表结构，跳过
        }
        
        // 找到外键字段的索引
        int fkFieldIndex = -1;
        for (size_t i = 0; i < refTableInfo.fields.size(); ++i) {
            if (std::string(refTableInfo.fields[i].sFieldName) == std::string(fk.fieldName)) {
                fkFieldIndex = static_cast<int>(i);
                break;
            }
        }
        
        if (fkFieldIndex < 0) {
            continue;  // 找不到外键字段，跳过
        }
        
        // 检查是否有记录引用此值
        bool hasReference = false;
        for (const auto& refRecord : referencingRecords) {
            if (refRecord.validFlag == FLAG_VALID && 
                fkFieldIndex < static_cast<int>(refRecord.values.size()) &&
                refRecord.values[fkFieldIndex] == primaryKeyValue) {
                hasReference = true;
                break;
            }
        }
        
        if (hasReference) {
            // 根据ON DELETE动作处理
            std::string onDeleteAction = fk.onDeleteAction;
            if (onDeleteAction == "RESTRICT" || onDeleteAction == "NO ACTION" || onDeleteAction.empty()) {
                setError("Cannot delete record: Foreign key constraint violation. " +
                        std::string(fk.constraintName) + " in table '" + referencingTableName + 
                        "' references this record");
                return false;
            } else if (onDeleteAction == "CASCADE") {
                // 级联删除：删除引用此记录的所有记录
                // 注意：这里只处理一层级联，多层嵌套需要递归处理
                // 收集需要删除的记录索引（从后往前，避免索引变化）
                std::vector<size_t> recordsToCascadeDelete;
                for (size_t i = 0; i < referencingRecords.size(); ++i) {
                    if (referencingRecords[i].validFlag == FLAG_VALID && 
                        fkFieldIndex < static_cast<int>(referencingRecords[i].values.size()) &&
                        referencingRecords[i].values[fkFieldIndex] == primaryKeyValue) {
                        recordsToCascadeDelete.push_back(i);
                    }
                }
                
                // 从后往前删除，避免索引变化
                for (auto it = recordsToCascadeDelete.rbegin(); it != recordsToCascadeDelete.rend(); ++it) {
                    size_t recordIndex = *it;
                    const Record& recordToDelete = referencingRecords[recordIndex];
                    
                    // 递归检查该记录是否也被其他表引用（处理多层嵌套）
                    // 先检查该记录的外键约束，触发级联删除
                    TableInfo refTableInfo;
                    TableManager tableManager;
                    tableManager.setDatabasePath(dbName);
                    if (tableManager.readTable(referencingTableName, refTableInfo)) {
                        // 递归调用checkForeignKeyConstraints，处理多层嵌套的级联删除
                        if (!checkForeignKeyConstraints(referencingTableName, refTableInfo, 
                                                       recordToDelete, dbName)) {
                            // 如果级联删除失败，返回错误
                            return false;
                        }
                    }
                    
                    // 重新读取记录列表，因为递归调用可能已经删除了一些记录，索引可能已经变化
                    std::vector<Record> currentRecords;
                    if (!m_dataManager.readAllRecords(referencingTableName, currentRecords)) {
                        setError("Failed to read records for cascade delete in table '" + referencingTableName + "'");
                        return false;
                    }
                    
                    // 找到当前记录在最新列表中的索引（通过比较外键字段值）
                    size_t currentIndex = SIZE_MAX;
                    for (size_t j = 0; j < currentRecords.size(); ++j) {
                        if (currentRecords[j].validFlag == FLAG_VALID &&
                            fkFieldIndex < static_cast<int>(currentRecords[j].values.size()) &&
                            fkFieldIndex < static_cast<int>(recordToDelete.values.size()) &&
                            currentRecords[j].values[fkFieldIndex] == recordToDelete.values[fkFieldIndex] &&
                            currentRecords[j].values[fkFieldIndex] == primaryKeyValue) {
                            // 找到匹配的记录（通过外键字段值匹配）
                            currentIndex = j;
                            break;
                        }
                    }
                    
                    if (currentIndex != SIZE_MAX) {
                        if (!m_dataManager.deleteRecord(referencingTableName, currentIndex)) {
                            setError("Failed to cascade delete record in table '" + referencingTableName + "'");
                            return false;
                        }
                    }
                }
            } else if (onDeleteAction == "SET NULL") {
                // SET NULL：将引用字段设置为NULL
                // 注意：这需要字段允许NULL
                for (size_t i = 0; i < referencingRecords.size(); ++i) {
                    if (referencingRecords[i].validFlag == FLAG_VALID && 
                        fkFieldIndex < static_cast<int>(referencingRecords[i].values.size()) &&
                        referencingRecords[i].values[fkFieldIndex] == primaryKeyValue) {
                        Record updatedRecord = referencingRecords[i];
                        updatedRecord.values[fkFieldIndex] = "";
                        if (!m_dataManager.updateRecord(referencingTableName, i, updatedRecord)) {
                            setError("Failed to set NULL for foreign key in table '" + referencingTableName + "'");
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    return true;
}

