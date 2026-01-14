/**
 * @file test_update_handler.cpp
 * @brief UPDATE处理器测试程序
 * 
 * 测试UpdateHandler类的功能
 */

#include "../../include/dml/update_handler.h"
#include "../../include/dml/insert_handler.h"
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
const char* TEST_DB_FILE = "test_db_update";

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
 * @brief 测试基本UPDATE
 */
void testBasicUpdate() {
    std::cout << "\n[测试1] 基本UPDATE测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult = insertHandler.execute(insertSql);
    TEST_ASSERT(insertResult == true, "插入记录成功");
    
    // 更新记录
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET Status='inactive' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功");
    TEST_ASSERT(handler.getUpdatedCount() == 1, "更新记录数量正确");
    
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
 * @brief 测试UPDATE多条记录
 */
void testUpdateMultipleRecords() {
    std::cout << "\n[测试2] UPDATE多条记录测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '3', 'Charlie', 'inactive' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 更新所有Status='active'的记录
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET Status='suspended' WHERE Status='active' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功");
    TEST_ASSERT(handler.getUpdatedCount() == 2, "更新记录数量正确");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 3, "记录数量正确");
    
    // 检查更新后的值
    bool allUpdated = true;
    for (const auto& record : records) {
        if (record.values[0] == "1" || record.values[0] == "2") {
            if (record.values[2] != "suspended") {
                allUpdated = false;
                break;
            }
        }
    }
    TEST_ASSERT(allUpdated == true, "所有匹配的记录都已更新");
}

/**
 * @brief 测试UPDATE不存在的记录
 */
void testUpdateNonExistentRecord() {
    std::cout << "\n[测试3] UPDATE不存在的记录测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult = insertHandler.execute(insertSql);
    TEST_ASSERT(insertResult == true, "插入记录成功");
    
    // 更新不存在的记录
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET UserName='Bob' WHERE UserID='999' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功（没有匹配的记录不算错误）");
    TEST_ASSERT(handler.getUpdatedCount() == 0, "更新记录数量为0");
    
    // 验证记录未被更新
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].values[1] == "Alice", "记录未被更新");
}

/**
 * @brief 测试UPDATE到不存在的表
 */
void testUpdateNonExistentTable() {
    std::cout << "\n[测试4] UPDATE到不存在的表测试" << std::endl;
    
    cleanupTestFiles();
    
    UpdateHandler handler;
    std::string sql = "UPDATE NonExistent ( SET UserName='Bob' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "UPDATE应该失败（表不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试UPDATE不存在的字段
 */
void testUpdateNonExistentField() {
    std::cout << "\n[测试5] UPDATE不存在的字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult = insertHandler.execute(insertSql);
    TEST_ASSERT(insertResult == true, "插入记录成功");
    
    // 使用不存在的SET字段更新
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET NonExistentField='value' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "UPDATE应该失败（SET字段不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
    
    // 使用不存在的WHERE字段更新
    std::string sql2 = "UPDATE Users ( SET UserName='Bob' WHERE NonExistentField='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = handler.execute(sql2);
    TEST_ASSERT(result2 == false, "UPDATE应该失败（WHERE字段不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试UPDATE整数类型验证
 */
void testUpdateIntTypeValidation() {
    std::cout << "\n[测试6] UPDATE整数类型验证测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Age int NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice', '25' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult = insertHandler.execute(insertSql);
    TEST_ASSERT(insertResult == true, "插入记录成功");
    
    // 更新整数字段
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET Age='30' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records[0].values[2] == "30", "整数值已更新");
}

/**
 * @brief 测试UPDATE字符串字段
 */
void testUpdateStringField() {
    std::cout << "\n[测试7] UPDATE字符串字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult = insertHandler.execute(insertSql);
    TEST_ASSERT(insertResult == true, "插入记录成功");
    
    // 更新字符串字段
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET UserName='Bob' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records[0].values[1] == "Bob", "字符串值已更新");
}

/**
 * @brief 测试UPDATE NULL值
 */
void testUpdateNullValue() {
    std::cout << "\n[测试8] UPDATE NULL值测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表（允许NULL的字段）并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Email char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql = "INSERT INTO Users VALUES ( '1', 'Alice', 'alice@example.com' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult = insertHandler.execute(insertSql);
    TEST_ASSERT(insertResult == true, "插入记录成功");
    
    // 更新为NULL（空字符串）
    UpdateHandler handler;
    std::string sql = "UPDATE Users ( SET Email='' WHERE UserID='1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "UPDATE执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records[0].values[2].empty() == true, "NULL值已设置");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "UPDATE Handler 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testBasicUpdate();
    testUpdateMultipleRecords();
    testUpdateNonExistentRecord();
    testUpdateNonExistentTable();
    testUpdateNonExistentField();
    testUpdateIntTypeValidation();
    testUpdateStringField();
    testUpdateNullValue();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;
    
    cleanupTestFiles();
    
    return (testsFailed == 0) ? 0 : 1;
}

