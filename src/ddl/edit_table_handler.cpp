/**
 * @file edit_table_handler.cpp
 * @brief EDIT TABLE语句处理器实现
 */

#include "ddl/edit_table_handler.h"
#include <iostream>
#include <cstring>
#include <algorithm>

EditTableHandler::EditTableHandler() {
}

EditTableHandler::~EditTableHandler() {
}

bool EditTableHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL解析失败: " + parser.getLastError());
        return false;
    }
    
    // 转换为EditTableNode
    EditTableNode* editNode = dynamic_cast<EditTableNode*>(node.get());
    if (editNode == nullptr) {
        setError("不是EDIT TABLE语句");
        return false;
    }
    
    // 验证字段定义
    if (!validateFieldDefinition(editNode)) {
        return false;
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(editNode->databaseFileName);
    
    // 检查表是否存在
    if (!m_tableManager.tableExists(editNode->tableName)) {
        setError("表不存在: " + editNode->tableName);
        return false;
    }
    
    // 更新表结构
    return updateTableStructure(editNode);
}

bool EditTableHandler::validateFieldDefinition(EditTableNode* node) {
    // 验证表名
    if (node->tableName.empty()) {
        setError("表名不能为空");
        return false;
    }
    
    // 验证字段名
    if (strlen(node->field.sFieldName) == 0) {
        setError("字段名不能为空");
        return false;
    }
    
    // 验证数据类型
    if (strlen(node->field.sType) == 0) {
        setError("数据类型不能为空");
        return false;
    }
    
    // 验证字段大小（对于char类型）
    if (strcmp(node->field.sType, "char") == 0 && node->field.iSize <= 0) {
        setError("char类型字段的大小必须大于0");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("数据库文件名不能为空");
        return false;
    }
    
    return true;
}

bool EditTableHandler::updateTableStructure(EditTableNode* node) {
    // 读取当前表结构
    TableInfo currentTableInfo;
    if (!m_tableManager.readTable(node->tableName, currentTableInfo)) {
        setError("读取表结构失败: " + node->tableName);
        return false;
    }
    
    // 检查字段是否已存在
    bool fieldExists = false;
    size_t fieldIndex = 0;
    for (size_t i = 0; i < currentTableInfo.fields.size(); i++) {
        if (strcmp(currentTableInfo.fields[i].sFieldName, node->field.sFieldName) == 0) {
            fieldExists = true;
            fieldIndex = i;
            break;
        }
    }
    
    if (fieldExists) {
        // 字段已存在，更新字段定义
        currentTableInfo.fields[fieldIndex] = node->field;
    } else {
        // 字段不存在，添加新字段
        currentTableInfo.fields.push_back(node->field);
    }
    
    // 更新表结构
    if (!m_tableManager.updateTable(node->tableName, currentTableInfo)) {
        setError("更新表结构失败: " + node->tableName);
        return false;
    }
    
    return true;
}

std::string EditTableHandler::getLastError() const {
    return m_lastError;
}

void EditTableHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "EditTableHandler错误: " << error << std::endl;
}

