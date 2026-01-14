#pragma once

#include "sql_parser/token.h"
#include <string>
#include <istream>

/**
 * @file lexer.h
 * @brief SQL词法分析器
 *
 * 负责将SQL语句字符串分解为Token序列
 */

/**
 * @brief SQL词法分析器类
 */
class Lexer {
public:
    Lexer();
    explicit Lexer(const std::string& sql);
    
    /**
     * @brief 设置要分析的SQL语句
     */
    void setInput(const std::string& sql);
    
    /**
     * @brief 获取下一个Token
     * @return 下一个Token
     */
    Token nextToken();
    
    /**
     * @brief 获取当前Token（不移动指针）
     */
    Token peekToken();
    
    /**
     * @brief 检查是否还有更多Token
     */
    bool hasMoreTokens() const;
    
    /**
     * @brief 获取当前位置
     */
    size_t getPosition() const;
    
    /**
     * @brief 重置词法分析器
     */
    void reset();
    
private:
    std::string m_input;       // 输入SQL语句
    size_t m_position;         // 当前位置
    size_t m_line;             // 当前行号
    size_t m_column;           // 当前列号
    
    /**
     * @brief 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * @brief 跳过注释（如果需要）
     */
    void skipComment();
    
    /**
     * @brief 读取标识符或关键词
     */
    Token readIdentifierOrKeyword();
    
    /**
     * @brief 读取数字
     */
    Token readNumber();
    
    /**
     * @brief 读取字符串字面量
     */
    Token readStringLiteral();
    
    /**
     * @brief 读取当前字符（不移动指针）
     */
    char peekChar() const;
    
    /**
     * @brief 读取当前字符并移动指针
     */
    char readChar();
    
    /**
     * @brief 检查是否到达字符串末尾
     */
    bool isEOF() const;
};

