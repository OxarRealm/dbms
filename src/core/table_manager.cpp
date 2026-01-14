/**
 * @file table_manager.cpp
 * @brief 表结构管理器实现
 */

#include "core/table_manager.h"
#include <iostream>
#include <cstring>
#include <algorithm>

TableManager::TableManager() {
}

TableManager::~TableManager() {
}

void TableManager::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
}

std::string TableManager::getDbfFilePath() const {
    return m_dbFilePath + ".dbf";
}

bool TableManager::createTable(const TableInfo& tableInfo) {
    // 检查表是否已存在
    if (tableExists(tableInfo.tableName)) {
        std::cerr << "错误: 表 " << tableInfo.tableName << " 已存在" << std::endl;
        return false;
    }

    // 以追加模式打开文件
    std::ofstream file(getDbfFilePath(), std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件 " << getDbfFilePath() << std::endl;
        return false;
    }

    // 写入表结构
    bool success = writeTableToStream(file, tableInfo);
    file.close();

    return success;
}

bool TableManager::readTable(const std::string& tableName, TableInfo& tableInfo) {
    std::ifstream file(getDbfFilePath(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件 " << getDbfFilePath() << std::endl;
        return false;
    }

    // 读取所有表，查找目标表
    char separator;
    while (file.read(&separator, 1)) {
        if (separator != TABLE_SEPARATOR) {
            std::cerr << "错误: 文件格式错误，期望分隔符" << std::endl;
            file.close();
            return false;
        }

        TableInfo currentTable;
        if (!readTableFromStream(file, currentTable)) {
            file.close();
            return false;
        }

        if (strcmp(currentTable.tableName, tableName.c_str()) == 0) {
            tableInfo = currentTable;
            file.close();
            return true;
        }
    }

    file.close();
    return false;  // 表不存在
}

bool TableManager::readAllTables(std::vector<TableInfo>& tableList) {
    tableList.clear();

    std::ifstream file(getDbfFilePath(), std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在，返回空列表（不算错误）
        return true;
    }

    // 读取所有表
    char separator;
    while (file.read(&separator, 1)) {
        if (separator != TABLE_SEPARATOR) {
            std::cerr << "错误: 文件格式错误，期望分隔符" << std::endl;
            file.close();
            return false;
        }

        TableInfo tableInfo;
        if (!readTableFromStream(file, tableInfo)) {
            file.close();
            return false;
        }

        tableList.push_back(tableInfo);
    }

    file.close();
    return true;
}

bool TableManager::updateTable(const std::string& tableName, const TableInfo& tableInfo) {
    // 读取所有表
    std::vector<TableInfo> tableList;
    if (!readAllTables(tableList)) {
        return false;
    }

    // 查找并更新目标表
    bool found = false;
    for (auto& table : tableList) {
        if (strcmp(table.tableName, tableName.c_str()) == 0) {
            table = tableInfo;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "错误: 表 " << tableName << " 不存在" << std::endl;
        return false;
    }

    // 重新写入所有表
    return rewriteAllTables(tableList);
}

bool TableManager::deleteTable(const std::string& tableName) {
    // 读取所有表
    std::vector<TableInfo> tableList;
    if (!readAllTables(tableList)) {
        return false;
    }

    // 删除目标表
    auto it = std::remove_if(tableList.begin(), tableList.end(),
        [&tableName](const TableInfo& table) {
            return strcmp(table.tableName, tableName.c_str()) == 0;
        });

    if (it == tableList.end()) {
        std::cerr << "错误: 表 " << tableName << " 不存在" << std::endl;
        return false;
    }

    tableList.erase(it, tableList.end());

    // 重新写入所有表
    return rewriteAllTables(tableList);
}

bool TableManager::renameTable(const std::string& oldTableName, const std::string& newTableName) {
    // 检查新表名是否已存在
    if (tableExists(newTableName)) {
        std::cerr << "错误: 表 " << newTableName << " 已存在" << std::endl;
        return false;
    }

    // 读取所有表
    std::vector<TableInfo> tableList;
    if (!readAllTables(tableList)) {
        return false;
    }

    // 查找并重命名目标表
    bool found = false;
    for (auto& table : tableList) {
        if (strcmp(table.tableName, oldTableName.c_str()) == 0) {
            strncpy(table.tableName, newTableName.c_str(), TABLE_NAME_LENGTH - 1);
            table.tableName[TABLE_NAME_LENGTH - 1] = '\0';
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "错误: 表 " << oldTableName << " 不存在" << std::endl;
        return false;
    }

    // 重新写入所有表
    return rewriteAllTables(tableList);
}

bool TableManager::tableExists(const std::string& tableName) {
    TableInfo tableInfo;
    return readTable(tableName, tableInfo);
}

bool TableManager::getAllTableNames(std::vector<std::string>& tableNames) {
    tableNames.clear();

    std::vector<TableInfo> tableList;
    if (!readAllTables(tableList)) {
        return false;
    }

    for (const auto& table : tableList) {
        tableNames.push_back(std::string(table.tableName));
    }

    return true;
}

bool TableManager::readTableFromStream(std::ifstream& file, TableInfo& tableInfo) {
    // 读取表名
    file.read(tableInfo.tableName, TABLE_NAME_LENGTH);
    if (file.gcount() != TABLE_NAME_LENGTH) {
        std::cerr << "错误: 读取表名失败" << std::endl;
        return false;
    }
    tableInfo.tableName[TABLE_NAME_LENGTH - 1] = '\0';  // 确保字符串结束

    // 读取字段数量
    int fieldCount;
    file.read(reinterpret_cast<char*>(&fieldCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "错误: 读取字段数量失败" << std::endl;
        return false;
    }

    // 读取字段列表
    tableInfo.fields.clear();
    tableInfo.fields.reserve(fieldCount);
    for (int i = 0; i < fieldCount; i++) {
        TableMode field;
        file.read(reinterpret_cast<char*>(&field), sizeof(TableMode));
        if (file.gcount() != sizeof(TableMode)) {
            std::cerr << "错误: 读取字段 " << i << " 失败" << std::endl;
            return false;
        }
        tableInfo.fields.push_back(field);
    }

    return true;
}

bool TableManager::writeTableToStream(std::ofstream& file, const TableInfo& tableInfo) {
    // 写入分隔符
    char separator = TABLE_SEPARATOR;
    file.write(&separator, 1);
    if (!file.good()) {
        std::cerr << "错误: 写入分隔符失败" << std::endl;
        return false;
    }

    // 写入表名（固定长度）
    char tableNameBuffer[TABLE_NAME_LENGTH];
    strncpy(tableNameBuffer, tableInfo.tableName, TABLE_NAME_LENGTH - 1);
    tableNameBuffer[TABLE_NAME_LENGTH - 1] = '\0';
    // 填充剩余空间为0
    for (int i = strlen(tableNameBuffer); i < TABLE_NAME_LENGTH; i++) {
        tableNameBuffer[i] = '\0';
    }
    file.write(tableNameBuffer, TABLE_NAME_LENGTH);
    if (!file.good()) {
        std::cerr << "错误: 写入表名失败" << std::endl;
        return false;
    }

    // 写入字段数量
    int fieldCount = static_cast<int>(tableInfo.fields.size());
    file.write(reinterpret_cast<const char*>(&fieldCount), sizeof(int));
    if (!file.good()) {
        std::cerr << "错误: 写入字段数量失败" << std::endl;
        return false;
    }

    // 写入字段列表
    for (const auto& field : tableInfo.fields) {
        file.write(reinterpret_cast<const char*>(&field), sizeof(TableMode));
        if (!file.good()) {
            std::cerr << "错误: 写入字段失败" << std::endl;
            return false;
        }
    }

    return true;
}

bool TableManager::rewriteAllTables(const std::vector<TableInfo>& tableList) {
    // 删除原文件
    std::remove(getDbfFilePath().c_str());

    // 创建新文件并写入所有表
    std::ofstream file(getDbfFilePath(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "错误: 无法创建文件 " << getDbfFilePath() << std::endl;
        return false;
    }

    for (const auto& tableInfo : tableList) {
        if (!writeTableToStream(file, tableInfo)) {
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

