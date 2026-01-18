/**
 * @file test_parser_where.cpp
 * @brief WHERE条件解析测试程序
 * 
 * 测试Parser类的WHERE条件解析功能
 * 包括：复杂条件、LIKE、IN、BETWEEN、子查询等
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
 * @brief 测试简单WHERE条件
 */
void testParseSimpleWhere() {
    std::cout << "\n[测试1] 简单WHERE条件解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age = '25';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "简单WHERE条件解析成功");
    TEST_ASSERT(parser.getLastError().empty(), "无错误信息");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->whereClause != nullptr, "WHERE条件存在");
        }
    }
}

/**
 * @brief 测试比较运算符
 */
void testParseComparisonOperators() {
    std::cout << "\n[测试2] 比较运算符解析测试" << std::endl;
    
    const char* operators[] = {">", "<", ">=", "<=", "!=", "="};
    const char* operatorNames[] = {">", "<", ">=", "<=", "!=", "="};
    
    for (int i = 0; i < 6; i++) {
        std::string sql = "SELECT * FROM Users WHERE Age " + std::string(operators[i]) + " '25';";
        Parser parser(sql);
        auto node = parser.parse();
        
        TEST_ASSERT(node != nullptr, std::string("运算符") + operatorNames[i] + "解析成功");
    }
}

/**
 * @brief 测试AND运算符
 */
void testParseAnd() {
    std::cout << "\n[测试3] AND运算符解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age > '25' AND Age < '30';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "AND运算符解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->operator_ == "AND", "AND运算符正确");
        }
    }
}

/**
 * @brief 测试OR运算符
 */
void testParseOr() {
    std::cout << "\n[测试4] OR运算符解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age = '26' OR Age = '30';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "OR运算符解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->operator_ == "OR", "OR运算符正确");
        }
    }
}

/**
 * @brief 测试NOT运算符
 */
void testParseNot() {
    std::cout << "\n[测试5] NOT运算符解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE NOT Age = '26';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "NOT运算符解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->operator_ == "NOT", "NOT运算符正确");
        }
    }
}

/**
 * @brief 测试括号优先级
 */
void testParseParentheses() {
    std::cout << "\n[测试6] 括号优先级解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE (Age > '25' AND Age < '30') OR UserID = '1';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "括号优先级解析成功");
}

/**
 * @brief 测试LIKE模式匹配
 */
void testParseLike() {
    std::cout << "\n[测试7] LIKE模式匹配解析测试" << std::endl;
    
    // 前缀匹配
    std::string sql1 = "SELECT * FROM Users WHERE UserName LIKE 'John%';";
    Parser parser1(sql1);
    auto node1 = parser1.parse();
    TEST_ASSERT(node1 != nullptr, "LIKE前缀匹配解析成功");
    
    // 后缀匹配
    std::string sql2 = "SELECT * FROM Users WHERE Email LIKE '%@example.com';";
    Parser parser2(sql2);
    auto node2 = parser2.parse();
    TEST_ASSERT(node2 != nullptr, "LIKE后缀匹配解析成功");
    
    // 包含匹配
    std::string sql3 = "SELECT * FROM Users WHERE UserName LIKE '%Smith%';";
    Parser parser3(sql3);
    auto node3 = parser3.parse();
    TEST_ASSERT(node3 != nullptr, "LIKE包含匹配解析成功");
}

/**
 * @brief 测试IN子句
 */
void testParseIn() {
    std::cout << "\n[测试8] IN子句解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age IN ('26', '30', '28');";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "IN子句解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->operator_ == "IN", "IN运算符正确");
        }
    }
}

/**
 * @brief 测试BETWEEN范围查询
 */
void testParseBetween() {
    std::cout << "\n[测试9] BETWEEN范围查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age BETWEEN '25' AND '30';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "BETWEEN范围查询解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->operator_ == "BETWEEN", "BETWEEN运算符正确");
        }
    }
}

/**
 * @brief 测试标量子查询
 */
void testParseSubqueryScalar() {
    std::cout << "\n[测试10] 标量子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE UserID = (SELECT UserID FROM Users WHERE UserName = 'John Doe');";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "标量子查询解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->hasSubquery(), "子查询存在");
        }
    }
}

/**
 * @brief 测试IN子查询
 */
void testParseSubqueryIn() {
    std::cout << "\n[测试11] IN子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE UserID IN (SELECT UserID FROM Products);";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "IN子查询解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->hasSubquery(), "子查询存在");
        }
    }
}

/**
 * @brief 测试EXISTS子查询
 */
void testParseSubqueryExists() {
    std::cout << "\n[测试12] EXISTS子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE EXISTS (SELECT * FROM Products WHERE Products.UserID = Users.UserID);";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "EXISTS子查询解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode && selectNode->whereClause) {
            TEST_ASSERT(selectNode->whereClause->operator_ == "EXISTS", "EXISTS运算符正确");
            TEST_ASSERT(selectNode->whereClause->hasSubquery(), "子查询存在");
        }
    }
}

/**
 * @brief 测试NOT EXISTS子查询
 */
void testParseSubqueryNotExists() {
    std::cout << "\n[测试13] NOT EXISTS子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE NOT EXISTS (SELECT * FROM Products WHERE Products.UserID = Users.UserID);";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "NOT EXISTS子查询解析成功");
}

/**
 * @brief 测试复杂组合条件
 */
void testParseComplexCombined() {
    std::cout << "\n[测试14] 复杂组合条件解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE (Age > '25' AND Age < '30') OR (UserName LIKE 'John%' AND Email IN ('john@example.com', 'john@test.com')) OR UserID = (SELECT UserID FROM Users WHERE UserName = 'John Doe');";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "复杂组合条件解析成功");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SQL WHERE Parser Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testParseSimpleWhere();
        testParseComparisonOperators();
        testParseAnd();
        testParseOr();
        testParseNot();
        testParseParentheses();
        testParseLike();
        testParseIn();
        testParseBetween();
        testParseSubqueryScalar();
        testParseSubqueryIn();
        testParseSubqueryExists();
        testParseSubqueryNotExists();
        testParseComplexCombined();
        
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

