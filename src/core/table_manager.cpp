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
        std::cerr << "Error: Table " << tableInfo.tableName << " already exists" << std::endl;
        return false;
    }

    // 以追加模式打开文件
    std::ofstream file(getDbfFilePath(), std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << getDbfFilePath() << std::endl;
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
        std::cerr << "Error: Cannot open file " << getDbfFilePath() << std::endl;
        return false;
    }

    // 读取所有表，查找目标表
    char separator;
    while (file.read(&separator, 1)) {
        if (separator != TABLE_SEPARATOR) {
            std::cerr << "Error: File format error, expected separator" << std::endl;
            file.close();
            return false;
        }

        TableInfo currentTable;
        if (!readTableFromStream(file, currentTable)) {
            file.close();
            return false;
        }

        if (strcasecmp_custom(currentTable.tableName, tableName.c_str()) == 0) {
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
            std::cerr << "Error: File format error, expected separator" << std::endl;
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
        if (strcasecmp_custom(table.tableName, tableName.c_str()) == 0) {
            table = tableInfo;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "Error: Table " << tableName << " does not exist" << std::endl;
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
            return strcasecmp_custom(table.tableName, tableName.c_str()) == 0;
        });

    if (it == tableList.end()) {
        std::cerr << "Error: Table " << tableName << " does not exist" << std::endl;
        return false;
    }

    tableList.erase(it, tableList.end());

    // 重新写入所有表
    return rewriteAllTables(tableList);
}

bool TableManager::renameTable(const std::string& oldTableName, const std::string& newTableName) {
    // 检查新表名是否已存在
    if (tableExists(newTableName)) {
        std::cerr << "Error: Table " << newTableName << " already exists" << std::endl;
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
        if (strcasecmp_custom(table.tableName, oldTableName.c_str()) == 0) {
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
        std::cerr << "Error: Failed to read table name" << std::endl;
        return false;
    }
    tableInfo.tableName[TABLE_NAME_LENGTH - 1] = '\0';  // 确保字符串结束

    // 读取字段数量
    int fieldCount;
    file.read(reinterpret_cast<char*>(&fieldCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read field count" << std::endl;
        return false;
    }

    // 读取字段列表
    // 注意：为了向后兼容，需要处理旧格式的TableMode（不包含sDefaultValue和bUnique字段）
    // 旧格式TableMode大小 = FIELD_NAME_LENGTH(32) + TYPE_NAME_LENGTH(8) + sizeof(int)(4) + 3*sizeof(char)(3) = 47字节
    // 新格式TableMode大小 = 旧格式 + 128(sDefaultValue) + 1(bUnique) = 176字节
    const size_t OLD_TABLEMODE_SIZE = FIELD_NAME_LENGTH + TYPE_NAME_LENGTH + sizeof(int) + 3 * sizeof(char);  // 47字节
    const size_t NEW_TABLEMODE_SIZE = sizeof(TableMode);  // 176字节
    
    tableInfo.fields.clear();
    tableInfo.fields.reserve(fieldCount);
    
    // 获取当前位置，用于检测文件格式
    std::streampos startPos = file.tellg();
    
    // 先尝试读取新格式（完整TableMode）
    bool isNewFormat = true;
    if (fieldCount > 0) {
        TableMode testField;
        file.read(reinterpret_cast<char*>(&testField), NEW_TABLEMODE_SIZE);
        if (file.gcount() != NEW_TABLEMODE_SIZE) {
            // 读取失败，可能是旧格式
            isNewFormat = false;
        }
        file.seekg(startPos);  // 回到开始位置
    }
    
    // 根据格式读取字段
    if (isNewFormat) {
        // 新格式：包含sDefaultValue和bUnique
        for (int i = 0; i < fieldCount; i++) {
            TableMode field;
            file.read(reinterpret_cast<char*>(&field), NEW_TABLEMODE_SIZE);
            if (file.gcount() != NEW_TABLEMODE_SIZE) {
                std::cerr << "Error: Failed to read field " << i << " (new format)" << std::endl;
                return false;
            }
            tableInfo.fields.push_back(field);
        }
    } else {
        // 旧格式：不包含sDefaultValue和bUnique，需要手动初始化
        for (int i = 0; i < fieldCount; i++) {
            TableMode field;
            memset(&field, 0, sizeof(TableMode));  // 先清零，确保新字段为0
            // 读取旧格式数据
            file.read(reinterpret_cast<char*>(&field), OLD_TABLEMODE_SIZE);
            if (file.gcount() != OLD_TABLEMODE_SIZE) {
                std::cerr << "Error: Failed to read field " << i << " (old format)" << std::endl;
                return false;
            }
            // 初始化新字段为默认值
            field.sDefaultValue[0] = '\0';  // 空默认值
            field.bUnique = 0;  // 默认不是唯一约束（0 = NOT_UNIQUE）
            tableInfo.fields.push_back(field);
        }
    }

    return true;
}

bool TableManager::writeTableToStream(std::ofstream& file, const TableInfo& tableInfo) {
    // 写入分隔符
    char separator = TABLE_SEPARATOR;
    file.write(&separator, 1);
    if (!file.good()) {
        std::cerr << "Error: Failed to write separator" << std::endl;
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
        std::cerr << "Error: Failed to write table name" << std::endl;
        return false;
    }

    // 写入字段数量
    int fieldCount = static_cast<int>(tableInfo.fields.size());
    file.write(reinterpret_cast<const char*>(&fieldCount), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write field count" << std::endl;
        return false;
    }

    // 写入字段列表
    for (const auto& field : tableInfo.fields) {
        file.write(reinterpret_cast<const char*>(&field), sizeof(TableMode));
        if (!file.good()) {
            std::cerr << "Error: Failed to write field" << std::endl;
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
        std::cerr << "Error: Cannot create file " << getDbfFilePath() << std::endl;
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

