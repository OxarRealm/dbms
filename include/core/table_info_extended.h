#pragma once

#include "table_mode.h"
#include "constraint.h"

/**
 * @file table_info_extended.h
 * @brief 扩展的TableInfo结构（包含约束信息）
 * 
 * 这个文件提供了包含约束信息的TableInfo扩展结构
 * 用于需要约束信息的场景
 */

/**
 * @brief 扩展的表结构信息（包含约束）
 */
struct TableInfoExtended : public TableInfo {
    // 表级约束信息
    std::vector<ForeignKeyConstraint> foreignKeys;    // 外键约束列表
    std::vector<UniqueConstraint> uniqueConstraints;  // 唯一约束列表（多字段）
    std::vector<CheckConstraint> checkConstraints;    // 检查约束列表
    
    TableInfoExtended() : TableInfo() {}
    
    TableInfoExtended(const char* name) : TableInfo(name) {}
    
    // 从基础TableInfo转换
    TableInfoExtended(const TableInfo& base) : TableInfo(base) {}
};

