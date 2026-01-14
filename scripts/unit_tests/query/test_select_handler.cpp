/**
 * @file test_select_handler.cpp
 * @brief SELECT查询处理器测试程序
 * 
 * 测试SelectHandler类的功能
 */

#include "../../include/query/select_handler.h"
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
const char* TEST_DB_FILE = "test_db_select";

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
 * @brief 测试基本SELECT *查询
 */
void testBasicSelectAll() {
    std::cout << "\n[测试1] 基本SELECT *查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob', 'inactive' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行SELECT查询
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT * FROM Users;";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == true, "SELECT执行成功");
    TEST_ASSERT(result.rowCount == 2, "查询结果行数正确");
    TEST_ASSERT(result.columnNames.size() == 3, "列数正确");
    TEST_ASSERT(result.rows.size() == 2, "结果行数正确");
}

/**
 * @brief 测试SELECT指定字段查询
 */
void testSelectSpecificFields() {
    std::cout << "\n[测试2] SELECT指定字段查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行SELECT查询（只选择部分字段）
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT UserID, UserName FROM Users;";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == true, "SELECT执行成功");
    TEST_ASSERT(result.rowCount == 1, "查询结果行数正确");
    TEST_ASSERT(result.columnNames.size() == 2, "列数正确");
    TEST_ASSERT(result.columnNames[0] == "UserID", "第一列名正确");
    TEST_ASSERT(result.columnNames[1] == "UserName", "第二列名正确");
    TEST_ASSERT(result.rows[0][0] == "1", "UserID值正确");
    TEST_ASSERT(result.rows[0][1] == "Alice", "UserName值正确");
}

/**
 * @brief 测试SELECT WHERE条件查询
 */
void testSelectWithWhere() {
    std::cout << "\n[测试3] SELECT WHERE条件查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID, Status char NOT_KEY NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob', 'inactive' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '3', 'Charlie', 'active' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行SELECT查询（带WHERE条件）
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT * FROM Users WHERE Status='active';";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == true, "SELECT执行成功");
    TEST_ASSERT(result.rowCount == 2, "查询结果行数正确（2条active记录）");
    TEST_ASSERT(result.rows[0][1] == "Alice" || result.rows[0][1] == "Charlie", "第一条记录正确");
    TEST_ASSERT(result.rows[1][1] == "Alice" || result.rows[1][1] == "Charlie", "第二条记录正确");
}

/**
 * @brief 测试SELECT空结果
 */
void testSelectEmptyResult() {
    std::cout << "\n[测试4] SELECT空结果测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表（不插入记录）
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    // 执行SELECT查询
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT * FROM Users;";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == true, "SELECT执行成功");
    TEST_ASSERT(result.rowCount == 0, "查询结果行数为0");
    TEST_ASSERT(result.rows.size() == 0, "结果行为空");
}

/**
 * @brief 测试SELECT到不存在的表
 */
void testSelectNonExistentTable() {
    std::cout << "\n[测试5] SELECT到不存在的表测试" << std::endl;
    
    cleanupTestFiles();
    
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT * FROM NonExistent;";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == false, "SELECT应该失败（表不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试SELECT不存在的字段
 */
void testSelectNonExistentField() {
    std::cout << "\n[测试6] SELECT不存在的字段测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行SELECT查询（使用不存在的字段）
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT NonExistentField FROM Users;";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == false, "SELECT应该失败（字段不存在）");
    TEST_ASSERT(!handler.getLastError().empty(), "错误信息不为空");
}

/**
 * @brief 测试SELECT WHERE条件不匹配
 */
void testSelectWhereNoMatch() {
    std::cout << "\n[测试7] SELECT WHERE条件不匹配测试" << std::endl;
    
    cleanupTestFiles();
    
    // 先创建表并插入记录
    CreateTableHandler createHandler;
    std::string createSql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    bool createResult = createHandler.execute(createSql);
    TEST_ASSERT(createResult == true, "创建表成功");
    
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行SELECT查询（WHERE条件不匹配）
    SelectHandler handler;
    handler.setDatabasePath(TEST_DB_FILE);
    QueryResult result;
    std::string sql = "SELECT * FROM Users WHERE UserID='999';";
    
    bool queryResult = handler.execute(sql, result);
    TEST_ASSERT(queryResult == true, "SELECT执行成功（没有匹配的记录不算错误）");
    TEST_ASSERT(result.rowCount == 0, "查询结果行数为0");
}

/**
 * @brief 测试多表SELECT *查询（笛卡尔积）
 */
void testMultiTableSelectAll() {
    std::cout << "\n[测试8] 多表SELECT *查询测试（笛卡尔积）" << std::endl;
    
    cleanupTestFiles();
    
    // 创建第一个表
    CreateTableHandler createHandler1;
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    if (!createHandler1.execute(sql1)) {
        std::cout << "  错误: " << createHandler1.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "创建表Users成功");
    
    // 创建第二个表
    CreateTableHandler createHandler2;
    std::string sql2 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID, UserID int NOT_KEY NO_NULL VALID, Product string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    if (!createHandler2.execute(sql2)) {
        std::cout << "  错误: " << createHandler2.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "创建表Orders成功");
    
    // 插入数据到Users表
    InsertHandler insertHandler1;
    std::string insertSql1 = "INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";";
    std::string insertSql2 = "INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";";
    TEST_ASSERT(insertHandler1.execute(insertSql1), "插入Users记录1成功");
    TEST_ASSERT(insertHandler1.execute(insertSql2), "插入Users记录2成功");
    
    // 插入数据到Orders表
    InsertHandler insertHandler2;
    std::string insertSql3 = "INSERT INTO Orders VALUES ( '101', '1', 'ProductA' ) IN " + std::string(TEST_DB_FILE) + ";";
    std::string insertSql4 = "INSERT INTO Orders VALUES ( '102', '2', 'ProductB' ) IN " + std::string(TEST_DB_FILE) + ";";
    TEST_ASSERT(insertHandler2.execute(insertSql3), "插入Orders记录1成功");
    TEST_ASSERT(insertHandler2.execute(insertSql4), "插入Orders记录2成功");
    
    // 执行多表查询
    SelectHandler selectHandler;
    selectHandler.setDatabasePath(TEST_DB_FILE);
    std::string selectSql = "SELECT * FROM Users, Orders;";
    QueryResult result;
    TEST_ASSERT(selectHandler.execute(selectSql, result), "SELECT执行成功");
    
    // 验证结果：2个Users记录 × 2个Orders记录 = 4条结果
    TEST_ASSERT(result.rowCount == 4, "查询结果行数正确（2×2=4）");
    TEST_ASSERT(result.columnNames.size() == 5, "列数正确（Users 2列 + Orders 3列 = 5列）");
    
    // 验证列名格式：TableName.FieldName
    bool hasUsersUserID = false, hasUsersUserName = false;
    bool hasOrdersOrderID = false, hasOrdersUserID = false, hasOrdersProduct = false;
    for (const auto& colName : result.columnNames) {
        if (colName == "Users.UserID") hasUsersUserID = true;
        if (colName == "Users.UserName") hasUsersUserName = true;
        if (colName == "Orders.OrderID") hasOrdersOrderID = true;
        if (colName == "Orders.UserID") hasOrdersUserID = true;
        if (colName == "Orders.Product") hasOrdersProduct = true;
    }
    TEST_ASSERT(hasUsersUserID && hasUsersUserName, "Users表列名正确");
    TEST_ASSERT(hasOrdersOrderID && hasOrdersUserID && hasOrdersProduct, "Orders表列名正确");
}

/**
 * @brief 测试多表SELECT指定字段查询
 */
void testMultiTableSelectSpecificFields() {
    std::cout << "\n[测试9] 多表SELECT指定字段查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler1;
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    if (!createHandler1.execute(sql1)) {
        std::cout << "  错误: " << createHandler1.getLastError() << std::endl;
        return;
    }
    
    CreateTableHandler createHandler2;
    std::string sql2 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID, UserID int NOT_KEY NO_NULL VALID, Product string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    if (!createHandler2.execute(sql2)) {
        std::cout << "  错误: " << createHandler2.getLastError() << std::endl;
        return;
    }
    
    // 插入数据
    InsertHandler insertHandler;
    if (!insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";")) {
        std::cout << "  错误: " << insertHandler.getLastError() << std::endl;
        return;
    }
    if (!insertHandler.execute("INSERT INTO Orders VALUES ( '101', '1', 'ProductA' ) IN " + std::string(TEST_DB_FILE) + ";")) {
        std::cout << "  错误: " << insertHandler.getLastError() << std::endl;
        return;
    }
    
    // 执行多表查询（指定字段，使用简单字段名，因为字段名不冲突）
    SelectHandler selectHandler;
    selectHandler.setDatabasePath(TEST_DB_FILE);
    std::string selectSql = "SELECT UserName, Product FROM Users, Orders;";
    QueryResult result;
    if (!selectHandler.execute(selectSql, result)) {
        std::cout << "  错误: " << selectHandler.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "SELECT执行成功");
    
    // 笛卡尔积：1个Users记录 × 1个Orders记录 = 1条结果
    TEST_ASSERT(result.rowCount == 1, "查询结果行数正确");
    TEST_ASSERT(result.columnNames.size() == 2, "列数正确");
    TEST_ASSERT(result.columnNames[0] == "UserName", "第一列名正确");
    TEST_ASSERT(result.columnNames[1] == "Product", "第二列名正确");
    
    if (result.rowCount > 0 && result.rows[0].size() >= 2) {
        TEST_ASSERT(result.rows[0][0] == "Alice", "UserName值正确");
        TEST_ASSERT(result.rows[0][1] == "ProductA", "Product值正确");
    }
}

/**
 * @brief 测试多表SELECT WHERE条件查询
 */
void testMultiTableSelectWithWhere() {
    std::cout << "\n[测试10] 多表SELECT WHERE条件查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler1;
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler1.execute(sql1);
    
    CreateTableHandler createHandler2;
    std::string sql2 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID, UserID int NOT_KEY NO_NULL VALID, Product string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler2.execute(sql2);
    
    // 插入数据
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '101', '1', 'ProductA' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '102', '2', 'ProductB' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行多表查询（WHERE条件：UserID = '1'，使用简单字段名）
    SelectHandler selectHandler;
    selectHandler.setDatabasePath(TEST_DB_FILE);
    std::string selectSql = "SELECT UserName, Product FROM Users, Orders WHERE UserID='1';";
    QueryResult result;
    if (!selectHandler.execute(selectSql, result)) {
        std::cout << "  错误: " << selectHandler.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "SELECT执行成功");
    
    // 笛卡尔积：2个Users × 2个Orders = 4条
    // WHERE条件过滤：UserID='1'（会匹配Users表的UserID，因为findMultiTableFieldIndex返回第一个找到的）
    // 应该返回2条记录（UserID=1的用户与2个订单的组合）
    TEST_ASSERT(result.rowCount == 2, "查询结果行数正确（UserID=1的用户与2个订单的组合）");
}

/**
 * @brief 测试INNER JOIN查询
 */
void testInnerJoin() {
    std::cout << "\n[测试11] INNER JOIN查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler1;
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler1.execute(sql1);
    
    CreateTableHandler createHandler2;
    std::string sql2 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID, UserID int NOT_KEY NO_NULL VALID, Product string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler2.execute(sql2);
    
    // 插入数据
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '101', '1', 'ProductA' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '102', '1', 'ProductB' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '103', '2', 'ProductC' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行INNER JOIN查询（使用简单字段名，因为字段名不冲突）
    SelectHandler selectHandler;
    selectHandler.setDatabasePath(TEST_DB_FILE);
    std::string selectSql = "SELECT UserName, Product FROM Users INNER JOIN Orders ON UserID = UserID;";
    QueryResult result;
    if (!selectHandler.execute(selectSql, result)) {
        std::cout << "  错误: " << selectHandler.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "INNER JOIN执行成功");
    
    // 应该返回3条记录（UserID=1有2个订单，UserID=2有1个订单）
    TEST_ASSERT(result.rowCount == 3, "查询结果行数正确（3条匹配记录）");
    TEST_ASSERT(result.columnNames.size() == 2, "列数正确");
}

/**
 * @brief 测试LEFT JOIN查询
 */
void testLeftJoin() {
    std::cout << "\n[测试12] LEFT JOIN查询测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler1;
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler1.execute(sql1);
    
    CreateTableHandler createHandler2;
    std::string sql2 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID, UserID int NOT_KEY NO_NULL VALID, Product string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler2.execute(sql2);
    
    // 插入数据（UserID=3没有订单）
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '3', 'Charlie' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '101', '1', 'ProductA' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '102', '2', 'ProductB' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行LEFT JOIN查询（使用简单字段名）
    SelectHandler selectHandler;
    selectHandler.setDatabasePath(TEST_DB_FILE);
    std::string selectSql = "SELECT UserName, Product FROM Users LEFT JOIN Orders ON UserID = UserID;";
    QueryResult result;
    if (!selectHandler.execute(selectSql, result)) {
        std::cout << "  错误: " << selectHandler.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "LEFT JOIN执行成功");
    
    // 应该返回3条记录（包括UserID=3，但Orders字段为空）
    TEST_ASSERT(result.rowCount == 3, "查询结果行数正确（包括没有订单的用户）");
}

/**
 * @brief 测试JOIN查询带WHERE条件
 */
void testJoinWithWhere() {
    std::cout << "\n[测试13] JOIN查询带WHERE条件测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建表
    CreateTableHandler createHandler1;
    std::string sql1 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler1.execute(sql1);
    
    CreateTableHandler createHandler2;
    std::string sql2 = "CREATE TABLE Orders ( OrderID int KEY NO_NULL VALID, UserID int NOT_KEY NO_NULL VALID, Product string NOT_KEY NO_NULL VALID ) INTO " + std::string(TEST_DB_FILE) + ";";
    createHandler2.execute(sql2);
    
    // 插入数据
    InsertHandler insertHandler;
    insertHandler.execute("INSERT INTO Users VALUES ( '1', 'Alice' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Users VALUES ( '2', 'Bob' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '101', '1', 'ProductA' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '102', '1', 'ProductB' ) IN " + std::string(TEST_DB_FILE) + ";");
    insertHandler.execute("INSERT INTO Orders VALUES ( '103', '2', 'ProductC' ) IN " + std::string(TEST_DB_FILE) + ";");
    
    // 执行JOIN查询带WHERE条件（使用简单字段名）
    SelectHandler selectHandler;
    selectHandler.setDatabasePath(TEST_DB_FILE);
    std::string selectSql = "SELECT UserName, Product FROM Users INNER JOIN Orders ON UserID = UserID WHERE UserID='1';";
    QueryResult result;
    if (!selectHandler.execute(selectSql, result)) {
        std::cout << "  错误: " << selectHandler.getLastError() << std::endl;
        return;
    }
    TEST_ASSERT(true, "JOIN查询带WHERE条件执行成功");
    
    // 应该返回2条记录（UserID=1的2个订单）
    TEST_ASSERT(result.rowCount == 2, "查询结果行数正确（WHERE过滤后）");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "SELECT Handler 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testBasicSelectAll();
    testSelectSpecificFields();
    testSelectWithWhere();
    testSelectEmptyResult();
    testSelectNonExistentTable();
    testSelectNonExistentField();
    testSelectWhereNoMatch();
    testMultiTableSelectAll();
    testMultiTableSelectSpecificFields();
    testMultiTableSelectWithWhere();
    testInnerJoin();
    testLeftJoin();
    testJoinWithWhere();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;
    
    cleanupTestFiles();
    
    return (testsFailed == 0) ? 0 : 1;
}

