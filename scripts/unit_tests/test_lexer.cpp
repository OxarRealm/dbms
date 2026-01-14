/**
 * @file test_lexer.cpp
 * @brief SQL词法分析器测试程序
 * 
 * 测试Lexer类的基础功能
 */

#include "../../include/sql_parser/lexer.h"
#include "../../include/sql_parser/token.h"
#include <iostream>
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

/**
 * @brief 测试关键词识别
 */
void testKeywords() {
    std::cout << "\n[测试1] 关键词识别测试" << std::endl;
    
    Lexer lexer("CREATE TABLE EDIT RENAME DROP");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    Token token3 = lexer.nextToken();
    Token token4 = lexer.nextToken();
    Token token5 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::CREATE, "CREATE关键词");
    TEST_ASSERT(token2.type == TokenType::TABLE, "TABLE关键词");
    TEST_ASSERT(token3.type == TokenType::EDIT, "EDIT关键词");
    TEST_ASSERT(token4.type == TokenType::RENAME, "RENAME关键词");
    TEST_ASSERT(token5.type == TokenType::DROP, "DROP关键词");
}

/**
 * @brief 测试标识符识别
 */
void testIdentifiers() {
    std::cout << "\n[测试2] 标识符识别测试" << std::endl;
    
    Lexer lexer("TableName FieldName database_file");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    Token token3 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::IDENTIFIER, "标识符1类型");
    TEST_ASSERT(token1.value == "TableName", "标识符1值");
    TEST_ASSERT(token2.type == TokenType::IDENTIFIER, "标识符2类型");
    TEST_ASSERT(token2.value == "FieldName", "标识符2值");
    TEST_ASSERT(token3.type == TokenType::IDENTIFIER, "标识符3类型");
    TEST_ASSERT(token3.value == "database_file", "标识符3值");
}

/**
 * @brief 测试数字识别
 */
void testNumbers() {
    std::cout << "\n[测试3] 数字识别测试" << std::endl;
    
    Lexer lexer("123 456.789 0");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    Token token3 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::NUMBER, "整数类型");
    TEST_ASSERT(token1.value == "123", "整数值");
    TEST_ASSERT(token2.type == TokenType::NUMBER, "浮点数类型");
    TEST_ASSERT(token2.value == "456.789", "浮点数值");
    TEST_ASSERT(token3.type == TokenType::NUMBER, "零类型");
    TEST_ASSERT(token3.value == "0", "零值");
}

/**
 * @brief 测试字符串字面量
 */
void testStringLiterals() {
    std::cout << "\n[测试4] 字符串字面量测试" << std::endl;
    
    Lexer lexer("'hello' \"world\" 'test string'");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    Token token3 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::STRING_LITERAL, "字符串1类型");
    TEST_ASSERT(token1.value == "hello", "字符串1值");
    TEST_ASSERT(token2.type == TokenType::STRING_LITERAL, "字符串2类型");
    TEST_ASSERT(token2.value == "world", "字符串2值");
    TEST_ASSERT(token3.type == TokenType::STRING_LITERAL, "字符串3类型");
    TEST_ASSERT(token3.value == "test string", "字符串3值");
}

/**
 * @brief 测试运算符和分隔符
 */
void testOperators() {
    std::cout << "\n[测试5] 运算符和分隔符测试" << std::endl;
    
    Lexer lexer("= , ; ( )");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    Token token3 = lexer.nextToken();
    Token token4 = lexer.nextToken();
    Token token5 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::EQUALS, "EQUALS运算符");
    TEST_ASSERT(token2.type == TokenType::COMMA, "COMMA分隔符");
    TEST_ASSERT(token3.type == TokenType::SEMICOLON, "SEMICOLON分隔符");
    TEST_ASSERT(token4.type == TokenType::LEFT_PAREN, "LEFT_PAREN");
    TEST_ASSERT(token5.type == TokenType::RIGHT_PAREN, "RIGHT_PAREN");
}

/**
 * @brief 测试简单SQL语句
 */
void testSimpleSQL() {
    std::cout << "\n[测试6] 简单SQL语句测试" << std::endl;
    
    Lexer lexer("CREATE TABLE Users ( UserID int KEY NO_NULL VALID ) INTO MusicDB;");
    std::vector<Token> tokens;
    
    Token token;
    do {
        token = lexer.nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::EOF_TOKEN && token.type != TokenType::ERROR);
    
    TEST_ASSERT(tokens.size() >= 10, "Token数量");
    TEST_ASSERT(tokens[0].type == TokenType::CREATE, "CREATE Token");
    TEST_ASSERT(tokens[1].type == TokenType::TABLE, "TABLE Token");
    TEST_ASSERT(tokens[2].type == TokenType::IDENTIFIER, "表名Token");
    TEST_ASSERT(tokens[2].value == "Users", "表名值");
}

/**
 * @brief 测试空白字符处理
 */
void testWhitespace() {
    std::cout << "\n[测试7] 空白字符处理测试" << std::endl;
    
    Lexer lexer("   CREATE   TABLE   \n   Users   ");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    Token token3 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::CREATE, "CREATE（跳过空白）");
    TEST_ASSERT(token2.type == TokenType::TABLE, "TABLE（跳过空白）");
    TEST_ASSERT(token3.type == TokenType::IDENTIFIER, "Users（跳过换行）");
    TEST_ASSERT(token3.value == "Users", "Users值");
}

/**
 * @brief 测试EOF处理
 */
void testEOF() {
    std::cout << "\n[测试8] EOF处理测试" << std::endl;
    
    Lexer lexer("CREATE");
    Token token1 = lexer.nextToken();
    Token token2 = lexer.nextToken();
    
    TEST_ASSERT(token1.type == TokenType::CREATE, "CREATE Token");
    TEST_ASSERT(token2.type == TokenType::EOF_TOKEN, "EOF Token");
}

/**
 * @brief 测试peekToken功能
 */
void testPeekToken() {
    std::cout << "\n[测试9] PeekToken测试" << std::endl;
    
    Lexer lexer("CREATE TABLE");
    Token peek1 = lexer.peekToken();
    Token token1 = lexer.nextToken();
    Token peek2 = lexer.peekToken();
    Token token2 = lexer.nextToken();
    
    TEST_ASSERT(peek1.type == TokenType::CREATE, "Peek CREATE");
    TEST_ASSERT(token1.type == TokenType::CREATE, "Read CREATE");
    TEST_ASSERT(peek2.type == TokenType::TABLE, "Peek TABLE");
    TEST_ASSERT(token2.type == TokenType::TABLE, "Read TABLE");
    TEST_ASSERT(peek1.value == token1.value, "Peek和Read值一致");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SQL词法分析器测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testKeywords();
        testIdentifiers();
        testNumbers();
        testStringLiterals();
        testOperators();
        testSimpleSQL();
        testWhitespace();
        testEOF();
        testPeekToken();
        
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

