/**
 * @file test_rename_table_handler.cpp
 * @brief RENAME TABLE处理器测试程序
 * 
 * 测试RenameTableHandler类的功能
 */

#include "../../include/ddl/rename_table_handler.h"
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
const char* TEST_DB_FILE = "test_db_rename";

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
 * @brief 测试基本重命名（无数据）
 */
void testBasicRename() {
    std::cout << "\n[测试1] 基本重命名测试（无数据）" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 重命名表
    RenameTableHandler handler;
    std::string sql = "RENAME TABLE Users Customers IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "RENAME TABLE执行成功");
    
    // 验证表已重命名
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Users"), "旧表名不存在");
    TEST_ASSERT(manager.tableExists("Customers"), "新表名存在");
}

/**
 * @brief 测试重命名（有数据）
 */
void testRenameWithData() {
    std::cout << "\n[测试2] 重命名测试（有数据）" << std::endl;
    
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
    
    // 重命名表
    RenameTableHandler handler;
    std::string sql = "RENAME TABLE Users Customers IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "RENAME TABLE执行成功");
    
    // 验证表已重命名
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Users"), "旧表名不存在");
    TEST_ASSERT(manager.tableExists("Customers"), "新表名存在");
    
    // 验证数据已迁移
    DataManager newDataManager;
    newDataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    newDataManager.readAllRecords("Customers", records);
    TEST_ASSERT(records.size() == 2, "数据已迁移");
    TEST_ASSERT(records[0].getValue(0) == "1", "第一条记录正确");
    TEST_ASSERT(records[1].getValue(0) == "2", "第二条记录正确");
    
    // 验证旧表名的数据已清空
    std::vector<Record> oldRecords;
    newDataManager.readAllRecords("Users", oldRecords);
    TEST_ASSERT(oldRecords.empty(), "旧表名的数据已清空");
}

/**
 * @brief 测试错误处理（表不存在）
 */
void testErrorHandlingTableNotExists() {
    std::cout << "\n[测试3] 错误处理测试（表不存在）" << std::endl;
    
    cleanupTestFiles();
    
    RenameTableHandler handler;
    std::string sql = "RENAME TABLE NonExistentTable NewTable IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "表不存在返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（新表名已存在）
 */
void testErrorHandlingNewTableExists() {
    std::cout << "\n[测试4] 错误处理测试（新表名已存在）" << std::endl;
    
    cleanupTestFiles();
    
    // 创建两个表
    CreateTableHandler createHandler;
    std::string createSql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql2 = "CREATE TABLE Customers ( CustomerID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql1);
    createHandler.execute(createSql2);
    
    // 尝试重命名为已存在的表名
    RenameTableHandler handler;
    std::string sql = "RENAME TABLE Users Customers IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "新表名已存在返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（新旧表名相同）
 */
void testErrorHandlingSameName() {
    std::cout << "\n[测试5] 错误处理测试（新旧表名相同）" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 尝试重命名为相同名称
    RenameTableHandler handler;
    std::string sql = "RENAME TABLE Users Users IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "新旧表名相同返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（无效SQL）
 */
void testErrorHandlingInvalidSQL() {
    std::cout << "\n[测试6] 错误处理测试（无效SQL）" << std::endl;
    
    RenameTableHandler handler;
    std::string sql = "INVALID SQL STATEMENT;";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "无效SQL返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试多次重命名
 */
void testMultipleRenames() {
    std::cout << "\n[测试7] 多次重命名测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Table1 ( Field1 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    RenameTableHandler handler;
    
    // 第一次重命名
    std::string sql1 = "RENAME TABLE Table1 Table2 IN " + std::string(TEST_DB_FILE) + ";";
    bool result1 = handler.execute(sql1);
    TEST_ASSERT(result1 == true, "第一次重命名成功");
    
    // 第二次重命名
    std::string sql2 = "RENAME TABLE Table2 Table3 IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = handler.execute(sql2);
    TEST_ASSERT(result2 == true, "第二次重命名成功");
    
    // 验证最终表名
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Table1"), "Table1不存在");
    TEST_ASSERT(!manager.tableExists("Table2"), "Table2不存在");
    TEST_ASSERT(manager.tableExists("Table3"), "Table3存在");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  RENAME TABLE Handler测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testBasicRename();
        testRenameWithData();
        testErrorHandlingTableNotExists();
        testErrorHandlingNewTableExists();
        testErrorHandlingSameName();
        testErrorHandlingInvalidSQL();
        testMultipleRenames();
        
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

