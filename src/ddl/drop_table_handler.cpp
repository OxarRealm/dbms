/**
 * @file drop_table_handler.cpp
 * @brief DROP TABLE语句处理器实现
 */

#include "ddl/drop_table_handler.h"
#include <iostream>
#include <cstring>

DropTableHandler::DropTableHandler() {
}

DropTableHandler::~DropTableHandler() {
}

bool DropTableHandler::execute(const std::string& sql) {
    m_lastError = "";
    
    // 解析SQL语句
    Parser parser(sql);
    std::unique_ptr<ASTNode> node = parser.parse();
    
    if (node == nullptr) {
        setError("SQL解析失败: " + parser.getLastError());
        return false;
    }
    
    // 转换为DropTableNode
    DropTableNode* dropNode = dynamic_cast<DropTableNode*>(node.get());
    if (dropNode == nullptr) {
        setError("不是DROP TABLE语句");
        return false;
    }
    
    // 验证删除参数
    if (!validateDropParameters(dropNode)) {
        return false;
    }
    
    // 设置数据库路径
    m_tableManager.setDatabasePath(dropNode->databaseFileName);
    m_dataManager.setDatabasePath(dropNode->databaseFileName);
    
    // 删除表
    return dropTable(dropNode);
}

bool DropTableHandler::validateDropParameters(DropTableNode* node) {
    // 验证表名
    if (node->tableName.empty()) {
        setError("表名不能为空");
        return false;
    }
    
    if (node->tableName.length() > TABLE_NAME_LENGTH - 1) {
        setError("表名过长（最大" + std::to_string(TABLE_NAME_LENGTH - 1) + "字符）");
        return false;
    }
    
    // 验证数据库文件名
    if (node->databaseFileName.empty()) {
        setError("数据库文件名不能为空");
        return false;
    }
    
    return true;
}

bool DropTableHandler::dropTable(DropTableNode* node) {
    // 检查表是否存在
    if (!m_tableManager.tableExists(node->tableName)) {
        setError("表不存在: " + node->tableName);
        return false;
    }
    
    // 先删除表的数据（.dat文件中的记录）
    // 注意：即使表没有数据，clearTable也不会报错
    m_dataManager.clearTable(node->tableName);
    
    // 删除表结构（.dbf文件中的表定义）
    if (!m_tableManager.deleteTable(node->tableName)) {
        setError("删除表结构失败: " + node->tableName);
        return false;
    }
    
    return true;
}

std::string DropTableHandler::getLastError() const {
    return m_lastError;
}

void DropTableHandler::setError(const std::string& error) {
    m_lastError = error;
    std::cerr << "DropTableHandler错误: " << error << std::endl;
}

