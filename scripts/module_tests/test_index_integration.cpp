/**
 * @file test_index_integration.cpp
 * @brief 索引功能集成测试脚本
 * 
 * 测试索引创建、使用、智能推荐等完整功能
 * 创建大型数据库，插入大量数据，测试各种索引类型
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "core/index_manager.h"
#include "core/adjacent_index.h"
#include "core/hash_index.h"
#include "core/btree_index.h"
#include "index/index_advisor.h"
#include "core/table_manager.h"
#include "core/data_manager.h"
#include "ddl/create_table_handler.h"
#include "dml/insert_handler.h"
#include "query/query_executor.h"

void printTestResult(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << std::endl;
}

void printSection(const std::string& sectionName) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << sectionName << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║      索引功能集成测试 (Index Integration Test)          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "开始时间: " << getCurrentTime() << std::endl;
    std::cout << std::endl;
    
    int passedTests = 0;
    int totalTests = 0;
    
    // 测试数据库名称
    std::string dbName = "test_index_db";
    std::string dbPath = dbName;
    
    // ========== 第一部分：创建数据库和表结构 ==========
    printSection("第一部分：创建数据库和表结构");
    
    // 清理之前的测试数据
    {
        totalTests++;
        std::cout << "\n[1.1] 清理之前的测试数据..." << std::endl;
        // 删除可能存在的数据库文件
        std::remove((dbPath + ".dbf").c_str());
        std::remove((dbPath + ".dat").c_str());
        printTestResult("清理测试数据", true);
        passedTests++;
    }
    
    // 创建Users表
    {
        totalTests++;
        std::cout << "\n[1.2] 创建Users表..." << std::endl;
        CreateTableHandler handler;
        std::string sql = "CREATE TABLE Users ("
                         "UserID int KEY NO_NULL VALID,"
                         "UserName char[50] NOT_KEY NULL VALID,"
                         "Email char[100] NOT_KEY NULL VALID,"
                         "Age int NOT_KEY NULL VALID,"
                         "Phone char[20] NOT_KEY NULL VALID,"
                         "Salary float NOT_KEY NULL VALID,"
                         "Department char[50] NOT_KEY NULL VALID"
                         ") INTO " + dbName + ";";
        
        bool result = handler.execute(sql);
        printTestResult("创建Users表", result);
        if (result) {
            passedTests++;
        } else {
            std::cout << "  错误: " << handler.getLastError() << std::endl;
        }
    }
    
    // 创建Products表
    {
        totalTests++;
        std::cout << "\n[1.3] 创建Products表..." << std::endl;
        CreateTableHandler handler;
        std::string sql = "CREATE TABLE Products ("
                         "ProductID int KEY NO_NULL VALID,"
                         "ProductName char[100] NOT_KEY NULL VALID,"
                         "Price float NOT_KEY NULL VALID,"
                         "Category char[50] NOT_KEY NULL VALID,"
                         "Stock int NOT_KEY NULL VALID,"
                         "UserID int NOT_KEY NULL VALID"
                         ") INTO " + dbName + ";";
        
        bool result = handler.execute(sql);
        printTestResult("创建Products表", result);
        if (result) {
            passedTests++;
        } else {
            std::cout << "  错误: " << handler.getLastError() << std::endl;
        }
    }
    
    // ========== 第二部分：插入大量测试数据 ==========
    printSection("第二部分：插入大量测试数据");
    
    // 插入Users数据（100条记录）
    {
        totalTests++;
        std::cout << "\n[2.1] 插入Users数据（100条记录）..." << std::endl;
        InsertHandler handler;
        
        int successCount = 0;
        for (int i = 1; i <= 100; i++) {
            std::stringstream ss;
            ss << "INSERT INTO Users VALUES ("
               << "'" << i << "', "
               << "'User" << i << "', "
               << "'user" << i << "@example.com', "
               << "'" << (20 + (i % 50)) << "', "
               << "'138" << std::setfill('0') << std::setw(8) << i << "', "
               << "'" << (5000.0 + i * 100) << "', "
               << "'Dept" << (i % 10 + 1) << "'"
               << ") IN " << dbName << ";";
            
            if (handler.execute(ss.str())) {
                successCount++;
            }
        }
        
        bool result = (successCount == 100);
        printTestResult("插入Users数据（100条）", result);
        std::cout << "  成功插入: " << successCount << " 条记录" << std::endl;
        if (result) {
            passedTests++;
        }
    }
    
    // 插入Products数据（200条记录）
    {
        totalTests++;
        std::cout << "\n[2.2] 插入Products数据（200条记录）..." << std::endl;
        InsertHandler handler;
        
        int successCount = 0;
        for (int i = 1; i <= 200; i++) {
            std::stringstream ss;
            ss << "INSERT INTO Products VALUES ("
               << "'" << i << "', "
               << "'Product" << i << "', "
               << "'" << (10.0 + (i % 1000)) << "', "
               << "'Category" << (i % 5 + 1) << "', "
               << "'" << (i % 100) << "', "
               << "'" << ((i % 100) + 1) << "'"
               << ") IN " << dbName << ";";
            
            if (handler.execute(ss.str())) {
                successCount++;
            }
        }
        
        bool result = (successCount == 200);
        printTestResult("插入Products数据（200条）", result);
        std::cout << "  成功插入: " << successCount << " 条记录" << std::endl;
        if (result) {
            passedTests++;
        }
    }
    
    // ========== 第三部分：创建索引 ==========
    printSection("第三部分：创建索引");
    
    IndexManager indexManager;
    indexManager.setDatabasePath(dbPath);
    
    // 创建Hash Index on Users.UserID
    {
        totalTests++;
        std::cout << "\n[3.1] 创建Hash Index on Users.UserID..." << std::endl;
        bool result = indexManager.createIndex("Users", "UserID", "hash");
        printTestResult("创建Hash Index (Users.UserID)", result);
        if (result) {
            passedTests++;
        }
    }
    
    // 创建Adjacent Index on Users.Age
    {
        totalTests++;
        std::cout << "\n[3.2] 创建Adjacent Index on Users.Age..." << std::endl;
        bool result = indexManager.createIndex("Users", "Age", "adjacent");
        printTestResult("创建Adjacent Index (Users.Age)", result);
        if (result) {
            passedTests++;
        }
    }
    
    // 创建B+ Tree Index on Users.Salary
    {
        totalTests++;
        std::cout << "\n[3.3] 创建B+ Tree Index on Users.Salary..." << std::endl;
        bool result = indexManager.createIndex("Users", "Salary", "btree");
        printTestResult("创建B+ Tree Index (Users.Salary)", result);
        if (result) {
            passedTests++;
        }
    }
    
    // 创建Hash Index on Products.ProductID
    {
        totalTests++;
        std::cout << "\n[3.4] 创建Hash Index on Products.ProductID..." << std::endl;
        bool result = indexManager.createIndex("Products", "ProductID", "hash");
        printTestResult("创建Hash Index (Products.ProductID)", result);
        if (result) {
            passedTests++;
        }
    }
    
    // 创建Adjacent Index on Products.Price
    {
        totalTests++;
        std::cout << "\n[3.5] 创建Adjacent Index on Products.Price..." << std::endl;
        bool result = indexManager.createIndex("Products", "Price", "adjacent");
        printTestResult("创建Adjacent Index (Products.Price)", result);
        if (result) {
            passedTests++;
        }
    }
    
    // 验证索引列表
    {
        totalTests++;
        std::cout << "\n[3.6] 验证索引列表..." << std::endl;
        std::vector<IndexInfo> indices;
        bool result = indexManager.getAllIndices(indices);
        printTestResult("获取所有索引", result);
        std::cout << "  索引数量: " << indices.size() << std::endl;
        for (const auto& idx : indices) {
            std::cout << "    - " << idx.indexName << " (" << idx.indexType << ")" << std::endl;
        }
        if (result && indices.size() >= 5) {
            passedTests++;
        }
    }
    
    // ========== 第四部分：测试索引功能 ==========
    printSection("第四部分：测试索引功能");
    
    // 测试Hash Index点查询
    {
        totalTests++;
        std::cout << "\n[4.1] 测试Hash Index点查询..." << std::endl;
        HashIndex hashIndex;
        hashIndex.setDatabasePath(dbPath);
        
        std::vector<size_t> resultIndices;
        bool result = hashIndex.pointQuery("Users", "UserID", "50", resultIndices);
        printTestResult("Hash Index点查询 (UserID=50)", result && resultIndices.size() > 0);
        std::cout << "  找到记录数: " << resultIndices.size() << std::endl;
        if (result && resultIndices.size() > 0) {
            passedTests++;
        }
    }
    
    // 测试Adjacent Index范围查询
    {
        totalTests++;
        std::cout << "\n[4.2] 测试Adjacent Index范围查询..." << std::endl;
        AdjacentIndex adjIndex;
        adjIndex.setDatabasePath(dbPath);
        
        std::vector<size_t> resultIndices;
        bool result = adjIndex.rangeQuery("Users", "Age", "25", "35", resultIndices);
        printTestResult("Adjacent Index范围查询 (Age 25-35)", result);
        std::cout << "  找到记录数: " << resultIndices.size() << std::endl;
        if (result) {
            passedTests++;
        }
    }
    
    // 测试B+ Tree Index点查询
    {
        totalTests++;
        std::cout << "\n[4.3] 测试B+ Tree Index点查询..." << std::endl;
        BTreeIndex btreeIndex;
        btreeIndex.setDatabasePath(dbPath);
        
        std::vector<size_t> resultIndices;
        bool result = btreeIndex.pointQuery("Users", "Salary", "10000", resultIndices);
        printTestResult("B+ Tree Index点查询 (Salary=10000)", result);
        std::cout << "  找到记录数: " << resultIndices.size() << std::endl;
        if (result) {
            passedTests++;
        }
    }
    
    // 测试B+ Tree Index范围查询
    {
        totalTests++;
        std::cout << "\n[4.4] 测试B+ Tree Index范围查询..." << std::endl;
        BTreeIndex btreeIndex;
        btreeIndex.setDatabasePath(dbPath);
        
        std::vector<size_t> resultIndices;
        bool result = btreeIndex.rangeQuery("Users", "Salary", "10000", "15000", resultIndices);
        printTestResult("B+ Tree Index范围查询 (Salary 10000-15000)", result);
        std::cout << "  找到记录数: " << resultIndices.size() << std::endl;
        if (result) {
            passedTests++;
        }
    }
    
    // 测试索引统计信息
    {
        totalTests++;
        std::cout << "\n[4.5] 测试索引统计信息..." << std::endl;
        std::map<std::string, std::string> stats;
        bool result = indexManager.getIndexStats("Users", "UserID", "hash", stats);
        printTestResult("获取Hash Index统计信息", result);
        if (result) {
            std::cout << "  统计信息:" << std::endl;
            for (const auto& pair : stats) {
                std::cout << "    " << pair.first << ": " << pair.second << std::endl;
            }
            passedTests++;
        }
    }
    
    // ========== 第五部分：测试智能索引推荐 ==========
    printSection("第五部分：测试智能索引推荐");
    
    IndexAdvisor advisor;
    advisor.setDatabasePath(dbPath);
    
    // 执行一些查询以生成查询日志
    {
        totalTests++;
        std::cout << "\n[5.1] 执行查询以生成查询日志..." << std::endl;
        QueryExecutor executor;
        executor.setDatabasePath(dbPath);
        
        // 执行多个查询
        std::vector<std::string> queries = {
            "SELECT * FROM Users WHERE UserID = '10';",
            "SELECT * FROM Users WHERE Age > '30';",
            "SELECT * FROM Users WHERE Age BETWEEN '25' AND '35';",
            "SELECT * FROM Products WHERE Price > '100';",
            "SELECT * FROM Products WHERE Category = 'Category1';",
            "SELECT * FROM Users WHERE Salary > '12000';",
            "SELECT * FROM Users WHERE Department = 'Dept1';"
        };
        
        int queryCount = 0;
        for (const auto& sql : queries) {
            auto start = std::chrono::high_resolution_clock::now();
            auto result = executor.execute(sql);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            double durationMs = static_cast<double>(duration.count());
            
            // 记录查询日志
            std::string tableName = (sql.find("Users") != std::string::npos) ? "Users" : "Products";
            std::vector<std::string> whereFields;
            
            // 简单提取WHERE字段（实际应该用SQL解析器）
            if (sql.find("UserID") != std::string::npos) whereFields.push_back("UserID");
            if (sql.find("Age") != std::string::npos) whereFields.push_back("Age");
            if (sql.find("Price") != std::string::npos) whereFields.push_back("Price");
            if (sql.find("Category") != std::string::npos) whereFields.push_back("Category");
            if (sql.find("Salary") != std::string::npos) whereFields.push_back("Salary");
            if (sql.find("Department") != std::string::npos) whereFields.push_back("Department");
            
            size_t resultCount = (result.type == ExecutionResultType::QUERY_RESULT) ? result.queryResult.rows.size() : 0;
            advisor.logQuery(sql, tableName, whereFields, durationMs, resultCount);
            queryCount++;
        }
        
        printTestResult("执行查询并记录日志", queryCount == queries.size());
        std::cout << "  执行查询数: " << queryCount << std::endl;
        if (queryCount == queries.size()) {
            passedTests++;
        }
    }
    
    // 分析查询日志
    {
        totalTests++;
        std::cout << "\n[5.2] 分析查询日志..." << std::endl;
        std::vector<FieldUsageStats> stats;
        bool result = advisor.analyzeQueryLogs(stats);
        printTestResult("分析查询日志", result);
        std::cout << "  字段使用统计数: " << stats.size() << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), stats.size()); i++) {
            std::cout << "    " << stats[i].tableName << "." << stats[i].fieldName 
                      << " - 使用次数: " << stats[i].usageCount 
                      << ", 平均时间: " << std::fixed << std::setprecision(2) 
                      << stats[i].avgExecutionTime << "ms" << std::endl;
        }
        if (result && stats.size() > 0) {
            passedTests++;
        }
    }
    
    // 生成索引推荐
    {
        totalTests++;
        std::cout << "\n[5.3] 生成索引推荐..." << std::endl;
        std::vector<IndexRecommendation> recommendations;
        bool result = advisor.generateRecommendations(recommendations, 10);
        printTestResult("生成索引推荐", result);
        std::cout << "  推荐数量: " << recommendations.size() << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), recommendations.size()); i++) {
            std::cout << "    [" << (i+1) << "] " << recommendations[i].tableName 
                      << "." << recommendations[i].fieldName 
                      << " (" << recommendations[i].indexType << ")"
                      << " - 预期提升: " << std::fixed << std::setprecision(1) 
                      << recommendations[i].expectedImprovement << "%"
                      << " - 理由: " << recommendations[i].reason << std::endl;
        }
        if (result && recommendations.size() > 0) {
            passedTests++;
        }
    }
    
    // 识别慢查询
    {
        totalTests++;
        std::cout << "\n[5.4] 识别慢查询..." << std::endl;
        std::vector<QueryLogEntry> slowQueries;
        bool result = advisor.identifySlowQueries(slowQueries, 10.0);  // 10ms阈值
        printTestResult("识别慢查询", result);
        std::cout << "  慢查询数量: " << slowQueries.size() << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), slowQueries.size()); i++) {
            std::cout << "    [" << (i+1) << "] " << slowQueries[i].tableName 
                      << " - 执行时间: " << std::fixed << std::setprecision(2) 
                      << slowQueries[i].executionTime << "ms" << std::endl;
        }
        if (result) {
            passedTests++;
        }
    }
    
    // ========== 测试总结 ==========
    printSection("测试总结");
    
    std::cout << "\n测试完成时间: " << getCurrentTime() << std::endl;
    std::cout << "\n总测试数: " << totalTests << std::endl;
    std::cout << "通过测试: " << passedTests << std::endl;
    std::cout << "失败测试: " << (totalTests - passedTests) << std::endl;
    std::cout << "通过率: " << std::fixed << std::setprecision(1) 
              << (totalTests > 0 ? (100.0 * passedTests / totalTests) : 0.0) << "%" << std::endl;
    
    std::cout << "\n数据库文件位置:" << std::endl;
    std::cout << "  - " << dbPath << ".dbf (表结构)" << std::endl;
    std::cout << "  - " << dbPath << ".dat (数据)" << std::endl;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    if (passedTests == totalTests) {
        std::cout << "✓ 所有测试通过！" << std::endl;
        return 0;
    } else {
        std::cout << "✗ 部分测试失败" << std::endl;
        return 1;
    }
}

