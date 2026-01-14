/**
 * @file data_manager.cpp
 * @brief 数据记录管理器实现
 */

#include "core/data_manager.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <map>

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
            
            allTableData[currentTableName] = records;
        }
        readFile.close();
    }
    
    // 添加新记录
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
            
            allTableData[currentTableName] = currentRecords;
        }
        readFile.close();
    }
    
    // 添加新记录
    allTableData[tableName].insert(allTableData[tableName].end(), 
                                    records.begin(), records.end());
    
    // 重新写入所有数据
    return rewriteAllTableData(allTableData);
}

bool DataManager::readAllRecords(const std::string& tableName, std::vector<Record>& records) {
    records.clear();
    
    std::ifstream file(getDatFilePath(), std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在，返回空列表（不算错误）
        return true;
    }
    
    // 读取所有表的数据，查找目标表
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
        
        if (currentTableName == tableName) {
            records = currentRecords;
            file.close();
            return true;
        }
    }
    
    file.close();
    return true;  // 表不存在，返回空列表（不算错误）
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
            
            allTableData[currentTableName] = currentRecords;
        }
        readFile.close();
    }
    
    // 查找并更新记录
    auto it = allTableData.find(tableName);
    if (it == allTableData.end() || recordIndex >= it->second.size()) {
        std::cerr << "错误: 记录索引超出范围" << std::endl;
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
            
            allTableData[currentTableName] = currentRecords;
        }
        readFile.close();
    }
    
    // 查找并标记记录为无效
    auto it = allTableData.find(tableName);
    if (it == allTableData.end() || recordIndex >= it->second.size()) {
        std::cerr << "错误: 记录索引超出范围" << std::endl;
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
            
            allTableData[currentTableName] = currentRecords;
        }
        readFile.close();
    }
    
    // 清空目标表的数据
    allTableData[tableName].clear();
    
    // 重新写入所有数据
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
    
    // 创建新文件并写入所有数据
    std::ofstream file(getDatFilePath(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "错误: 无法创建文件 " << getDatFilePath() << std::endl;
        return false;
    }
    
    for (const auto& pair : allTableData) {
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

