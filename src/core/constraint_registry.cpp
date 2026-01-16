/**
 * @file constraint_registry.cpp
 * @brief 约束注册表实现
 */

#include "core/constraint_registry.h"
#include <algorithm>

ConstraintRegistry& ConstraintRegistry::getInstance() {
    static ConstraintRegistry instance;
    return instance;
}

void ConstraintRegistry::registerTableConstraints(const std::string& dbName, 
                                                  const std::string& tableName,
                                                  const TableConstraints& constraints) {
    m_constraints[dbName][tableName] = constraints;
}

bool ConstraintRegistry::getTableConstraints(const std::string& dbName,
                                            const std::string& tableName,
                                            TableConstraints& constraints) const {
    auto dbIt = m_constraints.find(dbName);
    if (dbIt == m_constraints.end()) {
        return false;
    }
    
    auto tableIt = dbIt->second.find(tableName);
    if (tableIt == dbIt->second.end()) {
        return false;
    }
    
    constraints = tableIt->second;
    return true;
}

std::vector<ForeignKeyConstraint> ConstraintRegistry::getForeignKeyConstraints(
    const std::string& dbName,
    const std::string& tableName) const {
    std::vector<ForeignKeyConstraint> result;
    
    TableConstraints constraints;
    if (getTableConstraints(dbName, tableName, constraints)) {
        result = constraints.foreignKeys;
    }
    
    return result;
}

std::vector<UniqueConstraint> ConstraintRegistry::getUniqueConstraints(
    const std::string& dbName,
    const std::string& tableName) const {
    std::vector<UniqueConstraint> result;
    
    TableConstraints constraints;
    if (getTableConstraints(dbName, tableName, constraints)) {
        result = constraints.uniqueConstraints;
    }
    
    return result;
}

std::vector<CheckConstraint> ConstraintRegistry::getCheckConstraints(
    const std::string& dbName,
    const std::string& tableName) const {
    std::vector<CheckConstraint> result;
    
    TableConstraints constraints;
    if (getTableConstraints(dbName, tableName, constraints)) {
        result = constraints.checkConstraints;
    }
    
    return result;
}

std::vector<std::pair<std::string, ForeignKeyConstraint>> ConstraintRegistry::getReferencingConstraints(
    const std::string& dbName,
    const std::string& referencedTableName) const {
    std::vector<std::pair<std::string, ForeignKeyConstraint>> result;
    
    auto dbIt = m_constraints.find(dbName);
    if (dbIt == m_constraints.end()) {
        return result;
    }
    
    // 遍历该数据库下的所有表
    for (const auto& tablePair : dbIt->second) {
        const std::string& tableName = tablePair.first;
        const TableConstraints& constraints = tablePair.second;
        
        // 检查该表的所有外键约束
        for (const auto& fk : constraints.foreignKeys) {
            if (std::string(fk.referencedTable) == referencedTableName) {
                result.push_back({tableName, fk});
            }
        }
    }
    
    return result;
}

void ConstraintRegistry::removeTableConstraints(const std::string& dbName, const std::string& tableName) {
    auto dbIt = m_constraints.find(dbName);
    if (dbIt != m_constraints.end()) {
        dbIt->second.erase(tableName);
        if (dbIt->second.empty()) {
            m_constraints.erase(dbIt);
        }
    }
}

void ConstraintRegistry::clear() {
    m_constraints.clear();
}

void ConstraintRegistry::clearDatabase(const std::string& dbName) {
    m_constraints.erase(dbName);
}

std::vector<std::string> ConstraintRegistry::getTableNames(const std::string& dbName) const {
    std::vector<std::string> result;
    auto dbIt = m_constraints.find(dbName);
    if (dbIt != m_constraints.end()) {
        for (const auto& tablePair : dbIt->second) {
            result.push_back(tablePair.first);
        }
    }
    return result;
}

