/**
 * @file insert_handler_constraints.cpp
 * @brief INSERT处理器约束检查函数实现
 */

#include "dml/insert_handler.h"
#include "core/constraint.h"
#include "core/constraint_registry.h"
#include <cstring>
#include <iostream>

void InsertHandler::applyDefaultValues(InsertNode* node, const TableInfo& tableInfo) {
    // 如果值数量不足，补充默认值
    while (node->values.size() < tableInfo.fields.size()) {
        node->values.push_back("");
    }
    
    // 对于空值且有默认值的字段，应用默认值
    for (size_t i = 0; i < tableInfo.fields.size() && i < node->values.size(); ++i) {
        const TableMode& field = tableInfo.fields[i];
        std::string& value = node->values[i];
        
        // 如果值为空（空字符串或只包含空白字符）且有默认值，应用默认值
        // 注意：trim空白字符，因为GUI可能传递空白字符串
        std::string trimmedValue = value;
        // 移除前后空白
        if (!trimmedValue.empty()) {
            size_t start = trimmedValue.find_first_not_of(" \t\n\r");
            if (start != std::string::npos) {
                trimmedValue = trimmedValue.substr(start);
            } else {
                trimmedValue = "";
            }
            size_t end = trimmedValue.find_last_not_of(" \t\n\r");
            if (end != std::string::npos) {
                trimmedValue = trimmedValue.substr(0, end + 1);
            } else {
                trimmedValue = "";
            }
        }
        
        if (trimmedValue.empty() && field.sDefaultValue[0] != '\0') {
            value = std::string(field.sDefaultValue);
        }
    }
}

bool InsertHandler::checkUniqueConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                            const Record& record, const std::string& dbName) {
    // 检查字段级唯一约束（bUnique标志）
    for (size_t i = 0; i < tableInfo.fields.size() && i < record.values.size(); ++i) {
        const TableMode& field = tableInfo.fields[i];
        if (field.bUnique == FLAG_KEY) {  // 使用FLAG_KEY表示唯一约束
            if (!m_constraintManager.checkUniqueField(tableName, field.sFieldName, 
                                                      record.values[i], 
                                                      dbName, -1)) {
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
                                                       record, tableInfo, dbName, -1)) {
            setError(m_constraintManager.getLastError());
            return false;
        }
    }
    
    return true;
}

bool InsertHandler::checkForeignKeyConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                                const Record& record, const std::string& dbName) {
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
        
        if (fieldIndex >= 0 && fieldIndex < static_cast<int>(record.values.size())) {
            std::string value = record.values[fieldIndex];
            if (!m_constraintManager.checkForeignKey(fk, value, dbName)) {
                setError(m_constraintManager.getLastError());
                return false;
            }
        }
    }
    
    return true;
}

bool InsertHandler::checkCheckConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                          const Record& record, const std::string& dbName) {
    // 从约束注册表获取检查约束
    std::vector<CheckConstraint> checkConstraints = 
        ConstraintRegistry::getInstance().getCheckConstraints(dbName, tableName);
    
    // 调试输出：检查是否获取到约束
    std::cerr << "[INSERT CHECK] dbName=" << dbName << ", tableName=" << tableName 
              << ", checkConstraints.size()=" << checkConstraints.size() << std::endl;
    
    for (const auto& checkConstraint : checkConstraints) {
        std::cerr << "[INSERT CHECK] Processing constraint: fieldName=" << checkConstraint.fieldName 
                  << ", expression=" << checkConstraint.expression << std::endl;
        
        // 找到检查约束字段的值
        int fieldIndex = -1;
        std::string fieldType;
        for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
            if (std::string(tableInfo.fields[i].sFieldName) == std::string(checkConstraint.fieldName)) {
                fieldIndex = static_cast<int>(i);
                fieldType = tableInfo.fields[i].sType;
                break;
            }
        }
        
        std::cerr << "[INSERT CHECK] fieldIndex=" << fieldIndex 
                  << ", record.values.size()=" << record.values.size() << std::endl;
        
        if (fieldIndex >= 0 && fieldIndex < static_cast<int>(record.values.size())) {
            std::string value = record.values[fieldIndex];
            std::cerr << "[INSERT CHECK] Checking value=" << value << ", fieldType=" << fieldType 
                      << ", expression=" << checkConstraint.expression << std::endl;
            
            if (!m_constraintManager.checkCheckConstraint(checkConstraint, value, fieldType)) {
                std::cerr << "[INSERT CHECK] Constraint violation: " << m_constraintManager.getLastError() << std::endl;
                setError(m_constraintManager.getLastError());
                return false;
            }
            std::cerr << "[INSERT CHECK] Constraint passed" << std::endl;
        } else {
            std::cerr << "[INSERT CHECK] WARNING: fieldIndex out of range or not found" << std::endl;
        }
    }
    
    return true;
}


