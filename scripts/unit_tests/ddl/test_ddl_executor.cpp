/**
 * @file test_ddl_executor.cpp
 * @brief DDL执行器测试程序
 * 
 * 测试DDLExecutor类的功能
 */

#include "../../include/ddl/ddl_executor.h"
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
const char* TEST_DB_FILE = "test_db_ddl_executor";

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
 * @brief 测试CREATE TABLE执行
 */
void testCreateTable() {
    std::cout << "\n[测试1] CREATE TABLE执行测试" << std::endl;
    
    cleanupTestFiles();
    
    DDLExecutor executor;
    std::string sql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result = executor.execute(sql);
    TEST_ASSERT(result == true, "CREATE TABLE执行成功");
    
    // 验证表已创建
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(manager.tableExists("Users"), "表已创建");
}

/**
 * @brief 测试EDIT TABLE执行
 */
void testEditTable() {
    std::cout << "\n[测试2] EDIT TABLE执行测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    DDLExecutor executor;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    executor.execute(createSql);
    
    // 编辑表（添加字段）
    std::string editSql = "EDIT TABLE Users ( UserName char NOT_KEY NO_NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result = executor.execute(editSql);
    TEST_ASSERT(result == true, "EDIT TABLE执行成功");
    
    // 验证字段已添加
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TableInfo tableInfo;
    manager.readTable("Users", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 2, "字段已添加");
}

/**
 * @brief 测试RENAME TABLE执行
 */
void testRenameTable() {
    std::cout << "\n[测试3] RENAME TABLE执行测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    DDLExecutor executor;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    executor.execute(createSql);
    
    // 重命名表
    std::string renameSql = "RENAME TABLE Users Customers IN " + std::string(TEST_DB_FILE) + ";";
    bool result = executor.execute(renameSql);
    TEST_ASSERT(result == true, "RENAME TABLE执行成功");
    
    // 验证表已重命名
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Users"), "旧表名不存在");
    TEST_ASSERT(manager.tableExists("Customers"), "新表名存在");
}

/**
 * @brief 测试DROP TABLE执行
 */
void testDropTable() {
    std::cout << "\n[测试4] DROP TABLE执行测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表
    DDLExecutor executor;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    executor.execute(createSql);
    
    // 删除表
    std::string dropSql = "DROP TABLE Users IN " + std::string(TEST_DB_FILE) + ";";
    bool result = executor.execute(dropSql);
    TEST_ASSERT(result == true, "DROP TABLE执行成功");
    
    // 验证表已删除
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Users"), "表已删除");
}

/**
 * @brief 测试完整的DDL操作流程
 */
void testFullDDLWorkflow() {
    std::cout << "\n[测试5] 完整DDL操作流程测试" << std::endl;
    
    cleanupTestFiles();
    
    DDLExecutor executor;
    
    // 1. 创建表
    std::string createSql = "CREATE TABLE Products ( ProductID int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool result1 = executor.execute(createSql);
    TEST_ASSERT(result1 == true, "步骤1: 创建表成功");
    
    // 2. 编辑表（添加字段）
    std::string editSql = "EDIT TABLE Products ( ProductName char NOT_KEY NO_NULL VALID ) IN " + std::string(TEST_DB_FILE) + ";";
    bool result2 = executor.execute(editSql);
    TEST_ASSERT(result2 == true, "步骤2: 编辑表成功");
    
    // 3. 重命名表
    std::string renameSql = "RENAME TABLE Products Items IN " + std::string(TEST_DB_FILE) + ";";
    bool result3 = executor.execute(renameSql);
    TEST_ASSERT(result3 == true, "步骤3: 重命名表成功");
    
    // 4. 验证最终状态
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(!manager.tableExists("Products"), "旧表名不存在");
    TEST_ASSERT(manager.tableExists("Items"), "新表名存在");
    
    TableInfo tableInfo;
    manager.readTable("Items", tableInfo);
    TEST_ASSERT(tableInfo.fields.size() == 2, "表结构正确（2个字段）");
    
    // 5. 删除表
    std::string dropSql = "DROP TABLE Items IN " + std::string(TEST_DB_FILE) + ";";
    bool result4 = executor.execute(dropSql);
    TEST_ASSERT(result4 == true, "步骤4: 删除表成功");
    
    TEST_ASSERT(!manager.tableExists("Items"), "表已删除");
}

/**
 * @brief 测试错误处理（无效SQL）
 */
void testErrorHandlingInvalidSQL() {
    std::cout << "\n[测试6] 错误处理测试（无效SQL）" << std::endl;
    
    DDLExecutor executor;
    std::string sql = "INVALID SQL STATEMENT;";
    
    bool result = executor.execute(sql);
    TEST_ASSERT(result == false, "无效SQL返回false");
    TEST_ASSERT(!executor.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试错误处理（非DDL语句）
 */
void testErrorHandlingNonDDL() {
    std::cout << "\n[测试7] 错误处理测试（非DDL语句）" << std::endl;
    
    DDLExecutor executor;
    std::string sql = "SELECT * FROM Users;";
    
    bool result = executor.execute(sql);
    TEST_ASSERT(result == false, "非DDL语句返回false");
    TEST_ASSERT(!executor.getLastError().empty(), "有错误信息");
}

/**
 * @brief 测试isDDLStatement静态方法
 */
void testIsDDLStatement() {
    std::cout << "\n[测试8] isDDLStatement静态方法测试" << std::endl;
    
    TEST_ASSERT(DDLExecutor::isDDLStatement("CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO test;"), "识别CREATE TABLE");
    TEST_ASSERT(DDLExecutor::isDDLStatement("EDIT TABLE Users ( UserName char NOT_KEY NO_NULL VALID ) IN test;"), "识别EDIT TABLE");
    TEST_ASSERT(DDLExecutor::isDDLStatement("RENAME TABLE Users Customers IN test;"), "识别RENAME TABLE");
    TEST_ASSERT(DDLExecutor::isDDLStatement("DROP TABLE Users IN test;"), "识别DROP TABLE");
    TEST_ASSERT(!DDLExecutor::isDDLStatement("SELECT * FROM Users;"), "不识别SELECT");
    TEST_ASSERT(!DDLExecutor::isDDLStatement("INSERT INTO Users VALUES (1);"), "不识别INSERT");
    TEST_ASSERT(!DDLExecutor::isDDLStatement(""), "不识别空字符串");
}

/**
 * @brief 测试多个表的DDL操作
 */
void testMultipleTablesDDL() {
    std::cout << "\n[测试9] 多个表的DDL操作测试" << std::endl;
    
    cleanupTestFiles();
    
    DDLExecutor executor;
    
    // 创建多个表
    std::string createSql1 = "CREATE TABLE Table1 ( Field1 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql2 = "CREATE TABLE Table2 ( Field2 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    std::string createSql3 = "CREATE TABLE Table3 ( Field3 int KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    
    bool result1 = executor.execute(createSql1);
    bool result2 = executor.execute(createSql2);
    bool result3 = executor.execute(createSql3);
    
    TEST_ASSERT(result1 == true, "创建Table1成功");
    TEST_ASSERT(result2 == true, "创建Table2成功");
    TEST_ASSERT(result3 == true, "创建Table3成功");
    
    // 验证所有表存在
    TableManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(manager.tableExists("Table1"), "Table1存在");
    TEST_ASSERT(manager.tableExists("Table2"), "Table2存在");
    TEST_ASSERT(manager.tableExists("Table3"), "Table3存在");
    
    // 删除其中一个表
    std::string dropSql = "DROP TABLE Table2 IN " + std::string(TEST_DB_FILE) + ";";
    bool result4 = executor.execute(dropSql);
    TEST_ASSERT(result4 == true, "删除Table2成功");
    
    // 验证只有Table2被删除
    TEST_ASSERT(manager.tableExists("Table1"), "Table1仍然存在");
    TEST_ASSERT(!manager.tableExists("Table2"), "Table2已删除");
    TEST_ASSERT(manager.tableExists("Table3"), "Table3仍然存在");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  DDL Executor测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testCreateTable();
        testEditTable();
        testRenameTable();
        testDropTable();
        testFullDDLWorkflow();
        testErrorHandlingInvalidSQL();
        testErrorHandlingNonDDL();
        testIsDDLStatement();
        testMultipleTablesDDL();
        
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

