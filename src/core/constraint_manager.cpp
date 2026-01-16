/**
 * @file constraint_manager.cpp
 * @brief 约束管理器实现
 */

#include "core/constraint_manager.h"
#include "core/constraint.h"
#include "core/table_manager.h"
#include "core/table_mode.h"
#include <algorithm>
#include <sstream>
#include <cmath>
#include <iostream>

ConstraintManager::ConstraintManager(DataManager* dataManager) 
    : m_dataManager(dataManager) {
}

void ConstraintManager::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "ConstraintManager错误: " << error << std::endl;
}

bool ConstraintManager::checkForeignKey(const ForeignKeyConstraint& constraint, 
                                        const std::string& value, 
                                        const std::string& dbName) {
    // 如果值为空，外键约束不检查（由NOT NULL约束处理）
    if (value.empty()) {
        return true;
    }
    
    // 读取引用表的所有记录
    std::vector<Record> records;
    if (!m_dataManager->readAllRecords(constraint.referencedTable, records)) {
        setError("Failed to read referenced table: " + std::string(constraint.referencedTable));
        return false;
    }
    
    // 获取引用表的表结构
    TableManager tableManager;
    tableManager.setDatabasePath(dbName);
    TableInfo refTableInfo;
    if (!tableManager.readTable(constraint.referencedTable, refTableInfo)) {
        setError("Failed to load referenced table structure: " + std::string(constraint.referencedTable));
        return false;
    }
    
    // 找到引用字段的索引
    int refFieldIndex = -1;
    for (size_t i = 0; i < refTableInfo.fields.size(); ++i) {
        if (strcmp(refTableInfo.fields[i].sFieldName, constraint.referencedField) == 0) {
            refFieldIndex = static_cast<int>(i);
            break;
        }
    }
    
    if (refFieldIndex == -1) {
        setError("Referenced field not found: " + std::string(constraint.referencedField));
        return false;
    }
    
    // 检查值是否存在于引用表中
    for (const auto& record : records) {
        if (record.isValid() && record.getValue(refFieldIndex) == value) {
            return true;  // 找到匹配值，外键约束通过
        }
    }
    
    setError("Foreign key constraint violation: value '" + value + 
             "' not found in referenced table '" + std::string(constraint.referencedTable) + 
             "' field '" + std::string(constraint.referencedField) + "'");
    return false;
}

bool ConstraintManager::checkUniqueField(const std::string& tableName,
                                        const std::string& fieldName,
                                        const std::string& value,
                                        const std::string& dbName,
                                        int excludeRecordIndex) {
    // 如果值为空，唯一约束不检查（由NOT NULL约束处理）
    if (value.empty()) {
        return true;
    }
    
    // 读取所有记录
    std::vector<Record> records;
    if (!m_dataManager->readAllRecords(tableName, records)) {
        return true;  // 如果读取失败，假设没有冲突
    }
    
    // 获取表结构
    TableManager tableManager;
    tableManager.setDatabasePath(dbName);
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return true;  // 如果加载失败，假设没有冲突
    }
    
    // 找到字段索引
    int fieldIndex = -1;
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (strcmp(tableInfo.fields[i].sFieldName, fieldName.c_str()) == 0) {
            fieldIndex = static_cast<int>(i);
            break;
        }
    }
    
    if (fieldIndex == -1) {
        return true;  // 字段不存在，不检查
    }
    
    // 检查唯一性
    for (size_t i = 0; i < records.size(); ++i) {
        if (static_cast<int>(i) == excludeRecordIndex) {
            continue;  // 排除当前记录（用于UPDATE）
        }
        
        if (records[i].isValid() && records[i].getValue(fieldIndex) == value) {
            setError("Unique constraint violation: value '" + value + 
                     "' already exists in field '" + fieldName + "'");
            return false;
        }
    }
    
    return true;
}

bool ConstraintManager::checkUniqueConstraint(const std::string& tableName,
                                             const UniqueConstraint& constraint,
                                             const Record& record,
                                             const TableInfo& tableInfo,
                                             const std::string& dbName,
                                             int excludeRecordIndex) {
    // 读取所有记录
    std::vector<Record> records;
    if (!m_dataManager->readAllRecords(tableName, records)) {
        return true;  // 如果读取失败，假设没有冲突
    }
    
    // 找到所有字段的索引
    std::vector<int> fieldIndices;
    for (const auto& fieldName : constraint.fieldNames) {
        int fieldIndex = -1;
        for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
            if (strcmp(tableInfo.fields[i].sFieldName, fieldName.c_str()) == 0) {
                fieldIndex = static_cast<int>(i);
                break;
            }
        }
        if (fieldIndex == -1) {
            setError("Field not found in unique constraint: " + fieldName);
            return false;
        }
        fieldIndices.push_back(fieldIndex);
    }
    
    // 检查唯一性
    for (size_t i = 0; i < records.size(); ++i) {
        if (static_cast<int>(i) == excludeRecordIndex) {
            continue;  // 排除当前记录（用于UPDATE）
        }
        
        if (!records[i].isValid()) {
            continue;
        }
        
        // 检查所有字段值是否都匹配
        bool allMatch = true;
        for (int fieldIndex : fieldIndices) {
            if (records[i].getValue(fieldIndex) != record.getValue(fieldIndex)) {
                allMatch = false;
                break;
            }
        }
        
        if (allMatch) {
            std::string fieldNamesStr;
            for (size_t j = 0; j < constraint.fieldNames.size(); ++j) {
                if (j > 0) fieldNamesStr += ", ";
                fieldNamesStr += constraint.fieldNames[j];
            }
            setError("Unique constraint violation: combination of values already exists in fields (" + fieldNamesStr + ")");
            return false;
        }
    }
    
    return true;
}

bool ConstraintManager::checkCheckConstraint(const CheckConstraint& constraint,
                                            const std::string& value,
                                            const std::string& fieldType) {
    if (!evaluateCheckExpression(constraint.expression, value, fieldType)) {
        // 生成详细的错误消息
        std::string constraintName = constraint.constraintName[0] != '\0' ? 
                                     std::string(constraint.constraintName) : "unnamed";
        setError("Check constraint violation: value '" + value + 
                "' does not satisfy constraint '" + constraint.expression + 
                "' on field '" + std::string(constraint.fieldName) + "'");
        return false;
    }
    return true;
}

bool ConstraintManager::evaluateCheckExpression(const std::string& expression,
                                               const std::string& value,
                                               const std::string& fieldType) {
    // 支持：>, <, >=, <=, =, !=, AND, OR
    // 示例：Age > 0 AND Age < 150, Price > 0 AND Price < 100
    
    // 如果值为空，检查约束不检查（由NOT NULL约束处理）
    if (value.empty()) {
        return true;
    }
    
    // 将表达式转换为小写以便解析（但保留运算符的大小写）
    std::string expr = expression;
    
    // 尝试解析数值比较
    try {
        double valueNum = 0.0;
        if (fieldType == "int" || fieldType == "float" || fieldType == "double") {
            valueNum = std::stod(value);
        } else {
            // 非数值类型，不支持复杂表达式
            return true;
        }
        
        // 检查是否包含AND或OR
        std::string exprLower = expr;
        std::transform(exprLower.begin(), exprLower.end(), exprLower.begin(), ::tolower);
        
        size_t andPos = exprLower.find(" and ");
        size_t orPos = exprLower.find(" or ");
        
        if (andPos != std::string::npos) {
            // 包含AND，需要同时满足两个条件
            std::string leftExpr = expr.substr(0, andPos);
            std::string rightExpr = expr.substr(andPos + 5); // " and " 长度为5
            
            // 递归评估左右两个表达式
            bool leftResult = evaluateSingleCondition(leftExpr, valueNum);
            bool rightResult = evaluateSingleCondition(rightExpr, valueNum);
            return leftResult && rightResult;
        } else if (orPos != std::string::npos) {
            // 包含OR，满足任一条件即可
            std::string leftExpr = expr.substr(0, orPos);
            std::string rightExpr = expr.substr(orPos + 4); // " or " 长度为4
            
            // 递归评估左右两个表达式
            bool leftResult = evaluateSingleCondition(leftExpr, valueNum);
            bool rightResult = evaluateSingleCondition(rightExpr, valueNum);
            return leftResult || rightResult;
        } else {
            // 单个条件
            return evaluateSingleCondition(expr, valueNum);
        }
    } catch (...) {
        // 如果转换失败，默认返回true（假设通过）
        return true;
    }
}

bool ConstraintManager::evaluateSingleCondition(const std::string& condition, double valueNum) {
    // 移除前后空格
    std::string cond = condition;
    while (!cond.empty() && std::isspace(cond[0])) {
        cond.erase(0, 1);
    }
    while (!cond.empty() && std::isspace(cond.back())) {
        cond.pop_back();
    }
    
    // 查找比较运算符（按优先级：>=, <=, !=, >, <, =）
    size_t pos = cond.find(">=");
    if (pos != std::string::npos) {
        std::string rightStr = cond.substr(pos + 2);
        rightStr.erase(0, rightStr.find_first_not_of(" \t"));
        try {
            double rightNum = std::stod(rightStr);
            return valueNum >= rightNum;
        } catch (...) {
            return true;
        }
    }
    
    pos = cond.find("<=");
    if (pos != std::string::npos) {
        std::string rightStr = cond.substr(pos + 2);
        rightStr.erase(0, rightStr.find_first_not_of(" \t"));
        try {
            double rightNum = std::stod(rightStr);
            return valueNum <= rightNum;
        } catch (...) {
            return true;
        }
    }
    
    pos = cond.find("!=");
    if (pos != std::string::npos) {
        std::string rightStr = cond.substr(pos + 2);
        rightStr.erase(0, rightStr.find_first_not_of(" \t"));
        try {
            double rightNum = std::stod(rightStr);
            return std::abs(valueNum - rightNum) > 1e-9;
        } catch (...) {
            return true;
        }
    }
    
    pos = cond.find(">");
    if (pos != std::string::npos) {
        std::string rightStr = cond.substr(pos + 1);
        rightStr.erase(0, rightStr.find_first_not_of(" \t"));
        try {
            double rightNum = std::stod(rightStr);
            return valueNum > rightNum;
        } catch (...) {
            return true;
        }
    }
    
    pos = cond.find("<");
    if (pos != std::string::npos) {
        std::string rightStr = cond.substr(pos + 1);
        rightStr.erase(0, rightStr.find_first_not_of(" \t"));
        try {
            double rightNum = std::stod(rightStr);
            return valueNum < rightNum;
        } catch (...) {
            return true;
        }
    }
    
    pos = cond.find("=");
    if (pos != std::string::npos) {
        std::string rightStr = cond.substr(pos + 1);
        rightStr.erase(0, rightStr.find_first_not_of(" \t"));
        try {
            double rightNum = std::stod(rightStr);
            return std::abs(valueNum - rightNum) < 1e-9;
        } catch (...) {
            return true;
        }
    }
    
    // 如果无法解析，默认返回true
    return true;
}

bool ConstraintManager::cascadeDelete(const ForeignKeyConstraint& constraint,
                                     const std::string& referencedValue,
                                     const std::string& dbName) {
    if (strcmp(constraint.onDeleteAction, "CASCADE") != 0) {
        return true;  // 不是级联删除，不需要处理
    }
    
    // 读取所有引用该值的记录
    // 注意：这里需要知道引用该外键的表名，暂时简化处理
    // 实际实现中需要维护外键反向索引
    // 级联删除的完整实现需要维护外键反向索引
    // 暂时简化处理，返回true
    return true;
}

bool ConstraintManager::cascadeUpdate(const ForeignKeyConstraint& constraint,
                                     const std::string& oldValue,
                                     const std::string& newValue,
                                     const std::string& dbName) {
    if (strcmp(constraint.onUpdateAction, "CASCADE") != 0) {
        return true;  // 不是级联更新，不需要处理
    }
    
    // 类似cascadeDelete的实现，更新所有引用该值的记录
    // 这里简化处理
    return true;
}

