/**
 * @file parser_select.cpp
 * @brief SELECT语句解析实现
 */

#include "sql_parser/parser.h"

std::unique_ptr<ASTNode> Parser::parseSelect() {
    // SELECT * FROM TableName WHERE Field=Value;
    // SELECT Field1, Field2 FROM TableName WHERE Field=Value;
    // SELECT * FROM TableName1, TableName2 WHERE Condition;
    auto node = std::make_unique<SelectNode>();
    
    // 已经匹配了SELECT，跳过
    advance();
    
    // 解析SELECT字段列表
    if (m_currentToken.type == TokenType::ASTERISK) {
        // SELECT * 表示选择所有字段
        node->selectFields.push_back("*");
        advance();
    } else if (m_currentToken.type == TokenType::IDENTIFIER) {
        // 字段列表：Field1, Field2, ...
        while (true) {
            std::string fieldName = parseIdentifier();
            if (fieldName.empty()) {
                return nullptr;
            }
            node->selectFields.push_back(fieldName);
            
            if (m_currentToken.type == TokenType::COMMA) {
                advance();
            } else {
                break;
            }
        }
    } else {
        setError("期望字段名或*，但得到: " + m_currentToken.value);
        return nullptr;
    }
    
    // FROM
    if (!expect(TokenType::FROM, "FROM")) {
        return nullptr;
    }
    
    // 解析FROM表列表（支持JOIN语法）
    // 格式1：FROM Table1, Table2, ...（多表查询）
    // 格式2：FROM Table1 JOIN Table2 ON ...（JOIN查询）
    std::string firstTable = parseIdentifier();
    if (firstTable.empty()) {
        return nullptr;
    }
    node->fromTables.push_back(firstTable);
    
    // 检查是否有JOIN
    if (m_currentToken.type == TokenType::JOIN || 
        m_currentToken.type == TokenType::INNER ||
        m_currentToken.type == TokenType::LEFT ||
        m_currentToken.type == TokenType::RIGHT) {
        
        // 解析JOIN子句
        while (true) {
            JoinInfo joinInfo;
            
            // 解析JOIN类型（INNER, LEFT, RIGHT）
            if (m_currentToken.type == TokenType::INNER) {
                joinInfo.joinType = "INNER";
                advance();
                if (!expect(TokenType::JOIN, "JOIN")) {
                    return nullptr;
                }
            } else if (m_currentToken.type == TokenType::LEFT) {
                joinInfo.joinType = "LEFT";
                advance();
                if (!expect(TokenType::JOIN, "JOIN")) {
                    return nullptr;
                }
            } else if (m_currentToken.type == TokenType::RIGHT) {
                joinInfo.joinType = "RIGHT";
                advance();
                if (!expect(TokenType::JOIN, "JOIN")) {
                    return nullptr;
                }
            } else if (m_currentToken.type == TokenType::JOIN) {
                // 默认INNER JOIN
                joinInfo.joinType = "INNER";
                advance();
            } else {
                break;  // 没有更多JOIN
            }
            
            // 解析右表名
            joinInfo.rightTable = parseIdentifier();
            if (joinInfo.rightTable.empty()) {
                return nullptr;
            }
            node->fromTables.push_back(joinInfo.rightTable);
            
            // 解析ON子句
            if (!expect(TokenType::ON, "ON")) {
                return nullptr;
            }
            
            // 解析左表字段
            joinInfo.leftField = parseIdentifier();
            if (joinInfo.leftField.empty()) {
                return nullptr;
            }
            
            // 解析运算符（当前只支持=）
            if (m_currentToken.type == TokenType::EQUALS) {
                joinInfo.operator_ = "=";
                advance();
            } else {
                setError("期望 =，但得到: " + m_currentToken.value);
                return nullptr;
            }
            
            // 解析右表字段
            joinInfo.rightField = parseIdentifier();
            if (joinInfo.rightField.empty()) {
                return nullptr;
            }
            
            node->joins.push_back(joinInfo);
            
            // 检查是否有更多JOIN
            if (m_currentToken.type != TokenType::JOIN &&
                m_currentToken.type != TokenType::INNER &&
                m_currentToken.type != TokenType::LEFT &&
                m_currentToken.type != TokenType::RIGHT) {
                break;
            }
        }
    } else {
        // 多表查询（逗号分隔）
        while (m_currentToken.type == TokenType::COMMA) {
            advance();
            std::string tableName = parseIdentifier();
            if (tableName.empty()) {
                return nullptr;
            }
            node->fromTables.push_back(tableName);
        }
    }
    
    // WHERE子句（可选）
    if (m_currentToken.type == TokenType::WHERE) {
        advance();
        
        // Field
        node->whereField = parseIdentifier();
        if (node->whereField.empty()) {
            return nullptr;
        }
        
        // 运算符（当前只支持=）
        if (m_currentToken.type == TokenType::EQUALS) {
            node->whereOperator = "=";
            advance();
        } else {
            setError("期望 =，但得到: " + m_currentToken.value);
            return nullptr;
        }
        
        // Value
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            node->whereValue = m_currentToken.value;
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            node->whereValue = m_currentToken.value;
            advance();
        } else {
            setError("期望值（字符串或数字），但得到: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    // ;
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

