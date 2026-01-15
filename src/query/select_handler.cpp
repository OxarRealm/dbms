/**
 * @file select_handler.cpp
 * @brief SELECT查询处理器实现
 */

#include "query/select_handler.h"
#include "core/table_mode.h"
#include <algorithm>
#include <cstring>
#ifdef _WIN32
#include <string.h>
#else
#include <strings.h>
#endif

SelectHandler::SelectHandler() {
}

SelectHandler::~SelectHandler() {
}

bool SelectHandler::execute(const std::string& sql, QueryResult& result) {
    m_lastError = "";
    result = QueryResult();  // 清空结果
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> ast = parser.parse();
    
    if (!ast) {
        setError("SQL parsing failed: " + parser.getLastError());
        return false;
    }
    
    // 检查是否为SELECT节点
    SelectNode* selectNode = dynamic_cast<SelectNode*>(ast.get());
    if (!selectNode) {
        setError("Not a SELECT statement");
        return false;
    }
    
    // 根据查询类型选择执行方式
    if (selectNode->fromTables.size() == 1) {
        // 单表查询
        return executeSingleTableQuery(selectNode, result);
    } else if (!selectNode->joins.empty()) {
        // JOIN查询
        return executeJoinQuery(selectNode, result);
    } else {
        // 多表查询（笛卡尔积）
        return executeMultiTableQuery(selectNode, result);
    }
}

std::string SelectHandler::getLastError() const {
    return m_lastError;
}

void SelectHandler::setDatabasePath(const std::string& dbPath) {
    m_tableManager.setDatabasePath(dbPath);
    m_dataManager.setDatabasePath(dbPath);
}

void SelectHandler::setError(const std::string& error) {
    m_lastError = error;
}

bool SelectHandler::executeSingleTableQuery(SelectNode* node, QueryResult& result) {
    std::string tableName = node->fromTables[0];
    
    // 读取表结构
    TableInfo tableInfo;
    if (!m_tableManager.readTable(tableName, tableInfo)) {
        setError("Table does not exist: " + tableName);
        return false;
    }
    
    // 注意：数据库路径应该通过setDatabasePath方法提前设置
    
    // 读取所有有效记录
    std::vector<Record> records;
    if (!m_dataManager.readValidRecords(tableName, records)) {
        setError("Failed to read records");
        return false;
    }
    
    // 确定要选择的字段
    std::vector<std::string> selectFields = node->selectFields;
    if (selectFields.empty() || (selectFields.size() == 1 && selectFields[0] == "*")) {
        // SELECT * 表示选择所有字段
        selectFields.clear();
        for (const auto& field : tableInfo.fields) {
            selectFields.push_back(std::string(field.sFieldName));
        }
    }
    
    // 设置列名
    result.columnNames = selectFields;
    
    // 处理每条记录
    for (const auto& record : records) {
        // 如果有WHERE子句，先评估条件
        if (!node->whereField.empty()) {
            if (!evaluateCondition(record, tableInfo, node->whereField, node->whereValue)) {
                continue;  // 不匹配，跳过这条记录
            }
        }
        
        // 执行投影操作
        std::vector<std::string> row;
        if (!projectFields(record, tableInfo, selectFields, row)) {
            setError("Projection operation failed");
            return false;
        }
        
        result.rows.push_back(row);
    }
    
    result.rowCount = result.rows.size();
    return true;
}

bool SelectHandler::evaluateCondition(const Record& record, const TableInfo& tableInfo, 
                                      const std::string& conditionField, const std::string& conditionValue) {
    // 查找字段索引
    int fieldIndex = findFieldIndex(tableInfo, conditionField);
    if (fieldIndex == -1 || fieldIndex >= static_cast<int>(record.values.size())) {
        return false;
    }
    
    // 获取字段值
    const std::string& fieldValue = record.values[fieldIndex];
    
    // 简单相等比较（字符串比较）
    return fieldValue == conditionValue;
}

int SelectHandler::findFieldIndex(const TableInfo& tableInfo, const std::string& fieldName) {
    for (size_t i = 0; i < tableInfo.fields.size(); ++i) {
        if (strcmp(tableInfo.fields[i].sFieldName, fieldName.c_str()) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool SelectHandler::projectFields(const Record& record, const TableInfo& tableInfo, 
                                  const std::vector<std::string>& selectFields, 
                                  std::vector<std::string>& row) {
    row.clear();
    
    for (const std::string& fieldName : selectFields) {
        int fieldIndex = findFieldIndex(tableInfo, fieldName);
        if (fieldIndex == -1) {
            setError("字段不存在: " + fieldName);
            return false;
        }
        
        if (fieldIndex >= static_cast<int>(record.values.size())) {
            setError("记录字段数量不匹配");
            return false;
        }
        
        row.push_back(record.values[fieldIndex]);
    }
    
    return true;
}

bool SelectHandler::executeMultiTableQuery(SelectNode* node, QueryResult& result) {
    // 读取所有表的结构和数据
    std::vector<TableInfo> tableInfos;
    std::vector<std::vector<Record>> allTableRecords;
    
    for (const std::string& tableName : node->fromTables) {
        // 读取表结构
        TableInfo tableInfo;
        if (!m_tableManager.readTable(tableName, tableInfo)) {
            setError("Table does not exist: " + tableName);
            return false;
        }
        tableInfos.push_back(tableInfo);
        
        // 读取表数据
        std::vector<Record> records;
        if (!m_dataManager.readValidRecords(tableName, records)) {
            setError("Failed to read table data: " + tableName);
            return false;
        }
        allTableRecords.push_back(records);
    }
    
    // 确定要选择的字段
    std::vector<std::string> selectFields = node->selectFields;
    if (selectFields.empty() || (selectFields.size() == 1 && selectFields[0] == "*")) {
        // SELECT * 表示选择所有字段（所有表的所有字段）
        selectFields.clear();
        for (size_t i = 0; i < tableInfos.size(); ++i) {
            for (const auto& field : tableInfos[i].fields) {
                // 格式：TableName.FieldName
                std::string qualifiedName = std::string(tableInfos[i].tableName) + "." + std::string(field.sFieldName);
                selectFields.push_back(qualifiedName);
            }
        }
    }
    
    // 设置列名
    result.columnNames = selectFields;
    
    // 计算笛卡尔积
    // 使用递归方式生成所有可能的记录组合
    std::vector<std::vector<Record>> cartesianProduct;
    generateCartesianProduct(allTableRecords, 0, std::vector<Record>(), cartesianProduct);
    
    // 处理每条组合记录
    for (const auto& recordCombination : cartesianProduct) {
        // 将多个表的记录组合成一个记录（字段值拼接）
        std::vector<std::string> combinedRecord;
        for (size_t i = 0; i < recordCombination.size(); ++i) {
            const Record& record = recordCombination[i];
            for (const std::string& value : record.values) {
                combinedRecord.push_back(value);
            }
        }
        
        // 如果有WHERE子句，先评估条件
        if (!node->whereField.empty()) {
            if (!evaluateMultiTableCondition(combinedRecord, tableInfos, node->whereField, node->whereValue)) {
                continue;  // 不匹配，跳过这条记录
            }
        }
        
        // 执行投影操作
        std::vector<std::string> row;
        if (!projectMultiTableFields(combinedRecord, tableInfos, selectFields, row)) {
            setError("Projection operation failed");
            return false;
        }
        
        result.rows.push_back(row);
    }
    
    result.rowCount = result.rows.size();
    return true;
}

void SelectHandler::generateCartesianProduct(const std::vector<std::vector<Record>>& allTableRecords, 
                                             size_t tableIndex, 
                                             std::vector<Record> currentCombination,
                                             std::vector<std::vector<Record>>& result) {
    if (tableIndex >= allTableRecords.size()) {
        // 所有表都已处理，保存当前组合
        result.push_back(currentCombination);
        return;
    }
    
    // 对当前表的每条记录，递归处理下一个表
    for (const Record& record : allTableRecords[tableIndex]) {
        std::vector<Record> newCombination = currentCombination;
        newCombination.push_back(record);
        generateCartesianProduct(allTableRecords, tableIndex + 1, newCombination, result);
    }
}

bool SelectHandler::projectMultiTableFields(const std::vector<std::string>& combinedRecord, 
                                           const std::vector<TableInfo>& tableInfos,
                                           const std::vector<std::string>& selectFields, 
                                           std::vector<std::string>& row) {
    row.clear();
    
    for (const std::string& fieldName : selectFields) {
        size_t tableIndex;
        int fieldIndex;
        if (!findMultiTableFieldIndex(tableInfos, fieldName, tableIndex, fieldIndex)) {
            setError("字段不存在: " + fieldName);
            return false;
        }
        
        // 计算在combinedRecord中的索引
        size_t combinedIndex = 0;
        for (size_t i = 0; i < tableIndex; ++i) {
            combinedIndex += tableInfos[i].fields.size();
        }
        combinedIndex += fieldIndex;
        
        if (combinedIndex >= combinedRecord.size()) {
            setError("记录字段数量不匹配");
            return false;
        }
        
        row.push_back(combinedRecord[combinedIndex]);
    }
    
    return true;
}

bool SelectHandler::evaluateMultiTableCondition(const std::vector<std::string>& combinedRecord, 
                                                 const std::vector<TableInfo>& tableInfos,
                                                 const std::string& whereField, 
                                                 const std::string& whereValue) {
    size_t tableIndex;
    int fieldIndex;
    if (!findMultiTableFieldIndex(tableInfos, whereField, tableIndex, fieldIndex)) {
        return false;
    }
    
    // 计算在combinedRecord中的索引
    size_t combinedIndex = 0;
    for (size_t i = 0; i < tableIndex; ++i) {
        combinedIndex += tableInfos[i].fields.size();
    }
    combinedIndex += fieldIndex;
    
    if (combinedIndex >= combinedRecord.size()) {
        return false;
    }
    
    // 获取字段值
    const std::string& fieldValue = combinedRecord[combinedIndex];
    
    // 简单相等比较（字符串比较）
    return fieldValue == whereValue;
}

bool SelectHandler::findMultiTableFieldIndex(const std::vector<TableInfo>& tableInfos, 
                                             const std::string& fieldName,
                                             size_t& tableIndex, int& fieldIndex) {
    // 检查字段名格式：TableName.FieldName 或 FieldName
    size_t dotPos = fieldName.find('.');
    
    if (dotPos != std::string::npos) {
        // 格式：TableName.FieldName
        std::string tableName = fieldName.substr(0, dotPos);
        std::string fieldNameOnly = fieldName.substr(dotPos + 1);
        
        // 查找表
        for (size_t i = 0; i < tableInfos.size(); ++i) {
            if (strcasecmp_custom(tableInfos[i].tableName, tableName.c_str()) == 0) {
                // 查找字段
                for (size_t j = 0; j < tableInfos[i].fields.size(); ++j) {
                    if (strcmp(tableInfos[i].fields[j].sFieldName, fieldNameOnly.c_str()) == 0) {
                        tableIndex = i;
                        fieldIndex = static_cast<int>(j);
                        return true;
                    }
                }
            }
        }
    } else {
        // 格式：FieldName（需要在所有表中查找，如果多个表有同名字段，返回第一个）
        for (size_t i = 0; i < tableInfos.size(); ++i) {
            for (size_t j = 0; j < tableInfos[i].fields.size(); ++j) {
                if (strcmp(tableInfos[i].fields[j].sFieldName, fieldName.c_str()) == 0) {
                    tableIndex = i;
                    fieldIndex = static_cast<int>(j);
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool SelectHandler::executeJoinQuery(SelectNode* node, QueryResult& result) {
    // 读取所有表的结构和数据
    std::vector<TableInfo> tableInfos;
    std::vector<std::vector<Record>> allTableRecords;
    
    for (const std::string& tableName : node->fromTables) {
        // 读取表结构
        TableInfo tableInfo;
        if (!m_tableManager.readTable(tableName, tableInfo)) {
            setError("Table does not exist: " + tableName);
            return false;
        }
        tableInfos.push_back(tableInfo);
        
        // 读取表数据
        std::vector<Record> records;
        if (!m_dataManager.readValidRecords(tableName, records)) {
            setError("Failed to read table data: " + tableName);
            return false;
        }
        allTableRecords.push_back(records);
    }
    
    // 确定要选择的字段
    std::vector<std::string> selectFields = node->selectFields;
    if (selectFields.empty() || (selectFields.size() == 1 && selectFields[0] == "*")) {
        // SELECT * 表示选择所有字段（所有表的所有字段）
        selectFields.clear();
        for (size_t i = 0; i < tableInfos.size(); ++i) {
            for (const auto& field : tableInfos[i].fields) {
                // 格式：TableName.FieldName
                std::string qualifiedName = std::string(tableInfos[i].tableName) + "." + std::string(field.sFieldName);
                selectFields.push_back(qualifiedName);
            }
        }
    }
    
    // 设置列名
    result.columnNames = selectFields;
    
    // 执行JOIN连接
    // 从第一个表开始，逐步连接后续表
    std::vector<std::vector<Record>> joinedResults;
    
    // 初始化：第一个表的所有记录
    for (const Record& record : allTableRecords[0]) {
        std::vector<Record> combination;
        combination.push_back(record);
        joinedResults.push_back(combination);
    }
    
    // 逐步连接后续表
    for (size_t joinIndex = 0; joinIndex < node->joins.size(); ++joinIndex) {
        const JoinInfo& joinInfo = node->joins[joinIndex];
        size_t rightTableIndex = joinIndex + 1;  // 右表索引
        
        if (rightTableIndex >= allTableRecords.size()) {
            setError("JOIN表索引超出范围");
            return false;
        }
        
        // 查找左表和右表的字段索引
        size_t leftTableIndex = 0;  // 默认从第一个表开始
        int leftFieldIndex = -1, rightFieldIndex = -1;
        
        // 查找左表字段（可能在前面的连接结果中）
        // 简化处理：假设左表字段在已连接的表中的第一个表
        for (size_t i = 0; i < rightTableIndex; ++i) {
            size_t tempTableIndex;
            int tempFieldIndex;
            if (findMultiTableFieldIndex(std::vector<TableInfo>(tableInfos.begin(), tableInfos.begin() + rightTableIndex),
                                        joinInfo.leftField, tempTableIndex, tempFieldIndex)) {
                leftTableIndex = tempTableIndex;
                leftFieldIndex = tempFieldIndex;
                break;
            }
        }
        
        // 查找右表字段
        if (!findMultiTableFieldIndex(std::vector<TableInfo>(1, tableInfos[rightTableIndex]),
                                      joinInfo.rightField, leftTableIndex, rightFieldIndex)) {
            // 尝试简单字段名
            for (size_t j = 0; j < tableInfos[rightTableIndex].fields.size(); ++j) {
                if (strcmp(tableInfos[rightTableIndex].fields[j].sFieldName, joinInfo.rightField.c_str()) == 0) {
                    rightFieldIndex = static_cast<int>(j);
                    break;
                }
            }
        }
        
        if (leftFieldIndex == -1 || rightFieldIndex == -1) {
            setError("JOIN条件字段不存在");
            return false;
        }
        
        // 执行连接
        std::vector<std::vector<Record>> newJoinedResults;
        
        for (const auto& leftCombination : joinedResults) {
            // 获取左表字段值
            if (leftCombination.size() <= leftTableIndex || 
                leftCombination[leftTableIndex].values.size() <= static_cast<size_t>(leftFieldIndex)) {
                continue;
            }
            const std::string& leftValue = leftCombination[leftTableIndex].values[leftFieldIndex];
            
            // 在右表中查找匹配的记录
            bool foundMatch = false;
            for (const Record& rightRecord : allTableRecords[rightTableIndex]) {
                if (rightRecord.values.size() <= static_cast<size_t>(rightFieldIndex)) {
                    continue;
                }
                const std::string& rightValue = rightRecord.values[rightFieldIndex];
                
                // 检查连接条件
                bool matches = false;
                if (joinInfo.operator_ == "=") {
                    matches = (leftValue == rightValue);
                } else {
                    setError("不支持的JOIN运算符: " + joinInfo.operator_);
                    return false;
                }
                
                if (matches) {
                    // 匹配：创建新的组合
                    std::vector<Record> newCombination = leftCombination;
                    newCombination.push_back(rightRecord);
                    newJoinedResults.push_back(newCombination);
                    foundMatch = true;
                }
            }
            
            // LEFT JOIN：即使没有匹配，也保留左表记录（右表字段为NULL）
            if (joinInfo.joinType == "LEFT" && !foundMatch) {
                // 创建空记录作为右表记录（使用空值）
                Record emptyRecord;
                // 填充空值
                for (size_t j = 0; j < tableInfos[rightTableIndex].fields.size(); ++j) {
                    emptyRecord.values.push_back("");
                }
                std::vector<Record> newCombination = leftCombination;
                newCombination.push_back(emptyRecord);
                newJoinedResults.push_back(newCombination);
            }
        }
        
        joinedResults = newJoinedResults;
    }
    
    // 处理每条连接结果
    for (const auto& recordCombination : joinedResults) {
        // 将多个表的记录组合成一个记录（字段值拼接）
        std::vector<std::string> combinedRecord;
        for (size_t i = 0; i < recordCombination.size(); ++i) {
            const Record& record = recordCombination[i];
            // 如果记录值为空（LEFT JOIN的NULL情况），填充空字符串
            if (record.values.empty() && i < tableInfos.size()) {
                for (size_t j = 0; j < tableInfos[i].fields.size(); ++j) {
                    combinedRecord.push_back("");
                }
            } else {
                for (const std::string& value : record.values) {
                    combinedRecord.push_back(value);
                }
            }
        }
        
        // 如果有WHERE子句，先评估条件
        if (!node->whereField.empty()) {
            if (!evaluateMultiTableCondition(combinedRecord, tableInfos, node->whereField, node->whereValue)) {
                continue;  // 不匹配，跳过这条记录
            }
        }
        
        // 执行投影操作
        std::vector<std::string> row;
        if (!projectMultiTableFields(combinedRecord, tableInfos, selectFields, row)) {
            setError("Projection operation failed");
            return false;
        }
        
        result.rows.push_back(row);
    }
    
    result.rowCount = result.rows.size();
    return true;
}

