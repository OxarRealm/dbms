/**
 * @file parser_constraints.cpp
 * @brief SQL解析器约束解析函数实现
 */

#include "sql_parser/parser.h"
#include "core/constraint.h"
#include <cstring>

bool Parser::parseForeignKeyConstraint(CreateTableNode& node, const std::string& constraintName) {
    // FOREIGN KEY (field) REFERENCES table(field) [ON DELETE action] [ON UPDATE action]
    if (!expect(TokenType::FOREIGN, "FOREIGN")) return false;
    if (!expect(TokenType::KEY, "KEY")) return false;
    
    ForeignKeyConstraint fk;
    if (!constraintName.empty()) {
        strncpy(fk.constraintName, constraintName.c_str(), CONSTRAINT_NAME_LENGTH - 1);
        fk.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
    }
    
    // (field)
    if (!expect(TokenType::LEFT_PAREN, "(")) return false;
    std::string fieldName = parseIdentifier();
    if (fieldName.empty()) return false;
    strncpy(fk.fieldName, fieldName.c_str(), FIELD_NAME_LENGTH - 1);
    fk.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
    if (!expect(TokenType::RIGHT_PAREN, ")")) return false;
    
    // REFERENCES table(field)
    if (!expect(TokenType::REFERENCES, "REFERENCES")) return false;
    std::string refTable = parseIdentifier();
    if (refTable.empty()) return false;
    strncpy(fk.referencedTable, refTable.c_str(), TABLE_NAME_LENGTH - 1);
    fk.referencedTable[TABLE_NAME_LENGTH - 1] = '\0';
    
    if (!expect(TokenType::LEFT_PAREN, "(")) return false;
    std::string refField = parseIdentifier();
    if (refField.empty()) return false;
    strncpy(fk.referencedField, refField.c_str(), FIELD_NAME_LENGTH - 1);
    fk.referencedField[FIELD_NAME_LENGTH - 1] = '\0';
    if (!expect(TokenType::RIGHT_PAREN, ")")) return false;
    
    // [ON DELETE action]
    if (m_currentToken.type == TokenType::ON) {
        advance();
        if (m_currentToken.type == TokenType::DELETE) {
            advance();
            if (m_currentToken.type == TokenType::CASCADE) {
                strncpy(fk.onDeleteAction, "CASCADE", 15);
                fk.onDeleteAction[15] = '\0';
                advance();
            } else if (m_currentToken.type == TokenType::RESTRICT) {
                strncpy(fk.onDeleteAction, "RESTRICT", 15);
                fk.onDeleteAction[15] = '\0';
                advance();
            } else if (m_currentToken.type == TokenType::SET) {
                advance();
                if (m_currentToken.type == TokenType::NULL_KEYWORD) {
                    strncpy(fk.onDeleteAction, "SET NULL", 15);
                    fk.onDeleteAction[15] = '\0';
                    advance();
                } else {
                    setError("Expected NULL after SET in ON DELETE, but got: " + m_currentToken.value);
                    return false;
                }
            } else if (m_currentToken.type == TokenType::IDENTIFIER && m_currentToken.value == "NO") {
                // 支持 NO ACTION
                advance();
                if (m_currentToken.type == TokenType::IDENTIFIER && m_currentToken.value == "ACTION") {
                    strncpy(fk.onDeleteAction, "NO ACTION", 15);
                    fk.onDeleteAction[15] = '\0';
                    advance();
                } else {
                    setError("Expected ACTION after NO in ON DELETE, but got: " + m_currentToken.value);
                    return false;
                }
            } else {
                setError("Expected CASCADE, RESTRICT, SET NULL, or NO ACTION after ON DELETE, but got: " + m_currentToken.value);
                return false;
            }
        }
    }
    
    // [ON UPDATE action]
    if (m_currentToken.type == TokenType::ON) {
        advance();
        if (m_currentToken.type == TokenType::UPDATE) {
            advance();
            if (m_currentToken.type == TokenType::CASCADE) {
                strncpy(fk.onUpdateAction, "CASCADE", 15);
                fk.onUpdateAction[15] = '\0';
                advance();
            } else if (m_currentToken.type == TokenType::RESTRICT) {
                strncpy(fk.onUpdateAction, "RESTRICT", 15);
                fk.onUpdateAction[15] = '\0';
                advance();
            } else if (m_currentToken.type == TokenType::SET) {
                advance();
                if (m_currentToken.type == TokenType::NULL_KEYWORD) {
                    strncpy(fk.onUpdateAction, "SET NULL", 15);
                    fk.onUpdateAction[15] = '\0';
                    advance();
                } else {
                    setError("Expected NULL after SET in ON UPDATE, but got: " + m_currentToken.value);
                    return false;
                }
            } else if (m_currentToken.type == TokenType::IDENTIFIER && m_currentToken.value == "NO") {
                // 支持 NO ACTION
                advance();
                if (m_currentToken.type == TokenType::IDENTIFIER && m_currentToken.value == "ACTION") {
                    strncpy(fk.onUpdateAction, "NO ACTION", 15);
                    fk.onUpdateAction[15] = '\0';
                    advance();
                } else {
                    setError("Expected ACTION after NO in ON UPDATE, but got: " + m_currentToken.value);
                    return false;
                }
            } else {
                setError("Expected CASCADE, RESTRICT, SET NULL, or NO ACTION after ON UPDATE, but got: " + m_currentToken.value);
                return false;
            }
        }
    }
    
    node.foreignKeys.push_back(fk);
    return true;
}

bool Parser::parseCheckConstraint(CreateTableNode& node, const std::string& constraintName) {
    // CHECK (expression)
    // 支持：Value > 0 AND Value < 100, Price > 0 OR Price < 0
    if (!expect(TokenType::CHECK, "CHECK")) return false;
    
    CheckConstraint check;
    if (!constraintName.empty()) {
        strncpy(check.constraintName, constraintName.c_str(), CONSTRAINT_NAME_LENGTH - 1);
        check.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
    }
    
    // 解析表达式
    if (!expect(TokenType::LEFT_PAREN, "(")) return false;
    
    // 解析第一个字段名（用于check.fieldName）
    std::string firstFieldName = parseIdentifier();
    if (firstFieldName.empty()) return false;
    strncpy(check.fieldName, firstFieldName.c_str(), FIELD_NAME_LENGTH - 1);
    check.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
    
    // 开始构建表达式字符串
    std::string expression = firstFieldName + " ";
    
    // 循环解析条件，直到遇到右括号
    while (m_currentToken.type != TokenType::RIGHT_PAREN) {
        // 解析运算符
        if (m_currentToken.type == TokenType::GREATER_THAN || 
            m_currentToken.type == TokenType::LESS_THAN ||
            m_currentToken.type == TokenType::GREATER_EQUAL ||
            m_currentToken.type == TokenType::LESS_EQUAL ||
            m_currentToken.type == TokenType::EQUALS ||
            m_currentToken.type == TokenType::NOT_EQUALS) {
            expression += m_currentToken.value + " ";
            advance();
            
            // 解析值（支持负数）
            if (m_currentToken.type == TokenType::STRING_LITERAL || 
                m_currentToken.type == TokenType::NUMBER) {
                expression += m_currentToken.value;
                advance();
            } else if (m_currentToken.type == TokenType::ERROR && m_currentToken.value == "-") {
                // 处理负数
                advance();  // 跳过'-'
                if (m_currentToken.type == TokenType::NUMBER) {
                    expression += "-" + m_currentToken.value;
                    advance();
                } else {
                    setError("Expected number after '-' in CHECK constraint, but got: " + m_currentToken.value);
                    return false;
                }
            } else {
                setError("Expected value in CHECK constraint, but got: " + m_currentToken.value);
                return false;
            }
        } else if (m_currentToken.type == TokenType::AND || 
                   m_currentToken.type == TokenType::OR) {
            // 遇到逻辑运算符，添加到表达式
            expression += " " + m_currentToken.value + " ";
            advance();
            
            // 解析下一个字段名（可能是同一个字段，也可能是不同字段）
            std::string nextFieldName = parseIdentifier();
            if (nextFieldName.empty()) return false;
            expression += nextFieldName + " ";
        } else {
            setError("Expected operator, AND, OR, or ')' in CHECK constraint, but got: " + m_currentToken.value);
            return false;
        }
    }
    
    check.expression = expression;
    if (!expect(TokenType::RIGHT_PAREN, ")")) return false;
    
    node.checkConstraints.push_back(check);
    return true;
}

bool Parser::parseUniqueConstraint(CreateTableNode& node, const std::string& constraintName) {
    // UNIQUE (field1, field2, ...)
    if (!expect(TokenType::UNIQUE, "UNIQUE")) return false;
    
    UniqueConstraint unique;
    if (!constraintName.empty()) {
        strncpy(unique.constraintName, constraintName.c_str(), CONSTRAINT_NAME_LENGTH - 1);
        unique.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
    }
    
    // (field1, field2, ...)
    if (!expect(TokenType::LEFT_PAREN, "(")) return false;
    
    while (m_currentToken.type != TokenType::RIGHT_PAREN) {
        std::string fieldName = parseIdentifier();
        if (fieldName.empty()) return false;
        unique.fieldNames.push_back(fieldName);
        
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else if (m_currentToken.type != TokenType::RIGHT_PAREN) {
            setError("Expected ',' or ')', but got: " + m_currentToken.value);
            return false;
        }
    }
    
    if (!expect(TokenType::RIGHT_PAREN, ")")) return false;
    
    node.uniqueConstraints.push_back(unique);
    return true;
}

