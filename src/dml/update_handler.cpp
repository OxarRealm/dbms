/**
 * @file update_handler.cpp
 * @brief UPDATE语句处理器实现
 */

#include "dml/update_handler.h"
#include "core/table_mode.h"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>

UpdateHandler::UpdateHandler() : m_updatedCount(0) {
}

UpdateHandler::~UpdateHandler() {
}

bool UpdateHandler::execute(const std::string& sql) {
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
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(updateNode->databaseFileName);
    m_dataManager.setDatabasePath(updateNode->databaseFileName);
    
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
        
        // 更新记录
        if (m_dataManager.updateRecord(updateNode->tableName, recordIndex, updatedRecord)) {
            m_updatedCount++;
        } else {
            setError("更新记录失败");
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
            std::stoi(value);
        } catch (...) {
            setError("字段 " + std::string(field.sFieldName) + " 期望整数，但得到: " + value);
            return false;
        }
    } else if (fieldType == "float" || fieldType == "double") {
        // 验证是否为浮点数
        try {
            if (fieldType == "float") {
                std::stof(value);
            } else {
                std::stod(value);
            }
        } catch (...) {
            setError("字段 " + std::string(field.sFieldName) + " 期望浮点数，但得到: " + value);
            return false;
        }
    }
    // char和string类型不需要额外验证
    
    return true;
}

