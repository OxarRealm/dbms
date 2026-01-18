/**
 * @file index_advisor.cpp
 * @brief 智能索引建议系统实现
 */

#include "index/index_advisor.h"
#include "core/table_manager.h"
#include "core/index_manager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstring>
#include <regex>
#include <sstream>
#include <iomanip>

IndexAdvisor::IndexAdvisor() : m_indexManager(nullptr) {
}

IndexAdvisor::~IndexAdvisor() {
}

void IndexAdvisor::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
    m_adjacentIndex.setDatabasePath(dbFilePath);
    m_hashIndex.setDatabasePath(dbFilePath);
    m_btreeIndex.setDatabasePath(dbFilePath);
}

void IndexAdvisor::setIndexManager(IndexManager* indexManager) {
    m_indexManager = indexManager;
}

bool IndexAdvisor::checkHasIndex(const std::string& tableName, const std::string& fieldName) const {
    // 优先使用IndexManager检查索引（如果已设置）
    if (m_indexManager) {
        return m_indexManager->hasIndex(tableName, fieldName);
    }
    
    // 如果未设置IndexManager，使用内部索引对象（向后兼容）
    return m_adjacentIndex.hasIndex(tableName, fieldName) ||
           m_hashIndex.hasIndex(tableName, fieldName) ||
           m_btreeIndex.hasIndex(tableName, fieldName);
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
        
        // 检查是否已有索引（使用checkHasIndex，优先使用IndexManager）
        fieldStats.hasIndex = checkHasIndex(fieldStats.tableName, fieldStats.fieldName);
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
        
        // 如果平均执行时间太短，跳过（至少0.1ms才推荐）
        // 注意：在小数据集上执行时间可能很短，但仍然应该推荐索引
        // 因为索引在大数据集上仍然有效。0.1ms是一个很低的阈值，主要用于过滤无效查询
        if (fieldStats.avgExecutionTime < 0.1) {
            continue;
        }
        
        IndexRecommendation recommendation;
        recommendation.tableName = fieldStats.tableName;
        recommendation.fieldName = fieldStats.fieldName;
        recommendation.usageCount = fieldStats.usageCount;
        recommendation.avgExecutionTime = fieldStats.avgExecutionTime;
        
        // 判断索引类型（优先推荐B+树索引，因为它是通用索引）
        // 对于点查询，优先推荐hash；对于范围查询，推荐btree或adjacent；默认推荐btree（通用）
        if (isSuitableForHashIndex(fieldStats.tableName, fieldStats.fieldName)) {
            // 对于主键或整数类型，如果主要用于等值查询，推荐hash
            recommendation.indexType = "hash";
            recommendation.expectedImprovement = 50.0;  // 哈希索引预期提升50%
            recommendation.reason = "字段适合哈希索引，可优化点查询性能（O(1)时间复杂度）";
        } else if (isSuitableForBTreeIndex(fieldStats.tableName, fieldStats.fieldName)) {
            // B+树索引是通用索引，适合所有查询类型
            recommendation.indexType = "btree";
            recommendation.expectedImprovement = 40.0;  // B+树索引预期提升40%
            recommendation.reason = "字段适合B+树索引，可优化点查询、范围查询和排序（通用索引）";
        } else if (isSuitableForAdjacentIndex(fieldStats.tableName, fieldStats.fieldName)) {
            // 相邻索引适合范围查询
            recommendation.indexType = "adjacent";
            recommendation.expectedImprovement = 30.0;  // 相邻索引预期提升30%
            recommendation.reason = "字段适合相邻索引，可优化范围查询性能";
        } else {
            // 默认推荐B+树索引（最通用）
            recommendation.indexType = "btree";
            recommendation.expectedImprovement = 25.0;
            recommendation.reason = "字段可创建B+树索引，优化查询性能（通用索引）";
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
    
    // 检查是否有索引（使用checkHasIndex，优先使用IndexManager）
    bool hasIndex = checkHasIndex(tableName, fieldName);
    
    if (!hasIndex) {
        // 没有索引，无法评估效果
        return false;
    }
    
    // 简单评估：基于使用频率和执行时间
    // 假设索引可以提升30-50%的性能
    double baseImprovement = 30.0;
    // 检查是否为哈希索引（如果有IndexManager，使用它；否则使用内部索引对象）
    if (m_indexManager) {
        if (m_indexManager->hasIndex(tableName, fieldName, "hash")) {
            baseImprovement = 50.0;  // 哈希索引提升更大
        }
    } else if (m_hashIndex.hasIndex(tableName, fieldName)) {
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
        if (strcmp(field.sFieldName, fieldName.c_str()) == 0) {
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
        if (strcmp(field.sFieldName, fieldName.c_str()) == 0) {
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

bool IndexAdvisor::isSuitableForBTreeIndex(const std::string& tableName,
                                           const std::string& fieldName) const {
    // B+树索引是通用索引，几乎所有可排序的字段类型都适合
    // 读取表结构
    TableManager tableManager;
    tableManager.setDatabasePath(m_dbFilePath);
    TableInfo tableInfo;
    if (!tableManager.readTable(tableName, tableInfo)) {
        return false;
    }
    
    // 查找字段
    for (const auto& field : tableInfo.fields) {
        if (strcmp(field.sFieldName, fieldName.c_str()) == 0) {
            // B+树索引支持所有可排序的类型（int, float, double, string, char等）
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

size_t IndexAdvisor::detectAntiPatterns(const std::string& sql, std::vector<QueryAdvice>& adviceList) const {
    size_t count = 0;
    std::string upperSql = sql;
    std::transform(upperSql.begin(), upperSql.end(), upperSql.begin(), ::toupper);
    
    // 检测 SELECT *
    std::regex selectStarPattern(R"(\bSELECT\s+\*\s+FROM)", std::regex_constants::icase);
    if (std::regex_search(sql, selectStarPattern)) {
        QueryAdvice advice;
        advice.type = QueryAdviceType::ANTI_PATTERN;
        advice.title = "SELECT * Anti-pattern";
        advice.message = "Using SELECT * retrieves all columns, which may impact performance.";
        advice.severity = "warning";
        advice.suggestion = "Specify only the columns you need, e.g., SELECT column1, column2 FROM table;";
        adviceList.push_back(advice);
        count++;
    }
    
    // 检测 LIKE 前通配符
    std::regex likePattern(R"(\bLIKE\s+['"]%[^'"]*['"])", std::regex_constants::icase);
    if (std::regex_search(sql, likePattern)) {
        QueryAdvice advice;
        advice.type = QueryAdviceType::ANTI_PATTERN;
        advice.title = "LIKE with Leading Wildcard";
        advice.message = "LIKE patterns starting with '%' cannot use indexes and require full table scan.";
        advice.severity = "warning";
        advice.suggestion = "Consider using LIKE 'pattern%' (trailing wildcard) or full-text search if available.";
        adviceList.push_back(advice);
        count++;
    }
    
    // 检测 LIKE 前后都有通配符
    std::regex likeBothPattern(R"(\bLIKE\s+['"]%[^'"]*%['"])", std::regex_constants::icase);
    if (std::regex_search(sql, likeBothPattern)) {
        QueryAdvice advice;
        advice.type = QueryAdviceType::ANTI_PATTERN;
        advice.title = "LIKE with Both Wildcards";
        advice.message = "LIKE patterns with '%' on both sides cannot use indexes.";
        advice.severity = "warning";
        advice.suggestion = "Consider using full-text search or restructuring the query if possible.";
        adviceList.push_back(advice);
        count++;
    }
    
    return count;
}

size_t IndexAdvisor::generateIndexAdviceForQuery(const std::string& tableName,
                                                 const std::vector<std::string>& whereFields,
                                                 double executionTime,
                                                 std::vector<QueryAdvice>& adviceList) const {
    size_t count = 0;
    
    if (whereFields.empty() || tableName.empty()) {
        return 0;
    }
    
    // 检查每个WHERE字段的统计信息
    for (const auto& fieldName : whereFields) {
        FieldUsageStats stats;
        if (!getFieldStats(tableName, fieldName, stats)) {
            continue;
        }
        
        // 如果已有索引，跳过
        if (stats.hasIndex) {
            continue;
        }
        
        // 如果使用次数 >= 3 且平均执行时间 >= 0.1ms，推荐创建索引
        if (stats.usageCount >= 3 && stats.avgExecutionTime >= 0.1) {
            QueryAdvice advice;
            advice.type = QueryAdviceType::INDEX_RECOMMEND;
            
            // 根据字段类型和查询模式判断索引类型
            std::string indexType = "btree";  // 默认B+树索引
            std::string reason = "General-purpose index for point and range queries";
            
            if (isSuitableForHashIndex(tableName, fieldName)) {
                indexType = "hash";
                reason = "Hash index for fast point queries (O(1) time complexity)";
            } else if (isSuitableForBTreeIndex(tableName, fieldName)) {
                indexType = "btree";
                reason = "B+ tree index for point queries, range queries, and sorting";
            } else if (isSuitableForAdjacentIndex(tableName, fieldName)) {
                indexType = "adjacent";
                reason = "Adjacent index for range queries";
            }
            
            std::ostringstream oss;
            oss << "Field '" << fieldName << "' in table '" << tableName 
                << "' is frequently used (" << stats.usageCount << " times, avg " 
                << std::fixed << std::setprecision(2) << stats.avgExecutionTime << "ms) but has no index.";
            
            advice.title = "Index Recommendation";
            advice.message = oss.str();
            advice.severity = "info";
            
            std::ostringstream suggOss;
            suggOss << "CREATE INDEX " << tableName << "_" << fieldName << "_idx ON " 
                    << tableName << "(" << fieldName << ") USING " << indexType << ";";
            advice.suggestion = suggOss.str();
            
            adviceList.push_back(advice);
            count++;
        }
    }
    
    return count;
}

size_t IndexAdvisor::detectRangeOptimization(const std::string& sql, const std::string& tableName,
                                             const std::vector<std::string>& whereFields,
                                             double executionTime, size_t resultCount,
                                             std::vector<QueryAdvice>& adviceList) const {
    size_t count = 0;
    
    // 检测全表扫描：检查SQL中是否真的包含WHERE关键字
    // 如果SQL中没有WHERE关键字，即使whereFields不为空（可能是解析错误），也应该显示全表扫描警告
    std::string upperSql = sql;
    std::transform(upperSql.begin(), upperSql.end(), upperSql.begin(), ::toupper);
    bool hasWhereInSQL = upperSql.find(" WHERE ") != std::string::npos;
    
    // 检测全表扫描（没有WHERE条件）
    if (!hasWhereInSQL || whereFields.empty()) {
        QueryAdvice advice;
        advice.type = QueryAdviceType::RANGE_OPTIMIZATION;
        advice.title = "Full Table Scan Detected";
        advice.message = "Query has no WHERE clause, causing full table scan.";
        advice.severity = "warning";
        advice.suggestion = "Add WHERE clause to filter results and improve performance.";
        adviceList.push_back(advice);
        count++;
    } else {
        // 检查WHERE字段是否都有索引
        bool allHaveIndex = true;
        std::vector<std::string> fieldsWithoutIndex;
        
        // 使用IndexManager检查索引（更准确）
        // 注意：这里需要访问IndexManager，但IndexAdvisor没有直接访问
        // 所以我们使用getFieldStats来检查，它内部会检查索引
        
        for (const auto& fieldName : whereFields) {
            FieldUsageStats stats;
            // 尝试获取字段统计（如果字段存在但无索引，stats.hasIndex会是false）
            if (getFieldStats(tableName, fieldName, stats)) {
                if (!stats.hasIndex) {
                    allHaveIndex = false;
                    fieldsWithoutIndex.push_back(fieldName);
                }
            } else {
                // 如果无法获取统计（字段可能不存在或首次使用），假设没有索引
                // 这种情况下，我们仍然检查，但降低警告级别
                allHaveIndex = false;
                fieldsWithoutIndex.push_back(fieldName);
            }
        }
        
        // 如果有字段没有索引，显示警告
        // 降低执行时间阈值，让警告更容易触发
        if (!allHaveIndex && !fieldsWithoutIndex.empty()) {
            QueryAdvice advice;
            advice.type = QueryAdviceType::RANGE_OPTIMIZATION;
            advice.title = "Missing Indexes on WHERE Fields";
            
            std::ostringstream oss;
            oss << "WHERE clause fields without indexes: ";
            for (size_t i = 0; i < fieldsWithoutIndex.size(); i++) {
                if (i > 0) oss << ", ";
                oss << fieldsWithoutIndex[i];
            }
            if (executionTime > 1.0) {
                oss << ". Query took " << std::fixed << std::setprecision(2) << executionTime << "ms.";
            } else {
                oss << ".";
            }
            
            advice.message = oss.str();
            // 如果执行时间较长，使用warning；否则使用info
            advice.severity = (executionTime > 10.0) ? "warning" : "info";
            advice.suggestion = "Consider creating indexes on these fields to improve query performance.";
            adviceList.push_back(advice);
            count++;
        }
    }
    
    // 检测返回大量记录（可能缺少LIMIT）
    if (resultCount > 1000 && executionTime > 50.0) {
        QueryAdvice advice;
        advice.type = QueryAdviceType::RANGE_OPTIMIZATION;
        advice.title = "Large Result Set";
        advice.message = "Query returned " + std::to_string(resultCount) + " rows, which may impact performance.";
        advice.severity = "info";
        advice.suggestion = "Consider adding LIMIT clause or more specific WHERE conditions to reduce result set size.";
        adviceList.push_back(advice);
        count++;
    }
    
    return count;
}

bool IndexAdvisor::analyzeCurrentQuery(const std::string& sql, const std::string& tableName,
                                       const std::vector<std::string>& whereFields,
                                       double executionTime, size_t resultCount,
                                       std::vector<QueryAdvice>& adviceList) const {
    adviceList.clear();
    
    // 1. 检测反模式
    detectAntiPatterns(sql, adviceList);
    
    // 2. 生成索引建议（仅对SELECT查询）
    std::string upperSql = sql;
    std::transform(upperSql.begin(), upperSql.end(), upperSql.begin(), ::toupper);
    if (upperSql.find("SELECT") == 0) {
        generateIndexAdviceForQuery(tableName, whereFields, executionTime, adviceList);
    }
    
    // 3. 检测范围优化
    detectRangeOptimization(sql, tableName, whereFields, executionTime, resultCount, adviceList);
    
    return true;
}

