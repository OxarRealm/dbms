#pragma once

#include "constraint.h"
#include <string>
#include <fstream>

/**
 * @file constraint_storage.h
 * @brief 约束存储管理器
 * 
 * 负责将约束信息保存到.cst文件，并在数据库打开时加载约束
 */

/**
 * @brief 约束存储管理器类
 */
class ConstraintStorageManager {
public:
    /**
     * @brief 保存数据库的所有约束到.cst文件
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径（用于确定.cst文件位置）
     * @return 成功返回true，失败返回false
     */
    static bool saveConstraints(const std::string& dbName, const std::string& dbPath);
    
    /**
     * @brief 从.cst文件加载数据库的所有约束
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径（用于确定.cst文件位置）
     * @return 成功返回true，失败返回false（文件不存在不算错误）
     */
    static bool loadConstraints(const std::string& dbName, const std::string& dbPath);
    
    /**
     * @brief 删除数据库的约束文件（当数据库被删除时）
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径（用于确定.cst文件位置）
     * @return 成功返回true，失败返回false
     */
    static bool deleteConstraints(const std::string& dbName, const std::string& dbPath);
    
    /**
     * @brief 获取约束文件路径
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径
     * @return 约束文件完整路径
     */
    static std::string getConstraintFilePath(const std::string& dbName, const std::string& dbPath);
    
private:
    /**
     * @brief 写入外键约束到文件流
     */
    static bool writeForeignKey(std::ofstream& file, const ForeignKeyConstraint& fk);
    
    /**
     * @brief 从文件流读取外键约束
     */
    static bool readForeignKey(std::ifstream& file, ForeignKeyConstraint& fk);
    
    /**
     * @brief 写入唯一约束到文件流
     */
    static bool writeUniqueConstraint(std::ofstream& file, const UniqueConstraint& unique);
    
    /**
     * @brief 从文件流读取唯一约束
     */
    static bool readUniqueConstraint(std::ifstream& file, UniqueConstraint& unique);
    
    /**
     * @brief 写入检查约束到文件流
     */
    static bool writeCheckConstraint(std::ofstream& file, const CheckConstraint& check);
    
    /**
     * @brief 从文件流读取检查约束
     */
    static bool readCheckConstraint(std::ifstream& file, CheckConstraint& check);
};
