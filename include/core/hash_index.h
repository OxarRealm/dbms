#pragma once

#include "core/table_mode.h"
#include <string>
#include <vector>
#include <map>
#include <list>

/**
 * @file hash_index.h
 * @brief 哈希索引实现
 *
 * 哈希索引用于优化点查询（等值查询）性能
 * 通过哈希表结构，支持O(1)平均时间复杂度的主键查询
 * 使用链地址法解决哈希冲突
 */

/**
 * @brief 哈希索引项
 * 存储哈希值对应的记录索引列表（处理冲突）
 */
struct HashIndexItem {
    std::string keyValue;              // 键值（字段值）
    std::vector<size_t> recordIndices; // 记录索引列表（可能有多个记录具有相同的键值）
    
    HashIndexItem() {}
    HashIndexItem(const std::string& value) : keyValue(value) {}
};

/**
 * @brief 哈希索引类
 *
 * 为字段建立哈希索引，优化点查询性能
 * 主要用于主键字段的快速查找
 */
class HashIndex {
public:
    /**
     * @brief 构造函数
     */
    HashIndex();

    /**
     * @brief 析构函数
     */
    ~HashIndex();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 为表的指定字段构建哈希索引
     * @param tableName 表名
     * @param fieldName 字段名（用于建立哈希索引）
     * @param fieldIndex 字段索引（在表结构中的位置）
     * @return 成功返回true，失败返回false
     */
    bool buildIndex(const std::string& tableName, const std::string& fieldName, size_t fieldIndex);

    /**
     * @brief 更新索引（当记录插入、更新或删除时）
     * @param tableName 表名
     * @param fieldName 字段名
     * @param fieldIndex 字段索引
     * @return 成功返回true，失败返回false
     */
    bool updateIndex(const std::string& tableName, const std::string& fieldName, size_t fieldIndex);

    /**
     * @brief 删除索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @return 成功返回true，失败返回false
     */
    bool removeIndex(const std::string& tableName, const std::string& fieldName);

    /**
     * @brief 检查索引是否存在
     * @param tableName 表名
     * @param fieldName 字段名
     * @return 存在返回true，不存在返回false
     */
    bool hasIndex(const std::string& tableName, const std::string& fieldName) const;

    /**
     * @brief 使用索引进行点查询（等值查询）
     * @param tableName 表名
     * @param fieldName 字段名
     * @param keyValue 要查找的键值
     * @param resultIndices 输出参数，匹配的记录索引列表
     * @return 成功返回true，失败返回false
     */
    bool pointQuery(const std::string& tableName, const std::string& fieldName,
                   const std::string& keyValue, std::vector<size_t>& resultIndices) const;

    /**
     * @brief 清除所有索引
     */
    void clear();

    /**
     * @brief 清除指定表的所有索引
     * @param tableName 表名
     */
    void clearTable(const std::string& tableName);

    /**
     * @brief 获取索引统计信息
     * @param tableName 表名
     * @param fieldName 字段名
     * @param bucketCount 输出参数，哈希桶数量
     * @param totalItems 输出参数，索引项总数
     * @param maxChainLength 输出参数，最长冲突链长度
     * @return 成功返回true，失败返回false
     */
    bool getIndexStats(const std::string& tableName, const std::string& fieldName,
                      size_t& bucketCount, size_t& totalItems, size_t& maxChainLength) const;

private:
    /**
     * @brief 默认哈希表大小
     */
    static const size_t DEFAULT_HASH_TABLE_SIZE = 1009;  // 质数，减少冲突

    /**
     * @brief 哈希表结构
     * 使用链地址法解决冲突：哈希值 -> 冲突链（list）
     */
    struct HashTable {
        std::vector<std::list<HashIndexItem>> buckets;  // 哈希桶数组
        size_t size;  // 哈希表大小
        
        HashTable(size_t tableSize = DEFAULT_HASH_TABLE_SIZE) : size(tableSize) {
            buckets.resize(size);
        }
    };

    /**
     * @brief 索引结构
     * 表名 -> 字段名 -> HashTable
     */
    std::map<std::string, std::map<std::string, HashTable>> m_indices;

    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）

    /**
     * @brief 计算字符串的哈希值
     * @param str 字符串
     * @param tableSize 哈希表大小
     * @return 哈希值（0 到 tableSize-1）
     */
    size_t hashFunction(const std::string& str, size_t tableSize) const;

    /**
     * @brief 计算整数的哈希值
     * @param value 整数值（字符串形式）
     * @param tableSize 哈希表大小
     * @return 哈希值（0 到 tableSize-1）
     */
    size_t hashInt(const std::string& value, size_t tableSize) const;

    /**
     * @brief 计算浮点数的哈希值
     * @param value 浮点数值（字符串形式）
     * @param tableSize 哈希表大小
     * @return 哈希值（0 到 tableSize-1）
     */
    size_t hashFloat(const std::string& value, size_t tableSize) const;

    /**
     * @brief 读取表的记录（用于构建索引）
     * @param tableName 表名
     * @param records 输出参数，记录列表
     * @return 成功返回true，失败返回false
     */
    bool readRecords(const std::string& tableName, std::vector<Record>& records) const;

    /**
     * @brief 读取表结构（用于获取字段类型）
     * @param tableName 表名
     * @param fields 输出参数，字段列表
     * @return 成功返回true，失败返回false
     */
    bool readTableFields(const std::string& tableName, std::vector<TableMode>& fields) const;
};

