#pragma once

#include "constraint.h"
#include <string>
#include <map>
#include <vector>

/**
 * @file constraint_registry.h
 * @brief 约束注册表（内存存储）
 * 
 * 负责在内存中存储和管理表的约束信息
 * 注意：这是临时方案，将来可以扩展为持久化存储
 */

/**
 * @brief 约束注册表类
 * 
 * 使用单例模式，全局管理所有表的约束信息
 */
class ConstraintRegistry {
public:
    /**
     * @brief 获取单例实例
     */
    static ConstraintRegistry& getInstance();
    
    /**
     * @brief 注册表的约束信息
     * @param dbName 数据库名
     * @param tableName 表名
     * @param constraints 约束信息
     */
    void registerTableConstraints(const std::string& dbName, 
                                  const std::string& tableName,
                                  const TableConstraints& constraints);
    
    /**
     * @brief 获取表的约束信息
     * @param dbName 数据库名
     * @param tableName 表名
     * @param constraints 输出参数，约束信息
     * @return 如果找到返回true，否则返回false
     */
    bool getTableConstraints(const std::string& dbName,
                            const std::string& tableName,
                            TableConstraints& constraints) const;
    
    /**
     * @brief 获取表的所有外键约束
     * @param dbName 数据库名
     * @param tableName 表名
     * @return 外键约束列表
     */
    std::vector<ForeignKeyConstraint> getForeignKeyConstraints(
        const std::string& dbName,
        const std::string& tableName) const;
    
    /**
     * @brief 获取表的所有唯一约束
     * @param dbName 数据库名
     * @param tableName 表名
     * @return 唯一约束列表
     */
    std::vector<UniqueConstraint> getUniqueConstraints(
        const std::string& dbName,
        const std::string& tableName) const;
    
    /**
     * @brief 获取表的所有检查约束
     * @param dbName 数据库名
     * @param tableName 表名
     * @return 检查约束列表
     */
    std::vector<CheckConstraint> getCheckConstraints(
        const std::string& dbName,
        const std::string& tableName) const;
    
    /**
     * @brief 获取引用指定表的所有外键约束（用于DELETE检查）
     * @param dbName 数据库名
     * @param referencedTableName 被引用的表名
     * @return 外键约束列表（包含表名和字段名）
     */
    std::vector<std::pair<std::string, ForeignKeyConstraint>> getReferencingConstraints(
        const std::string& dbName,
        const std::string& referencedTableName) const;
    
    /**
     * @brief 删除表的约束信息
     * @param dbName 数据库名
     * @param tableName 表名
     */
    void removeTableConstraints(const std::string& dbName, const std::string& tableName);
    
    /**
     * @brief 清空所有约束信息（用于切换数据库）
     */
    void clear();
    
    /**
     * @brief 清空指定数据库的所有约束信息
     * @param dbName 数据库名
     */
    void clearDatabase(const std::string& dbName);
    
    /**
     * @brief 获取指定数据库的所有表名（用于约束存储）
     * @param dbName 数据库名
     * @return 表名列表
     */
    std::vector<std::string> getTableNames(const std::string& dbName) const;
    
private:
    ConstraintRegistry() = default;
    ~ConstraintRegistry() = default;
    ConstraintRegistry(const ConstraintRegistry&) = delete;
    ConstraintRegistry& operator=(const ConstraintRegistry&) = delete;
    
    // 存储结构：dbName -> tableName -> TableConstraints
    std::map<std::string, std::map<std::string, TableConstraints>> m_constraints;
};

