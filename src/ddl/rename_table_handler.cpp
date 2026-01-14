/**
 * @file rename_table_handler.cpp
 * @brief RENAME TABLE语句处理器实现
 */

#include "ddl/rename_table_handler.h"
#include <iostream>
#include <cstring>

RenameTableHandler::RenameTableHandler() {
}

RenameTableHandler::~RenameTableHandler() {
}

bool RenameTableHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL解析失败: " + parser.getLastError());
        return false;
    }
    
    // 转换为RenameTableNode
    RenameTableNode* renameNode = dynamic_cast<RenameTableNode*>(node.get());
    if (renameNode == nullptr) {
        setError("不是RENAME TABLE语句");
        return false;
    }
    
    // 验证重命名参数
    if (!validateRenameParameters(renameNode)) {
        return false;
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(renameNode->databaseFileName);
    m_dataManager.setDatabasePath(renameNode->databaseFileName);
    
    // 重命名表
    return renameTable(renameNode);
}

bool RenameTableHandler::validateRenameParameters(RenameTableNode* node) {
    // 验证旧表名
    if (node->oldTableName.empty()) {
        setError("旧表名不能为空");
        return false;
    }
    
    if (node->oldTableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("旧表名过长（最大" + std::to_string(TABLE_NAME_LENGTH - 1) + "字符）");
        return false;
    }
    
    // 验证新表名
    if (node->newTableName.empty()) {
        setError("新表名不能为空");
        return false;
    }
    
    if (node->newTableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("新表名过长（最大" + std::to_string(TABLE_NAME_LENGTH - 1) + "字符）");
        return false;
    }
    
    // 验证新表名和旧表名不同
    if (node->oldTableName == node->newTableName) {
        setError("新表名和旧表名相同");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("数据库文件名不能为空");
        return false;
    }
    
    return true;
}

bool RenameTableHandler::renameTable(RenameTableNode* node) {
    // 检查旧表是否存在
    if (!m_tableManager.tableExists(node->oldTableName)) {
        setError("表不存在: " + node->oldTableName);
        return false;
    }
    
    // 检查新表名是否已存在
    if (m_tableManager.tableExists(node->newTableName)) {
        setError("表已存在: " + node->newTableName);
        return false;
    }
    
    // 先读取旧表的所有记录（在重命名表结构之前）
    std::vector<Record> records;
    bool hasData = m_dataManager.readAllRecords(node->oldTableName, records);
    
    // 使用TableManager重命名表（会更新.dbf文件）
    if (!m_tableManager.renameTable(node->oldTableName, node->newTableName)) {
        setError("重命名表失败: " + node->oldTableName);
        return false;
    }
    
    // 如果旧表有数据，需要迁移到新表名
    if (hasData && !records.empty()) {
        // 将记录写入新表名下
        if (!m_dataManager.insertRecords(node->newTableName, records)) {
            setError("迁移数据到新表名失败");
            // 尝试恢复：将表名改回旧名称
            m_tableManager.renameTable(node->newTableName, node->oldTableName);
            return false;
        }
        
        // 清空旧表名的数据
        m_dataManager.clearTable(node->oldTableName);
    }
    
    return true;
}

std::string RenameTableHandler::getLastError() const {
    return m_lastError;
}

void RenameTableHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "RenameTableHandler错误: " << error << std::endl;
}

