/**
 * @file insert_handler.cpp
 * @brief INSERT语句处理器实现
 */

#include "dml/insert_handler.h"
#include "core/table_mode.h"
#include "core/constraint.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <iostream>

InsertHandler::InsertHandler() : m_constraintManager(&m_dataManager) {
}

InsertHandler::~InsertHandler() {
}

bool InsertHandler::execute(const std::string& sql, const std::string& basePath) {
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
    
    // 解析数据库路径：如果提供了basePath，使用它来解析相对路径
    // 否则，假设databaseFileName是完整路径或当前目录下的文件名
    std::string dbPath = insertNode->databaseFileName;
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
            dbPath = baseDir + insertNode->databaseFileName;
        }
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(dbPath);
    m_dataManager.setDatabasePath(dbPath);
    
    // 提取数据库名（baseName）用于约束查询
    // 约束注册时使用baseName，所以这里也需要提取baseName
    // 注意：这里使用insertNode->databaseFileName（SQL中的数据库名），而不是dbPath（完整路径）
    std::string dbNameForConstraints = insertNode->databaseFileName;
    try {
        // 尝试从路径中提取文件名（不含扩展名）
        std::filesystem::path dbPathObj(insertNode->databaseFileName);
        std::string fileName = dbPathObj.filename().string();
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
        std::string dbPathStr = insertNode->databaseFileName;
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
    std::cerr << "[INSERT] dbNameForConstraints=" << dbNameForConstraints 
              << ", insertNode->databaseFileName=" << insertNode->databaseFileName 
              << ", dbPath=" << dbPath << std::endl;
    
    // 读取表结构
    TableInfo tableInfo;
    if (!m_tableManager.readTable(insertNode->tableName, tableInfo)) {
        setError("Table does not exist: " + insertNode->tableName);
        return false;
    }
    
    // 应用默认值
    applyDefaultValues(insertNode, tableInfo);
    
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
    
    // 检查字段级唯一约束
    if (!checkUniqueConstraints(insertNode->tableName, tableInfo, record, dbNameForConstraints)) {
        return false;
    }
    
    // 检查外键约束
    if (!checkForeignKeyConstraints(insertNode->tableName, tableInfo, record, dbNameForConstraints)) {
        return false;
    }
    
    // 检查检查约束
    if (!checkCheckConstraints(insertNode->tableName, tableInfo, record, dbNameForConstraints)) {
        return false;
    }
    
    // 插入记录
    if (!m_dataManager.insertRecord(insertNode->tableName, record)) {
        setError("Failed to insert record into table '" + insertNode->tableName + "'");
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
                (void)std::stoi(value);  // 显式忽略返回值，避免[[nodiscard]]警告
            } catch (...) {
                setError("Field " + std::string(field.sFieldName) + " expects integer, but got: " + value);
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

