/**
 * @file test_edit_table_handler.cpp
 * @brief EDIT TABLE处理器测试程序
 * 
 * 测试EditTableHandler类的功能
 */

#include "../../include/ddl/edit_table_handler.h"
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
const char* TEST_DB_FILE = "test_db_edit";

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::string dbfFile = std::string(TEST_DB_FILE) + ".dbf";
    std::remove(dbfFile.c_str());
}

/**
 * @brief 测试添加新字段
 */
void testAddField() {
    std::cout << "\n[测试1] 添加新字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 添加新字段
    EditTableHandler handler;
    std::string sql = "EDIT TABLE Users ( UserName char NOT_KEY NO_NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "EDIT TABLE执行成功");
    
    // 验证字段已添加
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 2, "字段数量正确");
    TEST_ASSERT(strcmp(tableInfo.fields[1].sFieldName, "UserName") == 0, "新字段名正确");
}

/**
 * @brief 测试修改现有字段
 */
void testModifyField() {
    std::cout << "\n[测试2] 修改现有字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 修改字段（将UserName改为允许NULL）
    EditTableHandler handler;
    std::string sql = "EDIT TABLE Users ( UserName char NOT_KEY NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "EDIT TABLE执行成功");
    
    // 验证字段已修改
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 2, "字段数量不变");
    TEST_ASSERT(strcmp(tableInfo.fields[1].sFieldName, "UserName") == 0, "字段名不变");
    TEST_ASSERT(tableInfo.fields[1].bNullFlag == FLAG_NULL, "NULL标志已修改");
}

/**
 * @brief 测试修改字段类型
 */
void testModifyFieldType() {
    std::cout << "\n[测试3] 修改字段类型测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, Age char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 修改字段类型（将Age从char改为int）
    EditTableHandler handler;
    std::string sql = "EDIT TABLE Users ( Age int NOT_KEY NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "EDIT TABLE执行成功");
    
    // 验证字段类型已修改
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(strcmp(tableInfo.fields[1].sType, "int") == 0, "字段类型已修改");
    TEST_ASSERT(tableInfo.fields[1].iSize == 4, "字段大小已更新");
}

/**
 * @brief 测试修改KEY标志
 */
void testModifyKeyFlag() {
    std::cout << "\n[测试4] 修改KEY标志测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    // 修改KEY标志（将UserName改为KEY）
    EditTableHandler handler;
    std::string sql = "EDIT TABLE Users ( UserName char KEY NO_NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == true, "EDIT TABLE执行成功");
    
    // 验证KEY标志已修改
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(tableInfo.fields[1].bKey == FLAG_KEY, "KEY标志已修改");
}

/**
 * @brief 测试错误处理（表不存在）
 */
void testErrorHandlingTableNotExists() {
    std::cout << "\n[测试5] 错误处理测试（表不存在）" << std::endl;
    
    cleanupTestFiles();
    
    EditTableHandler handler;
    std::string sql = "EDIT TABLE NonExistentTable ( FieldName int KEY NO_NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "表不存在返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（无效SQL）
 */
void testErrorHandlingInvalidSQL() {
    std::cout << "\n[测试6] 错误处理测试（无效SQL）" << std::endl;
    
    EditTableHandler handler;
    std::string sql = "INVALID SQL STATEMENT;";
    
    bool result = handler.execute(sql);
    TEST_ASSERT(result == false, "无效SQL返回false");
    TEST_ASSERT(!handler.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试多次EDIT操作
 */
void testMultipleEditOperations() {
    std::cout << "\n[测试7] 多次EDIT操作测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler.execute(createSql);
    
    EditTableHandler handler;
    
    // 第一次EDIT：添加UserName字段
    std::string sql1 = "EDIT TABLE Users ( UserName char NOT_KEY NO_NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result1 = handler.execute(sql1);
    TEST_ASSERT(result1 == true, "第一次EDIT成功");
    
    // 第二次EDIT：添加Email字段
    std::string sql2 = "EDIT TABLE Users ( Email char NOT_KEY NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = handler.execute(sql2);
    TEST_ASSERT(result2 == true, "第二次EDIT成功");
    
    // 第三次EDIT：修改UserName字段
    std::string sql3 = "EDIT TABLE Users ( UserName char NOT_KEY NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result3 = handler.execute(sql3);
    TEST_ASSERT(result3 == true, "第三次EDIT成功");
    
    // 验证最终表结构
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 3, "字段数量正确");
    TEST_ASSERT(tableInfo.fields[1].bNullFlag == FLAG_NULL, "UserName字段已修改");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  EDIT TABLE Handler测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testAddField();
        testModifyField();
        testModifyFieldType();
        testModifyKeyFlag();
        testErrorHandlingTableNotExists();
        testErrorHandlingInvalidSQL();
        testMultipleEditOperations();
        
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

