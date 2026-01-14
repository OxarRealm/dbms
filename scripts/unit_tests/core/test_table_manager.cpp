/**
 * @file test_table_manager.cpp
 * @brief TableManager类测试程序
 * 
 * 测试.dbf文件读写功能
 */

#include "../../include/core/table_manager.h"
#include "../../include/core/table_mode.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>

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
const char* TEST_DB_FILE = "test_db";

/**
 * @brief 创建测试表结构
 */
TableInfo createTestTableInfo(const char* tableName) {
    TableInfo tableInfo(tableName);
    
    TableMode field1, field2, field3;
    initTableMode(field1, "ID", "int", 4, FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(field2, "Name", "char", 50, FLAG_NOT_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(field3, "Age", "int", 4, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    tableInfo.fields.push_back(field1);
    tableInfo.fields.push_back(field2);
    tableInfo.fields.push_back(field3);
    
    return tableInfo;
}

/**
 * @brief 测试创建表
 */
void testCreateTable() {
    std::cout << "\n[测试1] 创建表测试" << std::endl;
    
    // 删除测试文件（如果存在）
    std::remove((std::string(TEST_DB_FILE) + ".dbf").c_str());
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    TableInfo tableInfo = createTestTableInfo("Users");
    bool result = manager.createTable(tableInfo);
    
    TEST_ASSERT(result == true, "创建表成功");
    TEST_ASSERT(manager.tableExists("Users") == true, "表存在检查");
}

/**
 * @brief 测试读取表
 */
void testReadTable() {
    std::cout << "\n[测试2] 读取表测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    TableInfo tableInfo;
    bool result = manager.readTable("Users", tableInfo);
    
    TEST_ASSERT(result == true, "读取表成功");
    TEST_ASSERT(strcmp(tableInfo.tableName, "Users") == 0, "表名正确");
    TEST_ASSERT(tableInfo.fields.size() == 3, "字段数量正确");
    TEST_ASSERT(strcmp(tableInfo.fields[0].sFieldName, "ID") == 0, "第一个字段名正确");
    TEST_ASSERT(strcmp(tableInfo.fields[1].sFieldName, "Name") == 0, "第二个字段名正确");
    TEST_ASSERT(strcmp(tableInfo.fields[2].sFieldName, "Age") == 0, "第三个字段名正确");
}

/**
 * @brief 测试多表存储
 */
void testMultipleTables() {
    std::cout << "\n[测试3] 多表存储测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 创建第二个表
    TableInfo tableInfo2 = createTestTableInfo("Songs");
    tableInfo2.fields[0].sFieldName[0] = 'S';
    tableInfo2.fields[0].sFieldName[1] = 'o';
    tableInfo2.fields[0].sFieldName[2] = 'n';
    tableInfo2.fields[0].sFieldName[3] = 'g';
    tableInfo2.fields[0].sFieldName[4] = 'I';
    tableInfo2.fields[0].sFieldName[5] = 'D';
    tableInfo2.fields[0].sFieldName[6] = '\0';
    
    bool result = manager.createTable(tableInfo2);
    TEST_ASSERT(result == true, "创建第二个表成功");
    
    // 检查两个表都存在
    TEST_ASSERT(manager.tableExists("Users") == true, "Users表存在");
    TEST_ASSERT(manager.tableExists("Songs") == true, "Songs表存在");
    
    // 读取所有表
    std::vector<TableInfo> tableList;
    result = manager.readAllTables(tableList);
    TEST_ASSERT(result == true, "读取所有表成功");
    TEST_ASSERT(tableList.size() == 2, "表数量正确");
}

/**
 * @brief 测试更新表
 */
void testUpdateTable() {
    std::cout << "\n[测试4] 更新表测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 读取原表
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    
    // 添加新字段
    TableMode newField;
    initTableMode(newField, "Email", "char", 100, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    tableInfo.fields.push_back(newField);
    
    // 更新表
    bool result = manager.updateTable("Users", tableInfo);
    TEST_ASSERT(result == true, "更新表成功");
    
    // 验证更新
    TableInfo updatedTableInfo;
    manager.readTable("Users", updatedTableInfo);
    TEST_ASSERT(updatedTableInfo.fields.size() == 4, "字段数量更新正确");
    TEST_ASSERT(strcmp(updatedTableInfo.fields[3].sFieldName, "Email") == 0, "新字段名正确");
}

/**
 * @brief 测试删除表
 */
void testDeleteTable() {
    std::cout << "\n[测试5] 删除表测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 删除表
    bool result = manager.deleteTable("Songs");
    TEST_ASSERT(result == true, "删除表成功");
    
    // 验证删除
    TEST_ASSERT(manager.tableExists("Songs") == false, "表已删除");
    TEST_ASSERT(manager.tableExists("Users") == true, "其他表仍然存在");
    
    // 验证表数量
    std::vector<TableInfo> tableList;
    manager.readAllTables(tableList);
    TEST_ASSERT(tableList.size() == 1, "表数量正确");
}

/**
 * @brief 测试重命名表
 */
void testRenameTable() {
    std::cout << "\n[测试6] 重命名表测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 重命名表
    bool result = manager.renameTable("Users", "UserTable");
    TEST_ASSERT(result == true, "重命名表成功");
    
    // 验证重命名
    TEST_ASSERT(manager.tableExists("Users") == false, "旧表名不存在");
    TEST_ASSERT(manager.tableExists("UserTable") == true, "新表名存在");
    
    // 验证表结构
    TableInfo tableInfo;
    manager.readTable("UserTable", tableInfo);
    TEST_ASSERT(strcmp(tableInfo.tableName, "UserTable") == 0, "表名更新正确");
}

/**
 * @brief 测试获取所有表名
 */
void testGetAllTableNames() {
    std::cout << "\n[测试7] 获取所有表名测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<std::string> tableNames;
    bool result = manager.getAllTableNames(tableNames);
    
    TEST_ASSERT(result == true, "获取表名列表成功");
    TEST_ASSERT(tableNames.size() == 1, "表名数量正确");
    TEST_ASSERT(tableNames[0] == "UserTable", "表名正确");
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试8] 错误处理测试" << std::endl;
    
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 测试读取不存在的表
    TableInfo tableInfo;
    bool result = manager.readTable("NonExistentTable", tableInfo);
    TEST_ASSERT(result == false, "读取不存在的表返回false");
    
    // 测试删除不存在的表
    result = manager.deleteTable("NonExistentTable");
    TEST_ASSERT(result == false, "删除不存在的表返回false");
    
    // 测试创建已存在的表（应该失败）
    TableInfo newTable = createTestTableInfo("UserTable");
    result = manager.createTable(newTable);
    TEST_ASSERT(result == false, "创建已存在的表返回false");
}

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::remove((std::string(TEST_DB_FILE) + ".dbf").c_str());
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  TableManager测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testCreateTable();
        testReadTable();
        testMultipleTables();
        testUpdateTable();
        testDeleteTable();
        testRenameTable();
        testGetAllTableNames();
        testErrorHandling();
        
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

