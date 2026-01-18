/**
 * @file index_advisor.cpp
 * @brief 智能索引建议系统实现
 */

#include "index/index_advisor.h"
#include "core/table_manager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>

IndexAdvisor::IndexAdvisor() {
}

IndexAdvisor::~IndexAdvisor() {
}

void IndexAdvisor::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
    m_adjacentIndex.setDatabasePath(dbFilePath);
    m_hashIndex.setDatabasePath(dbFilePath);
}

void IndexAdvisor::logQuery(const std::string& sql, const std::string& tableName,
                           const std::vector<std::string>& whereFields,
                           double executionTime, size_t resultCount) {
    QueryLogEntry entry;
    entry.sql = sql;
    entry.tableName = tableName;
    entry.whereFields = whereFields;
    entry.executionTime = executionTime;
    entry.resultCount = resultCount;
    entry.timestamp = std::time(nullptr);
    
    m_queryLogs.push_back(entry);
    
    // 限制日志数量，避免内存占用过大（保留最近1000条）
    if (m_queryLogs.size() > 1000) {
        m_queryLogs.erase(m_queryLogs.begin(), m_queryLogs.begin() + (m_queryLogs.size() - 1000));
    }
}

bool IndexAdvisor::analyzeQueryLogs(std::vector<FieldUsageStats>& stats) const {
    stats.clear();
    
    // 使用map来聚合统计信息：表名+字段名 -> 统计信息
    std::map<std::string, FieldUsageStats> statsMap;
    
    // 遍历所有查询日志
    for (const auto& log : m_queryLogs) {
        // 遍历WHERE子句中的字段
        for (const std::string& fieldName : log.whereFields) {
            std::string key = log.tableName + "." + fieldName;
            
            // 如果不存在，创建新的统计项
            if (statsMap.find(key) == statsMap.end()) {
                FieldUsageStats fieldStats;
                fieldStats.tableName = log.tableName;
                fieldStats.fieldName = fieldName;
                statsMap[key] = fieldStats;
            }
            
            // 更新统计信息
            FieldUsageStats& fieldStats = statsMap[key];
            fieldStats.usageCount++;
            fieldStats.totalExecutionTime += log.executionTime;
            fieldStats.totalResultCount += log.resultCount;
        }
    }
    
    // 计算平均执行时间，检查索引是否存在
    for (auto& pair : statsMap) {
        FieldUsageStats& fieldStats = pair.second;
        if (fieldStats.usageCount > 0) {
            fieldStats.avgExecutionTime = fieldStats.totalExecutionTime / fieldStats.usageCount;
        }
        
        // 检查是否已有索引
        fieldStats.hasIndex = m_adjacentIndex.hasIndex(fieldStats.tableName, fieldStats.fieldName) ||
                             m_hashIndex.hasIndex(fieldStats.tableName, fieldStats.fieldName);
    }
    
    // 转换为vector
    for (const auto& pair : statsMap) {
        stats.push_back(pair.second);
    }
    
    // 按使用次数降序排序
    std::sort(stats.begin(), stats.end(), 
              [](const FieldUsageStats& a, const FieldUsageStats& b) {
                  return a.usageCount > b.usageCount;
              });
    
    return true;
}

bool IndexAdvisor::identifySlowQueries(std::vector<QueryLogEntry>& slowQueries, 
                                       double threshold) const {
    slowQueries.clear();
    
    for (const auto& log : m_queryLogs) {
        if (log.executionTime >= threshold) {
            slowQueries.push_back(log);
        }
    }
    
    // 按执行时间降序排序
    std::sort(slowQueries.begin(), slowQueries.end(),
              [](const QueryLogEntry& a, const QueryLogEntry& b) {
                  return a.executionTime > b.executionTime;
              });
    
    return true;
}

bool IndexAdvisor::generateRecommendations(std::vector<IndexRecommendation>& recommendations,
                                           size_t maxRecommendations) const {
    recommendations.clear();
    
    // 分析查询日志，获取字段使用统计
    std::vector<FieldUsageStats> stats;
    if (!analyzeQueryLogs(stats)) {
        return false;
    }
    
    // 为每个字段生成推荐
    for (const auto& fieldStats : stats) {
        // 如果已有索引，跳过
        if (fieldStats.hasIndex) {
            continue;
        }
        
        // 如果使用次数太少，跳过（至少使用3次才推荐）
        if (fieldStats.usageCount < 3) {
            continue;
        }
        
        // 如果平均执行时间太短，跳过（至少10ms才推荐）
        if (fieldStats.avgExecutionTime < 10.0) {
            continue;
        }
        
        IndexRecommendation recommendation;
        recommendation.tableName = fieldStats.tableName;
        recommendation.fieldName = fieldStats.fieldName;
        recommendation.usageCount = fieldStats.usageCount;
        recommendation.avgExecutionTime = fieldStats.avgExecutionTime;
        
        // 判断索引类型
        if (isSuitableForHashIndex(fieldStats.tableName, fieldStats.fieldName)) {
            recommendation.indexType = "hash";
            recommendation.expectedImprovement = 50.0;  // 哈希索引预期提升50%
            recommendation.reason = "字段适合哈希索引，可优化点查询性能";
        } else if (isSuitableForAdjacentIndex(fieldStats.tableName, fieldStats.fieldName)) {
            recommendation.indexType = "adjacent";
            recommendation.expectedImprovement = 30.0;  // 相邻索引预期提升30%
            recommendation.reason = "字段适合相邻索引，可优化范围查询性能";
        } else {
            // 默认推荐相邻索引
            recommendation.indexType = "adjacent";
            recommendation.expectedImprovement = 20.0;
            recommendation.reason = "字段可创建相邻索引，优化查询性能";
        }
        
        // 根据使用频率和执行时间调整预期提升
        double score = calculateRecommendationScore(fieldStats);
        recommendation.expectedImprovement *= (1.0 + score / 100.0);
        
        recommendations.push_back(recommendation);
    }
    
    // 按推荐分数排序
    std::sort(recommendations.begin(), recommendations.end(),
              [this](const IndexRecommendation& a, const IndexRecommendation& b) {
                  // 计算推荐分数
                  FieldUsageStats statsA, statsB;
                  getFieldStats(a.tableName, a.fieldName, statsA);
                  getFieldStats(b.tableName, b.fieldName, statsB);
                  
                  double scoreA = calculateRecommendationScore(statsA);
                  double scoreB = calculateRecommendationScore(statsB);
                  
                  return scoreA > scoreB;
              });
    
    // 限制推荐数量
    if (recommendations.size() > maxRecommendations) {
        recommendations.resize(maxRecommendations);
    }
    
    return true;
}

bool IndexAdvisor::evaluateIndexEffect(const std::string& tableName, 
                                       const std::string& fieldName,
                                       double& improvement) const {
    improvement = 0.0;
    
    // 获取字段统计信息
    FieldUsageStats stats;
    if (!getFieldStats(tableName, fieldName, stats)) {
        return false;
    }
    
    if (stats.usageCount == 0) {
        return false;
    }
    
    // 检查是否有索引
    bool hasIndex = m_adjacentIndex.hasIndex(tableName, fieldName) ||
                   m_hashIndex.hasIndex(tableName, fieldName);
    
    if (!hasIndex) {
        // 没有索引，无法评估效果
        return false;
    }
    
    // 简单评估：基于使用频率和执行时间
    // 假设索引可以提升30-50%的性能
    double baseImprovement = 30.0;
    if (m_hashIndex.hasIndex(tableName, fieldName)) {
        baseImprovement = 50.0;  // 哈希索引提升更大
    }
    
    // 根据使用频率调整
    if (stats.usageCount > 10) {
        baseImprovement += 10.0;
    }
    
    improvement = baseImprovement;
    return true;
}

void IndexAdvisor::clearLogs() {
    m_queryLogs.clear();
}

size_t IndexAdvisor::getLogCount() const {
    return m_queryLogs.size();
}

bool IndexAdvisor::getFieldStats(const std::string& tableName, 
                                 const std::string& fieldName,
                                 FieldUsageStats& stats) const {
    std::vector<FieldUsageStats> allStats;
    if (!analyzeQueryLogs(allStats)) {
        return false;
    }
    
    for (const auto& s : allStats) {
        if (s.tableName == tableName && s.fieldName == fieldName) {
            stats = s;
            return true;
        }
    }
    
    return false;
}

bool IndexAdvisor::isSuitableForHashIndex(const std::string& tableName,
                                          const std::string& fieldName) const {
    // 读取表结构
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return false;
    }
    
    // 查找字段
    for (const auto& field : tableInfo.fields) {
        if (field.sFieldName == fieldName) {
            // 主键字段或整数类型字段适合哈希索引
            if (field.bKey == FLAG_KEY) {
                return true;
            }
            std::string fieldType = field.sType;
            if (fieldType == "int" || fieldType == "string") {
                return true;
            }
            return false;
        }
    }
    
    return false;
}

bool IndexAdvisor::isSuitableForAdjacentIndex(const std::string& tableName,
                                              const std::string& fieldName) const {
    // 读取表结构
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return false;
    }
    
    // 查找字段
    for (const auto& field : tableInfo.fields) {
        if (field.sFieldName == fieldName) {
            // 可排序的字段类型适合相邻索引
            std::string fieldType = field.sType;
            if (fieldType == "int" || fieldType == "float" || fieldType == "double" ||
                fieldType == "string" || fieldType == "char") {
                return true;
            }
            return false;
        }
    }
    
    return false;
}

double IndexAdvisor::calculateRecommendationScore(const FieldUsageStats& stats) const {
    // 推荐分数 = 使用次数权重 + 执行时间权重
    double usageScore = std::min(stats.usageCount * 5.0, 50.0);  // 最多50分
    double timeScore = std::min(stats.avgExecutionTime / 2.0, 50.0);  // 最多50分
    
    return usageScore + timeScore;
}

