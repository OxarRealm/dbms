/**
 * @file parser_select.cpp
 * @brief SELECT语句解析实现
 */

#include "sql_parser/parser.h"

std::unique_ptr<SelectNode> Parser::parseSelectWithoutUnion() {
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
    
    // 解析SELECT字段列表（支持聚合函数）
    if (m_currentToken.type == TokenType::ASTERISK) {
        // SELECT * 表示选择所有字段
        node->selectFields.push_back("*");
        SelectField field;
        field.isAggregate = false;
        field.fieldName = "*";
        node->selectFieldsNew.push_back(field);
        advance();
    } else {
        // 字段列表：Field1, Field2, COUNT(Field), SUM(Field), ...
        while (true) {
            SelectField field;
            
            // 检查是否为聚合函数
            if (m_currentToken.type == TokenType::COUNT ||
                m_currentToken.type == TokenType::SUM ||
                m_currentToken.type == TokenType::AVG ||
                m_currentToken.type == TokenType::MAX ||
                m_currentToken.type == TokenType::MIN) {
                // 解析聚合函数
                field.isAggregate = true;
                if (m_currentToken.type == TokenType::COUNT) {
                    field.aggregateFunc.funcName = "COUNT";
                } else if (m_currentToken.type == TokenType::SUM) {
                    field.aggregateFunc.funcName = "SUM";
                } else if (m_currentToken.type == TokenType::AVG) {
                    field.aggregateFunc.funcName = "AVG";
                } else if (m_currentToken.type == TokenType::MAX) {
                    field.aggregateFunc.funcName = "MAX";
                } else if (m_currentToken.type == TokenType::MIN) {
                    field.aggregateFunc.funcName = "MIN";
                }
                advance();
                
                // 解析聚合函数的参数
                if (!expect(TokenType::LEFT_PAREN, "(")) {
                    return nullptr;
                }
                
                // 检查是否为COUNT(*)
                if (m_currentToken.type == TokenType::ASTERISK) {
                    field.aggregateFunc.isStar = true;
                    advance();
                } else {
                    // 解析字段名
                    field.aggregateFunc.fieldName = parseIdentifier();
                    if (field.aggregateFunc.fieldName.empty()) {
                        return nullptr;
                    }
                }
                
                if (!expect(TokenType::RIGHT_PAREN, ")")) {
                    return nullptr;
                }
                
                // 向后兼容：生成字符串表示
                std::string aggStr = field.aggregateFunc.funcName + "(";
                if (field.aggregateFunc.isStar) {
                    aggStr += "*";
                } else {
                    aggStr += field.aggregateFunc.fieldName;
                }
                aggStr += ")";
                node->selectFields.push_back(aggStr);
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                // 普通字段
                field.isAggregate = false;
                field.fieldName = parseIdentifier();
                if (field.fieldName.empty()) {
                    return nullptr;
                }
                node->selectFields.push_back(field.fieldName);
            } else {
                setError("Expected field name, aggregate function, or '*', but got: " + m_currentToken.value);
                return nullptr;
            }
            
            // 检查是否有别名（AS关键字可选）
            if (m_currentToken.type == TokenType::IDENTIFIER) {
                // 可能是别名（简化处理，如果下一个是逗号或FROM，则认为是别名）
                // 这里先不处理别名，保持简单
            }
            
            node->selectFieldsNew.push_back(field);
            
            if (m_currentToken.type == TokenType::COMMA) {
                advance();
            } else {
                break;
            }
        }
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
        m_currentToken.type == TokenType::RIGHT ||
        m_currentToken.type == TokenType::FULL ||
        m_currentToken.type == TokenType::NATURAL) {
        
        // 解析JOIN子句
        while (true) {
            JoinInfo joinInfo;
            
            // 解析JOIN类型（INNER, LEFT, RIGHT, FULL OUTER）
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
            } else if (m_currentToken.type == TokenType::FULL) {
                joinInfo.joinType = "FULL";
                advance();
                // OUTER是可选的（FULL OUTER JOIN 或 FULL JOIN）
                if (m_currentToken.type == TokenType::OUTER) {
                    advance();
                }
                if (!expect(TokenType::JOIN, "JOIN")) {
                    return nullptr;
                }
            } else if (m_currentToken.type == TokenType::NATURAL) {
                // NATURAL JOIN
                joinInfo.joinType = "NATURAL";
                advance();
                // NATURAL JOIN可以与其他JOIN类型组合（NATURAL LEFT JOIN等）
                if (m_currentToken.type == TokenType::LEFT) {
                    joinInfo.joinType = "NATURAL_LEFT";
                    advance();
                } else if (m_currentToken.type == TokenType::RIGHT) {
                    joinInfo.joinType = "NATURAL_RIGHT";
                    advance();
                } else if (m_currentToken.type == TokenType::INNER) {
                    joinInfo.joinType = "NATURAL_INNER";
                    advance();
                } else if (m_currentToken.type == TokenType::FULL) {
                    joinInfo.joinType = "NATURAL_FULL";
                    advance();
                    if (m_currentToken.type == TokenType::OUTER) {
                        advance();
                    }
                }
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
            
            // NATURAL JOIN不需要ON子句，其他JOIN需要
            if (joinInfo.joinType.find("NATURAL") == std::string::npos) {
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
            } else {
                // NATURAL JOIN：不需要ON子句，同名字段会在执行时自动匹配
                // leftField和rightField留空，表示使用NATURAL JOIN
            }
            
            node->joins.push_back(joinInfo);
            
            // 检查是否有更多JOIN
            if (m_currentToken.type != TokenType::JOIN &&
                m_currentToken.type != TokenType::INNER &&
                m_currentToken.type != TokenType::LEFT &&
                m_currentToken.type != TokenType::RIGHT &&
                m_currentToken.type != TokenType::FULL &&
                m_currentToken.type != TokenType::NATURAL) {
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
    
    // GROUP BY子句（可选）
    if (m_currentToken.type == TokenType::GROUP) {
        advance();
        if (!expect(TokenType::BY, "BY")) {
            return nullptr;
        }
        
        // 解析分组字段列表
        while (true) {
            std::string groupField = parseIdentifier();
            if (groupField.empty()) {
                return nullptr;
            }
            node->groupBy.push_back(groupField);
            
            // 检查是否有更多分组字段
            if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else {
                break;
            }
        }
    }
    
    // HAVING子句（可选，必须在GROUP BY之后）
    if (m_currentToken.type == TokenType::HAVING) {
        advance();
        
        // 解析HAVING条件（与WHERE条件结构相同）
        node->havingClause = parseWhereCondition();
        if (!node->havingClause) {
            return nullptr;
        }
    }
    
    // 注意：parseSelectWithoutUnion不解析ORDER BY和LIMIT
    // 因为这些可能在UNION之后，应该由parseSelect统一处理
    // parseSelectWithoutUnion不检查分号，由调用者处理
    return node;
}

std::unique_ptr<SelectNode> Parser::parseSelectAsSubquery() {
    // 解析SELECT语句作为子查询
    // 子查询可以有ORDER BY和LIMIT，但不期望分号（因为它在括号内）
    std::unique_ptr<SelectNode> node = parseSelectWithoutUnion();
    if (!node) {
        return nullptr;
    }
    
    // 子查询可以有自己的ORDER BY和LIMIT
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
    
    // 子查询不期望分号（由调用者处理右括号）
    return node;
}

std::unique_ptr<ASTNode> Parser::parseSelect() {
    // 解析第一个SELECT语句
    std::unique_ptr<SelectNode> node = parseSelectWithoutUnion();
    if (!node) {
        return nullptr;
    }
    
    // UNION子句（可选，在分号之前）
    bool hasUnion = false;
    
    while (m_currentToken.type == TokenType::UNION) {
        hasUnion = true;
        advance();
        
        // 检查是否有ALL关键字
        bool isUnionAll = false;
        if (m_currentToken.type == TokenType::ALL) {
            isUnionAll = true;
            advance();
        }
        
        // 解析下一个SELECT语句
        if (m_currentToken.type != TokenType::SELECT) {
            setError("Expected SELECT after UNION, but got: " + m_currentToken.value);
            return nullptr;
        }
        
        // 解析UNION的SELECT语句（不处理UNION，避免无限递归）
        std::unique_ptr<SelectNode> unionNode = parseSelectWithoutUnion();
        if (!unionNode) {
            return nullptr;
        }
        
        // 将UNION查询添加到unionQueries列表
        node->unionQueries.push_back(std::move(unionNode));
        
        // 设置unionAll标志（如果第一个UNION是ALL，则所有都是ALL）
        if (node->unionQueries.size() == 1) {
            node->unionAll = isUnionAll;
        } else if (node->unionAll != isUnionAll) {
            // 所有UNION必须都是ALL或都不是ALL
            setError("UNION and UNION ALL cannot be mixed");
            return nullptr;
        }
    }
    
    // 解析ORDER BY和LIMIT（在UNION之后，或者如果没有UNION，在第一个SELECT之后）
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
    
    // 检查分号
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

