/**
 * @file ddl_executor.cpp
 * @brief DDL执行器实现
 */

#include "ddl/ddl_executor.h"
#include "sql_parser/lexer.h"
#include <algorithm>
#include <cctype>
#include <iostream>

DDLExecutor::DDLExecutor() {
}

DDLExecutor::~DDLExecutor() {
}

bool DDLExecutor::execute(const std::string& sql) {
    m_lastError = "";
    
    if (sql.empty()) {
        setError("SQL statement is empty");
        return false;
    }
    
    // 识别语句类型
    StatementType type = identifyStatementType(sql);
    
    // 根据类型选择处理器
    bool result = false;
    switch (type) {
        case StatementType::CREATE_TABLE:
            result = m_createHandler.execute(sql);
            if (!result) {
                setError("CREATE TABLE execution failed: " + m_createHandler.getLastError());
            }
            break;
            
        case StatementType::EDIT_TABLE:
            result = m_editHandler.execute(sql);
            if (!result) {
                setError("EDIT TABLE execution failed: " + m_editHandler.getLastError());
            }
            break;
            
        case StatementType::RENAME_TABLE:
            result = m_renameHandler.execute(sql);
            if (!result) {
                setError("RENAME TABLE execution failed: " + m_renameHandler.getLastError());
            }
            break;
            
        case StatementType::DROP_TABLE:
            result = m_dropHandler.execute(sql);
            if (!result) {
                setError("DROP TABLE execution failed: " + m_dropHandler.getLastError());
            }
            break;
            
        case StatementType::UNKNOWN:
        default:
            setError("Unknown DDL statement type");
            return false;
    }
    
    return result;
}

std::string DDLExecutor::getLastError() const {
    return m_lastError;
}

void DDLExecutor::setDatabasePath(const std::string& dbPath) {
    m_databasePath = dbPath;
    // 注意：各个处理器会在执行时从SQL中解析数据库路径
    // 这里可以预留，如果将来需要统一设置路径
}

bool DDLExecutor::isDDLStatement(const std::string& sql) {
    // 简单的关键字检查
    std::string upperSql = sql;
    std::transform(upperSql.begin(), upperSql.end(), upperSql.begin(), ::toupper);
    
    // 去除前导空白
    size_t start = upperSql.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return false;
    }
    upperSql = upperSql.substr(start);
    
    // 检查DDL关键字
    if (upperSql.find("CREATE TABLE") == 0 ||
        upperSql.find("EDIT TABLE") == 0 ||
        upperSql.find("RENAME TABLE") == 0 ||
        upperSql.find("DROP TABLE") == 0) {
        return true;
    }
    
    return false;
}

DDLExecutor::StatementType DDLExecutor::identifyStatementType(const std::string& sql) {
    // 使用词法分析器识别关键字
    Lexer lexer(sql);
    Token token = lexer.nextToken();
    
    // Lexer会自动跳过空白，所以直接检查第一个关键字
    // 检查第一个关键字
    if (token.type == TokenType::CREATE) {
        // 检查下一个token是否为TABLE
        Token nextToken = lexer.nextToken();
        if (nextToken.type == TokenType::TABLE) {
            return StatementType::CREATE_TABLE;
        }
    } else if (token.type == TokenType::EDIT) {
        // 检查下一个token是否为TABLE
        Token nextToken = lexer.nextToken();
        if (nextToken.type == TokenType::TABLE) {
            return StatementType::EDIT_TABLE;
        }
    } else if (token.type == TokenType::RENAME) {
        // 检查下一个token是否为TABLE
        Token nextToken = lexer.nextToken();
        if (nextToken.type == TokenType::TABLE) {
            return StatementType::RENAME_TABLE;
        }
    } else if (token.type == TokenType::DROP) {
        // 检查下一个token是否为TABLE
        Token nextToken = lexer.nextToken();
        if (nextToken.type == TokenType::TABLE) {
            return StatementType::DROP_TABLE;
        }
    }
    
    return StatementType::UNKNOWN;
}

void DDLExecutor::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "DDLExecutor错误: " << error << std::endl;
}

