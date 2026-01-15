/**
 * @file parser_select.cpp
 * @brief SELECT语句解析实现
 */

#include "sql_parser/parser.h"

std::unique_ptr<ASTNode> Parser::parseSelect() {
    // SELECT * FROM TableName WHERE Field=Value;
    // SELECT Field1, Field2 FROM TableName WHERE Field=Value;
    // SELECT * FROM TableName1, TableName2 WHERE Condition;
    // SELECT DISTINCT * FROM TableName ORDER BY Field ASC LIMIT 10;
    auto node = std::make_unique<SelectNode>();
    
    // 已经匹配了SELECT，跳过
    advance();
    
    // DISTINCT（可选）
    if (m_currentToken.type == TokenType::DISTINCT) {
        node->distinct = true;
        advance();
    }
    
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
        setError("Expected field name or '*', but got: " + m_currentToken.value);
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
                setError("Expected '=', but got: " + m_currentToken.value);
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
        
        // 解析WHERE条件（支持复杂条件）
        node->whereClause = parseWhereCondition();
        if (!node->whereClause) {
            return nullptr;
        }
        
        // 向后兼容：如果whereClause是简单条件，也填充旧字段
        if (node->whereClause->isSimple()) {
            node->whereField = node->whereClause->fieldName;
            node->whereValue = node->whereClause->value;
            node->whereOperator = node->whereClause->operator_;
        }
    }
    
    // ORDER BY子句（可选）
    if (m_currentToken.type == TokenType::ORDER) {
        advance();
        if (!expect(TokenType::BY, "BY")) {
            return nullptr;
        }
        
        // 解析排序字段列表
        while (true) {
            OrderByInfo orderByInfo;
            orderByInfo.fieldName = parseIdentifier();
            if (orderByInfo.fieldName.empty()) {
                return nullptr;
            }
            
            // 解析排序方向（ASC或DESC，默认为ASC）
            if (m_currentToken.type == TokenType::ASC) {
                orderByInfo.direction = "ASC";
                advance();
            } else if (m_currentToken.type == TokenType::DESC) {
                orderByInfo.direction = "DESC";
                advance();
            }
            // 如果没有指定，默认为ASC（已在OrderByInfo构造函数中设置）
            
            node->orderBy.push_back(orderByInfo);
            
            // 检查是否有更多排序字段
            if (m_currentToken.type == TokenType::COMMA) {
                advance();
            } else {
                break;
            }
        }
    }
    
    // LIMIT子句（可选）
    if (m_currentToken.type == TokenType::LIMIT) {
        advance();
        if (m_currentToken.type == TokenType::NUMBER) {
            node->limitCount = std::stoi(m_currentToken.value);
            advance();
        } else {
            setError("Expected number after LIMIT, but got: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    // ;
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

