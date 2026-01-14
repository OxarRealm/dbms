/**
 * @file test_delete_handler.cpp
 * @brief DELETE处理器测试程序
 * 
 * 测试DeleteHandler类的功能
 */

#include "../../include/dml/delete_handler.h"
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
const char* TEST_DB_FILE = "test_db_delete";

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
 * @brief 测试基本DELETE
 */
void testBasicDelete() {
    std::cout << "\n[测试1] 基本DELETE测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    std::string insertSql1 = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult1 = insertHandler.execute(insertSql1);
    TEST_ASSERT(insertResult1 == true, "插入第一条记录成功");
    
    std::string insertSql2 = "INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool insertResult2 = insertHandler.execute(insertSql2);
    TEST_ASSERT(insertResult2 == true, "插入第二条记录成功");
    
    // 删除记录
    DeleteHandler handler;
    std::string sql = "DELETE FROM Users WHERE UserID='1' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DELETE执行成功");
    TEST_ASSERT(handler.getDeletedCount() == 1, "删除记录数量正确");
    
    // 验证记录是否被删除
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "剩余记录数量正确");
    TEST_ASSERT(records[0].values[1] == "Bob", "剩余记录正确");
}

/**
 * @brief 测试DELETE多条记录
 */
void testDeleteMultipleRecords() {
    std::cout << "\n[测试2] DELETE多条记录测试" << std::endl;
    
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
    insertHandler.execute("INSERT INTO Users VALUES ( '4', 'David', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 删除所有Status='active'的记录
    DeleteHandler handler;
    std::string sql = "DELETE FROM Users WHERE Status='active' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DELETE执行成功");
    TEST_ASSERT(handler.getDeletedCount() == 3, "删除记录数量正确");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "剩余记录数量正确");
    TEST_ASSERT(records[0].values[1] == "Charlie", "剩余记录正确");
}

/**
 * @brief 测试DELETE不存在的记录
 */
void testDeleteNonExistentRecord() {
    std::cout << "\n[测试3] DELETE不存在的记录测试" << std::endl;
    
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
    
    // 删除不存在的记录
    DeleteHandler handler;
    std::string sql = "DELETE FROM Users WHERE UserID='999' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DELETE执行成功（没有匹配的记录不算错误）");
    TEST_ASSERT(handler.getDeletedCount() == 0, "删除记录数量为0");
    
    // 验证记录仍然存在
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
}

/**
 * @brief 测试DELETE到不存在的表
 */
void testDeleteFromNonExistentTable() {
    std::cout << "\n[测试4] DELETE到不存在的表测试" << std::endl;
    
    cleanupTestFiles();
    
    DeleteHandler handler;
    std::string sql = "DELETE FROM NonExistent WHERE UserID='1' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "DELETE应该失败（表不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试DELETE不存在的字段
 */
void testDeleteWithNonExistentField() {
    std::cout << "\n[测试5] DELETE不存在的字段测试" << std::endl;
    
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
    
    // 使用不存在的字段删除
    DeleteHandler handler;
    std::string sql = "DELETE FROM Users WHERE NonExistentField='1' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "DELETE应该失败（字段不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试DELETE字符串字段
 */
void testDeleteStringField() {
    std::cout << "\n[测试6] DELETE字符串字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '3', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 删除UserName='Alice'的记录
    DeleteHandler handler;
    std::string sql = "DELETE FROM Users WHERE UserName='Alice' IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DELETE执行成功");
    TEST_ASSERT(handler.getDeletedCount() == 2, "删除记录数量正确");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "剩余记录数量正确");
    TEST_ASSERT(records[0].values[1] == "Bob", "剩余记录正确");
}

/**
 * @brief 测试DELETE所有记录
 */
void testDeleteAllRecords() {
    std::cout << "\n[测试7] DELETE所有记录测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '3', 'Charlie' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 删除所有记录（使用一个所有记录都满足的条件）
    DeleteHandler handler;
    std::string sql = "DELETE FROM Users WHERE UserID>'0' IN " + std::string(TEST_DB_FILE) + ";";
    
    // 注意：当前实现只支持等号比较，不支持>，所以这个测试会失败
    // 但我们可以使用一个所有记录都有的值来测试
    // 由于当前只支持等号，我们改为删除一个不存在的值，然后验证所有记录仍然存在
    // 或者我们可以测试删除所有记录，但需要修改条件
    
    // 实际上，由于当前只支持等号，我们无法用一个条件删除所有记录
    // 所以这个测试改为：删除所有记录（逐个删除）
    bool result1 = handler.execute("DELETE FROM Users WHERE UserID='1' IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(result1 == true, "删除第一条记录成功");
    
    bool result2 = handler.execute("DELETE FROM Users WHERE UserID='2' IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(result2 == true, "删除第二条记录成功");
    
    bool result3 = handler.execute("DELETE FROM Users WHERE UserID='3' IN " + std::string(TEST_DB_FILE) + ";");
    TEST_ASSERT(result3 == true, "删除第三条记录成功");
    
    // 验证所有记录都被删除
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 0, "所有记录都被删除");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "DELETE Handler 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testBasicDelete();
    testDeleteMultipleRecords();
    testDeleteNonExistentRecord();
    testDeleteFromNonExistentTable();
    testDeleteWithNonExistentField();
    testDeleteStringField();
    testDeleteAllRecords();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;
    
    cleanupTestFiles();
    
    return (testsFailed == 0) ? 0 : 1;
}

