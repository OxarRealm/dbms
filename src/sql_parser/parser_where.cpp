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
    // 或者EXISTS子查询（不需要字段名）
    auto condition = std::make_unique<WhereCondition>();
    
    // 首先检查是否为EXISTS（EXISTS不需要字段名）
    if (m_currentToken.type == TokenType::EXISTS) {
        condition->operator_ = "EXISTS";
        condition->fieldName = "";  // EXISTS不需要字段名
        advance();
        
        // EXISTS子查询：EXISTS (SELECT ...)
        if (!expect(TokenType::LEFT_PAREN, "(")) {
            return nullptr;
        }
        
        // 解析子查询
        if (m_currentToken.type != TokenType::SELECT) {
            setError("Expected SELECT in EXISTS subquery, but got: " + m_currentToken.value);
            return nullptr;
        }
        
        // 解析SELECT语句（作为子查询，不期望分号）
        std::unique_ptr<SelectNode> subqueryNode = parseSelectAsSubquery();
        if (!subqueryNode) {
            return nullptr;
        }
        
        // 将子查询移动到WhereCondition中
        condition->subquery = std::move(subqueryNode);
        
        // 检查右括号
        if (!expect(TokenType::RIGHT_PAREN, ")")) {
            return nullptr;
        }
        
        return condition;
    }
    
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
        setError("Expected comparison operator (=, !=, >, <, >=, <=, LIKE, IN, BETWEEN, EXISTS), but got: " + m_currentToken.value);
        return nullptr;
    }
    
    // Value - 根据运算符类型解析不同的值
    if (condition->operator_ == "IN") {
        // IN子句：IN (value1, value2, ...) 或 IN (SELECT ...)
        if (!expect(TokenType::LEFT_PAREN, "(")) {
            return nullptr;
        }
        
        // 检查是否为子查询
        if (m_currentToken.type == TokenType::SELECT) {
            // IN (SELECT ...) - 子查询
            std::unique_ptr<SelectNode> subqueryNode = parseSelectAsSubquery();
            if (!subqueryNode) {
                return nullptr;
            }
            
            condition->subquery = std::move(subqueryNode);
            
            // 检查右括号
            if (!expect(TokenType::RIGHT_PAREN, ")")) {
                return nullptr;
            }
        } else {
            // IN (value1, value2, ...) - 值列表
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
        // 普通运算符：单个值 或 子查询
        // 检查是否为子查询：(SELECT ...)
        if (m_currentToken.type == TokenType::LEFT_PAREN) {
            advance();
            
            // 检查是否为SELECT（子查询）
            if (m_currentToken.type == TokenType::SELECT) {
                // 子查询：(SELECT ...)
                std::unique_ptr<SelectNode> subqueryNode = parseSelectAsSubquery();
                if (!subqueryNode) {
                    return nullptr;
                }
                
                condition->subquery = std::move(subqueryNode);
                
                // 检查右括号
                if (!expect(TokenType::RIGHT_PAREN, ")")) {
                    return nullptr;
                }
            } else {
                // 不是子查询，可能是括号包围的值，回退并解析为普通值
                // 这里简化处理，不支持括号包围的值
                setError("Expected SELECT for subquery or value, but got: " + m_currentToken.value);
                return nullptr;
            }
        } else {
            // 普通值：字符串、数字或表名.字段名（用于关联子查询）
            if (m_currentToken.type == TokenType::STRING_LITERAL) {
                condition->value = m_currentToken.value;
                advance();
            } else if (m_currentToken.type == TokenType::NUMBER) {
                condition->value = m_currentToken.value;
                advance();
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                // 可能是表名.字段名格式（用于关联子查询，如 Users.UserID）
                std::string identifier = m_currentToken.value;
                advance();
                
                // 检查是否有点号（表名.字段名）
                if (m_currentToken.type == TokenType::DOT) {
                    advance();
                    if (m_currentToken.type != TokenType::IDENTIFIER) {
                        setError("Expected identifier after '.', but got: " + m_currentToken.value);
                        return nullptr;
                    }
                    identifier += "." + m_currentToken.value;
                    advance();
                }
                
                condition->value = identifier;  // 存储为 "Users.UserID" 格式
            } else {
                setError("Expected value (string, number, or identifier), but got: " + m_currentToken.value);
                return nullptr;
            }
        }
    }
    
    return condition;
}

