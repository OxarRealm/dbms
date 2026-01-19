/**
 * @file query_executor.cpp
 * @brief 查询执行器实现
 */

#include "query/query_executor.h"
#include "core/index_storage.h"
#include "core/session_manager.h"
#include "sql_parser/lexer.h"
#include "sql_parser/parser.h"
#include "sql_parser/ast_node.h"
#include "core/user_mode.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <string>
#include <iostream>

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
            // executeDDL已经设置了formattedOutput（可能包含调试信息），不要覆盖它
            if (result.formattedOutput.empty()) {
                result.formattedOutput = "DDL statement executed successfully";
            }
        }
    } else if (statementType == "DML") {
        if (!executeDML(sql, result)) {
            result.type = ExecutionResultType::ERROR;
            result.errorMessage = m_lastError;
        } else {
            result.type = ExecutionResultType::SUCCESS;
            std::ostringstream oss;
            oss << "DML statement executed successfully, " << result.affectedRows << " row(s) affected";
            if (result.affectedRows == 0) {
                oss << " (no matching records found)";
            }
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
        result.errorMessage = "Unknown SQL statement type";
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
    m_indexManager.setDatabasePath(dbPath);
    m_permissionManager.setDatabasePath(dbPath);
    m_roleManager.setDatabasePath(dbPath);
    
    // Load indices from .idx file to ensure we can detect index usage
    if (!dbPath.empty()) {
        std::string dbName = extractDatabaseName(dbPath);
        if (!dbName.empty()) {
            IndexStorageManager::loadIndices(dbName, dbPath, m_indexManager);
        }
    }
    
    // Set IndexManager to IndexAdvisor so it can check for existing indices
    m_indexAdvisor.setIndexManager(&m_indexManager);
}

// Helper function to extract database name from path
std::string QueryExecutor::extractDatabaseName(const std::string& dbPath) {
    if (dbPath.empty()) {
        return "";
    }
    
    // Find the last path separator
    size_t lastSlash = dbPath.find_last_of("/\\");
    size_t lastDot = dbPath.find_last_of('.');
    
    // Extract the filename without extension
    std::string filename;
    if (lastSlash != std::string::npos) {
        filename = dbPath.substr(lastSlash + 1);
    } else {
        filename = dbPath;
    }
    
    // Remove extension if present
    if (lastDot != std::string::npos && lastDot > (lastSlash == std::string::npos ? 0 : lastSlash + 1)) {
        filename = filename.substr(0, filename.find_last_of('.'));
    }
    
    return filename;
}

IndexAdvisor& QueryExecutor::getIndexAdvisor() {
    return m_indexAdvisor;
}

IndexManager* QueryExecutor::getIndexManager() {
    return &m_indexManager;
}

std::string QueryExecutor::getLastError() const {
    return m_lastError;
}

std::string QueryExecutor::formatQueryResult(const QueryResult& result) {
    if (result.columnNames.empty()) {
        return "Query result is empty";
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
    
    oss << "Total " << result.rowCount << " row(s)";
    
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
    
    // 对于REVOKE语句，添加调试信息到formattedOutput
    std::string upperSql = sql;
    std::transform(upperSql.begin(), upperSql.end(), upperSql.begin(), ::toupper);
    if (upperSql.find("REVOKE") == 0) {
        // 从DDLExecutor获取最后的错误信息（可能包含调试信息）
        std::string lastError = m_ddlExecutor.getLastError();
        if (!lastError.empty() && lastError.find("REVOKE executed") != std::string::npos) {
            // 包含调试信息
            result.formattedOutput = lastError;
        } else if (!lastError.empty()) {
            // 只有错误信息
            result.formattedOutput = "REVOKE statement executed successfully. " + lastError;
        } else {
            result.formattedOutput = "REVOKE statement executed successfully";
        }
    } else {
        result.formattedOutput = "DDL statement executed successfully";
    }
    
    return true;
}

bool QueryExecutor::executeDML(const std::string& sql, ExecutionResult& result) {
    // 解析SQL以提取表名和操作类型
    Parser parser(sql);
    std::unique_ptr<ASTNode> ast = parser.parse();
    
    if (!ast) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    std::string tableName = "";
    PermissionType permissionType = PermissionType::SELECT;
    
    // 识别DML语句类型并提取表名
    if (InsertNode* insertNode = dynamic_cast<InsertNode*>(ast.get())) {
        tableName = insertNode->tableName;
        permissionType = PermissionType::INSERT;
    } else if (UpdateNode* updateNode = dynamic_cast<UpdateNode*>(ast.get())) {
        tableName = updateNode->tableName;
        permissionType = PermissionType::UPDATE;
    } else if (DeleteNode* deleteNode = dynamic_cast<DeleteNode*>(ast.get())) {
        tableName = deleteNode->tableName;
        permissionType = PermissionType::DELETE;
    }
    
    // 权限检查
    if (!tableName.empty() && !checkPermission(OBJECT_TYPE_TABLE, tableName, permissionType)) {
        std::string permissionStr = (permissionType == PermissionType::INSERT) ? "INSERT" :
                                    (permissionType == PermissionType::UPDATE) ? "UPDATE" : "DELETE";
        setError("Permission denied: User does not have " + permissionStr + " permission on table '" + tableName + "'");
        return false;
    }
    
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
    result.usedIndices.clear();  // 清空索引使用列表
    
    if (ast) {
        SelectNode* selectNode = dynamic_cast<SelectNode*>(ast.get());
        if (selectNode && !selectNode->fromTables.empty()) {
            tableName = selectNode->fromTables[0];  // 单表查询，取第一个表
            
            // 权限检查：检查SELECT权限
            if (!checkPermission(OBJECT_TYPE_TABLE, tableName, PermissionType::SELECT)) {
                setError("Permission denied: User does not have SELECT permission on table '" + tableName + "'");
                return false;
            }
            
            // 提取WHERE字段（简单字段或从WHERE条件中提取）
            if (!selectNode->whereField.empty()) {
                whereFields.push_back(selectNode->whereField);
            } else if (selectNode->whereClause) {
                // 从复杂WHERE条件中提取字段（简化实现：只检查简单条件）
                extractWhereFields(selectNode->whereClause.get(), whereFields);
            }
            
            // 检测索引使用：检查WHERE字段是否有索引
            if (!tableName.empty() && !whereFields.empty()) {
                for (const std::string& fieldName : whereFields) {
                    // 检查hash索引
                    if (m_indexManager.hasIndex(tableName, fieldName, "hash")) {
                        result.usedIndices.push_back(tableName + "." + fieldName + "(hash)");
                    }
                    // 检查adjacent索引
                    if (m_indexManager.hasIndex(tableName, fieldName, "adjacent")) {
                        result.usedIndices.push_back(tableName + "." + fieldName + "(adjacent)");
                    }
                    // 检查btree索引
                    if (m_indexManager.hasIndex(tableName, fieldName, "btree")) {
                        result.usedIndices.push_back(tableName + "." + fieldName + "(btree)");
                    }
                }
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

// 辅助函数：从WHERE条件中提取字段名
void QueryExecutor::extractWhereFields(const WhereCondition* condition, std::vector<std::string>& fields) {
    if (!condition) return;
    
    // 如果是简单条件（有字段名）
    if (!condition->fieldName.empty()) {
        // 提取字段名（可能包含表名，如 "TableName.FieldName"）
        std::string fieldName = condition->fieldName;
        // 如果有点号，取点号后的部分作为字段名
        size_t dotPos = fieldName.find('.');
        if (dotPos != std::string::npos) {
            fieldName = fieldName.substr(dotPos + 1);
        }
        // 避免重复添加
        if (std::find(fields.begin(), fields.end(), fieldName) == fields.end()) {
            fields.push_back(fieldName);
        }
    }
    
    // 递归处理左右子条件
    if (condition->left) {
        extractWhereFields(condition->left.get(), fields);
    }
    if (condition->right) {
        extractWhereFields(condition->right.get(), fields);
    }
}

bool QueryExecutor::checkPermission(char objectType, const std::string& objectName, PermissionType permissionType) {
    // 获取当前用户
    std::string currentUser = SessionManager::getInstance().getCurrentUser();
    
    // 如果未登录，拒绝所有操作（除了admin）
    if (currentUser.empty()) {
        std::cerr << "Debug: No user logged in, permission denied" << std::endl;
        return false;
    }
    
    // admin用户拥有所有权限
    if (currentUser == "admin") {
        return true;
    }
    
    // 关键修复：在权限检查前重新加载权限数据，确保使用最新的权限信息
    // 这很重要，因为权限可能在运行时被修改（通过GRANT/REVOKE）
    if (!m_databasePath.empty()) {
        m_permissionManager.setDatabasePath(m_databasePath);  // 这会触发loadPermissions()
        m_roleManager.setDatabasePath(m_databasePath);        // 这会触发loadRoles()和loadUserRoles()
    }
    
    // 获取用户的角色
    std::vector<std::string> userRoles;
    m_roleManager.getUserRoles(currentUser, userRoles);
    
    // 检查用户权限（包括角色权限）
    bool hasPerm = m_permissionManager.hasPermission(currentUser, objectType, objectName, permissionType, userRoles);
    
    // 调试信息
    std::cerr << "Debug: Permission check - User: " << currentUser 
              << ", Object: " << objectName 
              << ", Permission: " << static_cast<int>(permissionType)
              << ", HasPermission: " << (hasPerm ? "Yes" : "No") << std::endl;
    
    return hasPerm;
}

