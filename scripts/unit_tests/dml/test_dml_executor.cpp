/**
 * @file test_dml_executor.cpp
 * @brief DML执行器测试程序
 * 
 * 测试DMLExecutor类的功能
 */

#include "../../include/dml/dml_executor.h"
#include "../../include/ddl/create_table_handler.h"
#include "../../include/core/table_manager.h"
#include "../../include/core/data_manager.h"
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
const char* TEST_DB_FILE = "test_db_dml";

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
 * @brief 测试INSERT语句执行
 */
void testInsertExecution() {
    std::cout << "\n[测试1] INSERT语句执行测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 使用DMLExecutor执行INSERT
    DMLExecutor executor;
    std::string sql = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = executor.execute(sql);
    TEST_ASSERT(result == true, "INSERT执行成功");
    TEST_ASSERT(executor.getAffectedRows() == 1, "影响的记录数正确");
    
    // 验证记录是否插入
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
}

/**
 * @brief 测试DELETE语句执行
 */
void testDeleteExecution() {
    std::cout << "\n[测试2] DELETE语句执行测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    DMLExecutor executor;
    executor.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    executor.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    executor.execute("INSERT INTO Users VALUES ( '3', 'Charlie' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 使用DMLExecutor执行DELETE
    std::string sql = "DELETE FROM Users WHERE UserID='2' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = executor.execute(sql);
    TEST_ASSERT(result == true, "DELETE执行成功");
    TEST_ASSERT(executor.getAffectedRows() == 1, "影响的记录数正确");
    
    // 验证记录是否被删除
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 2, "剩余记录数量正确");
}

/**
 * @brief 测试UPDATE语句执行
 */
void testUpdateExecution() {
    std::cout << "\n[测试3] UPDATE语句执行测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    DMLExecutor executor;
    executor.execute("INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 使用DMLExecutor执行UPDATE
    std::string sql = "UPDATE Users ( SET Status='inactive' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = executor.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功");
    TEST_ASSERT(executor.getAffectedRows() == 1, "影响的记录数正确");
    
    // 验证记录是否被更新
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].values[2] == "inactive", "Status字段已更新");
}

/**
 * @brief 测试isDMLStatement静态方法
 */
void testIsDMLStatement() {
    std::cout << "\n[测试4] isDMLStatement静态方法测试" << std::endl;
    
    // 测试INSERT语句
    bool result1 = DMLExecutor::isDMLStatement("INSERT INTO Users VALUES ( '1', 'Alice' ) IN TestDB;");
    TEST_ASSERT(result1 == true, "INSERT语句识别正确");
    
    // 测试DELETE语句
    bool result2 = DMLExecutor::isDMLStatement("DELETE FROM Users WHERE UserID='1' IN TestDB;");
    TEST_ASSERT(result2 == true, "DELETE语句识别正确");
    
    // 测试UPDATE语句
    bool result3 = DMLExecutor::isDMLStatement("UPDATE Users ( SET Status='active' WHERE UserID='1' ) IN TestDB;");
    TEST_ASSERT(result3 == true, "UPDATE语句识别正确");
    
    // 测试非DML语句
    bool result4 = DMLExecutor::isDMLStatement("CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO TestDB;");
    TEST_ASSERT(result4 == false, "CREATE TABLE语句识别为false");
    
    bool result5 = DMLExecutor::isDMLStatement("SELECT * FROM Users;");
    TEST_ASSERT(result5 == false, "SELECT语句识别为false");
    
    bool result6 = DMLExecutor::isDMLStatement("");
    TEST_ASSERT(result6 == false, "空语句识别为false");
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试5] 错误处理测试" << std::endl;
    
    cleanupTestFiles();
    
    DMLExecutor executor;
    
    // 测试INSERT到不存在的表
    std::string sql1 = "INSERT INTO NonExistent VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result1 = executor.execute(sql1);
    TEST_ASSERT(result1 == false, "INSERT到不存在的表应该失败");
    TEST_ASSERT(!executor.getLastError().empty(), "错误信息不为空");
    
    // 测试DELETE到不存在的表
    std::string sql2 = "DELETE FROM NonExistent WHERE UserID='1' IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = executor.execute(sql2);
    TEST_ASSERT(result2 == false, "DELETE到不存在的表应该失败");
    TEST_ASSERT(!executor.getLastError().empty(), "错误信息不为空");
    
    // 测试UPDATE到不存在的表
    std::string sql3 = "UPDATE NonExistent ( SET UserName='Bob' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result3 = executor.execute(sql3);
    TEST_ASSERT(result3 == false, "UPDATE到不存在的表应该失败");
    TEST_ASSERT(!executor.getLastError().empty(), "错误信息不为空");
    
    // 测试空SQL语句
    bool result4 = executor.execute("");
    TEST_ASSERT(result4 == false, "空SQL语句应该失败");
    TEST_ASSERT(!executor.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试批量操作
 */
void testBatchOperations() {
    std::cout << "\n[测试6] 批量操作测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    DMLExecutor executor;
    
    // 批量INSERT
    executor.execute("INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    executor.execute("INSERT INTO Users VALUES ( '2', 'Bob', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    executor.execute("INSERT INTO Users VALUES ( '3', 'Charlie', 'inactive' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 批量UPDATE
    bool result1 = executor.execute("UPDATE Users ( SET Status='suspended' WHERE Status='active' ) IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(result1 == true, "批量UPDATE执行成功");
    TEST_ASSERT(executor.getAffectedRows() == 2, "更新了2条记录");
    
    // 批量DELETE
    bool result2 = executor.execute("DELETE FROM Users WHERE Status='suspended' IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(result2 == true, "批量DELETE执行成功");
    TEST_ASSERT(executor.getAffectedRows() == 2, "删除了2条记录");
    
    // 验证最终结果
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "剩余1条记录");
    TEST_ASSERT(records[0].values[1] == "Charlie", "剩余记录正确");
}

/**
 * @brief 测试getAffectedRows方法
 */
void testGetAffectedRows() {
    std::cout << "\n[测试7] getAffectedRows方法测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    DMLExecutor executor;
    
    // INSERT影响1条记录
    executor.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(executor.getAffectedRows() == 1, "INSERT影响1条记录");
    
    // DELETE影响0条记录（不存在的记录）
    executor.execute("DELETE FROM Users WHERE UserID='999' IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(executor.getAffectedRows() == 0, "DELETE影响0条记录");
    
    // UPDATE影响0条记录（不存在的记录）
    executor.execute("UPDATE Users ( SET UserName='Bob' WHERE UserID='999' ) IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(executor.getAffectedRows() == 0, "UPDATE影响0条记录");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "DML Executor 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testInsertExecution();
    testDeleteExecution();
    testUpdateExecution();
    testIsDMLStatement();
    testErrorHandling();
    testBatchOperations();
    testGetAffectedRows();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;
    
    cleanupTestFiles();
    
    return (testsFailed == 0) ? 0 : 1;
}

