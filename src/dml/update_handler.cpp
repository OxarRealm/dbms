/**
 * @file update_handler.cpp
 * @brief UPDATE语句处理器实现
 */

#include "dml/update_handler.h"
#include "core/constraint_registry.h"
#include "core/table_mode.h"
#include "core/constraint.h"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <iostream>

UpdateHandler::UpdateHandler() : m_constraintManager(&m_dataManager), m_updatedCount(0) {
}

UpdateHandler::~UpdateHandler() {
}

bool UpdateHandler::execute(const std::string& sql, const std::string& basePath) {
    m_lastError = "";
    m_updatedCount = 0;
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> ast = parser.parse();
    
    if (!ast) {
        setError("SQL解析失败: " + parser.getLastError());
        return false;
    }
    
    // 检查是否为UPDATE节点
    UpdateNode* updateNode = dynamic_cast<UpdateNode*>(ast.get());
    if (!updateNode) {
        setError("不是UPDATE语句");
        return false;
    }
    
    // 解析数据库路径：如果提供了basePath，使用它来解析相对路径
    // 否则，假设databaseFileName是完整路径或当前目录下的文件名
    std::string dbPath = updateNode->databaseFileName;
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
            dbPath = baseDir + updateNode->databaseFileName;
        }
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(dbPath);
    m_dataManager.setDatabasePath(dbPath);
    
    // 提取数据库名（baseName）用于约束查询
    // 约束注册时使用baseName，所以这里也需要提取baseName
    // 注意：这里使用updateNode->databaseFileName（SQL中的数据库名），而不是dbPath（完整路径）
    std::string dbNameForConstraints = updateNode->databaseFileName;
    try {
        // 尝试从路径中提取文件名（不含扩展名）
        std::filesystem::path dbPath(updateNode->databaseFileName);
        std::string fileName = dbPath.filename().string();
        // 移除扩展名（如果有）
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos) {
            fileName = fileName.substr(0, dotPos);
        }
        // 如果提取成功且不为空，使用提取的文件名
        if (!fileName.empty()) {
            dbNameForConstraints = fileName;
        }
    } catch (...) {
        // 如果filesystem操作失败，使用原始值
        // 尝试手动提取
        std::string dbPathStr = updateNode->databaseFileName;
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
    
    // 调试输出
    std::cerr << "[UPDATE] dbNameForConstraints=" << dbNameForConstraints 
              << ", updateNode->databaseFileName=" << updateNode->databaseFileName 
              << ", dbPath=" << dbPath << std::endl;
    
    // 读取表结构
    TableInfo tableInfo;
    if (!m_tableManager.readTable(updateNode->tableName, tableInfo)) {
        setError("表不存在: " + updateNode->tableName);
        return false;
    }
    
    // 检查SET字段是否存在
    int setFieldIndex = findFieldIndex(tableInfo, updateNode->setField);
    if (setFieldIndex == -1) {
        setError("SET字段不存在: " + updateNode->setField);
        return false;
    }
    
    // 检查WHERE字段是否存在
    int whereFieldIndex = findFieldIndex(tableInfo, updateNode->whereField);
    if (whereFieldIndex == -1) {
        setError("WHERE字段不存在: " + updateNode->whereField);
        return false;
    }
    
    // 验证更新值
    if (!validateUpdateValue(updateNode->setValue, tableInfo.fields[setFieldIndex])) {
        return false;
    }
    
    // 读取所有记录
    std::vector<Record> records;
    if (!m_dataManager.readAllRecords(updateNode->tableName, records)) {
        setError("读取记录失败");
        return false;
    }
    
    // 查找匹配的记录并更新
    std::vector<size_t> recordsToUpdate;
    for (size_t i = 0; i < records.size(); ++i) {
        // 只处理有效记录
        if (records[i].validFlag == FLAG_VALID) {
            if (evaluateCondition(records[i], tableInfo, updateNode->whereField, updateNode->whereValue)) {
                recordsToUpdate.push_back(i);
            }
        }
    }
    
    // 更新匹配的记录
    for (size_t recordIndex : recordsToUpdate) {
        // 创建更新后的记录
        Record updatedRecord = records[recordIndex];
        
        // 转换并设置新值
        std::string convertedValue = convertValue(updateNode->setValue, std::string(tableInfo.fields[setFieldIndex].sType));
        updatedRecord.values[setFieldIndex] = convertedValue;
        
        // 检查主键唯一性约束（如果更新的是主键字段）
        bool isKeyField = (tableInfo.fields[setFieldIndex].bKey == FLAG_KEY);
        if (isKeyField) {
            if (!checkPrimaryKeyUnique(updateNode->tableName, tableInfo, updatedRecord, recordIndex)) {
                return false;
            }
        }
        
        // 检查唯一约束（包括字段级和表级多字段唯一约束）
        // 注意：即使更新的字段不是唯一字段，如果它是多字段唯一约束的一部分，也应该检查
        // 因此，始终检查唯一约束
        if (!checkUniqueConstraints(updateNode->tableName, tableInfo, updatedRecord, 
                                   recordIndex, dbNameForConstraints)) {
            return false;
        }
        
        // 检查外键约束
        if (!checkForeignKeyConstraints(updateNode->tableName, tableInfo, updatedRecord, dbNameForConstraints)) {
            return false;
        }
        
        // 检查检查约束
        if (!checkCheckConstraints(updateNode->tableName, tableInfo, updatedRecord, dbNameForConstraints)) {
            return false;
        }
        
        // 更新记录
        if (m_dataManager.updateRecord(updateNode->tableName, recordIndex, updatedRecord)) {
            m_updatedCount++;
        } else {
            setError("Failed to update record in table '" + updateNode->tableName + "' at index " + std::to_string(recordIndex));
            return false;
        }
    }
    
    if (m_updatedCount == 0) {
        // 没有匹配的记录，不算错误，但给出提示
        // 这里不返回false，因为UPDATE语句即使没有匹配的记录也是成功的
    }
    
    return true;
}

std::string UpdateHandler::getLastError() const {
    return m_lastError;
}

size_t UpdateHandler::getUpdatedCount() const {
    return m_updatedCount;
}

void UpdateHandler::setError(const std::string& error) {
    m_lastError = error;
}

bool UpdateHandler::evaluateCondition(const Record& record, const TableInfo& tableInfo, 
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

int UpdateHandler::findFieldIndex(const TableInfo& tableInfo, const std::string& fieldName) {
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (strcmp(tableInfo.fields[i].sFieldName, fieldName.c_str()) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::string UpdateHandler::convertValue(const std::string& value, const std::string& fieldType) {
    // 如果值为空，返回空字符串
    if (value.empty()) {
        return "";
    }
    
    // 根据字段类型转换值
    if (fieldType == "int") {
        // 确保是整数格式
        try {
            int intValue = std::stoi(value);
            return std::to_string(intValue);
        } catch (...) {
            return value;  // 如果转换失败，返回原值
        }
    } else if (fieldType == "float") {
        try {
            float floatValue = std::stof(value);
            // 保留小数点后2位
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << floatValue;
            std::string result = oss.str();
            // 移除末尾的0
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
            return result;
        } catch (...) {
            return value;
        }
    } else if (fieldType == "double") {
        try {
            double doubleValue = std::stod(value);
            // 保留小数点后2位
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << doubleValue;
            std::string result = oss.str();
            // 移除末尾的0
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
            return result;
        } catch (...) {
            return value;
        }
    } else {
        // char和string类型直接返回
        return value;
    }
}

bool UpdateHandler::validateUpdateValue(const std::string& value, const TableMode& field) {
    // 检查NULL约束
    if (value.empty() && field.bNullFlag == FLAG_NO_NULL) {
        setError("字段 " + std::string(field.sFieldName) + " 不允许为空");
        return false;
    }
    
    // 如果值为空且允许NULL，跳过类型检查
    if (value.empty() && field.bNullFlag == FLAG_NULL) {
        return true;
    }
    
    // 类型验证
    std::string fieldType(field.sType);
    if (fieldType == "int") {
        // 验证是否为整数
        try {
            (void)std::stoi(value);  // 显式忽略返回值，避免[[nodiscard]]警告
        } catch (...) {
            setError("字段 " + std::string(field.sFieldName) + " 期望整数，但得到: " + value);
            return false;
        }
    } else if (fieldType == "float" || fieldType == "double") {
        // 验证是否为浮点数
        try {
            if (fieldType == "float") {
                (void)std::stof(value);  // 显式忽略返回值，避免[[nodiscard]]警告
            } else {
                (void)std::stod(value);  // 显式忽略返回值，避免[[nodiscard]]警告
            }
        } catch (...) {
            setError("字段 " + std::string(field.sFieldName) + " 期望浮点数，但得到: " + value);
            return false;
        }
    }
    // char和string类型不需要额外验证
    
    return true;
}

bool UpdateHandler::checkPrimaryKeyUnique(const std::string& tableName, const TableInfo& tableInfo, 
                                          const Record& updatedRecord, size_t currentRecordIndex)
{
    // Find all primary key fields
    std::vector<int> keyFieldIndices;
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (tableInfo.fields[i].bKey == FLAG_KEY) {
            keyFieldIndices.push_back(static_cast<int>(i));
        }
    }
    
    // If no primary key fields, no need to check
    if (keyFieldIndices.empty()) {
        return true;
    }
    
    // Read all valid records
    std::vector<Record> existingRecords;
    if (!m_dataManager.readAllRecords(tableName, existingRecords)) {
        // If read fails, assume no conflict
        return true;
    }
    
    // Check if updated record's primary key values conflict with existing records
    for (size_t i = 0; i < existingRecords.size(); ++i) {
        // Skip the current record being updated
        if (i == currentRecordIndex) {
            continue;
        }
        
        // Only check valid records
        if (!existingRecords[i].isValid()) {
            continue;
        }
        
        const Record& existingRecord = existingRecords[i];
        bool allKeysMatch = true;
        for (int keyIndex : keyFieldIndices) {
            if (keyIndex >= 0 && keyIndex < static_cast<int>(updatedRecord.values.size()) &&
                keyIndex >= 0 && keyIndex < static_cast<int>(existingRecord.values.size())) {
                if (updatedRecord.values[keyIndex] != existingRecord.values[keyIndex]) {
                    allKeysMatch = false;
                    break;
                }
            } else {
                allKeysMatch = false;
                break;
            }
        }
        
        if (allKeysMatch) {
            // Build primary key field names list for error message
            std::string keyFields;
            for (size_t j = 0; j < keyFieldIndices.size(); ++j) {
                if (j > 0) keyFields += ", ";
                keyFields += std::string(tableInfo.fields[keyFieldIndices[j]].sFieldName);
            }
            setError("Primary key constraint violation: Field (" + keyFields + ") value already exists");
            return false;
        }
    }
    
    return true;
}

bool UpdateHandler::checkUniqueConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                            const Record& updatedRecord, size_t currentRecordIndex, const std::string& dbName) {
    // 检查字段级唯一约束（bUnique标志）
    for (size_t i = 0; i < tableInfo.fields.size() && i < updatedRecord.values.size(); ++i) {
        const TableMode& field = tableInfo.fields[i];
        if (field.bUnique == FLAG_KEY) {  // 使用FLAG_KEY表示唯一约束
            if (!m_constraintManager.checkUniqueField(tableName, field.sFieldName, 
                                                      updatedRecord.values[i], 
                                                      dbName, static_cast<int>(currentRecordIndex))) {
                setError(m_constraintManager.getLastError());
                return false;
            }
        }
    }
    
    // 检查表级唯一约束（多字段）
    std::vector<UniqueConstraint> uniqueConstraints = 
        ConstraintRegistry::getInstance().getUniqueConstraints(dbName, tableName);
    
    for (const auto& uniqueConstraint : uniqueConstraints) {
        if (!m_constraintManager.checkUniqueConstraint(tableName, uniqueConstraint, 
                                                       updatedRecord, tableInfo, dbName, 
                                                       static_cast<int>(currentRecordIndex))) {
            setError(m_constraintManager.getLastError());
            return false;
        }
    }
    
    return true;
}

bool UpdateHandler::checkForeignKeyConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                                const Record& updatedRecord, const std::string& dbName) {
    // 从约束注册表获取外键约束
    std::vector<ForeignKeyConstraint> foreignKeys = 
        ConstraintRegistry::getInstance().getForeignKeyConstraints(dbName, tableName);
    
    for (const auto& fk : foreignKeys) {
        // 找到外键字段的值
        int fieldIndex = -1;
        for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
            if (std::string(tableInfo.fields[i].sFieldName) == std::string(fk.fieldName)) {
                fieldIndex = static_cast<int>(i);
                break;
            }
        }
        
        if (fieldIndex >= 0 && fieldIndex < static_cast<int>(updatedRecord.values.size())) {
            std::string value = updatedRecord.values[fieldIndex];
            if (!m_constraintManager.checkForeignKey(fk, value, dbName)) {
                setError(m_constraintManager.getLastError());
                return false;
            }
        }
    }
    
    return true;
}

// checkCheckConstraints implementation moved to update_handler_constraints.cpp

