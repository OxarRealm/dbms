/**
 * @file test_insert_handler.cpp
 * @brief INSERT处理器测试程序
 * 
 * 测试InsertHandler类的功能
 */

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
const char* TEST_DB_FILE = "test_db_insert";

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
 * @brief 测试基本INSERT
 */
void testBasicInsert() {
    std::cout << "\n[测试1] 基本INSERT测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入记录
    InsertHandler handler;
    std::string sql = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "INSERT执行成功");
    
    // 验证记录是否插入
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].values.size() == 2, "字段数量正确");
    TEST_ASSERT(records[0].values[0] == "1", "UserID值正确");
    TEST_ASSERT(records[0].values[1] == "Alice", "UserName值正确");
}

/**
 * @brief 测试INSERT多个字段
 */
void testInsertMultipleFields() {
    std::cout << "\n[测试2] INSERT多个字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Songs ( SongID int KEY NO_NULL VALID, SongName char NOT_KEY NO_NULL VALID, Artist char NOT_KEY NULL VALID, Year int NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入记录
    InsertHandler handler;
    std::string sql = "INSERT INTO Songs VALUES ( '1', 'Bohemian Rhapsody', 'Queen', '1975' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "INSERT执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Songs", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].values.size() == 4, "字段数量正确");
    TEST_ASSERT(records[0].values[0] == "1", "SongID值正确");
    TEST_ASSERT(records[0].values[1] == "Bohemian Rhapsody", "SongName值正确");
    TEST_ASSERT(records[0].values[2] == "Queen", "Artist值正确");
    TEST_ASSERT(records[0].values[3] == "1975", "Year值正确");
}

/**
 * @brief 测试INSERT多条记录
 */
void testInsertMultipleRecords() {
    std::cout << "\n[测试3] INSERT多条记录测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入多条记录
    InsertHandler handler;
    
    std::string sql1 = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result1 = handler.execute(sql1);
    TEST_ASSERT(result1 == true, "插入第一条记录成功");
    
    std::string sql2 = "INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = handler.execute(sql2);
    TEST_ASSERT(result2 == true, "插入第二条记录成功");
    
    std::string sql3 = "INSERT INTO Users VALUES ( '3', 'Charlie' ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result3 = handler.execute(sql3);
    TEST_ASSERT(result3 == true, "插入第三条记录成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 3, "记录数量正确");
    TEST_ASSERT(records[0].values[1] == "Alice", "第一条记录正确");
    TEST_ASSERT(records[1].values[1] == "Bob", "第二条记录正确");
    TEST_ASSERT(records[2].values[1] == "Charlie", "第三条记录正确");
}

/**
 * @brief 测试INSERT NULL值
 */
void testInsertNullValue() {
    std::cout << "\n[测试4] INSERT NULL值测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表（允许NULL的字段）
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Songs ( SongID int KEY NO_NULL VALID, SongName char NOT_KEY NO_NULL VALID, Artist char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入记录（Artist字段为空字符串，表示NULL）
    InsertHandler handler;
    std::string sql = "INSERT INTO Songs VALUES ( '1', 'Song1', '' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "INSERT执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Songs", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].values[2].empty() == true, "NULL值正确");
}

/**
 * @brief 测试INSERT值数量不匹配
 */
void testInsertValueCountMismatch() {
    std::cout << "\n[测试5] INSERT值数量不匹配测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入记录（值数量不匹配）
    InsertHandler handler;
    std::string sql = "INSERT INTO Users VALUES ( '1' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "INSERT应该失败（值数量不匹配）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试INSERT到不存在的表
 */
void testInsertToNonExistentTable() {
    std::cout << "\n[测试6] INSERT到不存在的表测试" << std::endl;
    
    cleanupTestFiles();
    
    InsertHandler handler;
    std::string sql = "INSERT INTO NonExistent VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "INSERT应该失败（表不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试INSERT整数类型验证
 */
void testInsertIntTypeValidation() {
    std::cout << "\n[测试7] INSERT整数类型验证测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入记录（整数类型）
    InsertHandler handler;
    std::string sql = "INSERT INTO Users VALUES ( '123', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "INSERT执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Users", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records[0].values[0] == "123", "整数值正确");
}

/**
 * @brief 测试INSERT浮点数类型验证
 */
void testInsertFloatTypeValidation() {
    std::cout << "\n[测试8] INSERT浮点数类型验证测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表（包含float字段）
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Products ( ProductID int KEY NO_NULL VALID, Price float NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 插入记录（浮点数类型）
    InsertHandler handler;
    std::string sql = "INSERT INTO Products VALUES ( '1', '99.99' ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "INSERT执行成功");
    
    // 验证记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    bool readResult = dataManager.readValidRecords("Products", records);
    TEST_ASSERT(readResult == true, "读取记录成功");
    TEST_ASSERT(records[0].values[1] == "99.99", "浮点数值正确");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "INSERT Handler 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testBasicInsert();
    testInsertMultipleFields();
    testInsertMultipleRecords();
    testInsertNullValue();
    testInsertValueCountMismatch();
    testInsertToNonExistentTable();
    testInsertIntTypeValidation();
    testInsertFloatTypeValidation();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;
    
    cleanupTestFiles();
    
    return (testsFailed == 0) ? 0 : 1;
}

