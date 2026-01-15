/**
 * @file select_handler.cpp
 * @brief SELECT查询处理器实现
 */

#include "query/select_handler.h"
#include "core/table_mode.h"
#include <algorithm>
#include <cstring>
#include <cmath>
#include <map>
#include <set>
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
    
    // 检查是否有GROUP BY或聚合函数
    bool hasGroupBy = !node->groupBy.empty();
    bool hasAggregate = false;
    if (!node->selectFieldsNew.empty()) {
        for (const auto& field : node->selectFieldsNew) {
            if (field.isAggregate) {
                hasAggregate = true;
                break;
            }
        }
    }
    
    // 如果有GROUP BY或聚合函数，使用分组查询
    if (hasGroupBy || hasAggregate) {
        return executeGroupByQuery(node, tableInfo, records, result);
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
        if (node->whereClause) {
            if (!evaluateWhereCondition(record, tableInfo, node->whereClause.get())) {
                continue;  // 不匹配，跳过这条记录
            }
        } else if (!node->whereField.empty()) {
            // 向后兼容：使用旧字段
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
    
    // DISTINCT去重
    if (node->distinct) {
        applyDistinct(result.rows);
    }
    
    // ORDER BY排序
    if (!node->orderBy.empty()) {
        if (!applyOrderBy(result.rows, result.columnNames, tableInfo, node->orderBy)) {
            setError("ORDER BY operation failed");
            return false;
        }
    }
    
    // LIMIT限制
    if (node->limitCount >= 0) {
        applyLimit(result.rows, node->limitCount);
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

bool SelectHandler::evaluateWhereCondition(const Record& record, const TableInfo& tableInfo, 
                                           const WhereCondition* condition) {
    if (!condition) {
        return true;
    }
    
    // 如果是简单条件
    if (condition->isSimple()) {
        // 查找字段索引
        int fieldIndex = findFieldIndex(tableInfo, condition->fieldName);
        if (fieldIndex == -1 || fieldIndex >= static_cast<int>(record.values.size())) {
            return false;
        }
        
        // 获取字段值
        const std::string& fieldValue = record.values[fieldIndex];
        
        // 根据运算符进行比较
        if (condition->operator_ == "=") {
            return fieldValue == condition->value;
        } else if (condition->operator_ == "!=") {
            return fieldValue != condition->value;
        } else if (condition->operator_ == ">") {
            // 尝试数值比较
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum > valueNum;
            } catch (...) {
                // 如果转换失败，使用字符串比较
                return fieldValue > condition->value;
            }
        } else if (condition->operator_ == "<") {
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum < valueNum;
            } catch (...) {
                return fieldValue < condition->value;
            }
        } else if (condition->operator_ == ">=") {
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum >= valueNum;
            } catch (...) {
                return fieldValue >= condition->value;
            }
        } else if (condition->operator_ == "<=") {
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum <= valueNum;
            } catch (...) {
                return fieldValue <= condition->value;
            }
        } else if (condition->operator_ == "LIKE") {
            // LIKE模式匹配：支持%通配符
            std::string pattern = condition->value;
            
            // 如果模式中没有%，直接进行字符串比较
            if (pattern.find('%') == std::string::npos) {
                return fieldValue == pattern;
            }
            
            // 处理%通配符
            // 前缀匹配：%xxx
            if (pattern.front() == '%' && pattern.back() != '%') {
                std::string suffix = pattern.substr(1);
                return fieldValue.length() >= suffix.length() && 
                       fieldValue.substr(fieldValue.length() - suffix.length()) == suffix;
            }
            // 后缀匹配：xxx%
            else if (pattern.front() != '%' && pattern.back() == '%') {
                std::string prefix = pattern.substr(0, pattern.length() - 1);
                return fieldValue.length() >= prefix.length() && 
                       fieldValue.substr(0, prefix.length()) == prefix;
            }
            // 包含匹配：%xxx%
            else if (pattern.front() == '%' && pattern.back() == '%') {
                std::string substr = pattern.substr(1, pattern.length() - 2);
                return fieldValue.find(substr) != std::string::npos;
            }
            // 精确匹配（无%）
            else {
                return fieldValue == pattern;
            }
        } else if (condition->operator_ == "IN") {
            // IN子句：检查字段值是否在值列表中
            for (const std::string& inValue : condition->inValues) {
                if (fieldValue == inValue) {
                    return true;
                }
            }
            return false;
        } else if (condition->operator_ == "BETWEEN") {
            // BETWEEN范围查询：检查字段值是否在范围内（包含边界）
            try {
                double fieldNum = std::stod(fieldValue);
                double startNum = std::stod(condition->betweenStart);
                double endNum = std::stod(condition->betweenEnd);
                return fieldNum >= startNum && fieldNum <= endNum;
            } catch (...) {
                // 如果转换失败，使用字符串比较
                return fieldValue >= condition->betweenStart && fieldValue <= condition->betweenEnd;
            }
        }
        return false;
    }
    
    // 复杂条件：处理逻辑运算符
    if (condition->logicalOp == "NOT") {
        return !evaluateWhereCondition(record, tableInfo, condition->left.get());
    } else if (condition->logicalOp == "AND") {
        bool leftResult = evaluateWhereCondition(record, tableInfo, condition->left.get());
        bool rightResult = evaluateWhereCondition(record, tableInfo, condition->right.get());
        return leftResult && rightResult;
    } else if (condition->logicalOp == "OR") {
        bool leftResult = evaluateWhereCondition(record, tableInfo, condition->left.get());
        bool rightResult = evaluateWhereCondition(record, tableInfo, condition->right.get());
        return leftResult || rightResult;
    }
    
    return false;
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
    
    // DISTINCT去重
    if (node->distinct) {
        applyDistinct(result.rows);
    }
    
    // ORDER BY排序（使用第一个表的信息）
    if (!node->orderBy.empty() && !tableInfos.empty()) {
        if (!applyOrderBy(result.rows, result.columnNames, tableInfos[0], node->orderBy)) {
            setError("ORDER BY operation failed");
            return false;
        }
    }
    
    // LIMIT限制
    if (node->limitCount >= 0) {
        applyLimit(result.rows, node->limitCount);
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
        std::set<size_t> allMatchedRightIndices;  // 记录所有已匹配的右表记录索引（用于FULL OUTER JOIN和RIGHT JOIN）
        
        for (const auto& leftCombination : joinedResults) {
            // 获取左表字段值
            if (leftCombination.size() <= leftTableIndex || 
                leftCombination[leftTableIndex].values.size() <= static_cast<size_t>(leftFieldIndex)) {
                continue;
            }
            const std::string& leftValue = leftCombination[leftTableIndex].values[leftFieldIndex];
            
            // 在右表中查找匹配的记录
            bool foundMatch = false;
            
            for (size_t rightIdx = 0; rightIdx < allTableRecords[rightTableIndex].size(); ++rightIdx) {
                const Record& rightRecord = allTableRecords[rightTableIndex][rightIdx];
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
                    allMatchedRightIndices.insert(rightIdx);
                }
            }
            
            // 根据JOIN类型处理未匹配的情况
            if ((joinInfo.joinType == "LEFT" || joinInfo.joinType == "FULL") && !foundMatch) {
                // LEFT JOIN 或 FULL OUTER JOIN：即使没有匹配，也保留左表记录，右表字段为空
                Record emptyRecord;
                for (size_t j = 0; j < tableInfos[rightTableIndex].fields.size(); ++j) {
                    emptyRecord.values.push_back("");
                }
                std::vector<Record> newCombination = leftCombination;
                newCombination.push_back(emptyRecord);
                newJoinedResults.push_back(newCombination);
            }
            
        }
        
        // 处理RIGHT JOIN和FULL OUTER JOIN的右表未匹配记录
        if (joinInfo.joinType == "RIGHT" || joinInfo.joinType == "FULL") {
            // 对于RIGHT JOIN和FULL OUTER JOIN，需要处理未匹配的右表记录
            for (size_t rightIdx = 0; rightIdx < allTableRecords[rightTableIndex].size(); ++rightIdx) {
                if (allMatchedRightIndices.find(rightIdx) == allMatchedRightIndices.end()) {
                    // 右表记录未匹配
                    std::vector<Record> newCombination;
                    // 为左表创建空记录
                    for (size_t i = 0; i < rightTableIndex; ++i) {
                        Record emptyRecord;
                        emptyRecord.values.resize(tableInfos[i].fields.size(), "");
                        newCombination.push_back(emptyRecord);
                    }
                    // 添加右表记录
                    newCombination.push_back(allTableRecords[rightTableIndex][rightIdx]);
                    newJoinedResults.push_back(newCombination);
                }
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
    
    // DISTINCT去重
    if (node->distinct) {
        applyDistinct(result.rows);
    }
    
    // ORDER BY排序（使用第一个表的信息）
    if (!node->orderBy.empty() && !tableInfos.empty()) {
        if (!applyOrderBy(result.rows, result.columnNames, tableInfos[0], node->orderBy)) {
            setError("ORDER BY operation failed");
            return false;
        }
    }
    
    // LIMIT限制
    if (node->limitCount >= 0) {
        applyLimit(result.rows, node->limitCount);
    }
    
    result.rowCount = result.rows.size();
    return true;
}

void SelectHandler::applyDistinct(std::vector<std::vector<std::string>>& rows) {
    std::vector<std::vector<std::string>> distinctRows;
    
    for (const auto& row : rows) {
        // 检查是否已存在相同的行
        bool isDuplicate = false;
        for (const auto& existingRow : distinctRows) {
            if (existingRow.size() == row.size()) {
                bool isEqual = true;
                for (size_t i = 0; i < row.size(); ++i) {
                    if (existingRow[i] != row[i]) {
                        isEqual = false;
                        break;
                    }
                }
                if (isEqual) {
                    isDuplicate = true;
                    break;
                }
            }
        }
        
        if (!isDuplicate) {
            distinctRows.push_back(row);
        }
    }
    
    rows = distinctRows;
}

bool SelectHandler::applyOrderBy(std::vector<std::vector<std::string>>& rows,
                                 const std::vector<std::string>& columnNames,
                                 const TableInfo& tableInfo,
                                 const std::vector<OrderByInfo>& orderBy) {
    if (orderBy.empty()) {
        return true;
    }
    
    // 创建排序比较函数
    auto compare = [&](const std::vector<std::string>& a, const std::vector<std::string>& b) -> bool {
        for (const auto& orderInfo : orderBy) {
            // 查找字段在列名中的索引
            int colIndex = -1;
            for (size_t i = 0; i < columnNames.size(); ++i) {
                if (columnNames[i] == orderInfo.fieldName) {
                    colIndex = static_cast<int>(i);
                    break;
                }
            }
            
            if (colIndex == -1 || colIndex >= static_cast<int>(a.size()) || 
                colIndex >= static_cast<int>(b.size())) {
                continue;  // 字段不存在，跳过
            }
            
            const std::string& aValue = a[colIndex];
            const std::string& bValue = b[colIndex];
            
            // 尝试数值比较
            bool isNumeric = true;
            double aNum = 0, bNum = 0;
            try {
                aNum = std::stod(aValue);
                bNum = std::stod(bValue);
            } catch (...) {
                isNumeric = false;
            }
            
            int comparison = 0;
            if (isNumeric) {
                if (aNum < bNum) {
                    comparison = -1;
                } else if (aNum > bNum) {
                    comparison = 1;
                }
            } else {
                if (aValue < bValue) {
                    comparison = -1;
                } else if (aValue > bValue) {
                    comparison = 1;
                }
            }
            
            if (comparison != 0) {
                // 根据排序方向返回结果
                if (orderInfo.direction == "DESC") {
                    return comparison > 0;
                } else {
                    return comparison < 0;
                }
            }
        }
        return false;  // 所有排序字段都相等
    };
    
    std::sort(rows.begin(), rows.end(), compare);
    return true;
}

void SelectHandler::applyLimit(std::vector<std::vector<std::string>>& rows, int limitCount) {
    if (limitCount >= 0 && limitCount < static_cast<int>(rows.size())) {
        rows.resize(limitCount);
    }
}

bool SelectHandler::executeGroupByQuery(SelectNode* node, const TableInfo& tableInfo,
                                       const std::vector<Record>& records, QueryResult& result) {
    // 1. 先应用WHERE条件过滤记录
    std::vector<Record> filteredRecords;
    for (const auto& record : records) {
        if (node->whereClause) {
            if (!evaluateWhereCondition(record, tableInfo, node->whereClause.get())) {
                continue;
            }
        } else if (!node->whereField.empty()) {
            if (!evaluateCondition(record, tableInfo, node->whereField, node->whereValue)) {
                continue;
            }
        }
        filteredRecords.push_back(record);
    }
    
    // 确定要使用的字段列表（优先使用selectFieldsNew，否则使用selectFields）
    std::vector<SelectField> fieldsToUse = node->selectFieldsNew;
    if (fieldsToUse.empty()) {
        // 向后兼容：从selectFields构建SelectField列表
        for (const std::string& fieldStr : node->selectFields) {
            SelectField field;
            // 检查是否为聚合函数（简单检查，如COUNT(...)）
            if (fieldStr.find("COUNT(") == 0 || fieldStr.find("SUM(") == 0 ||
                fieldStr.find("AVG(") == 0 || fieldStr.find("MAX(") == 0 ||
                fieldStr.find("MIN(") == 0) {
                field.isAggregate = true;
                // 解析聚合函数（简化处理）
                size_t parenPos = fieldStr.find('(');
                size_t closeParenPos = fieldStr.find(')');
                if (parenPos != std::string::npos && closeParenPos != std::string::npos) {
                    field.aggregateFunc.funcName = fieldStr.substr(0, parenPos);
                    std::string arg = fieldStr.substr(parenPos + 1, closeParenPos - parenPos - 1);
                    if (arg == "*") {
                        field.aggregateFunc.isStar = true;
                    } else {
                        field.aggregateFunc.fieldName = arg;
                    }
                }
            } else {
                field.isAggregate = false;
                field.fieldName = fieldStr;
            }
            fieldsToUse.push_back(field);
        }
    }
    
    // 确定要选择的字段（用于投影）
    std::vector<std::string> selectFields = node->selectFields;
    if (selectFields.empty() || (selectFields.size() == 1 && selectFields[0] == "*")) {
        selectFields.clear();
        for (const auto& field : tableInfo.fields) {
            selectFields.push_back(std::string(field.sFieldName));
        }
    }
    
    // 2. 如果没有GROUP BY，但有聚合函数，将全部记录作为一组
    if (node->groupBy.empty()) {
        // 只有聚合函数，没有GROUP BY，返回一行聚合结果
        // 注意：聚合函数应该直接从原始记录中计算，而不是从投影后的行中计算
        
        // 计算聚合函数
        std::vector<std::string> resultRow;
        std::vector<std::string> columnNames;
        for (const auto& field : fieldsToUse) {
            if (field.isAggregate) {
                // 直接从原始记录计算聚合函数
                std::string aggResult = calculateAggregateFromRecords(filteredRecords, tableInfo, field.aggregateFunc);
                resultRow.push_back(aggResult);
                columnNames.push_back(field.aggregateFunc.funcName + "(" + 
                    (field.aggregateFunc.isStar ? "*" : field.aggregateFunc.fieldName) + ")");
            } else {
                // 非聚合字段（在没有GROUP BY时，应该报错，但这里简化处理）
                if (!filteredRecords.empty()) {
                    int fieldIndex = findFieldIndex(tableInfo, field.fieldName);
                    if (fieldIndex >= 0 && fieldIndex < static_cast<int>(filteredRecords[0].values.size())) {
                        resultRow.push_back(filteredRecords[0].values[fieldIndex]);
                        columnNames.push_back(field.fieldName);
                    }
                }
            }
        }
        
        if (!resultRow.empty()) {
            result.rows.push_back(resultRow);
        }
        result.columnNames = columnNames;
        result.rowCount = result.rows.size();
        return true;
    }
    
    // 3. 有GROUP BY，按分组字段分组
    // 使用map来存储分组：分组键 -> 记录列表
    std::map<std::vector<std::string>, std::vector<Record>> groups;
    
    // 获取GROUP BY字段的索引
    std::vector<int> groupByIndices;
    for (const std::string& groupField : node->groupBy) {
        int index = findFieldIndex(tableInfo, groupField);
        if (index == -1) {
            setError("GROUP BY field not found: " + groupField);
            return false;
        }
        groupByIndices.push_back(index);
    }
    
    // 对每条记录进行分组
    for (const auto& record : filteredRecords) {
        // 构建分组键（GROUP BY字段的值组合）
        std::vector<std::string> groupKey;
        for (int index : groupByIndices) {
            if (index >= 0 && index < static_cast<int>(record.values.size())) {
                groupKey.push_back(record.values[index]);
            } else {
                groupKey.push_back("");
            }
        }
        groups[groupKey].push_back(record);
    }
    
    // 4. 对每个分组计算聚合函数
    std::vector<std::string> columnNames;
    for (const auto& field : fieldsToUse) {
        if (field.isAggregate) {
            columnNames.push_back(field.aggregateFunc.funcName + "(" + 
                (field.aggregateFunc.isStar ? "*" : field.aggregateFunc.fieldName) + ")");
        } else {
            columnNames.push_back(field.fieldName);
        }
    }
    result.columnNames = columnNames;
    
    for (const auto& group : groups) {
        // 计算聚合函数，构建结果行
        // 注意：聚合函数应该直接从原始记录中计算，而不是从投影后的行中计算
        std::vector<std::string> resultRow;
        for (const auto& field : fieldsToUse) {
            if (field.isAggregate) {
                // 直接从原始记录计算聚合函数
                std::string aggResult = calculateAggregateFromRecords(group.second, tableInfo, field.aggregateFunc);
                resultRow.push_back(aggResult);
            } else {
                // 非聚合字段：使用分组键的值
                int fieldIndex = findFieldIndex(tableInfo, field.fieldName);
                if (fieldIndex >= 0) {
                    // 检查该字段是否在GROUP BY中
                    bool inGroupBy = false;
                    for (size_t i = 0; i < node->groupBy.size(); ++i) {
                        if (node->groupBy[i] == field.fieldName) {
                            resultRow.push_back(group.first[i]);
                            inGroupBy = true;
                            break;
                        }
                    }
                    if (!inGroupBy) {
                        // 字段不在GROUP BY中，使用第一条记录的值（简化处理）
                        if (!group.second.empty() && fieldIndex < static_cast<int>(group.second[0].values.size())) {
                            resultRow.push_back(group.second[0].values[fieldIndex]);
                        } else {
                            resultRow.push_back("");
                        }
                    }
                } else {
                    resultRow.push_back("");
                }
            }
        }
        
        result.rows.push_back(resultRow);
    }
    
    // 5. 应用HAVING过滤
    if (node->havingClause) {
        if (!applyHaving(result.rows, result.columnNames, tableInfo, node->havingClause.get())) {
            setError("HAVING operation failed");
            return false;
        }
    }
    
    // 6. 应用ORDER BY
    if (!node->orderBy.empty()) {
        if (!applyOrderBy(result.rows, result.columnNames, tableInfo, node->orderBy)) {
            setError("ORDER BY operation failed");
            return false;
        }
    }
    
    // 7. 应用LIMIT
    if (node->limitCount >= 0) {
        applyLimit(result.rows, node->limitCount);
    }
    
    result.rowCount = result.rows.size();
    return true;
}

// 从原始记录计算聚合函数（新方法）
std::string SelectHandler::calculateAggregateFromRecords(const std::vector<Record>& records,
                                                          const TableInfo& tableInfo,
                                                          const AggregateFunction& aggregateFunc) {
    if (records.empty()) {
        if (aggregateFunc.funcName == "COUNT") {
            return "0";
        }
        return "";
    }
    
    // 查找聚合字段的索引（从表结构中查找）
    int fieldIndex = -1;
    if (!aggregateFunc.isStar) {
        fieldIndex = findFieldIndex(tableInfo, aggregateFunc.fieldName);
        if (fieldIndex == -1) {
            // 字段不存在
            if (aggregateFunc.funcName == "COUNT") {
                return "0";
            }
            return "";
        }
    }
    
    if (aggregateFunc.funcName == "COUNT") {
        if (aggregateFunc.isStar) {
            return std::to_string(records.size());
        } else {
            // COUNT(Field)：统计非空值
            int count = 0;
            for (const auto& record : records) {
                if (fieldIndex >= 0 && fieldIndex < static_cast<int>(record.values.size())) {
                    if (!record.values[fieldIndex].empty()) {
                        count++;
                    }
                }
            }
            return std::to_string(count);
        }
    } else if (aggregateFunc.funcName == "SUM") {
        if (fieldIndex == -1) {
            return "0";
        }
        double sum = 0.0;
        for (const auto& record : records) {
            if (fieldIndex < static_cast<int>(record.values.size())) {
                try {
                    sum += std::stod(record.values[fieldIndex]);
                } catch (...) {
                    // 忽略非数值
                }
            }
        }
        return std::to_string(sum);
    } else if (aggregateFunc.funcName == "AVG") {
        if (fieldIndex == -1) {
            return "0";
        }
        double sum = 0.0;
        int count = 0;
        for (const auto& record : records) {
            if (fieldIndex < static_cast<int>(record.values.size())) {
                try {
                    sum += std::stod(record.values[fieldIndex]);
                    count++;
                } catch (...) {
                    // 忽略非数值
                }
            }
        }
        if (count == 0) {
            return "0";
        }
        return std::to_string(sum / count);
    } else if (aggregateFunc.funcName == "MAX") {
        if (fieldIndex == -1) {
            return "";
        }
        bool hasValue = false;
        double maxVal = 0.0;
        std::string maxStr = "";
        bool isNumeric = true;
        
        for (const auto& record : records) {
            if (fieldIndex < static_cast<int>(record.values.size())) {
                try {
                    double val = std::stod(record.values[fieldIndex]);
                    if (!hasValue || val > maxVal) {
                        maxVal = val;
                        hasValue = true;
                    }
                } catch (...) {
                    isNumeric = false;
                    if (!hasValue || record.values[fieldIndex] > maxStr) {
                        maxStr = record.values[fieldIndex];
                        hasValue = true;
                    }
                }
            }
        }
        if (!hasValue) {
            return "";
        }
        return isNumeric ? std::to_string(maxVal) : maxStr;
    } else if (aggregateFunc.funcName == "MIN") {
        if (fieldIndex == -1) {
            return "";
        }
        bool hasValue = false;
        double minVal = 0.0;
        std::string minStr = "";
        bool isNumeric = true;
        
        for (const auto& record : records) {
            if (fieldIndex < static_cast<int>(record.values.size())) {
                try {
                    double val = std::stod(record.values[fieldIndex]);
                    if (!hasValue || val < minVal) {
                        minVal = val;
                        hasValue = true;
                    }
                } catch (...) {
                    isNumeric = false;
                    if (!hasValue || record.values[fieldIndex] < minStr) {
                        minStr = record.values[fieldIndex];
                        hasValue = true;
                    }
                }
            }
        }
        if (!hasValue) {
            return "";
        }
        return isNumeric ? std::to_string(minVal) : minStr;
    }
    
    return "";
}

// 保留旧方法以保持兼容性（但不再使用）
std::string SelectHandler::calculateAggregate(const std::vector<std::vector<std::string>>& rows,
                                              const std::vector<std::string>& columnNames,
                                              const TableInfo& tableInfo,
                                              const AggregateFunction& aggregateFunc) {
    // 这个方法已废弃，保留以保持接口兼容性
    // 实际应该使用calculateAggregateFromRecords
    if (rows.empty()) {
        if (aggregateFunc.funcName == "COUNT") {
            return "0";
        }
        return "";
    }
    
    // 查找聚合字段的索引
    int fieldIndex = -1;
    if (!aggregateFunc.isStar) {
        // 在columnNames中查找字段
        for (size_t i = 0; i < columnNames.size(); ++i) {
            if (columnNames[i] == aggregateFunc.fieldName) {
                fieldIndex = static_cast<int>(i);
                break;
            }
        }
    }
    
    if (aggregateFunc.funcName == "COUNT") {
        if (aggregateFunc.isStar) {
            return std::to_string(rows.size());
        } else {
            // COUNT(Field)：统计非空值
            int count = 0;
            for (const auto& row : rows) {
                if (fieldIndex >= 0 && fieldIndex < static_cast<int>(row.size())) {
                    if (!row[fieldIndex].empty()) {
                        count++;
                    }
                }
            }
            return std::to_string(count);
        }
    } else if (aggregateFunc.funcName == "SUM") {
        if (fieldIndex == -1) {
            return "0";
        }
        double sum = 0.0;
        for (const auto& row : rows) {
            if (fieldIndex < static_cast<int>(row.size())) {
                try {
                    sum += std::stod(row[fieldIndex]);
                } catch (...) {
                    // 忽略非数值
                }
            }
        }
        return std::to_string(sum);
    } else if (aggregateFunc.funcName == "AVG") {
        if (fieldIndex == -1) {
            return "0";
        }
        double sum = 0.0;
        int count = 0;
        for (const auto& row : rows) {
            if (fieldIndex < static_cast<int>(row.size())) {
                try {
                    sum += std::stod(row[fieldIndex]);
                    count++;
                } catch (...) {
                    // 忽略非数值
                }
            }
        }
        if (count == 0) {
            return "0";
        }
        return std::to_string(sum / count);
    } else if (aggregateFunc.funcName == "MAX") {
        if (fieldIndex == -1) {
            return "";
        }
        bool hasValue = false;
        double maxVal = 0.0;
        std::string maxStr = "";
        bool isNumeric = true;
        
        for (const auto& row : rows) {
            if (fieldIndex < static_cast<int>(row.size())) {
                try {
                    double val = std::stod(row[fieldIndex]);
                    if (!hasValue || val > maxVal) {
                        maxVal = val;
                        hasValue = true;
                    }
                } catch (...) {
                    isNumeric = false;
                    if (!hasValue || row[fieldIndex] > maxStr) {
                        maxStr = row[fieldIndex];
                        hasValue = true;
                    }
                }
            }
        }
        if (!hasValue) {
            return "";
        }
        return isNumeric ? std::to_string(maxVal) : maxStr;
    } else if (aggregateFunc.funcName == "MIN") {
        if (fieldIndex == -1) {
            return "";
        }
        bool hasValue = false;
        double minVal = 0.0;
        std::string minStr = "";
        bool isNumeric = true;
        
        for (const auto& row : rows) {
            if (fieldIndex < static_cast<int>(row.size())) {
                try {
                    double val = std::stod(row[fieldIndex]);
                    if (!hasValue || val < minVal) {
                        minVal = val;
                        hasValue = true;
                    }
                } catch (...) {
                    isNumeric = false;
                    if (!hasValue || row[fieldIndex] < minStr) {
                        minStr = row[fieldIndex];
                        hasValue = true;
                    }
                }
            }
        }
        if (!hasValue) {
            return "";
        }
        return isNumeric ? std::to_string(minVal) : minStr;
    }
    
    return "";
}

bool SelectHandler::applyGroupBy(std::vector<std::vector<std::string>>& rows,
                                 const std::vector<std::string>& columnNames,
                                 const TableInfo& tableInfo,
                                 const std::vector<std::string>& groupByFields) {
    // 这个方法在executeGroupByQuery中已经实现，这里保留接口
    // 如果需要单独调用，可以在这里实现
    return true;
}

bool SelectHandler::applyHaving(std::vector<std::vector<std::string>>& groupedRows,
                               const std::vector<std::string>& columnNames,
                               const TableInfo& tableInfo,
                               const WhereCondition* havingClause) {
    if (!havingClause) {
        return true;
    }
    
    // 过滤分组后的行
    std::vector<std::vector<std::string>> filteredRows;
    for (const auto& row : groupedRows) {
        if (evaluateHavingCondition(row, columnNames, tableInfo, havingClause)) {
            filteredRows.push_back(row);
        }
    }
    
    groupedRows = filteredRows;
    return true;
}

// HAVING条件评估辅助函数
bool SelectHandler::evaluateHavingCondition(const std::vector<std::string>& row,
                                           const std::vector<std::string>& columnNames,
                                           const TableInfo& tableInfo,
                                           const WhereCondition* condition) {
    if (!condition) {
        return true;
    }
    
    // 如果是简单条件
    if (condition->isSimple()) {
        // 在结果列中查找字段（可能是聚合函数名或分组字段名）
        int fieldIndex = -1;
        
        // 首先尝试直接匹配字段名
        for (size_t i = 0; i < columnNames.size(); ++i) {
            if (columnNames[i] == condition->fieldName) {
                fieldIndex = static_cast<int>(i);
                break;
            }
        }
        
        // 如果没找到，尝试匹配聚合函数格式
        if (fieldIndex == -1) {
            std::string fieldName = condition->fieldName;
            
            // 处理聚合函数格式：COUNT(*), SUM(Age)等
            // 如果字段名是聚合函数（如COUNT, SUM等），尝试匹配列名
            if (fieldName == "COUNT" || fieldName == "SUM" || fieldName == "AVG" || 
                fieldName == "MAX" || fieldName == "MIN") {
                // 尝试匹配 "COUNT(*)" 格式
                for (size_t i = 0; i < columnNames.size(); ++i) {
                    if (columnNames[i].find(fieldName + "(") == 0) {
                        fieldIndex = static_cast<int>(i);
                        break;
                    }
                }
            } else {
                // 尝试部分匹配（处理字段名可能是聚合函数的一部分）
                for (size_t i = 0; i < columnNames.size(); ++i) {
                    // 检查列名是否以字段名开头（处理 COUNT(*) 匹配 COUNT 的情况）
                    if (columnNames[i].find(fieldName) == 0) {
                        fieldIndex = static_cast<int>(i);
                        break;
                    }
                }
            }
        }
        
        if (fieldIndex == -1 || fieldIndex >= static_cast<int>(row.size())) {
            return false;
        }
        
        // 获取字段值
        const std::string& fieldValue = row[fieldIndex];
        
        // 根据运算符进行比较
        if (condition->operator_ == "=") {
            return fieldValue == condition->value;
        } else if (condition->operator_ == "!=") {
            return fieldValue != condition->value;
        } else if (condition->operator_ == ">") {
            // 尝试数值比较
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum > valueNum;
            } catch (...) {
                return fieldValue > condition->value;
            }
        } else if (condition->operator_ == "<") {
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum < valueNum;
            } catch (...) {
                return fieldValue < condition->value;
            }
        } else if (condition->operator_ == ">=") {
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum >= valueNum;
            } catch (...) {
                return fieldValue >= condition->value;
            }
        } else if (condition->operator_ == "<=") {
            try {
                double fieldNum = std::stod(fieldValue);
                double valueNum = std::stod(condition->value);
                return fieldNum <= valueNum;
            } catch (...) {
                return fieldValue <= condition->value;
            }
        } else if (condition->operator_ == "LIKE") {
            // LIKE模式匹配
            std::string pattern = condition->value;
            if (pattern.find('%') == std::string::npos) {
                return fieldValue == pattern;
            }
            if (pattern.front() == '%' && pattern.back() != '%') {
                std::string suffix = pattern.substr(1);
                return fieldValue.length() >= suffix.length() && 
                       fieldValue.substr(fieldValue.length() - suffix.length()) == suffix;
            } else if (pattern.front() != '%' && pattern.back() == '%') {
                std::string prefix = pattern.substr(0, pattern.length() - 1);
                return fieldValue.length() >= prefix.length() && 
                       fieldValue.substr(0, prefix.length()) == prefix;
            } else if (pattern.front() == '%' && pattern.back() == '%') {
                std::string substr = pattern.substr(1, pattern.length() - 2);
                return fieldValue.find(substr) != std::string::npos;
            }
            return fieldValue == pattern;
        } else if (condition->operator_ == "IN") {
            // IN子句
            for (const std::string& inValue : condition->inValues) {
                if (fieldValue == inValue) {
                    return true;
                }
            }
            return false;
        } else if (condition->operator_ == "BETWEEN") {
            // BETWEEN范围查询
            try {
                double fieldNum = std::stod(fieldValue);
                double startNum = std::stod(condition->betweenStart);
                double endNum = std::stod(condition->betweenEnd);
                return fieldNum >= startNum && fieldNum <= endNum;
            } catch (...) {
                return fieldValue >= condition->betweenStart && fieldValue <= condition->betweenEnd;
            }
        }
        
        return false;
    } else {
        // 复杂条件：递归评估
        bool leftResult = true;
        bool rightResult = true;
        
        if (condition->left) {
            leftResult = evaluateHavingCondition(row, columnNames, tableInfo, condition->left.get());
        }
        if (condition->right) {
            rightResult = evaluateHavingCondition(row, columnNames, tableInfo, condition->right.get());
        }
        
        if (condition->logicalOp == "AND") {
            return leftResult && rightResult;
        } else if (condition->logicalOp == "OR") {
            return leftResult || rightResult;
        } else if (condition->logicalOp == "NOT") {
            return !leftResult;
        }
        
        return false;
    }
}

