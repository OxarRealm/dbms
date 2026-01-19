#pragma once

#include "core/table_mode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @file btree_index.h
 * @brief B+树索引实现
 *
 * B+树索引用于优化各种查询性能
 * 支持点查询、范围查询和排序查询
 * 提供O(log n)时间复杂度的查找性能
 */

/**
 * @brief B+树节点类型
 */
enum class BTreeNodeType {
    LEAF,      // 叶子节点
    INTERNAL   // 内部节点
};

/**
 * @brief B+树节点结构
 */
struct BTreeNode {
    BTreeNodeType type;                        // 节点类型
    std::vector<std::string> keys;             // 键值列表
    std::vector<size_t> values;                // 值列表（叶子节点存储记录索引，内部节点存储子节点索引）
    std::vector<std::shared_ptr<BTreeNode>> children;  // 子节点列表（仅内部节点使用）
    std::shared_ptr<BTreeNode> next;           // 下一个叶子节点（仅叶子节点使用）
    std::shared_ptr<BTreeNode> prev;           // 上一个叶子节点（仅叶子节点使用）
    std::shared_ptr<BTreeNode> parent;         // 父节点
    size_t parentIndex;                        // 在父节点中的索引
    bool isRoot;                               // 是否为根节点
    
    BTreeNode(BTreeNodeType nodeType) 
        : type(nodeType), parentIndex(0), isRoot(false) {}
};

/**
 * @brief B+树索引类
 *
 * 为字段建立B+树索引，优化各种查询性能
 * 支持点查询、范围查询和排序查询
 */
class BTreeIndex {
public:
    /**
     * @brief 构造函数
     */
    BTreeIndex();

    /**
     * @brief 析构函数
     */
    ~BTreeIndex();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 为表的指定字段构建B+树索引
     * @param tableName 表名
     * @param fieldName 字段名（用于排序和建立索引）
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

    /**
     * @brief 获取索引统计信息
     * @param tableName 表名
     * @param fieldName 字段名
     * @param nodeCount 输出参数，节点总数
     * @param leafCount 输出参数，叶子节点数
     * @param depth 输出参数，树深度
     * @return 成功返回true，失败返回false
     */
    bool getIndexStats(const std::string& tableName, const std::string& fieldName,
                      size_t& nodeCount, size_t& leafCount, size_t& depth) const;

private:
    /**
     * @brief 每个节点的最大键数（简化版B+树）
     */
    static const size_t MAX_KEYS = 10;  // 简化版B+树，每个节点最多10个键

    /**
     * @brief 索引结构
     * 表名 -> 字段名 -> B+树根节点
     */
    std::map<std::string, std::map<std::string, std::shared_ptr<BTreeNode>>> m_indices;

    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）

    /**
     * @brief 比较两个字段值（用于排序和查找）
     * @param value1 值1
     * @param value2 值2
     * @param fieldType 字段类型
     * @return value1 < value2 返回负数，value1 > value2 返回正数，相等返回0
     */
    int compareValues(const std::string& value1, const std::string& value2, 
                     const std::string& fieldType) const;

    /**
     * @brief 在节点中查找键的位置（二分查找）
     * @param node 节点
     * @param key 要查找的键
     * @param fieldType 字段类型
     * @return 键应该插入的位置
     */
    size_t findKeyPosition(const std::shared_ptr<BTreeNode>& node, 
                          const std::string& key, const std::string& fieldType) const;

    /**
     * @brief 在叶子节点中查找键
     * @param node 叶子节点
     * @param key 要查找的键
     * @param fieldType 字段类型
     * @return 找到返回位置索引，未找到返回SIZE_MAX
     */
    size_t findKeyInLeaf(const std::shared_ptr<BTreeNode>& node, 
                         const std::string& key, const std::string& fieldType) const;

    /**
     * @brief 统计节点数量（用于获取统计信息）
     * @param node 节点
     * @param nodeCount 输出参数，节点计数
     * @param leafCount 输出参数，叶子节点计数
     * @param depth 当前深度
     * @param maxDepth 输出参数，最大深度
     */
    void countNodes(const std::shared_ptr<BTreeNode>& node, 
                   size_t& nodeCount, size_t& leafCount, 
                   size_t depth, size_t& maxDepth) const;

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









