/**
 * @file query_executor.cpp
 * @brief 查询执行器实现
 */

#include "query/query_executor.h"
#include "sql_parser/lexer.h"
#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <chrono>

QueryExecutor::QueryExecutor() {
}

QueryExecutor::~QueryExecutor() {
}

ExecutionResult QueryExecutor::execute(const std::string& sql) {
    m_lastError = "";
    ExecutionResult result;
    
    // 识别SQL语句类型
    std::string statementType = identifyStatementType(sql);
    
    if (statementType == "DDL") {
        if (!executeDDL(sql, result)) {
            result.type = ExecutionResultType::ERROR;
            result.errorMessage = m_lastError;
        } else {
            result.type = ExecutionResultType::SUCCESS;
            result.formattedOutput = "DDL语句执行成功";
        }
    } else if (statementType == "DML") {
        if (!executeDML(sql, result)) {
            result.type = ExecutionResultType::ERROR;
            result.errorMessage = m_lastError;
        } else {
            result.type = ExecutionResultType::SUCCESS;
            std::ostringstream oss;
            oss << "DML语句执行成功，影响 " << result.affectedRows << " 条记录";
            result.formattedOutput = oss.str();
        }
    } else if (statementType == "QUERY") {
        if (!executeQuery(sql, result)) {
            result.type = ExecutionResultType::ERROR;
            result.errorMessage = m_lastError;
        } else {
            result.type = ExecutionResultType::QUERY_RESULT;
            result.formattedOutput = formatQueryResult(result.queryResult);
        }
    } else {
        result.type = ExecutionResultType::ERROR;
        result.errorMessage = "未知的SQL语句类型";
        setError(result.errorMessage);
    }
    
    return result;
}

void QueryExecutor::setDatabasePath(const std::string& dbPath) {
    m_databasePath = dbPath;
    m_ddlExecutor.setDatabasePath(dbPath);
    m_dmlExecutor.setDatabasePath(dbPath);
    m_selectHandler.setDatabasePath(dbPath);
    m_indexAdvisor.setDatabasePath(dbPath);
}

IndexAdvisor& QueryExecutor::getIndexAdvisor() {
    return m_indexAdvisor;
}

std::string QueryExecutor::getLastError() const {
    return m_lastError;
}

std::string QueryExecutor::formatQueryResult(const QueryResult& result) {
    if (result.columnNames.empty()) {
        return "查询结果为空";
    }
    
    std::ostringstream oss;
    
    // 计算每列的最大宽度
    std::vector<size_t> columnWidths;
    for (size_t i = 0; i < result.columnNames.size(); ++i) {
        size_t maxWidth = result.columnNames[i].length();
        for (const auto& row : result.rows) {
            if (i < row.size() && row[i].length() > maxWidth) {
                maxWidth = row[i].length();
            }
        }
        columnWidths.push_back(std::max(maxWidth, size_t(3)));  // 至少3个字符宽度
    }
    
    // 打印表头
    oss << "+";
    for (size_t width : columnWidths) {
        for (size_t i = 0; i < width + 2; ++i) {
            oss << "-";
        }
        oss << "+";
    }
    oss << "\n";
    
    oss << "|";
    for (size_t i = 0; i < result.columnNames.size(); ++i) {
        oss << " " << std::left << std::setw(columnWidths[i]) << result.columnNames[i] << " |";
    }
    oss << "\n";
    
    oss << "+";
    for (size_t width : columnWidths) {
        for (size_t i = 0; i < width + 2; ++i) {
            oss << "-";
        }
        oss << "+";
    }
    oss << "\n";
    
    // 打印数据行
    for (const auto& row : result.rows) {
        oss << "|";
        for (size_t i = 0; i < result.columnNames.size(); ++i) {
            std::string value = (i < row.size()) ? row[i] : "";
            oss << " " << std::left << std::setw(columnWidths[i]) << value << " |";
        }
        oss << "\n";
    }
    
    // 打印底部边框
    oss << "+";
    for (size_t width : columnWidths) {
        for (size_t i = 0; i < width + 2; ++i) {
            oss << "-";
        }
        oss << "+";
    }
    oss << "\n";
    
    oss << "共 " << result.rowCount << " 行";
    
    return oss.str();
}

std::string QueryExecutor::identifyStatementType(const std::string& sql) {
    // 去除前后空白
    std::string trimmedSql = sql;
    while (!trimmedSql.empty() && std::isspace(trimmedSql[0])) {
        trimmedSql.erase(0, 1);
    }
    while (!trimmedSql.empty() && std::isspace(trimmedSql[trimmedSql.length() - 1])) {
        trimmedSql.erase(trimmedSql.length() - 1, 1);
    }
    
    if (trimmedSql.empty()) {
        return "UNKNOWN";
    }
    
    // 转换为大写进行比较
    std::string upperSql = trimmedSql;
    for (char& c : upperSql) {
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
    }
    
    // 检查是否为DDL语句
    if (DDLExecutor::isDDLStatement(trimmedSql)) {
        return "DDL";
    }
    
    // 检查是否为DML语句
    if (DMLExecutor::isDMLStatement(trimmedSql)) {
        return "DML";
    }
    
    // 检查是否为查询语句（SELECT）
    if (upperSql.find("SELECT") == 0) {
        return "QUERY";
    }
    
    return "UNKNOWN";
}

void QueryExecutor::setError(const std::string& error) {
    m_lastError = error;
}

bool QueryExecutor::executeDDL(const std::string& sql, ExecutionResult& result) {
    if (!m_ddlExecutor.execute(sql)) {
        setError(m_ddlExecutor.getLastError());
        return false;
    }
    result.affectedRows = 0;  // DDL语句不影响记录数
    return true;
}

bool QueryExecutor::executeDML(const std::string& sql, ExecutionResult& result) {
    if (!m_dmlExecutor.execute(sql)) {
        setError(m_dmlExecutor.getLastError());
        return false;
    }
    result.affectedRows = m_dmlExecutor.getAffectedRows();
    return true;
}

bool QueryExecutor::executeQuery(const std::string& sql, ExecutionResult& result) {
    // 记录查询开始时间
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 解析SQL以提取表名和WHERE字段
    Parser parser(sql);
    std::unique_ptr<ASTNode> ast = parser.parse();
    std::string tableName = "";
    std::vector<std::string> whereFields;
    
    if (ast) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(ast.get());
        if (selectNode && !selectNode->fromTables.empty()) {
            tableName = selectNode->fromTables[0];  // 单表查询，取第一个表
            if (!selectNode->whereField.empty()) {
                whereFields.push_back(selectNode->whereField);
            }
        }
    }
    
    // 执行查询
    bool success = m_selectHandler.execute(sql, result.queryResult);
    
    // 记录查询结束时间
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    double executionTimeMs = duration.count() / 1000.0;  // 转换为毫秒
    
    // 记录查询日志（无论成功与否，都记录用于分析）
    if (!tableName.empty()) {
        size_t resultCount = success ? result.queryResult.rowCount : 0;
        m_indexAdvisor.logQuery(sql, tableName, whereFields, executionTimeMs, resultCount);
    }
    
    if (!success) {
        setError(m_selectHandler.getLastError());
        return false;
    }
    
    result.affectedRows = result.queryResult.rowCount;
    return true;
}

