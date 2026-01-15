/**
 * @file insert_handler.cpp
 * @brief INSERT语句处理器实现
 */

#include "dml/insert_handler.h"
#include "core/table_mode.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

InsertHandler::InsertHandler() {
}

InsertHandler::~InsertHandler() {
}

bool InsertHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> ast = parser.parse();
    
    if (!ast) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 检查是否为INSERT节点
    InsertNode* insertNode = dynamic_cast<InsertNode*>(ast.get());
    if (!insertNode) {
        setError("Not an INSERT statement");
        return false;
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(insertNode->databaseFileName);
    m_dataManager.setDatabasePath(insertNode->databaseFileName);
    
    // 读取表结构
    TableInfo tableInfo;
    if (!m_tableManager.readTable(insertNode->tableName, tableInfo)) {
        setError("Table does not exist: " + insertNode->tableName);
        return false;
    }
    
    // 验证插入的数据
    if (!validateInsertData(insertNode, tableInfo)) {
        return false;
    }
    
    // 创建记录
    Record record = createRecord(insertNode, tableInfo);
    
    // 检查主键唯一性约束
    if (!checkPrimaryKeyUnique(insertNode->tableName, tableInfo, record)) {
        return false;
    }
    
    // 插入记录
    if (!m_dataManager.insertRecord(insertNode->tableName, record)) {
        setError("Failed to insert record");
        return false;
    }
    
    return true;
}

std::string InsertHandler::getLastError() const {
    return m_lastError;
}

void InsertHandler::setError(const std::string& error) {
    m_lastError = error;
}

bool InsertHandler::validateInsertData(InsertNode* node, const TableInfo& tableInfo) {
    // 检查值数量是否匹配字段数量
    if (node->values.size() != tableInfo.fields.size()) {
        setError("Value count (" + std::to_string(node->values.size()) + 
                 ") does not match field count (" + std::to_string(tableInfo.fields.size()) + ")");
        return false;
    }
    
    // 检查每个值是否符合字段类型
    for (size_t i = 0; i < node->values.size(); ++i) {
        const std::string& value = node->values[i];
        const TableMode& field = tableInfo.fields[i];
        
        // 检查NULL约束
        if (value.empty() && field.bNullFlag == FLAG_NO_NULL) {
            setError("Field " + std::string(field.sFieldName) + " cannot be empty (NOT NULL constraint)");
            return false;
        }
        
        // 如果值为空且允许NULL，跳过类型检查
        if (value.empty() && field.bNullFlag == FLAG_NULL) {
            continue;
        }
        
        // 类型验证（基本验证，实际转换在createRecord中）
        std::string fieldType(field.sType);
        if (fieldType == "int") {
            // 验证是否为整数
            try {
                std::stoi(value);
            } catch (...) {
                setError("Field " + std::string(field.sFieldName) + " expects integer, but got: " + value);
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
                setError("Field " + std::string(field.sFieldName) + " expects float, but got: " + value);
                return false;
            }
        }
        // char和string类型不需要额外验证
    }
    
    return true;
}

std::string InsertHandler::convertValue(const std::string& value, const std::string& fieldType) {
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

Record InsertHandler::createRecord(InsertNode* node, const TableInfo& tableInfo) {
    Record record;
    record.validFlag = FLAG_VALID;
    
    // 转换并添加每个值
    for (size_t i = 0; i < node->values.size(); ++i) {
        const std::string& value = node->values[i];
        const TableMode& field = tableInfo.fields[i];
        
        std::string convertedValue = convertValue(value, std::string(field.sType));
        record.values.push_back(convertedValue);
    }
    
    return record;
}

bool InsertHandler::checkPrimaryKeyUnique(const std::string& tableName, const TableInfo& tableInfo, const Record& record) {
    // 查找所有主键字段
    std::vector<int> keyFieldIndices;
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (tableInfo.fields[i].bKey == FLAG_KEY) {
            keyFieldIndices.push_back(static_cast<int>(i));
        }
    }
    
    // 如果没有主键字段，不需要检查
    if (keyFieldIndices.empty()) {
        return true;
    }
    
    // 读取所有有效记录
    std::vector<Record> existingRecords;
    if (!m_dataManager.readValidRecords(tableName, existingRecords)) {
        // 如果读取失败，假设没有冲突（可能是新表）
        return true;
    }
    
    // 检查新记录的主键值是否与现有记录冲突
    for (const Record& existingRecord : existingRecords) {
        bool allKeysMatch = true;
        for (int keyIndex : keyFieldIndices) {
            if (keyIndex >= 0 && keyIndex < static_cast<int>(record.values.size()) &&
                keyIndex >= 0 && keyIndex < static_cast<int>(existingRecord.values.size())) {
                if (record.values[keyIndex] != existingRecord.values[keyIndex]) {
                    allKeysMatch = false;
                    break;
                }
            } else {
                allKeysMatch = false;
                break;
            }
        }
        
        if (allKeysMatch) {
            // 构建主键字段名列表用于错误信息
            std::string keyFields;
            for (size_t i = 0; i < keyFieldIndices.size(); ++i) {
                if (i > 0) keyFields += ", ";
                keyFields += std::string(tableInfo.fields[keyFieldIndices[i]].sFieldName);
            }
            setError("Primary key constraint violation: Field (" + keyFields + ") value already exists");
            return false;
        }
    }
    
    return true;
}

