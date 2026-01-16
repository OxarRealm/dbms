#pragma once

#include <string>
#include <vector>
#include <cstring>

/**
 * @file constraint.h
 * @brief 数据完整性约束定义
 * 
 * 定义外键、唯一约束、检查约束、默认值等约束结构
 */

// 常量定义（避免循环依赖，不包含table_mode.h）
#define CONSTRAINT_NAME_LENGTH 64
#define FIELD_NAME_LENGTH 32
#define TABLE_NAME_LENGTH 64

/**
 * @brief 外键约束信息
 */
struct ForeignKeyConstraint {
    char constraintName[CONSTRAINT_NAME_LENGTH];  // 约束名称
    char fieldName[FIELD_NAME_LENGTH];            // 本表字段名
    char referencedTable[TABLE_NAME_LENGTH];      // 引用表名
    char referencedField[FIELD_NAME_LENGTH];      // 引用字段名
    char onDeleteAction[16];                      // ON DELETE动作：CASCADE, SET NULL, RESTRICT, NO ACTION
    char onUpdateAction[16];                      // ON UPDATE动作：CASCADE, SET NULL, RESTRICT, NO ACTION
    
    ForeignKeyConstraint() {
        constraintName[0] = '\0';
        fieldName[0] = '\0';
        referencedTable[0] = '\0';
        referencedField[0] = '\0';
        strncpy(onDeleteAction, "RESTRICT", 15);
        onDeleteAction[15] = '\0';
        strncpy(onUpdateAction, "RESTRICT", 15);
        onUpdateAction[15] = '\0';
    }
};

/**
 * @brief 唯一约束信息
 */
struct UniqueConstraint {
    char constraintName[CONSTRAINT_NAME_LENGTH];  // 约束名称
    std::vector<std::string> fieldNames;          // 字段名列表（支持多字段唯一约束）
    
    UniqueConstraint() {
        constraintName[0] = '\0';
    }
};

/**
 * @brief 检查约束信息
 */
struct CheckConstraint {
    char constraintName[CONSTRAINT_NAME_LENGTH];  // 约束名称
    char fieldName[FIELD_NAME_LENGTH];            // 字段名
    std::string expression;                        // 检查表达式（如 "Age > 0 AND Age < 150"）
    
    CheckConstraint() {
        constraintName[0] = '\0';
        fieldName[0] = '\0';
    }
};

/**
 * @brief 字段约束信息（扩展TableMode）
 */
struct FieldConstraints {
    bool isUnique;                                // 是否为唯一约束
    std::string defaultValue;                    // 默认值（字符串形式）
    bool hasDefault;                              // 是否有默认值
    
    FieldConstraints() : isUnique(false), hasDefault(false) {}
};

/**
 * @brief 表级约束信息
 */
struct TableConstraints {
    std::vector<ForeignKeyConstraint> foreignKeys;    // 外键约束列表
    std::vector<UniqueConstraint> uniqueConstraints;  // 唯一约束列表
    std::vector<CheckConstraint> checkConstraints;    // 检查约束列表
    
    TableConstraints() {}
};

