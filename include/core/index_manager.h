#pragma once

#include "core/adjacent_index.h"
#include "core/hash_index.h"
#include "core/btree_index.h"
#include "core/table_mode.h"
#include <string>
#include <vector>
#include <map>

/**
 * @file index_manager.h
 * @brief 索引管理器
 *
 * 统一管理所有类型的索引（相邻索引、哈希索引、B+树索引）
 * 提供统一的索引创建、删除、查询接口
 */

/**
 * @brief 索引信息结构
 */
struct IndexInfo {
    std::string indexName;      // 索引名称（表名_字段名_类型）
    std::string tableName;      // 表名
    std::string fieldName;      // 字段名
    std::string indexType;      // 索引类型（"adjacent", "hash", "btree"）
    size_t fieldIndex;          // 字段索引
    bool isActive;              // 是否激活
    
    IndexInfo() : fieldIndex(0), isActive(true) {}
};

/**
 * @brief 索引管理器类
 *
 * 统一管理所有类型的索引，提供统一的接口
 */
class IndexManager {
public:
    /**
     * @brief 构造函数
     */
    IndexManager();

    /**
     * @brief 析构函数
     */
    ~IndexManager();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 创建索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @param indexType 索引类型（"adjacent", "hash", "btree"）
     * @return 成功返回true，失败返回false
     */
    bool createIndex(const std::string& tableName, const std::string& fieldName, 
                    const std::string& indexType);

    /**
     * @brief 删除索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @param indexType 索引类型（如果为空，删除该字段的所有索引）
     * @return 成功返回true，失败返回false
     */
    bool dropIndex(const std::string& tableName, const std::string& fieldName, 
                  const std::string& indexType = "");

    /**
     * @brief 检查索引是否存在
     * @param tableName 表名
     * @param fieldName 字段名
     * @param indexType 索引类型（如果为空，检查是否有任何类型的索引）
     * @return 存在返回true，不存在返回false
     */
    bool hasIndex(const std::string& tableName, const std::string& fieldName, 
                 const std::string& indexType = "") const;

    /**
     * @brief 获取表的所有索引
     * @param tableName 表名
     * @param indices 输出参数，索引信息列表
     * @return 成功返回true，失败返回false
     */
    bool getTableIndices(const std::string& tableName, std::vector<IndexInfo>& indices) const;

    /**
     * @brief 获取所有索引
     * @param indices 输出参数，索引信息列表
     * @return 成功返回true，失败返回false
     */
    bool getAllIndices(std::vector<IndexInfo>& indices) const;

    /**
     * @brief 更新索引（当记录插入、更新或删除时）
     * @param tableName 表名
     * @param fieldName 字段名（如果为空，更新该表的所有索引）
     * @return 成功返回true，失败返回false
     */
    bool updateIndex(const std::string& tableName, const std::string& fieldName = "");

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
     * @param indexType 索引类型
     * @param stats 输出参数，统计信息（格式：键值对）
     * @return 成功返回true，失败返回false
     */
    bool getIndexStats(const std::string& tableName, const std::string& fieldName, 
                      const std::string& indexType, 
                      std::map<std::string, std::string>& stats) const;

private:
    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）
    
    AdjacentIndex m_adjacentIndex;  // 相邻索引
    HashIndex m_hashIndex;          // 哈希索引
    BTreeIndex m_btreeIndex;        // B+树索引

    /**
     * @brief 索引信息映射
     * 表名 -> 字段名 -> 索引类型 -> IndexInfo
     */
    std::map<std::string, std::map<std::string, std::map<std::string, IndexInfo>>> m_indexInfo;

    /**
     * @brief 获取字段索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @param fieldIndex 输出参数，字段索引
     * @return 成功返回true，失败返回false
     */
    bool getFieldIndex(const std::string& tableName, const std::string& fieldName, 
                      size_t& fieldIndex) const;

    /**
     * @brief 生成索引名称
     * @param tableName 表名
     * @param fieldName 字段名
     * @param indexType 索引类型
     * @return 索引名称
     */
    std::string generateIndexName(const std::string& tableName, 
                                 const std::string& fieldName, 
                                 const std::string& indexType) const;
};









