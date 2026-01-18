/**
 * @file test_create_table_handler.cpp
 * @brief CREATE TABLE处理器测试程序
 * 
 * 测试CreateTableHandler类的功能
 */

#include "../../include/ddl/create_table_handler.h"
#include "../../include/core/table_manager.h"
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
const char* TEST_DB_FILE = "test_db_create";

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::string dbfFile = std::string(TEST_DB_FILE) + ".dbf";
    std::remove(dbfFile.c_str());
}

/**
 * @brief 测试基本CREATE TABLE
 */
void testBasicCreateTable() {
    std::cout << "\n[测试1] 基本CREATE TABLE测试" << std::endl;
    
    cleanupTestFiles();
    
    CreateTableHandler handler;
    std::string sql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "CREATE TABLE执行成功");
    
    // 验证表是否创建
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    bool exists = manager.tableExists("Users");
    TEST_ASSERT(exists == true, "表已创建");
    
    // 验证表结构
    TableInfo tableInfo;
    bool readResult = manager.readTable("Users", tableInfo);
    TEST_ASSERT(readResult == true, "读取表结构成功");
    TEST_ASSERT(strcmp(tableInfo.tableName, "Users") == 0, "表名正确");
    TEST_ASSERT(tableInfo.fields.size() == 2, "字段数量正确");
}

/**
 * @brief 测试复杂CREATE TABLE（多个字段）
 */
void testComplexCreateTable() {
    std::cout << "\n[测试2] 复杂CREATE TABLE测试" << std::endl;
    
    cleanupTestFiles();
    
    CreateTableHandler handler;
    std::string sql = "CREATE TABLE Songs ( SongID int KEY NO_NULL VALID, SongName char NOT_KEY NO_NULL VALID, Artist char NOT_KEY NULL VALID, Genre char NOT_KEY NULL VALID, Year int NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "CREATE TABLE执行成功");
    
    // 验证表结构
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Songs", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 5, "字段数量正确");
    TEST_ASSERT(strcmp(tableInfo.fields[0].sFieldName, "SongID") == 0, "第一个字段名正确");
    TEST_ASSERT(strcmp(tableInfo.fields[4].sFieldName, "Year") == 0, "最后一个字段名正确");
}

/**
 * @brief 测试错误处理（重复创建表）
 */
void testErrorHandlingDuplicate() {
    std::cout << "\n[测试3] 错误处理测试（重复创建表）" << std::endl;
    
    cleanupTestFiles();
    
    CreateTableHandler handler;
    std::string sql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result1 = handler.execute(sql);
    TEST_ASSERT(result1 == true, "第一次创建成功");
    
    bool result2 = handler.execute(sql);
    TEST_ASSERT(result2 == false, "重复创建失败");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（无效SQL）
 */
void testErrorHandlingInvalidSQL() {
    std::cout << "\n[测试4] 错误处理测试（无效SQL）" << std::endl;
    
    CreateTableHandler handler;
    std::string sql = "INVALID SQL STATEMENT;";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "无效SQL返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（空表名）
 */
void testErrorHandlingEmptyTableName() {
    std::cout << "\n[测试5] 错误处理测试（空字段）" << std::endl;
    
    CreateTableHandler handler;
    std::string sql = "CREATE TABLE ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "空表名返回false");
}

/**
 * @brief 测试多表创建
 */
void testMultipleTables() {
    std::cout << "\n[测试6] 多表创建测试" << std::endl;
    
    cleanupTestFiles();
    
    CreateTableHandler handler;
    
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string sql2 = "CREATE TABLE Songs ( SongID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result1 = handler.execute(sql1);
    bool result2 = handler.execute(sql2);
    
    TEST_ASSERT(result1 == true, "创建第一个表成功");
    TEST_ASSERT(result2 == true, "创建第二个表成功");
    
    // 验证两个表都存在
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(manager.tableExists("Users") == true, "Users表存在");
    TEST_ASSERT(manager.tableExists("Songs") == true, "Songs表存在");
}

/**
 * @brief 测试不同数据类型
 */
void testDifferentDataTypes() {
    std::cout << "\n[测试7] 不同数据类型测试" << std::endl;
    
    cleanupTestFiles();
    
    CreateTableHandler handler;
    std::string sql = "CREATE TABLE TestTable ( FieldInt int KEY NO_NULL VALID, FieldChar char NOT_KEY NO_NULL VALID, FieldFloat float NOT_KEY NULL VALID, FieldDouble double NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "CREATE TABLE执行成功");
    
    // 验证数据类型
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("TestTable", tableInfo);
    
    TEST_ASSERT(strcmp(tableInfo.fields[0].sType, "int") == 0, "int类型正确");
    TEST_ASSERT(strcmp(tableInfo.fields[1].sType, "char") == 0, "char类型正确");
    TEST_ASSERT(strcmp(tableInfo.fields[2].sType, "float") == 0, "float类型正确");
    TEST_ASSERT(strcmp(tableInfo.fields[3].sType, "double") == 0, "double类型正确");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  CREATE TABLE Handler测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testBasicCreateTable();
        testComplexCreateTable();
        testErrorHandlingDuplicate();
        testErrorHandlingInvalidSQL();
        testErrorHandlingEmptyTableName();
        testMultipleTables();
        testDifferentDataTypes();
        
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

