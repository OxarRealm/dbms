#pragma once

#include "constraint.h"
#include "table_mode.h"
#include "data_manager.h"
#include <string>
#include <vector>

/**
 * @file constraint_manager.h
 * @brief 约束管理器
 * 
 * 负责约束的检查、验证和管理
 */

class ConstraintManager {
public:
    ConstraintManager(DataManager* dataManager);
    
    /**
     * @brief 检查外键约束
     * @param constraint 外键约束
     * @param value 要检查的值
     * @param dbName 数据库名
     * @return true表示通过，false表示违反约束
     */
    bool checkForeignKey(const ForeignKeyConstraint& constraint, 
                        const std::string& value, 
                        const std::string& dbName);
    
    /**
     * @brief 检查唯一约束（单字段）
     * @param tableName 表名
     * @param fieldName 字段名
     * @param value 要检查的值
     * @param dbName 数据库名
     * @param excludeRecordIndex 排除的记录索引（用于UPDATE）
     * @return true表示通过，false表示违反约束
     */
    bool checkUniqueField(const std::string& tableName,
                         const std::string& fieldName,
                         const std::string& value,
                         const std::string& dbName,
                         int excludeRecordIndex = -1);
    
    /**
     * @brief 检查唯一约束（多字段）
     * @param tableName 表名
     * @param constraint 唯一约束
     * @param record 记录
     * @param dbName 数据库名
     * @param excludeRecordIndex 排除的记录索引（用于UPDATE）
     * @return true表示通过，false表示违反约束
     */
    bool checkUniqueConstraint(const std::string& tableName,
                              const UniqueConstraint& constraint,
                              const Record& record,
                              const TableInfo& tableInfo,
                              const std::string& dbName,
                              int excludeRecordIndex = -1);
    
    /**
     * @brief 检查检查约束
     * @param constraint 检查约束
     * @param value 字段值
     * @param fieldType 字段类型
     * @return true表示通过，false表示违反约束
     */
    bool checkCheckConstraint(const CheckConstraint& constraint,
                             const std::string& value,
                             const std::string& fieldType);
    
    /**
     * @brief 执行外键级联删除
     * @param constraint 外键约束
     * @param referencedValue 被引用的值
     * @param dbName 数据库名
     * @return true表示成功
     */
    bool cascadeDelete(const ForeignKeyConstraint& constraint,
                      const std::string& referencedValue,
                      const std::string& dbName);
    
    /**
     * @brief 执行外键级联更新
     * @param constraint 外键约束
     * @param oldValue 旧值
     * @param newValue 新值
     * @param dbName 数据库名
     * @return true表示成功
     */
    bool cascadeUpdate(const ForeignKeyConstraint& constraint,
                      const std::string& oldValue,
                      const std::string& newValue,
                      const std::string& dbName);
    
    /**
     * @brief 获取最后错误信息
     */
    std::string getLastError() const { return m_lastError; }
    
private:
    DataManager* m_dataManager;
    std::string m_lastError;
    
    void setError(const std::string& error);
    
    /**
     * @brief 评估检查约束表达式（简化版）
     * 支持：>, <, >=, <=, =, !=, AND, OR
     */
    bool evaluateCheckExpression(const std::string& expression,
                                const std::string& value,
                                const std::string& fieldType);
    
    /**
     * @brief 评估单个条件表达式
     * @param condition 条件表达式（如 "> 0", "< 100"）
     * @param valueNum 数值
     * @return true表示满足条件
     */
    bool evaluateSingleCondition(const std::string& condition, double valueNum);
};

