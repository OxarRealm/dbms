#pragma once

#include <string>

/**
 * @file token.h
 * @brief SQL词法分析 - Token定义
 *
 * 定义SQL语句的词法单元（Token）类型和结构
 */

/**
 * @brief Token类型枚举
 */
enum class TokenType {
    // 关键词
    CREATE, TABLE, EDIT, RENAME, DROP,
    INSERT, INTO, DELETE, FROM, UPDATE, SET,
    SELECT, WHERE,
    VALUES, IN, INTO_KEYWORD,
    JOIN, ON, INNER, LEFT, RIGHT, FULL, OUTER, NATURAL,
    ORDER, BY, ASC, DESC,
    DISTINCT,
    LIMIT,
    GROUP, HAVING,
    COUNT, SUM, AVG, MAX, MIN,
    AND, OR, NOT,
    LIKE, BETWEEN,
    UNION, ALL,
    EXISTS,
    
    // 数据类型
    INT, CHAR, FLOAT, DOUBLE, STRING,
    
    // 标志
    KEY, NOT_KEY,
    NULL_KEYWORD, NO_NULL,
    VALID, INVALID,
    
    // 标识符和字面量
    IDENTIFIER,        // 标识符（表名、字段名等）
    STRING_LITERAL,    // 字符串字面量
    NUMBER,            // 数字
    
    // 运算符和分隔符
    EQUALS,            // =
    NOT_EQUALS,        // !=
    GREATER_THAN,      // >
    LESS_THAN,         // <
    GREATER_EQUAL,     // >=
    LESS_EQUAL,        // <=
    COMMA,             // ,
    SEMICOLON,         // ;
    LEFT_PAREN,        // (
    RIGHT_PAREN,       // )
    LEFT_BRACKET,      // [
    RIGHT_BRACKET,     // ]
    ASTERISK,          // *
    DOT,               // .
    
    // 特殊
    EOF_TOKEN,         // 文件结束
    ERROR              // 错误Token
};

/**
 * @brief Token结构
 */
struct Token {
    TokenType type;        // Token类型
    std::string value;     // Token值（原始字符串）
    size_t line;           // 所在行号（用于错误报告）
    size_t column;         // 所在列号（用于错误报告）
    
    Token() : type(TokenType::ERROR), line(0), column(0) {}
    
    Token(TokenType t, const std::string& v, size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    /**
     * @brief 获取Token类型名称（用于调试）
     */
    std::string getTypeName() const;
    
    /**
     * @brief 检查Token是否为关键词
     */
    bool isKeyword() const;
};

/**
 * @brief 关键词字符串到TokenType的映射
 */
TokenType keywordToTokenType(const std::string& keyword);

