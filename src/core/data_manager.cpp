/**
 * @file data_manager.cpp
 * @brief 数据记录管理器实现
 */

#include "core/data_manager.h"
#include "core/table_mode.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <map>
#ifdef _WIN32
#include <string.h>
#else
#include <strings.h>
#endif

// Helper function to find key in map with case-insensitive comparison
static std::map<std::string, std::vector<Record>>::iterator 
findCaseInsensitiveKey(std::map<std::string, std::vector<Record>>& map, const std::string& key) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (strcasecmp_custom(it->first.c_str(), key.c_str()) == 0) {
            return it;
        }
    }
    return map.end();
}

// Helper function to get case-insensitive key from map (returns the actual key in map)
static std::string getCaseInsensitiveKey(const std::map<std::string, std::vector<Record>>& map, const std::string& key) {
    for (const auto& pair : map) {
        if (strcasecmp_custom(pair.first.c_str(), key.c_str()) == 0) {
            return pair.first;
        }
    }
    return key;  // If not found, return original key
}

DataManager::DataManager() {
}

DataManager::~DataManager() {
}

void DataManager::setDatabasePath(const std::string& dbFilePath) {
    m_dbFilePath = dbFilePath;
}

std::string DataManager::getDatFilePath() const {
    return m_dbFilePath + ".dat";
}

bool DataManager::insertRecord(const std::string& tableName, const Record& record) {
    // 读取所有表的数据
    std::map<std::string, std::vector<Record>> allTableData;
    
    std::ifstream readFile(getDatFilePath(), std::ios::binary);
    if (readFile.is_open()) {
        char separator;
        while (readFile.read(&separator, 1)) {
            if (separator != TABLE_SEPARATOR) {
                readFile.close();
                return false;
            }
            
            std::string currentTableName;
            std::vector<Record> records;
            if (!readTableDataFromStream(readFile, currentTableName, records)) {
                readFile.close();
                return false;
            }
            
            // 如果遇到同名表（大小写不敏感），使用新表名（统一大小写）
            // 这样可以确保同一表的所有数据使用相同的表名
            if (strcasecmp_custom(currentTableName.c_str(), tableName.c_str()) == 0) {
                // 同名表，使用新表名（统一大小写）
                allTableData[tableName] = records;
            } else {
                // 不同表，保持原表名
                allTableData[currentTableName] = records;
            }
        }
        readFile.close();
    }
    
    // 添加新记录（使用统一的表名）
    allTableData[tableName].push_back(record);
    
    // 重新写入所有数据
    return rewriteAllTableData(allTableData);
}

bool DataManager::insertRecords(const std::string& tableName, const std::vector<Record>& records) {
    // 读取所有表的数据
    std::map<std::string, std::vector<Record>> allTableData;
    
    std::ifstream readFile(getDatFilePath(), std::ios::binary);
    if (readFile.is_open()) {
        char separator;
        while (readFile.read(&separator, 1)) {
            if (separator != TABLE_SEPARATOR) {
                readFile.close();
                return false;
            }
            
            std::string currentTableName;
            std::vector<Record> currentRecords;
            if (!readTableDataFromStream(readFile, currentTableName, currentRecords)) {
                readFile.close();
                return false;
            }
            
            // 如果遇到同名表（大小写不敏感），使用新表名（统一大小写）
            // 这样可以确保同一表的所有数据使用相同的表名
            if (strcasecmp_custom(currentTableName.c_str(), tableName.c_str()) == 0) {
                // 同名表，使用新表名（统一大小写）
                allTableData[tableName] = currentRecords;
            } else {
                // 不同表，保持原表名
                allTableData[currentTableName] = currentRecords;
            }
        }
        readFile.close();
    }
    
    // 添加新记录（使用统一的表名）
    allTableData[tableName].insert(allTableData[tableName].end(), 
                                records.begin(), records.end());
    
    // 重新写入所有数据
    return rewriteAllTableData(allTableData);
}

bool DataManager::readAllRecords(const std::string& tableName, std::vector<Record>& records) {
    records.clear();
    
    // 注意：在RENAME TABLE操作中，可能需要读取旧表名的数据
    // 此时.dbf文件可能已经更新为新表名，但.dat文件还是旧表名
    // 所以这里不检查.dbf文件，直接从.dat文件读取
    // 这样可以支持RENAME TABLE操作中的数据迁移
    
    // 读取数据文件
    std::ifstream file(getDatFilePath(), std::ios::binary);
    if (!file.is_open()) {
        // 数据文件不存在，返回空列表（不算错误）
        return true;
    }
    
    // 读取所有表的数据，查找目标表（大小写不敏感）
    char separator;
    while (file.read(&separator, 1)) {
        if (separator != TABLE_SEPARATOR) {
            file.close();
            return false;
        }
        
        std::string currentTableName;
        std::vector<Record> currentRecords;
        if (!readTableDataFromStream(file, currentTableName, currentRecords)) {
            file.close();
            return false;
        }
        
        // 使用大小写不敏感比较查找目标表
        // 注意：这里只返回第一个匹配的表数据
        // 如果数据文件中有多个大小写变体的同名表数据，只返回第一个
        // 但正常情况下，clearTable应该已经删除了所有旧数据，不应该有多个变体
        if (strcasecmp_custom(currentTableName.c_str(), tableName.c_str()) == 0) {
            records = currentRecords;
            file.close();
            return true;
        }
    }
    
    file.close();
    // 数据文件中没有该表的数据，返回空列表（不算错误）
    // 这可能是新创建的表，还没有插入数据
    return true;
}

bool DataManager::readValidRecords(const std::string& tableName, std::vector<Record>& records) {
    std::vector<Record> allRecords;
    if (!readAllRecords(tableName, allRecords)) {
        return false;
    }
    
    records.clear();
    for (const auto& record : allRecords) {
        if (record.isValid()) {
            records.push_back(record);
        }
    }
    
    return true;
}

bool DataManager::updateRecord(const std::string& tableName, size_t recordIndex, const Record& record) {
    // 读取所有表的数据
    std::map<std::string, std::vector<Record>> allTableData;
    
    std::ifstream readFile(getDatFilePath(), std::ios::binary);
    if (readFile.is_open()) {
        char separator;
        while (readFile.read(&separator, 1)) {
            if (separator != TABLE_SEPARATOR) {
                readFile.close();
                return false;
            }
            
            std::string currentTableName;
            std::vector<Record> currentRecords;
            if (!readTableDataFromStream(readFile, currentTableName, currentRecords)) {
                readFile.close();
                return false;
            }
            
            // 如果遇到同名表（大小写不敏感），使用新表名（统一大小写）
            if (strcasecmp_custom(currentTableName.c_str(), tableName.c_str()) == 0) {
                allTableData[tableName] = currentRecords;
            } else {
                allTableData[currentTableName] = currentRecords;
            }
        }
        readFile.close();
    }
    
    // 查找并更新记录（使用统一的表名）
    auto it = allTableData.find(tableName);
    if (it == allTableData.end() || recordIndex >= it->second.size()) {
        std::cerr << "Error: Record index out of range" << std::endl;
        return false;
    }
    
    it->second[recordIndex] = record;
    
    // 重新写入所有数据
    return rewriteAllTableData(allTableData);
}

bool DataManager::deleteRecord(const std::string& tableName, size_t recordIndex) {
    // 读取所有表的数据
    std::map<std::string, std::vector<Record>> allTableData;
    
    std::ifstream readFile(getDatFilePath(), std::ios::binary);
    if (readFile.is_open()) {
        char separator;
        while (readFile.read(&separator, 1)) {
            if (separator != TABLE_SEPARATOR) {
                readFile.close();
                return false;
            }
            
            std::string currentTableName;
            std::vector<Record> currentRecords;
            if (!readTableDataFromStream(readFile, currentTableName, currentRecords)) {
                readFile.close();
                return false;
            }
            
            // 如果遇到同名表（大小写不敏感），使用新表名（统一大小写）
            if (strcasecmp_custom(currentTableName.c_str(), tableName.c_str()) == 0) {
                allTableData[tableName] = currentRecords;
            } else {
                allTableData[currentTableName] = currentRecords;
            }
        }
        readFile.close();
    }
    
    // 查找并标记记录为无效（使用统一的表名）
    auto it = allTableData.find(tableName);
    if (it == allTableData.end() || recordIndex >= it->second.size()) {
        std::cerr << "Error: Record index out of range" << std::endl;
        return false;
    }
    
    it->second[recordIndex].markInvalid();
    
    // 重新写入所有数据
    return rewriteAllTableData(allTableData);
}

bool DataManager::getRecordCount(const std::string& tableName, size_t& count) {
    std::vector<Record> records;
    if (!readAllRecords(tableName, records)) {
        return false;
    }
    
    count = records.size();
    return true;
}

bool DataManager::getValidRecordCount(const std::string& tableName, size_t& count) {
    std::vector<Record> records;
    if (!readValidRecords(tableName, records)) {
        return false;
    }
    
    count = records.size();
    return true;
}

bool DataManager::clearTable(const std::string& tableName) {
    // 读取所有表的数据，明确排除目标表（大小写不敏感）
    // 这是硬删除：完全删除目标表的所有数据，包括所有大小写变体
    std::map<std::string, std::vector<Record>> allTableData;
    
    // 如果数据文件不存在，直接返回成功（没有数据需要删除）
    std::ifstream readFile(getDatFilePath(), std::ios::binary);
    if (!readFile.is_open()) {
        // 文件不存在，没有数据需要删除，返回成功
        return true;
    }
    
    // 读取所有表的数据
    char separator;
    bool foundTargetTable = false;
    while (readFile.read(&separator, 1)) {
        if (separator != TABLE_SEPARATOR) {
            readFile.close();
            std::cerr << "Error: File format error, expected separator" << std::endl;
            return false;
        }
        
        std::string currentTableName;
        std::vector<Record> currentRecords;
        if (!readTableDataFromStream(readFile, currentTableName, currentRecords)) {
            readFile.close();
            std::cerr << "Error: Failed to read table data from stream" << std::endl;
            return false;
        }
        
        // 明确检查：如果是目标表（大小写不敏感），则跳过（不添加到map中）
        // 这样可以确保删除所有大小写变体的同名表数据（如 Products, products, PRODUCTS）
        if (strcasecmp_custom(currentTableName.c_str(), tableName.c_str()) == 0) {
            // 这是目标表，跳过，不添加到map中，相当于硬删除
            // 无论有多少条记录，都会被完全删除
            foundTargetTable = true;
            continue;
        }
        
        // 不是目标表，添加到map中保留
        allTableData[currentTableName] = currentRecords;
    }
    readFile.close();
    
    // 如果找到了目标表，需要重新写入文件（排除目标表）
    // 如果没有找到目标表，也需要重新写入文件（确保文件格式正确）
    // 这是硬删除：原文件被删除，新文件只包含非目标表的数据
    return rewriteAllTableData(allTableData);
}

bool DataManager::readTableDataFromStream(std::ifstream& file, std::string& tableName,
                                         std::vector<Record>& records) {
    // 读取表名
    char tableNameBuffer[TABLE_NAME_LENGTH];
    file.read(tableNameBuffer, TABLE_NAME_LENGTH);
    if (file.gcount() != TABLE_NAME_LENGTH) {
        return false;
    }
    tableNameBuffer[TABLE_NAME_LENGTH - 1] = '\0';
    tableName = std::string(tableNameBuffer);
    
    // 读取记录数量
    int recordCount;
    file.read(reinterpret_cast<char*>(&recordCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        return false;
    }
    
    // 读取字段数量
    int fieldCount;
    file.read(reinterpret_cast<char*>(&fieldCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        return false;
    }
    
    // 读取有效标识数组
    std::vector<char> validFlags(recordCount);
    if (recordCount > 0) {
        file.read(validFlags.data(), recordCount);
        if (file.gcount() != recordCount) {
            return false;
        }
    }
    
    // 读取所有记录
    records.clear();
    records.reserve(recordCount);
    for (int i = 0; i < recordCount; i++) {
        Record record;
        record.validFlag = validFlags[i];
        
        // 读取每个字段的值（字符串，变长存储）
        for (int j = 0; j < fieldCount; j++) {
            std::string value;
            if (!readStringValue(file, value)) {
                return false;
            }
            record.setValue(j, value);
        }
        
        records.push_back(record);
    }
    
    return true;
}

bool DataManager::writeTableDataToStream(std::ofstream& file, const std::string& tableName,
                                        const std::vector<Record>& records) {
    // 写入分隔符
    char separator = TABLE_SEPARATOR;
    file.write(&separator, 1);
    if (!file.good()) {
        return false;
    }
    
    // 写入表名（固定长度）
    char tableNameBuffer[TABLE_NAME_LENGTH];
    strncpy(tableNameBuffer, tableName.c_str(), TABLE_NAME_LENGTH - 1);
    tableNameBuffer[TABLE_NAME_LENGTH - 1] = '\0';
    for (size_t i = strlen(tableNameBuffer); i < TABLE_NAME_LENGTH; i++) {
        tableNameBuffer[i] = '\0';
    }
    file.write(tableNameBuffer, TABLE_NAME_LENGTH);
    if (!file.good()) {
        return false;
    }
    
    // 写入记录数量
    int recordCount = static_cast<int>(records.size());
    file.write(reinterpret_cast<const char*>(&recordCount), sizeof(int));
    if (!file.good()) {
        return false;
    }
    
    // 确定字段数量（从第一条记录获取）
    int fieldCount = 0;
    if (!records.empty()) {
        fieldCount = static_cast<int>(records[0].getFieldCount());
    }
    file.write(reinterpret_cast<const char*>(&fieldCount), sizeof(int));
    if (!file.good()) {
        return false;
    }
    
    // 写入有效标识数组
    if (recordCount > 0) {
        std::vector<char> validFlags(recordCount);
        for (size_t i = 0; i < records.size(); i++) {
            validFlags[i] = records[i].validFlag;
        }
        file.write(validFlags.data(), recordCount);
        if (!file.good()) {
            return false;
        }
    }
    
    // 写入所有记录
    for (const auto& record : records) {
        for (int j = 0; j < fieldCount; j++) {
            std::string value = record.getValue(j);
            if (!writeStringValue(file, value)) {
                return false;
            }
        }
    }
    
    return true;
}

bool DataManager::rewriteAllTableData(const std::map<std::string, std::vector<Record>>& allTableData) {
    // 删除原文件
    std::remove(getDatFilePath().c_str());
    
    // 如果所有表数据都为空，直接返回成功（不创建空文件）
    if (allTableData.empty()) {
        return true;
    }
    
    // 创建新文件并写入所有数据
    std::ofstream file(getDatFilePath(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create file " << getDatFilePath() << std::endl;
        return false;
    }
    
    for (const auto& pair : allTableData) {
        // 跳过空表（虽然clearTable已经删除了条目，但为了安全起见还是检查一下）
        if (pair.second.empty()) {
            continue;
        }
        if (!writeTableDataToStream(file, pair.first, pair.second)) {
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

bool DataManager::writeStringValue(std::ofstream& file, const std::string& value) {
    // 变长存储：先写入长度，再写入内容
    int length = static_cast<int>(value.length());
    file.write(reinterpret_cast<const char*>(&length), sizeof(int));
    if (!file.good()) {
        return false;
    }
    
    if (length > 0) {
        file.write(value.c_str(), length);
        if (!file.good()) {
            return false;
        }
    }
    
    return true;
}

bool DataManager::readStringValue(std::ifstream& file, std::string& value) {
    // 读取长度
    int length;
    file.read(reinterpret_cast<char*>(&length), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        return false;
    }
    
    // 读取内容
    if (length > 0) {
        value.resize(length);
        file.read(&value[0], length);
        if (file.gcount() != length) {
            return false;
        }
    } else {
        value.clear();
    }
    
    return true;
}

