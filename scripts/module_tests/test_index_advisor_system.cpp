/**
 * @file test_index_advisor_system.cpp
 * @brief 智能索引建议系统完整功能测试
 * 
 * 测试IndexAdvisor的完整功能，包括：
 * 1. 查询日志记录
 * 2. 字段使用统计
 * 3. 慢查询识别
 * 4. 索引推荐生成
 * 5. 索引效果评估
 */

#include "../../include/query/query_executor.h"
#include "../../include/index/index_advisor.h"
#include "../../include/core/index_manager.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>

// 测试数据库
const char* TEST_DB = "student_grade_db";

/**
 * @brief 打印分隔线
 */
void printSeparator() {
    std::cout << "========================================" << std::endl;
}

/**
 * @brief 打印标题
 */
void printTitle(const std::string& title) {
    printSeparator();
    std::cout << title << std::endl;
    printSeparator();
}

/**
 * @brief 测试查询日志记录和索引推荐
 */
void testIndexAdvisorRecommendations() {
    printTitle("智能索引建议系统测试");
    
    std::cout << "\n[步骤1] 初始化查询执行器..." << std::endl;
    QueryExecutor executor;
    executor.setDatabasePath(TEST_DB);
    
    std::cout << "[步骤2] 执行多个查询以生成查询日志..." << std::endl;
    std::cout << "  执行查询以模拟实际使用场景..." << std::endl;
    
    // 执行多个查询，模拟不同的查询模式
    // 1. 频繁查询 Students.id（应该推荐hash索引）
    for (int i = 0; i < 5; i++) {
        std::string sql = "SELECT * FROM Students WHERE id = " + std::to_string(1001 + i);
        auto result = executor.execute(sql);
        std::cout << "  查询: " << sql << " - " 
                  << (result.type == ExecutionResultType::QUERY_RESULT ? "成功" : "失败") << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 模拟查询时间
    }
    
    // 2. 查询 Students.name（应该推荐btree索引）
    for (int i = 0; i < 4; i++) {
        std::string sql = "SELECT * FROM Students WHERE name LIKE 'A%'";
        auto result = executor.execute(sql);
        std::cout << "  查询: " << sql << " - " 
                  << (result.type == ExecutionResultType::QUERY_RESULT ? "成功" : "失败") << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 3. 查询 Students.age（应该推荐adjacent或btree索引）
    for (int i = 0; i < 3; i++) {
        std::string sql = "SELECT * FROM Students WHERE age >= 20 AND age <= 25";
        auto result = executor.execute(sql);
        std::cout << "  查询: " << sql << " - " 
                  << (result.type == ExecutionResultType::QUERY_RESULT ? "成功" : "失败") << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 4. 查询 Grades.score（应该推荐btree索引）
    for (int i = 0; i < 3; i++) {
        std::string sql = "SELECT * FROM Grades WHERE score >= 90";
        auto result = executor.execute(sql);
        std::cout << "  查询: " << sql << " - " 
                  << (result.type == ExecutionResultType::QUERY_RESULT ? "成功" : "失败") << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "\n[步骤3] 获取索引建议器..." << std::endl;
    IndexAdvisor& advisor = executor.getIndexAdvisor();
    
    std::cout << "\n[步骤4] 查看查询日志统计..." << std::endl;
    size_t logCount = advisor.getLogCount();
    std::cout << "  查询日志总数: " << logCount << std::endl;
    
    std::cout << "\n[步骤5] 分析字段使用统计..." << std::endl;
    std::vector<FieldUsageStats> stats;
    if (advisor.analyzeQueryLogs(stats)) {
        std::cout << "  找到 " << stats.size() << " 个字段的统计信息:" << std::endl;
        for (const auto& s : stats) {
            std::cout << "    - " << s.tableName << "." << s.fieldName 
                      << ": 使用次数=" << s.usageCount 
                      << ", 平均执行时间=" << std::fixed << std::setprecision(2) 
                      << s.avgExecutionTime << "ms"
                      << ", 已有索引=" << (s.hasIndex ? "是" : "否") << std::endl;
        }
    } else {
        std::cout << "  分析失败" << std::endl;
    }
    
    std::cout << "\n[步骤6] 识别慢查询..." << std::endl;
    std::vector<QueryLogEntry> slowQueries;
    if (advisor.identifySlowQueries(slowQueries, 10.0)) {
        std::cout << "  找到 " << slowQueries.size() << " 个慢查询（>10ms):" << std::endl;
        for (size_t i = 0; i < std::min(slowQueries.size(), size_t(5)); i++) {
            const auto& q = slowQueries[i];
            std::cout << "    " << (i+1) << ". 执行时间: " << std::fixed << std::setprecision(2) 
                      << q.executionTime << "ms, SQL: " << q.sql.substr(0, 60) << "..." << std::endl;
        }
    }
    
    std::cout << "\n[步骤7] 生成索引推荐..." << std::endl;
    std::vector<IndexRecommendation> recommendations;
    if (advisor.generateRecommendations(recommendations, 10)) {
        std::cout << "  生成了 " << recommendations.size() << " 个索引推荐:" << std::endl;
        std::cout << std::endl;
        
        if (recommendations.empty()) {
            std::cout << "  ⚠ 没有生成推荐（可能原因：查询次数不足、执行时间太短、或字段已有索引）" << std::endl;
            std::cout << "  提示：需要至少3次查询且平均执行时间>10ms才会生成推荐" << std::endl;
        } else {
            for (size_t i = 0; i < recommendations.size(); i++) {
                const auto& rec = recommendations[i];
                std::cout << "  [" << (i+1) << "] " << rec.tableName << "." << rec.fieldName << std::endl;
                std::cout << "      索引类型: " << rec.indexType << std::endl;
                std::cout << "      使用次数: " << rec.usageCount << std::endl;
                std::cout << "      平均执行时间: " << std::fixed << std::setprecision(2) 
                          << rec.avgExecutionTime << "ms" << std::endl;
                std::cout << "      预期性能提升: " << std::fixed << std::setprecision(1) 
                          << rec.expectedImprovement << "%" << std::endl;
                std::cout << "      推荐理由: " << rec.reason << std::endl;
                std::cout << "      创建索引SQL: CREATE INDEX idx_" << rec.tableName << "_" 
                          << rec.fieldName << " ON " << rec.tableName << " (" << rec.fieldName 
                          << ") USING " << rec.indexType << " IN " << TEST_DB << ";" << std::endl;
                std::cout << std::endl;
            }
        }
    } else {
        std::cout << "  生成推荐失败" << std::endl;
    }
    
    std::cout << "\n[步骤8] 测试索引效果评估..." << std::endl;
    // 如果有推荐，尝试评估索引效果（需要先创建索引）
    if (!recommendations.empty()) {
        const auto& firstRec = recommendations[0];
        std::cout << "  评估字段: " << firstRec.tableName << "." << firstRec.fieldName << std::endl;
        
        // 检查是否已有索引
        IndexManager indexManager;
        indexManager.setDatabasePath(TEST_DB);
        bool hasIndex = indexManager.hasIndex(firstRec.tableName, firstRec.fieldName, firstRec.indexType);
        
        if (hasIndex) {
            double improvement = 0.0;
            if (advisor.evaluateIndexEffect(firstRec.tableName, firstRec.fieldName, improvement)) {
                std::cout << "  索引效果评估: 预期提升 " << std::fixed << std::setprecision(1) 
                          << improvement << "%" << std::endl;
            } else {
                std::cout << "  无法评估索引效果（可能需要更多查询日志）" << std::endl;
            }
        } else {
            std::cout << "  该字段尚未创建索引，无法评估效果" << std::endl;
            std::cout << "  建议先创建索引: CREATE INDEX idx_" << firstRec.tableName << "_" 
                      << firstRec.fieldName << " ON " << firstRec.tableName << " (" 
                      << firstRec.fieldName << ") USING " << firstRec.indexType << " IN " 
                      << TEST_DB << ";" << std::endl;
        }
    }
    
    printSeparator();
    std::cout << "测试完成！" << std::endl;
    printSeparator();
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "\n";
    printTitle("智能索引建议系统 - 完整功能测试");
    std::cout << "\n";
    std::cout << "测试数据库: " << TEST_DB << std::endl;
    std::cout << "注意: 请确保数据库已存在且包含数据" << std::endl;
    std::cout << "\n";
    
    try {
        testIndexAdvisorRecommendations();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}







