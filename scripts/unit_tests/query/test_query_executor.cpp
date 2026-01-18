/**
 * @file test_query_executor.cpp
 * @brief 查询执行器测试程序
 * 
 * 测试QueryExecutor类的功能
 */

#include "../../include/query/query_executor.h"
#include <iostream>
#include <vector>
#include <cstring>

// 测试结果统计
static int testsPassed = 0;
static int testsFailed = 0;

// 测试宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            std::cout << "  ✓ " << message << std::endl; \
            testsPassed++; \
        } else { \
            std::cout << "  ✗ " << message << " (FAILED)" << std::endl; \
            testsFailed++; \
        } \
    } while(0)

// 测试数据库文件名
const char* TEST_DB_FILE = "test_db_executor";

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::string dbfFile = std::string(TEST_DB_FILE) + ".dbf";
    std::string datFile = std::string(TEST_DB_FILE) + ".dat";
    std::remove(dbfFile.c_str());
    std::remove(datFile.c_str());
}

/**
 * @brief 测试DDL语句执行
 */
void testDDLExecution() {
    std::cout << "\n[测试1] DDL语句执行测试" << std::endl;
    
    cleanupTestFiles();
    
    QueryExecutor executor;
    executor.setDatabasePath(TEST_DB_FILE);
    
    // 测试CREATE TABLE
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    ExecutionResult result = executor.execute(createSql);
    TEST_ASSERT(result.type == ExecutionResultType::SUCCESS, "CREATE TABLE执行成功");
    TEST_ASSERT(result.errorMessage.empty(), "错误信息为空");
    
    // 测试DROP TABLE
    std::string dropSql = "DROP TABLE Users IN " + std::string(TEST_DB_FILE) + ";";
    result = executor.execute(dropSql);
    TEST_ASSERT(result.type == ExecutionResultType::SUCCESS, "DROP TABLE执行成功");
}

/**
 * @brief 测试DML语句执行
 */
void testDMLExecution() {
    std::cout << "\n[测试2] DML语句执行测试" << std::endl;
    
    cleanupTestFiles();
    
    QueryExecutor executor;
    executor.setDatabasePath(TEST_DB_FILE);
    
    // 先创建表
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    executor.execute(createSql);
    
    // 测试INSERT
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    ExecutionResult result = executor.execute(insertSql);
    TEST_ASSERT(result.type == ExecutionResultType::SUCCESS, "INSERT执行成功");
    TEST_ASSERT(result.affectedRows == 1, "影响记录数为1");
    
    // 测试UPDATE
    std::string updateSql = "UPDATE Users ( SET UserName='Bob' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    result = executor.execute(updateSql);
    TEST_ASSERT(result.type == ExecutionResultType::SUCCESS, "UPDATE执行成功");
    TEST_ASSERT(result.affectedRows == 1, "影响记录数为1");
    
    // 测试DELETE
    std::string deleteSql = "DELETE FROM Users WHERE UserID='1' IN " + std::string(TEST_DB_FILE) + ";";
    result = executor.execute(deleteSql);
    TEST_ASSERT(result.type == ExecutionResultType::SUCCESS, "DELETE执行成功");
    TEST_ASSERT(result.affectedRows == 1, "影响记录数为1");
}

/**
 * @brief 测试查询语句执行
 */
void testQueryExecution() {
    std::cout << "\n[测试3] 查询语句执行测试" << std::endl;
    
    cleanupTestFiles();
    
    QueryExecutor executor;
    executor.setDatabasePath(TEST_DB_FILE);
    
    // 先创建表并插入数据
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    executor.execute(createSql);
    
    executor.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    executor.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 测试SELECT查询
    std::string selectSql = "SELECT * FROM Users;";
    ExecutionResult result = executor.execute(selectSql);
    TEST_ASSERT(result.type == ExecutionResultType::QUERY_RESULT, "SELECT执行成功");
    TEST_ASSERT(result.queryResult.rowCount == 2, "查询结果行数正确");
    TEST_ASSERT(result.queryResult.columnNames.size() == 2, "列数正确");
    TEST_ASSERT(!result.formattedOutput.empty(), "格式化输出不为空");
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试4] 错误处理测试" << std::endl;
    
    cleanupTestFiles();
    
    QueryExecutor executor;
    executor.setDatabasePath(TEST_DB_FILE);
    
    // 测试不存在的表
    std::string selectSql = "SELECT * FROM NonExistentTable;";
    ExecutionResult result = executor.execute(selectSql);
    TEST_ASSERT(result.type == ExecutionResultType::ERROR, "应该返回错误");
    TEST_ASSERT(!result.errorMessage.empty(), "错误信息不为空");
    
    // 测试语法错误
    std::string invalidSql = "INVALID SQL STATEMENT;";
    result = executor.execute(invalidSql);
    TEST_ASSERT(result.type == ExecutionResultType::ERROR, "应该返回错误");
}

/**
 * @brief 测试结果格式化
 */
void testResultFormatting() {
    std::cout << "\n[测试5] 结果格式化测试" << std::endl;
    
    cleanupTestFiles();
    
    QueryExecutor executor;
    executor.setDatabasePath(TEST_DB_FILE);
    
    // 创建表并插入数据
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    executor.execute(createSql);
    
    executor.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    executor.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行查询并检查格式化输出
    std::string selectSql = "SELECT * FROM Users;";
    ExecutionResult result = executor.execute(selectSql);
    TEST_ASSERT(result.type == ExecutionResultType::QUERY_RESULT, "SELECT执行成功");
    
    // 检查格式化输出包含表格式
    std::string formatted = result.formattedOutput;
    TEST_ASSERT(formatted.find("UserID") != std::string::npos, "格式化输出包含列名");
    TEST_ASSERT(formatted.find("Alice") != std::string::npos, "格式化输出包含数据");
    TEST_ASSERT(formatted.find("共") != std::string::npos || formatted.find("行") != std::string::npos, "格式化输出包含行数信息");
}

/**
 * @brief 测试语句类型识别
 */
void testStatementTypeIdentification() {
    std::cout << "\n[测试6] 语句类型识别测试" << std::endl;
    
    std::string ddlSql = "CREATE TABLE Test ( Field int KEY NO_NULL VALID ) INTO TestDB;";
    std::string dmlSql = "INSERT INTO Test VALUES ( '1' ) IN TestDB;";
    std::string querySql = "SELECT * FROM Test;";
    std::string unknownSql = "INVALID STATEMENT;";
    
    TEST_ASSERT(QueryExecutor::identifyStatementType(ddlSql) == "DDL", "DDL语句识别正确");
    TEST_ASSERT(QueryExecutor::identifyStatementType(dmlSql) == "DML", "DML语句识别正确");
    TEST_ASSERT(QueryExecutor::identifyStatementType(querySql) == "QUERY", "查询语句识别正确");
    TEST_ASSERT(QueryExecutor::identifyStatementType(unknownSql) == "UNKNOWN", "未知语句识别正确");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Query Executor 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testDDLExecution();
    testDMLExecution();
    testQueryExecution();
    testErrorHandling();
    testResultFormatting();
    testStatementTypeIdentification();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;
    
    cleanupTestFiles();
    
    return (testsFailed == 0) ? 0 : 1;
}

