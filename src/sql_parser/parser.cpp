/**
 * @file parser.cpp
 * @brief SQL语法分析器实现
 */

#include "sql_parser/parser.h"
#include "core/table_mode.h"
#include <iostream>
#include <algorithm>
#include <cstring>

Parser::Parser() {
}

Parser::Parser(const std::string& sql) : m_lexer(sql) {
    advance();
}

void Parser::setInput(const std::string& sql) {
    m_lexer.setInput(sql);
    advance();
    m_lastError = "";
}

std::unique_ptr<ASTNode> Parser::parse() {
    m_lastError = "";
    
    if (m_currentToken.type == TokenType::EOF_TOKEN) {
        setError("输入为空");
        return nullptr;
    }
    
    // 根据第一个Token判断语句类型
    switch (m_currentToken.type) {
        case TokenType::CREATE:
        case TokenType::EDIT:
        case TokenType::RENAME:
        case TokenType::DROP:
            return parseDDL();
        case TokenType::INSERT:
            return parseInsert();
        case TokenType::DELETE:
            return parseDelete();
        case TokenType::UPDATE:
            return parseUpdate();
        case TokenType::SELECT:
            return parseSelect();
        default:
            setError("未知的SQL语句类型: " + m_currentToken.value);
            return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseDDL() {
    switch (m_currentToken.type) {
        case TokenType::CREATE:
            return parseCreateTable();
        case TokenType::EDIT:
            return parseEditTable();
        case TokenType::RENAME:
            return parseRenameTable();
        case TokenType::DROP:
            return parseDropTable();
        default:
            setError("未知的DDL语句类型");
            return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseCreateTable() {
    // CREATE TABLE TableName (
    if (!expect(TokenType::CREATE, "CREATE")) return nullptr;
    if (!expect(TokenType::TABLE, "TABLE")) return nullptr;
    
    std::string tableName = parseIdentifier();
    if (tableName.empty()) return nullptr;
    
    if (!expect(TokenType::LEFT_PAREN, "(")) return nullptr;
    
    // 解析字段列表
    auto node = std::make_unique<CreateTableNode>();
    node->tableName = tableName;
    
    while (m_currentToken.type != TokenType::RIGHT_PAREN) {
        TableMode field;
        if (!parseFieldDefinition(field)) {
            return nullptr;
        }
        node->fields.push_back(field);
        
        // 如果还有逗号，继续解析下一个字段
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else if (m_currentToken.type != TokenType::RIGHT_PAREN) {
            setError("期望 ',' 或 ')'，但得到: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    if (!expect(TokenType::RIGHT_PAREN, ")")) return nullptr;
    
    // INTO DatabaseFileName;
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseEditTable() {
    // EDIT TABLE TableName (
    if (!expect(TokenType::EDIT, "EDIT")) return nullptr;
    if (!expect(TokenType::TABLE, "TABLE")) return nullptr;
    
    std::string tableName = parseIdentifier();
    if (tableName.empty()) return nullptr;
    
    if (!expect(TokenType::LEFT_PAREN, "(")) return nullptr;
    
    // 解析单个字段
    auto node = std::make_unique<EditTableNode>();
    node->tableName = tableName;
    
    if (!parseField(node->field)) {
        return nullptr;
    }
    
    if (!expect(TokenType::RIGHT_PAREN, ")")) return nullptr;
    
    // IN DatabaseFileName;
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseRenameTable() {
    // RENAME TABLE OldTableName NewTableName IN DatabaseFileName;
    if (!expect(TokenType::RENAME, "RENAME")) return nullptr;
    if (!expect(TokenType::TABLE, "TABLE")) return nullptr;
    
    std::string oldTableName = parseIdentifier();
    if (oldTableName.empty()) return nullptr;
    
    std::string newTableName = parseIdentifier();
    if (newTableName.empty()) return nullptr;
    
    auto node = std::make_unique<RenameTableNode>();
    node->oldTableName = oldTableName;
    node->newTableName = newTableName;
    
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseDropTable() {
    // DROP TABLE TableName IN DatabaseFileName;
    if (!expect(TokenType::DROP, "DROP")) return nullptr;
    if (!expect(TokenType::TABLE, "TABLE")) return nullptr;
    
    std::string tableName = parseIdentifier();
    if (tableName.empty()) return nullptr;
    
    auto node = std::make_unique<DropTableNode>();
    node->tableName = tableName;
    
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

bool Parser::parseFieldDefinition(TableMode& field) {
    // FieldName Type KEY_Flag NULL_Flag VALID_Flag
    std::string fieldName = parseIdentifier();
    if (fieldName.empty()) return false;
    
    strncpy(field.sFieldName, fieldName.c_str(), FIELD_NAME_LENGTH - 1);
    field.sFieldName[FIELD_NAME_LENGTH - 1] = '\0';
    
    if (!parseDataType(field)) return false;
    if (!parseFlags(field)) return false;
    
    return true;
}

bool Parser::parseField(TableMode& field) {
    // 与parseFieldDefinition相同
    return parseFieldDefinition(field);
}

bool Parser::parseDataType(TableMode& field) {
    // 解析类型（int, char等）
    if (m_currentToken.type == TokenType::INT) {
        strncpy(field.sType, "int", TYPE_NAME_LENGTH - 1);
        field.sType[TYPE_NAME_LENGTH - 1] = '\0';
        field.iSize = 4;
        advance();
    } else if (m_currentToken.type == TokenType::CHAR) {
        strncpy(field.sType, "char", TYPE_NAME_LENGTH - 1);
        field.sType[TYPE_NAME_LENGTH - 1] = '\0';
        advance();
        
        // 如果是char[50]格式，需要解析大小
        if (m_currentToken.type == TokenType::LEFT_PAREN) {
            advance();
            if (m_currentToken.type == TokenType::NUMBER) {
                field.iSize = std::stoi(m_currentToken.value);
                advance();
            } else {
                setError("期望数字，但得到: " + m_currentToken.value);
                return false;
            }
            if (!expect(TokenType::RIGHT_PAREN, ")")) return false;
        } else {
            field.iSize = 1;  // 默认char大小为1
        }
    } else if (m_currentToken.type == TokenType::FLOAT) {
        strncpy(field.sType, "float", TYPE_NAME_LENGTH - 1);
        field.sType[TYPE_NAME_LENGTH - 1] = '\0';
        field.iSize = 4;
        advance();
    } else if (m_currentToken.type == TokenType::DOUBLE) {
        strncpy(field.sType, "double", TYPE_NAME_LENGTH - 1);
        field.sType[TYPE_NAME_LENGTH - 1] = '\0';
        field.iSize = 8;
        advance();
    } else if (m_currentToken.type == TokenType::STRING) {
        strncpy(field.sType, "string", TYPE_NAME_LENGTH - 1);
        field.sType[TYPE_NAME_LENGTH - 1] = '\0';
        field.iSize = 0;  // 变长字符串
        advance();
    } else {
        setError("未知的数据类型: " + m_currentToken.value);
        return false;
    }
    
    return true;
}

bool Parser::parseFlags(TableMode& field) {
    // 解析KEY标志
    if (m_currentToken.type == TokenType::KEY) {
        field.bKey = FLAG_KEY;
        advance();
    } else if (m_currentToken.type == TokenType::NOT_KEY) {
        field.bKey = FLAG_NOT_KEY;
        advance();
    } else {
        setError("期望 KEY 或 NOT_KEY，但得到: " + m_currentToken.value);
        return false;
    }
    
    // 解析NULL标志
    if (m_currentToken.type == TokenType::NULL_KEYWORD) {
        field.bNullFlag = FLAG_NULL;
        advance();
    } else if (m_currentToken.type == TokenType::NO_NULL) {
        field.bNullFlag = FLAG_NO_NULL;
        advance();
    } else {
        setError("期望 NULL 或 NO_NULL，但得到: " + m_currentToken.value);
        return false;
    }
    
    // 解析VALID标志
    if (m_currentToken.type == TokenType::VALID) {
        field.bValidFlag = FLAG_VALID;
        advance();
    } else if (m_currentToken.type == TokenType::INVALID) {
        field.bValidFlag = FLAG_INVALID;
        advance();
    } else {
        setError("期望 VALID 或 INVALID，但得到: " + m_currentToken.value);
        return false;
    }
    
    return true;
}

std::string Parser::parseIdentifier() {
    if (m_currentToken.type != TokenType::IDENTIFIER) {
        setError("期望标识符，但得到: " + m_currentToken.value);
        return "";
    }
    
    std::string identifier = m_currentToken.value;
    advance();
    return identifier;
}

std::string Parser::parseDatabaseFileName() {
    // INTO DatabaseFileName 或 IN DatabaseFileName
    if (m_currentToken.type == TokenType::INTO || m_currentToken.type == TokenType::IN) {
        advance();
        std::string fileName = parseIdentifier();
        return fileName;
    } else {
        setError("期望 INTO 或 IN，但得到: " + m_currentToken.value);
        return "";
    }
}

void Parser::advance() {
    m_currentToken = m_lexer.nextToken();
}

bool Parser::match(TokenType type) {
    return m_currentToken.type == type;
}

bool Parser::expect(TokenType type, const std::string& expected) {
    if (match(type)) {
        advance();
        return true;
    } else {
        setError("期望 " + expected + "，但得到: " + m_currentToken.value);
        return false;
    }
}

void Parser::setError(const std::string& error) {
    m_lastError = error;
}

std::string Parser::getLastError() const {
    return m_lastError;
}

std::unique_ptr<ASTNode> Parser::parseInsert() {
    // INSERT INTO TableName VALUES (value1, value2, ...) IN DatabaseFileName;
    auto node = std::make_unique<InsertNode>();
    
    // 已经匹配了INSERT，跳过
    advance();
    
    // INTO
    if (!expect(TokenType::INTO, "INTO")) {
        return nullptr;
    }
    
    // TableName
    node->tableName = parseIdentifier();
    if (node->tableName.empty()) {
        return nullptr;
    }
    
    // VALUES
    if (!expect(TokenType::VALUES, "VALUES")) {
        return nullptr;
    }
    
    // (
    if (!expect(TokenType::LEFT_PAREN, "(")) {
        return nullptr;
    }
    
    // 解析值列表
    while (true) {
        // 解析值（字符串字面量或数字）
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            node->values.push_back(m_currentToken.value);
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            node->values.push_back(m_currentToken.value);
            advance();
        } else {
            setError("期望值（字符串或数字），但得到: " + m_currentToken.value);
            return nullptr;
        }
        
        // 检查是否有逗号（继续解析）或右括号（结束）
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else if (m_currentToken.type == TokenType::RIGHT_PAREN) {
            advance();
            break;
        } else {
            setError("期望 , 或 )，但得到: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    // IN DatabaseFileName
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) {
        return nullptr;
    }
    
    // ;
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseDelete() {
    // DELETE FROM TableName WHERE Field=Value IN DatabaseFileName;
    auto node = std::make_unique<DeleteNode>();
    
    // 已经匹配了DELETE，跳过
    advance();
    
    // FROM
    if (!expect(TokenType::FROM, "FROM")) {
        return nullptr;
    }
    
    // TableName
    node->tableName = parseIdentifier();
    if (node->tableName.empty()) {
        return nullptr;
    }
    
    // WHERE
    if (!expect(TokenType::WHERE, "WHERE")) {
        return nullptr;
    }
    
    // Field
    node->conditionField = parseIdentifier();
    if (node->conditionField.empty()) {
        return nullptr;
    }
    
    // =
    if (!expect(TokenType::EQUALS, "=")) {
        return nullptr;
    }
    
    // Value
    if (m_currentToken.type == TokenType::STRING_LITERAL) {
        node->conditionValue = m_currentToken.value;
        advance();
    } else if (m_currentToken.type == TokenType::NUMBER) {
        node->conditionValue = m_currentToken.value;
        advance();
    } else {
        setError("期望值（字符串或数字），但得到: " + m_currentToken.value);
        return nullptr;
    }
    
    // IN DatabaseFileName
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) {
        return nullptr;
    }
    
    // ;
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::parseUpdate() {
    // UPDATE TableName (SET Field1=Value1 WHERE Field2=Value2) IN DatabaseFileName;
    auto node = std::make_unique<UpdateNode>();
    
    // 已经匹配了UPDATE，跳过
    advance();
    
    // TableName
    node->tableName = parseIdentifier();
    if (node->tableName.empty()) {
        return nullptr;
    }
    
    // (
    if (!expect(TokenType::LEFT_PAREN, "(")) {
        return nullptr;
    }
    
    // SET
    if (!expect(TokenType::SET, "SET")) {
        return nullptr;
    }
    
    // Field1
    node->setField = parseIdentifier();
    if (node->setField.empty()) {
        return nullptr;
    }
    
    // =
    if (!expect(TokenType::EQUALS, "=")) {
        return nullptr;
    }
    
    // Value1
    if (m_currentToken.type == TokenType::STRING_LITERAL) {
        node->setValue = m_currentToken.value;
        advance();
    } else if (m_currentToken.type == TokenType::NUMBER) {
        node->setValue = m_currentToken.value;
        advance();
    } else {
        setError("期望值（字符串或数字），但得到: " + m_currentToken.value);
        return nullptr;
    }
    
    // WHERE
    if (!expect(TokenType::WHERE, "WHERE")) {
        return nullptr;
    }
    
    // Field2
    node->whereField = parseIdentifier();
    if (node->whereField.empty()) {
        return nullptr;
    }
    
    // =
    if (!expect(TokenType::EQUALS, "=")) {
        return nullptr;
    }
    
    // Value2
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
    
    // )
    if (!expect(TokenType::RIGHT_PAREN, ")")) {
        return nullptr;
    }
    
    // IN DatabaseFileName
    node->databaseFileName = parseDatabaseFileName();
    if (node->databaseFileName.empty()) {
        return nullptr;
    }
    
    // ;
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

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

