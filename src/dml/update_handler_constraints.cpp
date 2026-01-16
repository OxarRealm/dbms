/**
 * @file update_handler_constraints.cpp
 * @brief UPDATE约束检查实现
 * 
 * 注意：checkCheckConstraints的实现与InsertHandler::checkCheckConstraints完全一致
 * 直接复制INSERT的逻辑，确保UPDATE和INSERT使用相同的约束检查逻辑
 */

#include "dml/update_handler.h"
#include "core/constraint_registry.h"
#include "core/constraint.h"
#include <iostream>

bool UpdateHandler::checkCheckConstraints(const std::string& tableName, const TableInfo& tableInfo, 
                                          const Record& updatedRecord, const std::string& dbName) {
    // 从约束注册表获取检查约束
    // 此实现与InsertHandler::checkCheckConstraints完全一致（直接复制）
    std::vector<CheckConstraint> checkConstraints = 
        ConstraintRegistry::getInstance().getCheckConstraints(dbName, tableName);
    
    // 调试输出：检查是否获取到约束
    std::cerr << "[UPDATE CHECK] dbName=" << dbName << ", tableName=" << tableName 
              << ", checkConstraints.size()=" << checkConstraints.size() << std::endl;
    
    for (const auto& checkConstraint : checkConstraints) {
        std::cerr << "[UPDATE CHECK] Processing constraint: fieldName=" << checkConstraint.fieldName 
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
        
        std::cerr << "[UPDATE CHECK] fieldIndex=" << fieldIndex 
                  << ", updatedRecord.values.size()=" << updatedRecord.values.size() << std::endl;
        
        if (fieldIndex >= 0 && fieldIndex < static_cast<int>(updatedRecord.values.size())) {
            std::string value = updatedRecord.values[fieldIndex];
            std::cerr << "[UPDATE CHECK] Checking value=" << value << ", fieldType=" << fieldType 
                      << ", expression=" << checkConstraint.expression << std::endl;
            
            if (!m_constraintManager.checkCheckConstraint(checkConstraint, value, fieldType)) {
                std::cerr << "[UPDATE CHECK] Constraint violation: " << m_constraintManager.getLastError() << std::endl;
                setError(m_constraintManager.getLastError());
                return false;
            }
            std::cerr << "[UPDATE CHECK] Constraint passed" << std::endl;
        } else {
            std::cerr << "[UPDATE CHECK] WARNING: fieldIndex out of range or not found" << std::endl;
        }
    }
    
    return true;
}

