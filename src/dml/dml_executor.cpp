/**
 * @file dml_executor.cpp
 * @brief DML执行器实现
 */

#include "dml/dml_executor.h"
#include "sql_parser/lexer.h"
#include <algorithm>
#include <cctype>
#include <iostream>

DMLExecutor::DMLExecutor() : m_affectedRows(0) {
}

DMLExecutor::~DMLExecutor() {
}

bool DMLExecutor::execute(const std::string& sql) {
    m_lastError = "";
    m_affectedRows = 0;
    
    if (sql.empty()) {
        setError("SQL statement is empty");
        return false;
    }
    
    // 识别语句类型
    StatementType type = identifyStatementType(sql);
    
    // 根据类型选择处理器
    bool result = false;
    switch (type) {
        case StatementType::INSERT:
            result = m_insertHandler.execute(sql);
            if (result) {
                m_affectedRows = 1;  // INSERT通常影响1条记录
            } else {
                setError("INSERT execution failed: " + m_insertHandler.getLastError());
            }
            break;
            
        case StatementType::DELETE:
            result = m_deleteHandler.execute(sql);
            if (result) {
                m_affectedRows = m_deleteHandler.getDeletedCount();
            } else {
                setError("DELETE execution failed: " + m_deleteHandler.getLastError());
            }
            break;
            
        case StatementType::UPDATE:
            result = m_updateHandler.execute(sql);
            if (result) {
                m_affectedRows = m_updateHandler.getUpdatedCount();
            } else {
                setError("UPDATE execution failed: " + m_updateHandler.getLastError());
            }
            break;
            
        case StatementType::UNKNOWN:
        default:
            setError("Unknown DML statement type");
            return false;
    }
    
    return result;
}

std::string DMLExecutor::getLastError() const {
    return m_lastError;
}

void DMLExecutor::setDatabasePath(const std::string& dbPath) {
    m_databasePath = dbPath;
    // 注意：各个处理器会在执行时从SQL中解析数据库路径
    // 这里可以预留，如果将来需要统一设置路径
}

size_t DMLExecutor::getAffectedRows() const {
    return m_affectedRows;
}

bool DMLExecutor::isDMLStatement(const std::string& sql) {
    // 简单的关键字检查
    std::string upperSql = sql;
    std::transform(upperSql.begin(), upperSql.end(), upperSql.begin(), ::toupper);
    
    // 去除前导空白
    size_t start = upperSql.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return false;
    }
    upperSql = upperSql.substr(start);
    
    // 检查DML关键字
    if (upperSql.find("INSERT INTO") == 0 ||
        upperSql.find("DELETE FROM") == 0 ||
        upperSql.find("UPDATE") == 0) {
        return true;
    }
    
    return false;
}

DMLExecutor::StatementType DMLExecutor::identifyStatementType(const std::string& sql) {
    // 使用词法分析器识别关键字
    Lexer lexer(sql);
    Token token = lexer.nextToken();
    
    // Lexer会自动跳过空白，所以直接检查第一个关键字
    if (token.type == TokenType::INSERT) {
        return StatementType::INSERT;
    } else if (token.type == TokenType::DELETE) {
        return StatementType::DELETE;
    } else if (token.type == TokenType::UPDATE) {
        return StatementType::UPDATE;
    }
    
    return StatementType::UNKNOWN;
}

void DMLExecutor::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "DMLExecutor错误: " << error << std::endl;
}

