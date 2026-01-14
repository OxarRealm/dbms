#pragma once

#include "core/table_mode.h"
#include "core/adjacent_index.h"
#include "core/hash_index.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>

/**
 * @file index_advisor.h
 * @brief 智能索引建议系统
 *
 * 通过分析查询日志，自动识别慢查询和频繁使用的字段，
 * 推荐创建合适的索引以优化查询性能
 */

/**
 * @brief 查询日志条目
 */
struct QueryLogEntry {
    std::string sql;                    // SQL语句
    std::string tableName;              // 涉及的表名
    std::vector<std::string> whereFields; // WHERE子句中使用的字段
    double executionTime;               // 执行时间（毫秒）
    size_t resultCount;                  // 返回的记录数
    std::time_t timestamp;              // 时间戳
    
    QueryLogEntry() : executionTime(0.0), resultCount(0), timestamp(0) {}
};

/**
 * @brief 字段使用统计
 */
struct FieldUsageStats {
    std::string tableName;              // 表名
    std::string fieldName;              // 字段名
    size_t usageCount;                  // 使用次数
    double totalExecutionTime;          // 总执行时间（毫秒）
    double avgExecutionTime;            // 平均执行时间（毫秒）
    size_t totalResultCount;            // 总返回记录数
    bool hasIndex;                      // 是否已有索引
    
    FieldUsageStats() : usageCount(0), totalExecutionTime(0.0), 
                        avgExecutionTime(0.0), totalResultCount(0), hasIndex(false) {}
};

/**
 * @brief 索引推荐项
 */
struct IndexRecommendation {
    std::string tableName;              // 表名
    std::string fieldName;              // 字段名
    std::string indexType;              // 索引类型（"adjacent" 或 "hash"）
    double expectedImprovement;         // 预期性能提升（百分比）
    std::string reason;                 // 推荐理由
    size_t usageCount;                  // 字段使用次数
    double avgExecutionTime;            // 平均执行时间（毫秒）
    
    IndexRecommendation() : expectedImprovement(0.0), usageCount(0), avgExecutionTime(0.0) {}
};

/**
 * @brief 智能索引建议系统类
 */
class IndexAdvisor {
public:
    /**
     * @brief 构造函数
     */
    IndexAdvisor();

    /**
     * @brief 析构函数
     */
    ~IndexAdvisor();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 记录查询日志
     * @param sql SQL语句
     * @param tableName 表名
     * @param whereFields WHERE子句中使用的字段列表
     * @param executionTime 执行时间（毫秒）
     * @param resultCount 返回的记录数
     */
    void logQuery(const std::string& sql, const std::string& tableName,
                 const std::vector<std::string>& whereFields,
                 double executionTime, size_t resultCount);

    /**
     * @brief 分析查询日志，生成字段使用统计
     * @param stats 输出参数，字段使用统计列表
     * @return 成功返回true，失败返回false
     */
    bool analyzeQueryLogs(std::vector<FieldUsageStats>& stats) const;

    /**
     * @brief 识别慢查询
     * @param slowQueries 输出参数，慢查询列表
     * @param threshold 慢查询阈值（毫秒，默认100ms）
     * @return 成功返回true，失败返回false
     */
    bool identifySlowQueries(std::vector<QueryLogEntry>& slowQueries, 
                            double threshold = 100.0) const;

    /**
     * @brief 生成索引推荐
     * @param recommendations 输出参数，索引推荐列表
     * @param maxRecommendations 最大推荐数量（默认10）
     * @return 成功返回true，失败返回false
     */
    bool generateRecommendations(std::vector<IndexRecommendation>& recommendations,
                                size_t maxRecommendations = 10) const;

    /**
     * @brief 评估索引效果
     * @param tableName 表名
     * @param fieldName 字段名
     * @param improvement 输出参数，性能提升百分比
     * @return 成功返回true，失败返回false
     */
    bool evaluateIndexEffect(const std::string& tableName, const std::string& fieldName,
                            double& improvement) const;

    /**
     * @brief 清除查询日志
     */
    void clearLogs();

    /**
     * @brief 获取查询日志数量
     * @return 查询日志数量
     */
    size_t getLogCount() const;

    /**
     * @brief 获取字段使用统计
     * @param tableName 表名
     * @param fieldName 字段名
     * @param stats 输出参数，字段使用统计
     * @return 成功返回true，失败返回false
     */
    bool getFieldStats(const std::string& tableName, const std::string& fieldName,
                      FieldUsageStats& stats) const;

private:
    std::vector<QueryLogEntry> m_queryLogs;  // 查询日志列表
    std::string m_dbFilePath;                // 数据库文件路径（不含扩展名）
    
    AdjacentIndex m_adjacentIndex;          // 相邻索引（用于检查索引是否存在）
    HashIndex m_hashIndex;                   // 哈希索引（用于检查索引是否存在）

    /**
     * @brief 判断字段类型是否适合哈希索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @return 适合返回true，否则返回false
     */
    bool isSuitableForHashIndex(const std::string& tableName, 
                                const std::string& fieldName) const;

    /**
     * @brief 判断字段类型是否适合相邻索引
     * @param tableName 表名
     * @param fieldName 字段名
     * @return 适合返回true，否则返回false
     */
    bool isSuitableForAdjacentIndex(const std::string& tableName,
                                   const std::string& fieldName) const;

    /**
     * @brief 计算推荐分数
     * @param stats 字段使用统计
     * @return 推荐分数（分数越高，推荐优先级越高）
     */
    double calculateRecommendationScore(const FieldUsageStats& stats) const;
};

