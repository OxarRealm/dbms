/**
 * @file parser_where.cpp
 * @brief WHERE条件解析实现
 */

#include "sql_parser/parser.h"

std::unique_ptr<WhereCondition> Parser::parseWhereCondition() {
    // 解析WHERE条件（支持AND, OR, NOT, 括号）
    // 优先级：NOT > AND > OR
    // 使用递归下降解析
    
    // 先解析OR表达式（最低优先级）
    return parseOrExpression();
}

std::unique_ptr<WhereCondition> Parser::parseOrExpression() {
    // 解析OR表达式：ANDExpr OR ANDExpr OR ...
    auto left = parseAndExpression();
    if (!left) {
        return nullptr;
    }
    
    while (m_currentToken.type == TokenType::OR) {
        advance();
        auto right = parseAndExpression();
        if (!right) {
            return nullptr;
        }
        
        auto orNode = std::make_unique<WhereCondition>();
        orNode->logicalOp = "OR";
        orNode->left = std::move(left);
        orNode->right = std::move(right);
        left = std::move(orNode);
    }
    
    return left;
}

std::unique_ptr<WhereCondition> Parser::parseAndExpression() {
    // 解析AND表达式：NOTExpr AND NOTExpr AND ...
    auto left = parseNotExpression();
    if (!left) {
        return nullptr;
    }
    
    while (m_currentToken.type == TokenType::AND) {
        advance();
        auto right = parseNotExpression();
        if (!right) {
            return nullptr;
        }
        
        auto andNode = std::make_unique<WhereCondition>();
        andNode->logicalOp = "AND";
        andNode->left = std::move(left);
        andNode->right = std::move(right);
        left = std::move(andNode);
    }
    
    return left;
}

std::unique_ptr<WhereCondition> Parser::parseNotExpression() {
    // 解析NOT表达式：NOT SimpleExpr 或 SimpleExpr
    if (m_currentToken.type == TokenType::NOT) {
        advance();
        auto expr = parseNotExpression();  // 递归解析（支持NOT NOT）
        if (!expr) {
            return nullptr;
        }
        
        auto notNode = std::make_unique<WhereCondition>();
        notNode->logicalOp = "NOT";
        notNode->left = std::move(expr);
        return notNode;
    }
    
    return parseSimpleExpression();
}

std::unique_ptr<WhereCondition> Parser::parseSimpleExpression() {
    // 解析简单表达式：SimpleCondition 或 (WhereCondition)
    
    // 检查是否有括号
    if (m_currentToken.type == TokenType::LEFT_PAREN) {
        advance();
        auto condition = parseWhereCondition();
        if (!condition) {
            return nullptr;
        }
        if (!expect(TokenType::RIGHT_PAREN, ")")) {
            return nullptr;
        }
        return condition;
    }
    
    // 解析简单条件
    return parseSimpleCondition();
}

std::unique_ptr<WhereCondition> Parser::parseSimpleCondition() {
    // 解析简单条件：Field Operator Value
    // Field可以是普通字段名或聚合函数（如COUNT(*), SUM(Age)等）
    auto condition = std::make_unique<WhereCondition>();
    
    // Field - 检查是否为聚合函数
    if (m_currentToken.type == TokenType::COUNT ||
        m_currentToken.type == TokenType::SUM ||
        m_currentToken.type == TokenType::AVG ||
        m_currentToken.type == TokenType::MAX ||
        m_currentToken.type == TokenType::MIN) {
        // 解析聚合函数：COUNT(*), COUNT(Field), SUM(Field)等
        std::string funcName;
        if (m_currentToken.type == TokenType::COUNT) {
            funcName = "COUNT";
        } else if (m_currentToken.type == TokenType::SUM) {
            funcName = "SUM";
        } else if (m_currentToken.type == TokenType::AVG) {
            funcName = "AVG";
        } else if (m_currentToken.type == TokenType::MAX) {
            funcName = "MAX";
        } else if (m_currentToken.type == TokenType::MIN) {
            funcName = "MIN";
        }
        advance();
        
        // 解析聚合函数的参数
        if (!expect(TokenType::LEFT_PAREN, "(")) {
            return nullptr;
        }
        
        // 检查是否为COUNT(*)
        if (m_currentToken.type == TokenType::ASTERISK) {
            condition->fieldName = funcName + "(*)";
            advance();
        } else {
            // 解析字段名
            std::string fieldName = parseIdentifier();
            if (fieldName.empty()) {
                return nullptr;
            }
            condition->fieldName = funcName + "(" + fieldName + ")";
        }
        
        if (!expect(TokenType::RIGHT_PAREN, ")")) {
            return nullptr;
        }
    } else {
        // 普通字段名
        condition->fieldName = parseIdentifier();
        if (condition->fieldName.empty()) {
            return nullptr;
        }
    }
    
    // Operator
    if (m_currentToken.type == TokenType::EQUALS) {
        condition->operator_ = "=";
        advance();
    } else if (m_currentToken.type == TokenType::NOT_EQUALS) {
        condition->operator_ = "!=";
        advance();
    } else if (m_currentToken.type == TokenType::GREATER_THAN) {
        condition->operator_ = ">";
        advance();
    } else if (m_currentToken.type == TokenType::LESS_THAN) {
        condition->operator_ = "<";
        advance();
    } else if (m_currentToken.type == TokenType::GREATER_EQUAL) {
        condition->operator_ = ">=";
        advance();
    } else if (m_currentToken.type == TokenType::LESS_EQUAL) {
        condition->operator_ = "<=";
        advance();
    } else if (m_currentToken.type == TokenType::LIKE) {
        condition->operator_ = "LIKE";
        advance();
    } else if (m_currentToken.type == TokenType::IN) {
        condition->operator_ = "IN";
        advance();
    } else if (m_currentToken.type == TokenType::BETWEEN) {
        condition->operator_ = "BETWEEN";
        advance();
    } else {
        setError("Expected comparison operator (=, !=, >, <, >=, <=, LIKE, IN, BETWEEN), but got: " + m_currentToken.value);
        return nullptr;
    }
    
    // Value - 根据运算符类型解析不同的值
    if (condition->operator_ == "IN") {
        // IN子句：IN (value1, value2, ...)
        if (!expect(TokenType::LEFT_PAREN, "(")) {
            return nullptr;
        }
        
        condition->inValues.clear();
        while (true) {
            if (m_currentToken.type == TokenType::STRING_LITERAL) {
                condition->inValues.push_back(m_currentToken.value);
                advance();
            } else if (m_currentToken.type == TokenType::NUMBER) {
                condition->inValues.push_back(m_currentToken.value);
                advance();
            } else {
                setError("Expected value in IN clause, but got: " + m_currentToken.value);
                return nullptr;
            }
            
            if (m_currentToken.type == TokenType::COMMA) {
                advance();
            } else if (m_currentToken.type == TokenType::RIGHT_PAREN) {
                advance();
                break;
            } else {
                setError("Expected ',' or ')' in IN clause, but got: " + m_currentToken.value);
                return nullptr;
            }
        }
    } else if (condition->operator_ == "BETWEEN") {
        // BETWEEN子句：BETWEEN value1 AND value2
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            condition->betweenStart = m_currentToken.value;
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            condition->betweenStart = m_currentToken.value;
            advance();
        } else {
            setError("Expected value after BETWEEN, but got: " + m_currentToken.value);
            return nullptr;
        }
        
        if (!expect(TokenType::AND, "AND")) {
            return nullptr;
        }
        
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            condition->betweenEnd = m_currentToken.value;
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            condition->betweenEnd = m_currentToken.value;
            advance();
        } else {
            setError("Expected value after AND in BETWEEN clause, but got: " + m_currentToken.value);
            return nullptr;
        }
    } else {
        // 普通运算符：单个值
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            condition->value = m_currentToken.value;
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            condition->value = m_currentToken.value;
            advance();
        } else {
            setError("Expected value (string or number), but got: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    return condition;
}

