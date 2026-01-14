/**
 * @file test_parser.cpp
 * @brief SQL语法分析器测试程序
 * 
 * 测试Parser类的基础功能
 */

#include "../../include/sql_parser/parser.h"
#include "../../include/sql_parser/ast_node.h"
#include "../../include/core/table_mode.h"
#include <iostream>
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

/**
 * @brief 测试CREATE TABLE解析
 */
void testParseCreateTable() {
    std::cout << "\n[测试1] CREATE TABLE解析测试" << std::endl;
    
    std::string sql = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID, UserName char KEY NO_NULL VALID ) INTO MusicDB;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "CREATE TABLE解析成功");
    TEST_ASSERT(parser.getLastError().empty(), "无错误信息");
    
    if (node) {
        CreateTableNode* createNode = dynamic_cast<CreateTableNode*>(node.get());
        TEST_ASSERT(createNode != nullptr, "节点类型正确");
        if (createNode) {
            TEST_ASSERT(createNode->tableName == "Users", "表名正确");
            TEST_ASSERT(createNode->databaseFileName == "MusicDB", "数据库文件名正确");
            TEST_ASSERT(createNode->fields.size() == 2, "字段数量正确");
            
            if (createNode->fields.size() >= 2) {
                TEST_ASSERT(strcmp(createNode->fields[0].sFieldName, "UserID") == 0, "第一个字段名正确");
                TEST_ASSERT(strcmp(createNode->fields[0].sType, "int") == 0, "第一个字段类型正确");
                TEST_ASSERT(createNode->fields[0].bKey == FLAG_KEY, "第一个字段KEY标志正确");
                
                TEST_ASSERT(strcmp(createNode->fields[1].sFieldName, "UserName") == 0, "第二个字段名正确");
                TEST_ASSERT(strcmp(createNode->fields[1].sType, "char") == 0, "第二个字段类型正确");
            }
        }
    }
}

/**
 * @brief 测试EDIT TABLE解析
 */
void testParseEditTable() {
    std::cout << "\n[测试2] EDIT TABLE解析测试" << std::endl;
    
    std::string sql = "EDIT TABLE Users ( Email char NOT_KEY NULL VALID ) IN MusicDB;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "EDIT TABLE解析成功");
    
    if (node) {
        EditTableNode* editNode = dynamic_cast<EditTableNode*>(node.get());
        TEST_ASSERT(editNode != nullptr, "节点类型正确");
        if (editNode) {
            TEST_ASSERT(editNode->tableName == "Users", "表名正确");
            TEST_ASSERT(editNode->databaseFileName == "MusicDB", "数据库文件名正确");
            TEST_ASSERT(strcmp(editNode->field.sFieldName, "Email") == 0, "字段名正确");
            TEST_ASSERT(strcmp(editNode->field.sType, "char") == 0, "字段类型正确");
            TEST_ASSERT(editNode->field.bKey == FLAG_NOT_KEY, "KEY标志正确");
            TEST_ASSERT(editNode->field.bNullFlag == FLAG_NULL, "NULL标志正确");
        }
    }
}

/**
 * @brief 测试RENAME TABLE解析
 */
void testParseRenameTable() {
    std::cout << "\n[测试3] RENAME TABLE解析测试" << std::endl;
    
    std::string sql = "RENAME TABLE Users UserTable IN MusicDB;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "RENAME TABLE解析成功");
    
    if (node) {
        RenameTableNode* renameNode = dynamic_cast<RenameTableNode*>(node.get());
        TEST_ASSERT(renameNode != nullptr, "节点类型正确");
        if (renameNode) {
            TEST_ASSERT(renameNode->oldTableName == "Users", "旧表名正确");
            TEST_ASSERT(renameNode->newTableName == "UserTable", "新表名正确");
            TEST_ASSERT(renameNode->databaseFileName == "MusicDB", "数据库文件名正确");
        }
    }
}

/**
 * @brief 测试DROP TABLE解析
 */
void testParseDropTable() {
    std::cout << "\n[测试4] DROP TABLE解析测试" << std::endl;
    
    std::string sql = "DROP TABLE Users IN MusicDB;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "DROP TABLE解析成功");
    
    if (node) {
        DropTableNode* dropNode = dynamic_cast<DropTableNode*>(node.get());
        TEST_ASSERT(dropNode != nullptr, "节点类型正确");
        if (dropNode) {
            TEST_ASSERT(dropNode->tableName == "Users", "表名正确");
            TEST_ASSERT(dropNode->databaseFileName == "MusicDB", "数据库文件名正确");
        }
    }
}

/**
 * @brief 测试复杂CREATE TABLE（多个字段）
 */
void testParseComplexCreateTable() {
    std::cout << "\n[测试5] 复杂CREATE TABLE解析测试" << std::endl;
    
    std::string sql = "CREATE TABLE Songs ( SongID int KEY NO_NULL VALID, SongName char NOT_KEY NO_NULL VALID, Artist char NOT_KEY NULL VALID, Genre char NOT_KEY NULL VALID ) INTO MusicDB;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "复杂CREATE TABLE解析成功");
    
    if (node) {
        CreateTableNode* createNode = dynamic_cast<CreateTableNode*>(node.get());
        TEST_ASSERT(createNode != nullptr, "节点类型正确");
        if (createNode) {
            TEST_ASSERT(createNode->fields.size() == 4, "字段数量正确");
        }
    }
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试6] 错误处理测试" << std::endl;
    
    // 测试语法错误
    std::string sql1 = "CREATE TABLE Users UserID int KEY NO_NULL VALID INTO MusicDB;";
    Parser parser1(sql1);
    auto node1 = parser1.parse();
    TEST_ASSERT(node1 == nullptr, "语法错误返回nullptr");
    TEST_ASSERT(!parser1.getLastError().empty(), "有错误信息");
    
    // 测试缺少分号
    std::string sql2 = "CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO MusicDB";
    Parser parser2(sql2);
    auto node2 = parser2.parse();
    TEST_ASSERT(node2 == nullptr, "缺少分号返回nullptr");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SQL语法分析器测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testParseCreateTable();
        testParseEditTable();
        testParseRenameTable();
        testParseDropTable();
        testParseComplexCreateTable();
        testErrorHandling();
        
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
        return 1;
    }
}

