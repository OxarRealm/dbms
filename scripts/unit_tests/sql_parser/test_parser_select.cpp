/**
 * @file test_parser_select.cpp
 * @brief SELECT语句解析测试程序
 * 
 * 测试Parser类的SELECT语句解析功能
 * 包括：ORDER BY, DISTINCT, LIMIT, GROUP BY, HAVING, JOIN, UNION, 子查询等
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
 * @brief 测试基本SELECT解析
 */
void testParseBasicSelect() {
    std::cout << "\n[测试1] 基本SELECT解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "SELECT解析成功");
    TEST_ASSERT(parser.getLastError().empty(), "无错误信息");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->selectFields.size() == 1 && selectNode->selectFields[0] == "*", "SELECT *正确");
            TEST_ASSERT(selectNode->fromTables.size() == 1 && selectNode->fromTables[0] == "Users", "FROM表名正确");
        }
    }
}

/**
 * @brief 测试SELECT DISTINCT解析
 */
void testParseSelectDistinct() {
    std::cout << "\n[测试2] SELECT DISTINCT解析测试" << std::endl;
    
    std::string sql = "SELECT DISTINCT Age FROM Users;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "SELECT DISTINCT解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->distinct == true, "DISTINCT标志正确");
            TEST_ASSERT(selectNode->selectFields.size() == 1 && selectNode->selectFields[0] == "Age", "字段列表正确");
        }
    }
}

/**
 * @brief 测试ORDER BY解析
 */
void testParseOrderBy() {
    std::cout << "\n[测试3] ORDER BY解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users ORDER BY Age ASC, UserID DESC;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "ORDER BY解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->orderBy.size() == 2, "ORDER BY字段数量正确");
            if (selectNode->orderBy.size() >= 2) {
                TEST_ASSERT(selectNode->orderBy[0].fieldName == "Age" && selectNode->orderBy[0].direction == "ASC", "第一个ORDER BY正确");
                TEST_ASSERT(selectNode->orderBy[1].fieldName == "UserID" && selectNode->orderBy[1].direction == "DESC", "第二个ORDER BY正确");
            }
        }
    }
}

/**
 * @brief 测试LIMIT解析
 */
void testParseLimit() {
    std::cout << "\n[测试4] LIMIT解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users LIMIT 10;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "LIMIT解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->limitCount == 10, "LIMIT值正确");
        }
    }
}

/**
 * @brief 测试WHERE条件解析（简单条件）
 */
void testParseWhereSimple() {
    std::cout << "\n[测试5] WHERE简单条件解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age > '25';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "WHERE条件解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->whereClause != nullptr, "WHERE条件存在");
        }
    }
}

/**
 * @brief 测试WHERE条件解析（复杂AND/OR）
 */
void testParseWhereComplex() {
    std::cout << "\n[测试6] WHERE复杂条件解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age > '25' AND Age < '30' OR UserID = '1';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "WHERE复杂条件解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->whereClause != nullptr, "WHERE条件存在");
        }
    }
}

/**
 * @brief 测试LIKE解析
 */
void testParseLike() {
    std::cout << "\n[测试7] LIKE解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE UserName LIKE 'John%';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "LIKE解析成功");
}

/**
 * @brief 测试IN解析
 */
void testParseIn() {
    std::cout << "\n[测试8] IN解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age IN ('26', '30', '28');";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "IN解析成功");
}

/**
 * @brief 测试BETWEEN解析
 */
void testParseBetween() {
    std::cout << "\n[测试9] BETWEEN解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE Age BETWEEN '25' AND '30';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "BETWEEN解析成功");
}

/**
 * @brief 测试GROUP BY解析
 */
void testParseGroupBy() {
    std::cout << "\n[测试10] GROUP BY解析测试" << std::endl;
    
    std::string sql = "SELECT UserID, COUNT(*) FROM Products GROUP BY UserID;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "GROUP BY解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->groupBy.size() == 1 && selectNode->groupBy[0] == "UserID", "GROUP BY字段正确");
        }
    }
}

/**
 * @brief 测试HAVING解析
 */
void testParseHaving() {
    std::cout << "\n[测试11] HAVING解析测试" << std::endl;
    
    std::string sql = "SELECT UserID, COUNT(*) FROM Products GROUP BY UserID HAVING COUNT(*) > 2;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "HAVING解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->havingClause != nullptr, "HAVING条件存在");
        }
    }
}

/**
 * @brief 测试聚合函数解析
 */
void testParseAggregateFunctions() {
    std::cout << "\n[测试12] 聚合函数解析测试" << std::endl;
    
    std::string sql = "SELECT COUNT(*), SUM(Price), AVG(Price), MAX(Price), MIN(Price) FROM Products;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "聚合函数解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->selectFieldsNew.size() == 5, "聚合函数数量正确");
        }
    }
}

/**
 * @brief 测试JOIN解析
 */
void testParseJoin() {
    std::cout << "\n[测试13] JOIN解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users INNER JOIN Products ON Users.UserID = Products.UserID;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "JOIN解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->joins.size() == 1, "JOIN数量正确");
            if (selectNode->joins.size() >= 1) {
                TEST_ASSERT(selectNode->joins[0].joinType == "INNER", "JOIN类型正确");
            }
        }
    }
}

/**
 * @brief 测试NATURAL JOIN解析
 */
void testParseNaturalJoin() {
    std::cout << "\n[测试14] NATURAL JOIN解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users NATURAL JOIN Products;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "NATURAL JOIN解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->joins.size() == 1, "JOIN数量正确");
            if (selectNode->joins.size() >= 1) {
                TEST_ASSERT(selectNode->joins[0].joinType == "NATURAL", "NATURAL JOIN类型正确");
            }
        }
    }
}

/**
 * @brief 测试UNION解析
 */
void testParseUnion() {
    std::cout << "\n[测试15] UNION解析测试" << std::endl;
    
    std::string sql = "SELECT UserID FROM Users WHERE Age = 24 UNION SELECT UserID FROM Users WHERE Age = 27;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "UNION解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->unionQueries.size() == 1, "UNION子查询数量正确");
            TEST_ASSERT(selectNode->unionAll == false, "UNION（非ALL）正确");
        }
    }
}

/**
 * @brief 测试UNION ALL解析
 */
void testParseUnionAll() {
    std::cout << "\n[测试16] UNION ALL解析测试" << std::endl;
    
    std::string sql = "SELECT UserID FROM Users WHERE Age = 24 UNION ALL SELECT UserID FROM Users WHERE Age = 27;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "UNION ALL解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->unionQueries.size() == 1, "UNION子查询数量正确");
            TEST_ASSERT(selectNode->unionAll == true, "UNION ALL正确");
        }
    }
}

/**
 * @brief 测试子查询解析（标量子查询）
 */
void testParseSubqueryScalar() {
    std::cout << "\n[测试17] 标量子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE UserID = (SELECT UserID FROM Users WHERE UserName = 'John Doe');";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "标量子查询解析成功");
}

/**
 * @brief 测试子查询解析（IN子查询）
 */
void testParseSubqueryIn() {
    std::cout << "\n[测试18] IN子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE UserID IN (SELECT UserID FROM Products);";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "IN子查询解析成功");
}

/**
 * @brief 测试子查询解析（EXISTS子查询）
 */
void testParseSubqueryExists() {
    std::cout << "\n[测试19] EXISTS子查询解析测试" << std::endl;
    
    std::string sql = "SELECT * FROM Users WHERE EXISTS (SELECT * FROM Products WHERE Products.UserID = Users.UserID);";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "EXISTS子查询解析成功");
}

/**
 * @brief 测试组合功能解析
 */
void testParseCombined() {
    std::cout << "\n[测试20] 组合功能解析测试" << std::endl;
    
    std::string sql = "SELECT DISTINCT UserID, COUNT(*) FROM Products WHERE Price > 50 GROUP BY UserID HAVING COUNT(*) > 1 ORDER BY COUNT(*) DESC LIMIT 5;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "组合功能解析成功");
    
    if (node) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(node.get());
        TEST_ASSERT(selectNode != nullptr, "节点类型正确");
        if (selectNode) {
            TEST_ASSERT(selectNode->distinct == true, "DISTINCT正确");
            TEST_ASSERT(selectNode->groupBy.size() == 1, "GROUP BY正确");
            TEST_ASSERT(selectNode->havingClause != nullptr, "HAVING正确");
            TEST_ASSERT(selectNode->orderBy.size() == 1, "ORDER BY正确");
            TEST_ASSERT(selectNode->limitCount == 5, "LIMIT正确");
        }
    }
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SQL SELECT Parser Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testParseBasicSelect();
        testParseSelectDistinct();
        testParseOrderBy();
        testParseLimit();
        testParseWhereSimple();
        testParseWhereComplex();
        testParseLike();
        testParseIn();
        testParseBetween();
        testParseGroupBy();
        testParseHaving();
        testParseAggregateFunctions();
        testParseJoin();
        testParseNaturalJoin();
        testParseUnion();
        testParseUnionAll();
        testParseSubqueryScalar();
        testParseSubqueryIn();
        testParseSubqueryExists();
        testParseCombined();
        
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

