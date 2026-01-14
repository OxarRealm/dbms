/**
 * @file test_drop_table_handler.cpp
 * @brief DROP TABLE处理器测试程序
 * 
 * 测试DropTableHandler类的功能
 */

#include "../../include/ddl/drop_table_handler.h"
#include "../../include/ddl/create_table_handler.h"
#include "../../include/core/table_manager.h"
#include "../../include/core/data_manager.h"
#include "../../include/core/table_mode.h"
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
const char* TEST_DB_FILE = "test_db_drop";

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
 * @brief 测试基本删除（无数据）
 */
void testBasicDrop() {
    std::cout << "\n[测试1] 基本删除测试（无数据）" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 验证表存在
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(manager.tableExists("Users"), "表创建成功");
    
    // 删除表
    DropTableHandler handler;
    std::string sql = "DROP TABLE Users IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DROP TABLE执行成功");
    
    // 验证表已删除
    TEST_ASSERT(!manager.tableExists("Users"), "表已删除");
}

/**
 * @brief 测试删除（有数据）
 */
void testDropWithData() {
    std::cout << "\n[测试2] 删除测试（有数据）" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 插入数据
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    Record record1;
    record1.values.push_back("1");
    record1.validFlag = FLAG_VALID;
    dataManager.insertRecord("Users", record1);
    
    Record record2;
    record2.values.push_back("2");
    record2.validFlag = FLAG_VALID;
    dataManager.insertRecord("Users", record2);
    
    // 验证数据存在
    std::vector<Record> records;
    dataManager.readAllRecords("Users", records);
    TEST_ASSERT(records.size() == 2, "数据插入成功");
    
    // 删除表
    DropTableHandler handler;
    std::string sql = "DROP TABLE Users IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DROP TABLE执行成功");
    
    // 验证表已删除
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Users"), "表已删除");
    
    // 验证数据已删除
    DataManager newDataManager;
    newDataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> remainingRecords;
    newDataManager.readAllRecords("Users", remainingRecords);
    TEST_ASSERT(remainingRecords.empty(), "数据已删除");
}

/**
 * @brief 测试删除多个表中的一个
 */
void testDropOneOfMultipleTables() {
    std::cout << "\n[测试3] 删除多个表中的一个测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建多个表
    CreateTableHandler createHandler;
    std::string createSql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql2 = "CREATE TABLE Products ( ProductID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql3 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql1);
    createHandler.execute(createSql2);
    createHandler.execute(createSql3);
    
    // 验证所有表存在
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(manager.tableExists("Users"), "Users表存在");
    TEST_ASSERT(manager.tableExists("Products"), "Products表存在");
    TEST_ASSERT(manager.tableExists("Orders"), "Orders表存在");
    
    // 删除其中一个表
    DropTableHandler handler;
    std::string sql = "DROP TABLE Products IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "DROP TABLE执行成功");
    
    // 验证只有Products表被删除
    TEST_ASSERT(manager.tableExists("Users"), "Users表仍然存在");
    TEST_ASSERT(!manager.tableExists("Products"), "Products表已删除");
    TEST_ASSERT(manager.tableExists("Orders"), "Orders表仍然存在");
}

/**
 * @brief 测试错误处理（表不存在）
 */
void testErrorHandlingTableNotExists() {
    std::cout << "\n[测试4] 错误处理测试（表不存在）" << std::endl;
    
    cleanupTestFiles();
    
    DropTableHandler handler;
    std::string sql = "DROP TABLE NonExistentTable IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "表不存在返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（无效SQL）
 */
void testErrorHandlingInvalidSQL() {
    std::cout << "\n[测试5] 错误处理测试（无效SQL）" << std::endl;
    
    DropTableHandler handler;
    std::string sql = "INVALID SQL STATEMENT;";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "无效SQL返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试删除后重新创建同名表
 */
void testDropAndRecreate() {
    std::cout << "\n[测试6] 删除后重新创建同名表测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler;
    std::string createSql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql1);
    
    // 删除表
    DropTableHandler dropHandler;
    std::string dropSql = "DROP TABLE Users IN " + std::string(TEST_DB_FILE) + ";";
    dropHandler.execute(dropSql);
    
    // 重新创建同名表（但结构不同）
    std::string createSql2 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool result = createHandler.execute(createSql2);
    TEST_ASSERT(result == true, "重新创建表成功");
    
    // 验证新表存在且结构不同
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(manager.tableExists("Users"), "新表存在");
    
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 2, "新表结构不同（2个字段）");
}

/**
 * @brief 测试连续删除多个表
 */
void testDropMultipleTables() {
    std::cout << "\n[测试7] 连续删除多个表测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建多个表
    CreateTableHandler createHandler;
    std::string createSql1 = "CREATE TABLE Table1 ( Field1 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql2 = "CREATE TABLE Table2 ( Field2 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql3 = "CREATE TABLE Table3 ( Field3 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql1);
    createHandler.execute(createSql2);
    createHandler.execute(createSql3);
    
    // 连续删除
    DropTableHandler handler;
    
    std::string sql1 = "DROP TABLE Table1 IN " + std::string(TEST_DB_FILE) + ";";
    bool result1 = handler.execute(sql1);
    TEST_ASSERT(result1 == true, "删除Table1成功");
    
    std::string sql2 = "DROP TABLE Table2 IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = handler.execute(sql2);
    TEST_ASSERT(result2 == true, "删除Table2成功");
    
    std::string sql3 = "DROP TABLE Table3 IN " + std::string(TEST_DB_FILE) + ";";
    bool result3 = handler.execute(sql3);
    TEST_ASSERT(result3 == true, "删除Table3成功");
    
    // 验证所有表都已删除
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Table1"), "Table1已删除");
    TEST_ASSERT(!manager.tableExists("Table2"), "Table2已删除");
    TEST_ASSERT(!manager.tableExists("Table3"), "Table3已删除");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  DROP TABLE Handler测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testBasicDrop();
        testDropWithData();
        testDropOneOfMultipleTables();
        testErrorHandlingTableNotExists();
        testErrorHandlingInvalidSQL();
        testDropAndRecreate();
        testDropMultipleTables();
        
        cleanupTestFiles();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  测试结果汇总" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "通过: " << testsPassed << " 个测试" << std::endl;
        std::cout << "失败: " << testsFailed << " 个测试" << std::endl;
        std::cout << "总计: " << (testsPassed + testsFailed) << " 个测试" << std::endl;
        
        if (testsFailed == 0) {
            std::cout << "\n✓ 所有测试通过！" << std::endl;
            return 0;
        } else {
            std::cout << "\n✗ 有测试失败！" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 测试过程中发生异常: " << e.what() << std::endl;
        cleanupTestFiles();
        return 1;
    }
}

