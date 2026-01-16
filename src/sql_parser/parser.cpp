/**
 * @file parser.cpp
 * @brief SQL语法分析器实现
 */

#include "sql_parser/parser.h"
#include "core/table_mode.h"
#include "core/constraint.h"
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
        setError("Input is empty");
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
            setError("Unknown SQL statement type: " + m_currentToken.value);
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
            setError("Unknown DDL statement type");
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
    
    // 解析字段列表和表级约束
    auto node = std::make_unique<CreateTableNode>();
    node->tableName = tableName;
    
    while (m_currentToken.type != TokenType::RIGHT_PAREN) {
        // 检查是否是约束关键字（表级约束）
        if (m_currentToken.type == TokenType::CONSTRAINT || 
            m_currentToken.type == TokenType::FOREIGN ||
            m_currentToken.type == TokenType::CHECK ||
            m_currentToken.type == TokenType::UNIQUE) {
            // 遇到约束关键字，退出字段解析循环，进入约束解析
            break;
        }
        
        // 解析字段定义
        TableMode field;
        if (!parseFieldDefinition(field)) {
            return nullptr;
        }
        node->fields.push_back(field);
        
        // 如果还有逗号，继续解析下一个字段或约束
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else if (m_currentToken.type != TokenType::RIGHT_PAREN &&
                   m_currentToken.type != TokenType::CONSTRAINT &&
                   m_currentToken.type != TokenType::FOREIGN &&
                   m_currentToken.type != TokenType::CHECK &&
                   m_currentToken.type != TokenType::UNIQUE) {
            setError("Expected ',', ')', or constraint keyword, but got: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    // 解析表级约束（可选）
    // 支持：FOREIGN KEY, CHECK, UNIQUE (多字段)
    // 注意：约束解析在字段列表之后，但在右括号之前
    while (m_currentToken.type == TokenType::CONSTRAINT || 
           m_currentToken.type == TokenType::FOREIGN ||
           m_currentToken.type == TokenType::CHECK ||
           m_currentToken.type == TokenType::UNIQUE) {
        
        if (m_currentToken.type == TokenType::FOREIGN) {
            // FOREIGN KEY (field) REFERENCES table(field) [ON DELETE action] [ON UPDATE action]
            if (!parseForeignKeyConstraint(*node)) {
                return nullptr;
            }
        } else if (m_currentToken.type == TokenType::CHECK) {
            // CHECK (expression)
            if (!parseCheckConstraint(*node)) {
                return nullptr;
            }
        } else if (m_currentToken.type == TokenType::UNIQUE) {
            // UNIQUE (field1, field2, ...) - 多字段唯一约束
            if (!parseUniqueConstraint(*node)) {
                return nullptr;
            }
        } else if (m_currentToken.type == TokenType::CONSTRAINT) {
            // CONSTRAINT constraint_name ...
            advance();
            // 解析约束名称
            std::string constraintName = parseIdentifier();
            if (constraintName.empty()) return nullptr;
            
            // 根据下一个关键词决定约束类型
            if (m_currentToken.type == TokenType::FOREIGN) {
                if (!parseForeignKeyConstraint(*node, constraintName)) {
                    return nullptr;
                }
            } else if (m_currentToken.type == TokenType::CHECK) {
                if (!parseCheckConstraint(*node, constraintName)) {
                    return nullptr;
                }
            } else if (m_currentToken.type == TokenType::UNIQUE) {
                if (!parseUniqueConstraint(*node, constraintName)) {
                    return nullptr;
                }
            } else {
                setError("Expected FOREIGN, CHECK, or UNIQUE after CONSTRAINT, but got: " + m_currentToken.value);
                return nullptr;
            }
        }
        
        // 如果还有逗号，继续解析下一个约束
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else if (m_currentToken.type != TokenType::RIGHT_PAREN) {
            setError("Expected ',' or ')', but got: " + m_currentToken.value);
            return nullptr;
        } else {
            break;  // 遇到右括号，约束解析结束
        }
    }
    
    // 期望右括号
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
        
        // 如果是char[50]或char(50)格式，需要解析大小
        // 支持两种格式：char[50] 和 char(50)
        if (m_currentToken.type == TokenType::LEFT_BRACKET || m_currentToken.type == TokenType::LEFT_PAREN) {
            TokenType openBracket = m_currentToken.type;
            TokenType closeBracket = (openBracket == TokenType::LEFT_BRACKET) ? 
                                     TokenType::RIGHT_BRACKET : TokenType::RIGHT_PAREN;
            std::string closeBracketStr = (openBracket == TokenType::LEFT_BRACKET) ? "]" : ")";
            
            advance();
            if (m_currentToken.type == TokenType::NUMBER) {
                field.iSize = std::stoi(m_currentToken.value);
                advance();
            } else {
                setError("Expected number, but got: " + m_currentToken.value);
                return false;
            }
            if (!expect(closeBracket, closeBracketStr)) return false;
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
        advance();
        
        // 如果是string[50]或string(50)格式，需要解析大小
        // 支持两种格式：string[50] 和 string(50)
        if (m_currentToken.type == TokenType::LEFT_BRACKET || m_currentToken.type == TokenType::LEFT_PAREN) {
            TokenType openBracket = m_currentToken.type;
            TokenType closeBracket = (openBracket == TokenType::LEFT_BRACKET) ? 
                                     TokenType::RIGHT_BRACKET : TokenType::RIGHT_PAREN;
            std::string closeBracketStr = (openBracket == TokenType::LEFT_BRACKET) ? "]" : ")";
            
            advance();
            if (m_currentToken.type == TokenType::NUMBER) {
                field.iSize = std::stoi(m_currentToken.value);
                advance();
            } else {
                setError("Expected number, but got: " + m_currentToken.value);
                return false;
            }
            if (!expect(closeBracket, closeBracketStr)) return false;
        } else {
            field.iSize = 0;  // 默认变长字符串，大小为0
        }
    } else {
        setError("Unknown data type: " + m_currentToken.value);
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
        setError("Expected KEY or NOT_KEY, but got: " + m_currentToken.value);
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
        setError("Expected NULL or NO_NULL, but got: " + m_currentToken.value);
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
        setError("Expected VALID or INVALID, but got: " + m_currentToken.value);
        return false;
    }
    
    // 扩展：解析字段级约束（可选）
    // UNIQUE - 唯一约束
    if (m_currentToken.type == TokenType::UNIQUE) {
        field.bUnique = FLAG_KEY;  // 使用FLAG_KEY表示唯一约束
        advance();
    } else {
        field.bUnique = 0;  // 默认不是唯一约束
    }
    
    // DEFAULT value - 默认值
    if (m_currentToken.type == TokenType::DEFAULT) {
        advance();
        // 解析默认值（字符串或数字）
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            std::string defaultValue = m_currentToken.value;
            // 去除引号
            if (defaultValue.length() >= 2 && 
                ((defaultValue.front() == '\'' && defaultValue.back() == '\'') ||
                 (defaultValue.front() == '"' && defaultValue.back() == '"'))) {
                defaultValue = defaultValue.substr(1, defaultValue.length() - 2);
            }
            strncpy(field.sDefaultValue, defaultValue.c_str(), 127);
            field.sDefaultValue[127] = '\0';
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            strncpy(field.sDefaultValue, m_currentToken.value.c_str(), 127);
            field.sDefaultValue[127] = '\0';
            advance();
        } else {
            setError("Expected string or number for DEFAULT value, but got: " + m_currentToken.value);
            return false;
        }
    } else {
        field.sDefaultValue[0] = '\0';  // 默认无默认值
    }
    
    return true;
}

std::string Parser::parseIdentifier() {
    if (m_currentToken.type != TokenType::IDENTIFIER) {
        setError("Expected identifier, but got: " + m_currentToken.value);
        return "";
    }
    
    std::string identifier = m_currentToken.value;
    advance();
    
    // 支持 TableName.FieldName 格式（用于JOIN查询）
    if (m_currentToken.type == TokenType::DOT) {
        advance();
        if (m_currentToken.type != TokenType::IDENTIFIER) {
            setError("Expected identifier after '.', but got: " + m_currentToken.value);
            return "";
        }
        identifier += "." + m_currentToken.value;
        advance();
    }
    
    return identifier;
}

std::string Parser::parseDatabaseFileName() {
    // INTO DatabaseFileName 或 IN DatabaseFileName
    // 注意：这个函数期望在调用时，当前token已经是IN或INTO
    // 如果当前token不是IN或INTO，说明调用方式错误
    if (m_currentToken.type != TokenType::INTO && m_currentToken.type != TokenType::IN) {
        setError("Expected INTO or IN, but got: " + m_currentToken.value);
        return "";
    }
    
    advance();  // 跳过IN或INTO
    
    // 数据库文件名可能是标识符，也可能是关键字（虽然不应该，但为了兼容性）
    // 接受任何标识符或关键字作为文件名
    std::string fileName = m_currentToken.value;
    advance();
    return fileName;
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
        setError("Expected " + expected + ", but got: " + m_currentToken.value);
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
    // 表名可能是关键字（虽然不应该，但为了兼容性）
    if (m_currentToken.type == TokenType::IDENTIFIER) {
        node->tableName = parseIdentifier();
    } else {
        // 如果是关键字，也接受作为表名
        node->tableName = m_currentToken.value;
        advance();
    }
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
        // 注意：也接受标识符和关键字作为值（可能是未加引号的字符串值）
        // 这样可以处理值恰好是关键字的情况（虽然不应该，但为了兼容性）
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            node->values.push_back(m_currentToken.value);
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            node->values.push_back(m_currentToken.value);
            advance();
        } else if (m_currentToken.type == TokenType::ERROR && m_currentToken.value == "-") {
            // 处理负数：-后面跟数字
            advance();  // 跳过'-'
            if (m_currentToken.type == TokenType::NUMBER) {
                // 组合成负数
                node->values.push_back("-" + m_currentToken.value);
                advance();
            } else {
                setError("Expected number after '-', but got: " + m_currentToken.value);
                return nullptr;
            }
        } else if (m_currentToken.type == TokenType::IDENTIFIER) {
            // 允许标识符作为值（用于未加引号的字符串或数字）
            // 这在GUI生成的SQL中可能出现，因为某些值可能没有正确加引号
            node->values.push_back(m_currentToken.value);
            advance();
        } else {
            // 如果遇到关键字，也接受其值（虽然不应该，但为了兼容性）
            // 这样可以处理值恰好是关键字的情况（例如：值"constraint"）
            // 但需要排除一些特殊关键字，这些关键字不应该作为值
            if (m_currentToken.type == TokenType::IN || 
                m_currentToken.type == TokenType::INTO ||
                m_currentToken.type == TokenType::VALUES ||
                m_currentToken.type == TokenType::RIGHT_PAREN ||
                m_currentToken.type == TokenType::COMMA ||
                m_currentToken.type == TokenType::SEMICOLON) {
                // 这些是语法关键字，不应该作为值
                setError("Expected value (string, number, or identifier), but got: " + m_currentToken.value);
                return nullptr;
            }
            // 其他关键字（如CONSTRAINT, UNIQUE等）可以作为值（虽然不应该）
            node->values.push_back(m_currentToken.value);
            advance();
        }
        
        // 检查是否有逗号（继续解析）或右括号（结束）
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else if (m_currentToken.type == TokenType::RIGHT_PAREN) {
            advance();
            break;
        } else {
            setError("Expected ',' or ')', but got: " + m_currentToken.value);
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
        setError("Expected value (string or number), but got: " + m_currentToken.value);
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
    
    // Value1（支持负数）
    if (m_currentToken.type == TokenType::STRING_LITERAL) {
        node->setValue = m_currentToken.value;
        advance();
    } else if (m_currentToken.type == TokenType::NUMBER) {
        node->setValue = m_currentToken.value;
        advance();
    } else if (m_currentToken.type == TokenType::ERROR && m_currentToken.value == "-") {
        // 处理负数：-后面跟数字
        advance();  // 跳过'-'
        if (m_currentToken.type == TokenType::NUMBER) {
            // 组合成负数
            node->setValue = "-" + m_currentToken.value;
            advance();
        } else {
            setError("Expected number after '-', but got: " + m_currentToken.value);
            return nullptr;
        }
    } else {
        setError("Expected value (string or number), but got: " + m_currentToken.value);
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
    
    // Value2（支持负数）
    if (m_currentToken.type == TokenType::STRING_LITERAL) {
        node->whereValue = m_currentToken.value;
        advance();
    } else if (m_currentToken.type == TokenType::NUMBER) {
        node->whereValue = m_currentToken.value;
        advance();
    } else if (m_currentToken.type == TokenType::ERROR && m_currentToken.value == "-") {
        // 处理负数：-后面跟数字
        advance();  // 跳过'-'
        if (m_currentToken.type == TokenType::NUMBER) {
            // 组合成负数
            node->whereValue = "-" + m_currentToken.value;
            advance();
        } else {
            setError("Expected number after '-', but got: " + m_currentToken.value);
            return nullptr;
        }
    } else {
        setError("Expected value (string or number), but got: " + m_currentToken.value);
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

