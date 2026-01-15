#pragma once

#include "sql_parser/lexer.h"
#include "sql_parser/ast_node.h"
#include <memory>
#include <string>

/**
 * @file parser.h
 * @brief SQL语法分析器
 *
 * 负责将Token序列解析为抽象语法树（AST）
 */

/**
 * @brief SQL语法分析器类
 */
class Parser {
public:
    Parser();
    explicit Parser(const std::string& sql);
    
    /**
     * @brief 设置要解析的SQL语句
     */
    void setInput(const std::string& sql);
    
    /**
     * @brief 解析SQL语句，返回AST节点
     * @return AST节点指针（如果解析失败返回nullptr）
     */
    std::unique_ptr<ASTNode> parse();
    
    /**
     * @brief 获取最后的错误信息
     */
    std::string getLastError() const;
    
private:
    Lexer m_lexer;              // 词法分析器
    Token m_currentToken;       // 当前Token
    std::string m_lastError;    // 最后的错误信息
    
    /**
     * @brief 移动到下一个Token
     */
    void advance();
    
    /**
     * @brief 检查当前Token类型是否匹配
     */
    bool match(TokenType type);
    
    /**
     * @brief 期望当前Token为指定类型，否则报错
     */
    bool expect(TokenType type, const std::string& expected);
    
    /**
     * @brief 解析DDL语句
     */
    std::unique_ptr<ASTNode> parseDDL();
    
    /**
     * @brief 解析CREATE TABLE语句
     */
    std::unique_ptr<ASTNode> parseCreateTable();
    
    /**
     * @brief 解析EDIT TABLE语句
     */
    std::unique_ptr<ASTNode> parseEditTable();
    
    /**
     * @brief 解析RENAME TABLE语句
     */
    std::unique_ptr<ASTNode> parseRenameTable();
    
    /**
     * @brief 解析DROP TABLE语句
     */
    std::unique_ptr<ASTNode> parseDropTable();
    
    /**
     * @brief 解析INSERT语句
     */
    std::unique_ptr<ASTNode> parseInsert();
    
    /**
     * @brief 解析DELETE语句
     */
    std::unique_ptr<ASTNode> parseDelete();
    
    /**
     * @brief 解析UPDATE语句
     */
    std::unique_ptr<ASTNode> parseUpdate();
    
    /**
     * @brief 解析SELECT语句
     */
    std::unique_ptr<ASTNode> parseSelect();
    
    /**
     * @brief 解析WHERE条件（支持复杂条件）
     * @return WHERE条件节点（如果解析失败返回nullptr）
     */
    std::unique_ptr<WhereCondition> parseWhereCondition();
    
    /**
     * @brief 解析OR表达式
     */
    std::unique_ptr<WhereCondition> parseOrExpression();
    
    /**
     * @brief 解析AND表达式
     */
    std::unique_ptr<WhereCondition> parseAndExpression();
    
    /**
     * @brief 解析NOT表达式
     */
    std::unique_ptr<WhereCondition> parseNotExpression();
    
    /**
     * @brief 解析简单表达式（括号或简单条件）
     */
    std::unique_ptr<WhereCondition> parseSimpleExpression();
    
    /**
     * @brief 解析简单WHERE条件（Field Operator Value）
     * @return WHERE条件节点（如果解析失败返回nullptr）
     */
    std::unique_ptr<WhereCondition> parseSimpleCondition();
    
    /**
     * @brief 解析字段定义（用于CREATE TABLE）
     */
    bool parseFieldDefinition(TableMode& field);
    
    /**
     * @brief 解析单个字段（用于EDIT TABLE）
     */
    bool parseField(TableMode& field);
    
    /**
     * @brief 解析数据类型
     */
    bool parseDataType(TableMode& field);
    
    /**
     * @brief 解析标志（KEY, NULL, VALID）
     */
    bool parseFlags(TableMode& field);
    
    /**
     * @brief 解析标识符（表名、字段名等）
     */
    std::string parseIdentifier();
    
    /**
     * @brief 解析数据库文件名（INTO/IN DatabaseFileName）
     */
    std::string parseDatabaseFileName();
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
};

