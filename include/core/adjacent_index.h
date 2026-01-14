#pragma once

#include "core/table_mode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @file adjacent_index.h
 * @brief 相邻索引实现
 *
 * 相邻索引用于优化范围查询和顺序扫描性能
 * 通过维护相邻记录的物理位置映射，支持快速的范围查询和顺序遍历
 */

/**
 * @brief 相邻索引类
 *
 * 为有序字段建立相邻记录指针，优化范围查询和顺序扫描
 */
class AdjacentIndex {
public:
    /**
     * @brief 构造函数
     */
    AdjacentIndex();

    /**
     * @brief 析构函数
     */
    ~AdjacentIndex();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 为表的指定字段构建相邻索引
     * @param tableName 表名
     * @param fieldName 字段名（用于排序和建立相邻关系）
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
     * @brief 获取下一个相邻记录的索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @param recordIndex 当前记录索引
     * @param nextIndex 输出参数，下一个相邻记录的索引（如果不存在则为-1）
     * @return 成功返回true，失败返回false
     */
    bool getNextIndex(const std::string& tableName, const std::string& fieldName, 
                     size_t recordIndex, size_t& nextIndex) const;

    /**
     * @brief 获取上一个相邻记录的索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @param recordIndex 当前记录索引
     * @param prevIndex 输出参数，上一个相邻记录的索引（如果不存在则为-1）
     * @return 成功返回true，失败返回false
     */
    bool getPrevIndex(const std::string& tableName, const std::string& fieldName,
                     size_t recordIndex, size_t& prevIndex) const;

    /**
     * @brief 使用索引进行范围查询
     * @param tableName 表名
     * @param fieldName 字段名
     * @param startValue 起始值（包含）
     * @param endValue 结束值（包含）
     * @param resultIndices 输出参数，匹配的记录索引列表
     * @return 成功返回true，失败返回false
     */
    bool rangeQuery(const std::string& tableName, const std::string& fieldName,
                   const std::string& startValue, const std::string& endValue,
                   std::vector<size_t>& resultIndices) const;

    /**
     * @brief 使用索引进行顺序遍历（从最小值开始）
     * @param tableName 表名
     * @param fieldName 字段名
     * @param resultIndices 输出参数，按顺序排列的记录索引列表
     * @return 成功返回true，失败返回false
     */
    bool sequentialScan(const std::string& tableName, const std::string& fieldName,
                       std::vector<size_t>& resultIndices) const;

    /**
     * @brief 清除所有索引
     */
    void clear();

    /**
     * @brief 清除指定表的所有索引
     * @param tableName 表名
     */
    void clearTable(const std::string& tableName);

private:
    /**
     * @brief 索引项结构
     * 存储每个记录的下一个和上一个相邻记录的索引
     */
    struct IndexItem {
        size_t nextIndex;  // 下一个相邻记录的索引（-1表示不存在）
        size_t prevIndex;  // 上一个相邻记录的索引（-1表示不存在）
        
        IndexItem() : nextIndex(SIZE_MAX), prevIndex(SIZE_MAX) {}
    };

    /**
     * @brief 索引结构
     * 表名 -> 字段名 -> 记录索引 -> IndexItem
     */
    std::map<std::string, std::map<std::string, std::vector<IndexItem>>> m_indices;

    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）

    /**
     * @brief 比较两个字段值（用于排序）
     * @param value1 值1
     * @param value2 值2
     * @param fieldType 字段类型
     * @return value1 < value2 返回true，否则返回false
     */
    bool compareValues(const std::string& value1, const std::string& value2, 
                      const std::string& fieldType) const;

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

