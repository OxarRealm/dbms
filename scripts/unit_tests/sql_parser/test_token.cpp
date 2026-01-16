/**
 * @file test_token.cpp
 * @brief Token模块测试程序
 * 
 * 测试Token类的基础功能：
 * - Token类型枚举完整性
 * - getTypeName()方法正确性
 * - keywordToTokenType()映射正确性
 * - isKeyword()方法正确性
 */

#include "sql_parser/token.h"
#include <iostream>
#include <vector>
#include <string>

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
 * @brief 测试getTypeName()方法
 */
void testGetTypeName() {
    std::cout << "\n[测试1] getTypeName()方法测试" << std::endl;
    
    // 测试所有主要Token类型
    Token token1(TokenType::CREATE, "CREATE");
    TEST_ASSERT(token1.getTypeName() == "CREATE", "CREATE类型名称");
    
    Token token2(TokenType::SELECT, "SELECT");
    TEST_ASSERT(token2.getTypeName() == "SELECT", "SELECT类型名称");
    
    Token token3(TokenType::IDENTIFIER, "TableName");
    TEST_ASSERT(token3.getTypeName() == "IDENTIFIER", "IDENTIFIER类型名称");
    
    Token token4(TokenType::STRING_LITERAL, "'value'");
    TEST_ASSERT(token4.getTypeName() == "STRING_LITERAL", "STRING_LITERAL类型名称");
    
    Token token5(TokenType::NUMBER, "123");
    TEST_ASSERT(token5.getTypeName() == "NUMBER", "NUMBER类型名称");
    
    Token token6(TokenType::EQUALS, "=");
    TEST_ASSERT(token6.getTypeName() == "EQUALS", "EQUALS类型名称");
    
    Token token7(TokenType::JOIN, "JOIN");
    TEST_ASSERT(token7.getTypeName() == "JOIN", "JOIN类型名称");
    
    Token token8(TokenType::ORDER, "ORDER");
    TEST_ASSERT(token8.getTypeName() == "ORDER", "ORDER类型名称");
    
    Token token9(TokenType::GROUP, "GROUP");
    TEST_ASSERT(token9.getTypeName() == "GROUP", "GROUP类型名称");
    
    Token token10(TokenType::COUNT, "COUNT");
    TEST_ASSERT(token10.getTypeName() == "COUNT", "COUNT类型名称");
    
    Token token11(TokenType::UNION, "UNION");
    TEST_ASSERT(token11.getTypeName() == "UNION", "UNION类型名称");
    
    Token token12(TokenType::EXISTS, "EXISTS");
    TEST_ASSERT(token12.getTypeName() == "EXISTS", "EXISTS类型名称");
    
    Token token13(TokenType::EOF_TOKEN, "");
    TEST_ASSERT(token13.getTypeName() == "EOF", "EOF类型名称");
    
    Token token14(TokenType::ERROR, "");
    TEST_ASSERT(token14.getTypeName() == "ERROR", "ERROR类型名称");
}

/**
 * @brief 测试keywordToTokenType()映射
 */
void testKeywordToTokenType() {
    std::cout << "\n[测试2] keywordToTokenType()映射测试" << std::endl;
    
    // 测试DDL关键词
    TEST_ASSERT(keywordToTokenType("CREATE") == TokenType::CREATE, "CREATE关键词映射");
    TEST_ASSERT(keywordToTokenType("TABLE") == TokenType::TABLE, "TABLE关键词映射");
    TEST_ASSERT(keywordToTokenType("EDIT") == TokenType::EDIT, "EDIT关键词映射");
    TEST_ASSERT(keywordToTokenType("RENAME") == TokenType::RENAME, "RENAME关键词映射");
    TEST_ASSERT(keywordToTokenType("DROP") == TokenType::DROP, "DROP关键词映射");
    
    // 测试DML关键词
    TEST_ASSERT(keywordToTokenType("INSERT") == TokenType::INSERT, "INSERT关键词映射");
    TEST_ASSERT(keywordToTokenType("INTO") == TokenType::INTO, "INTO关键词映射");
    TEST_ASSERT(keywordToTokenType("DELETE") == TokenType::DELETE, "DELETE关键词映射");
    TEST_ASSERT(keywordToTokenType("FROM") == TokenType::FROM, "FROM关键词映射");
    TEST_ASSERT(keywordToTokenType("UPDATE") == TokenType::UPDATE, "UPDATE关键词映射");
    TEST_ASSERT(keywordToTokenType("SET") == TokenType::SET, "SET关键词映射");
    
    // 测试查询关键词
    TEST_ASSERT(keywordToTokenType("SELECT") == TokenType::SELECT, "SELECT关键词映射");
    TEST_ASSERT(keywordToTokenType("WHERE") == TokenType::WHERE, "WHERE关键词映射");
    TEST_ASSERT(keywordToTokenType("JOIN") == TokenType::JOIN, "JOIN关键词映射");
    TEST_ASSERT(keywordToTokenType("INNER") == TokenType::INNER, "INNER关键词映射");
    TEST_ASSERT(keywordToTokenType("LEFT") == TokenType::LEFT, "LEFT关键词映射");
    TEST_ASSERT(keywordToTokenType("RIGHT") == TokenType::RIGHT, "RIGHT关键词映射");
    TEST_ASSERT(keywordToTokenType("FULL") == TokenType::FULL, "FULL关键词映射");
    TEST_ASSERT(keywordToTokenType("OUTER") == TokenType::OUTER, "OUTER关键词映射");
    TEST_ASSERT(keywordToTokenType("NATURAL") == TokenType::NATURAL, "NATURAL关键词映射");
    
    // 测试排序和分组关键词
    TEST_ASSERT(keywordToTokenType("ORDER") == TokenType::ORDER, "ORDER关键词映射");
    TEST_ASSERT(keywordToTokenType("BY") == TokenType::BY, "BY关键词映射");
    TEST_ASSERT(keywordToTokenType("ASC") == TokenType::ASC, "ASC关键词映射");
    TEST_ASSERT(keywordToTokenType("DESC") == TokenType::DESC, "DESC关键词映射");
    TEST_ASSERT(keywordToTokenType("DISTINCT") == TokenType::DISTINCT, "DISTINCT关键词映射");
    TEST_ASSERT(keywordToTokenType("LIMIT") == TokenType::LIMIT, "LIMIT关键词映射");
    TEST_ASSERT(keywordToTokenType("GROUP") == TokenType::GROUP, "GROUP关键词映射");
    TEST_ASSERT(keywordToTokenType("HAVING") == TokenType::HAVING, "HAVING关键词映射");
    
    // 测试聚合函数关键词
    TEST_ASSERT(keywordToTokenType("COUNT") == TokenType::COUNT, "COUNT关键词映射");
    TEST_ASSERT(keywordToTokenType("SUM") == TokenType::SUM, "SUM关键词映射");
    TEST_ASSERT(keywordToTokenType("AVG") == TokenType::AVG, "AVG关键词映射");
    TEST_ASSERT(keywordToTokenType("MAX") == TokenType::MAX, "MAX关键词映射");
    TEST_ASSERT(keywordToTokenType("MIN") == TokenType::MIN, "MIN关键词映射");
    
    // 测试逻辑运算符关键词
    TEST_ASSERT(keywordToTokenType("AND") == TokenType::AND, "AND关键词映射");
    TEST_ASSERT(keywordToTokenType("OR") == TokenType::OR, "OR关键词映射");
    TEST_ASSERT(keywordToTokenType("NOT") == TokenType::NOT, "NOT关键词映射");
    TEST_ASSERT(keywordToTokenType("LIKE") == TokenType::LIKE, "LIKE关键词映射");
    TEST_ASSERT(keywordToTokenType("BETWEEN") == TokenType::BETWEEN, "BETWEEN关键词映射");
    TEST_ASSERT(keywordToTokenType("IN") == TokenType::IN, "IN关键词映射");
    
    // 测试UNION和子查询关键词
    TEST_ASSERT(keywordToTokenType("UNION") == TokenType::UNION, "UNION关键词映射");
    TEST_ASSERT(keywordToTokenType("ALL") == TokenType::ALL, "ALL关键词映射");
    TEST_ASSERT(keywordToTokenType("EXISTS") == TokenType::EXISTS, "EXISTS关键词映射");
    
    // 测试数据类型关键词
    TEST_ASSERT(keywordToTokenType("INT") == TokenType::INT, "INT关键词映射");
    TEST_ASSERT(keywordToTokenType("CHAR") == TokenType::CHAR, "CHAR关键词映射");
    TEST_ASSERT(keywordToTokenType("FLOAT") == TokenType::FLOAT, "FLOAT关键词映射");
    TEST_ASSERT(keywordToTokenType("DOUBLE") == TokenType::DOUBLE, "DOUBLE关键词映射");
    TEST_ASSERT(keywordToTokenType("STRING") == TokenType::STRING, "STRING关键词映射");
    
    // 测试标志关键词
    TEST_ASSERT(keywordToTokenType("KEY") == TokenType::KEY, "KEY关键词映射");
    TEST_ASSERT(keywordToTokenType("NOT_KEY") == TokenType::NOT_KEY, "NOT_KEY关键词映射");
    TEST_ASSERT(keywordToTokenType("NULL") == TokenType::NULL_KEYWORD, "NULL关键词映射");
    TEST_ASSERT(keywordToTokenType("NO_NULL") == TokenType::NO_NULL, "NO_NULL关键词映射");
    TEST_ASSERT(keywordToTokenType("VALID") == TokenType::VALID, "VALID关键词映射");
    TEST_ASSERT(keywordToTokenType("INVALID") == TokenType::INVALID, "INVALID关键词映射");
    
    // 测试大小写不敏感
    TEST_ASSERT(keywordToTokenType("create") == TokenType::CREATE, "create（小写）关键词映射");
    TEST_ASSERT(keywordToTokenType("Create") == TokenType::CREATE, "Create（混合大小写）关键词映射");
    TEST_ASSERT(keywordToTokenType("SELECT") == TokenType::SELECT, "SELECT（大写）关键词映射");
    
    // 测试非关键词（应返回IDENTIFIER）
    TEST_ASSERT(keywordToTokenType("TableName") == TokenType::IDENTIFIER, "非关键词返回IDENTIFIER");
    TEST_ASSERT(keywordToTokenType("FieldName") == TokenType::IDENTIFIER, "非关键词返回IDENTIFIER");
}

/**
 * @brief 测试isKeyword()方法
 */
void testIsKeyword() {
    std::cout << "\n[测试3] isKeyword()方法测试" << std::endl;
    
    // 测试关键词
    Token token1(TokenType::CREATE, "CREATE");
    TEST_ASSERT(token1.isKeyword() == true, "CREATE是关键词");
    
    Token token2(TokenType::SELECT, "SELECT");
    TEST_ASSERT(token2.isKeyword() == true, "SELECT是关键词");
    
    Token token3(TokenType::JOIN, "JOIN");
    TEST_ASSERT(token3.isKeyword() == true, "JOIN是关键词");
    
    Token token4(TokenType::COUNT, "COUNT");
    TEST_ASSERT(token4.isKeyword() == true, "COUNT是关键词");
    
    // 测试非关键词
    Token token5(TokenType::IDENTIFIER, "TableName");
    TEST_ASSERT(token5.isKeyword() == false, "IDENTIFIER不是关键词");
    
    Token token6(TokenType::STRING_LITERAL, "'value'");
    TEST_ASSERT(token6.isKeyword() == false, "STRING_LITERAL不是关键词");
    
    Token token7(TokenType::NUMBER, "123");
    TEST_ASSERT(token7.isKeyword() == false, "NUMBER不是关键词");
    
    Token token8(TokenType::EQUALS, "=");
    TEST_ASSERT(token8.isKeyword() == false, "EQUALS不是关键词");
}

/**
 * @brief 测试Token构造函数
 */
void testTokenConstructor() {
    std::cout << "\n[测试4] Token构造函数测试" << std::endl;
    
    // 测试默认构造函数
    Token token1;
    TEST_ASSERT(token1.type == TokenType::ERROR, "默认构造函数type为ERROR");
    TEST_ASSERT(token1.value == "", "默认构造函数value为空");
    TEST_ASSERT(token1.line == 0, "默认构造函数line为0");
    TEST_ASSERT(token1.column == 0, "默认构造函数column为0");
    
    // 测试带参数的构造函数
    Token token2(TokenType::SELECT, "SELECT", 10, 5);
    TEST_ASSERT(token2.type == TokenType::SELECT, "带参数构造函数type正确");
    TEST_ASSERT(token2.value == "SELECT", "带参数构造函数value正确");
    TEST_ASSERT(token2.line == 10, "带参数构造函数line正确");
    TEST_ASSERT(token2.column == 5, "带参数构造函数column正确");
    
    // 测试不带行号和列号的构造函数
    Token token3(TokenType::IDENTIFIER, "TableName");
    TEST_ASSERT(token3.type == TokenType::IDENTIFIER, "不带行号列号构造函数type正确");
    TEST_ASSERT(token3.value == "TableName", "不带行号列号构造函数value正确");
    TEST_ASSERT(token3.line == 0, "不带行号列号构造函数line为0");
    TEST_ASSERT(token3.column == 0, "不带行号列号构造函数column为0");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Token Module Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 运行所有测试
    testGetTypeName();
    testKeywordToTokenType();
    testIsKeyword();
    testTokenConstructor();
    
    // 输出测试结果
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total tests: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "Passed: " << testsPassed << std::endl;
    std::cout << "Failed: " << testsFailed << std::endl;
    
    if (testsFailed == 0) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Some tests failed!" << std::endl;
        return 1;
    }
}

