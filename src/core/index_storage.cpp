/**
 * @file index_storage.cpp
 * @brief 索引存储管理器实现
 */

#include "core/index_storage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

// 文件格式：
// [MAGIC_HEADER: "DBMS_IDX_V1"]
// [索引数量: int]
// 对于每个索引：
//   [表名长度: int] [表名: char[]]
//   [字段名长度: int] [字段名: char[]]
//   [索引类型长度: int] [索引类型: char[]]

static const char* INDEX_MAGIC_HEADER = "DBMS_IDX_V1";

std::string IndexStorageManager::getIndexFilePath(const std::string& dbName, const std::string& dbPath) {
    // 从dbPath提取目录
    std::string dirStr;
    std::string filename;
    
    // 查找最后一个路径分隔符
    size_t lastSlash = dbPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirStr = dbPath.substr(0, lastSlash);
        filename = dbPath.substr(lastSlash + 1);
    } else {
        dirStr = ".";  // 当前目录
        filename = dbPath;
    }
    
    // 构建.idx文件路径：与.dbf文件在同一目录，文件名相同但扩展名为.idx
    std::string idxPath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        idxPath += "\\";
        #else
        idxPath += "/";
        #endif
    }
    idxPath += dbName + ".idx";
    return idxPath;
}

bool IndexStorageManager::saveIndices(const std::string& dbName, const std::string& dbPath, 
                                     IndexManager& indexManager) {
    std::string filePath = getIndexFilePath(dbName, dbPath);
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open index file for writing: " << filePath << std::endl;
        return false;
    }
    
    // 写入魔数头
    file.write(INDEX_MAGIC_HEADER, strlen(INDEX_MAGIC_HEADER));
    if (!file.good()) {
        std::cerr << "Error: Failed to write magic header" << std::endl;
        return false;
    }
    
    // 获取所有索引
    std::vector<IndexInfo> indices;
    if (!indexManager.getAllIndices(indices)) {
        std::cerr << "Error: Failed to get all indices" << std::endl;
        return false;
    }
    
    // 写入索引数量
    int indexCount = static_cast<int>(indices.size());
    file.write(reinterpret_cast<const char*>(&indexCount), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write index count" << std::endl;
        return false;
    }
    
    // 写入每个索引
    for (const auto& index : indices) {
        if (!writeIndexInfo(file, index)) {
            std::cerr << "Error: Failed to write index info" << std::endl;
            return false;
        }
    }
    
    file.close();
    return true;
}

bool IndexStorageManager::loadIndices(const std::string& dbName, const std::string& dbPath, 
                                      IndexManager& indexManager) {
    std::string filePath = getIndexFilePath(dbName, dbPath);
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在不算错误（可能是新数据库或没有索引）
        return true;
    }
    
    // 读取魔数头
    char header[16];
    file.read(header, strlen(INDEX_MAGIC_HEADER));
    if (file.gcount() != strlen(INDEX_MAGIC_HEADER) || 
        strncmp(header, INDEX_MAGIC_HEADER, strlen(INDEX_MAGIC_HEADER)) != 0) {
        std::cerr << "Error: Invalid index file format" << std::endl;
        return false;
    }
    
    // 读取索引数量
    int indexCount;
    file.read(reinterpret_cast<char*>(&indexCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read index count" << std::endl;
        return false;
    }
    
    // 确保数据库路径已设置
    indexManager.setDatabasePath(dbPath);
    
    // 读取每个索引并重建
    for (int i = 0; i < indexCount; ++i) {
        IndexInfo info;
        if (!readIndexInfo(file, info)) {
            std::cerr << "Error: Failed to read index info" << std::endl;
            return false;
        }
        
        // 重建索引
        if (!indexManager.createIndex(info.tableName, info.fieldName, info.indexType)) {
            std::cerr << "Warning: Failed to rebuild index " << info.indexName 
                      << " for table " << info.tableName << ", field " << info.fieldName << std::endl;
            // 继续处理其他索引，不中断
        }
    }
    
    file.close();
    return true;
}

bool IndexStorageManager::deleteIndices(const std::string& dbName, const std::string& dbPath) {
    std::string filePath = getIndexFilePath(dbName, dbPath);
    // 检查文件是否存在
    std::ifstream testFile(filePath);
    if (testFile.good()) {
        testFile.close();
        // 文件存在，删除它
        return std::remove(filePath.c_str()) == 0;
    }
    return true;  // 文件不存在也算成功
}

bool IndexStorageManager::writeIndexInfo(std::ofstream& file, const IndexInfo& info) {
    // 写入表名长度和表名
    int tableNameLen = static_cast<int>(info.tableName.length());
    file.write(reinterpret_cast<const char*>(&tableNameLen), sizeof(int));
    if (!file.good()) return false;
    file.write(info.tableName.c_str(), tableNameLen);
    if (!file.good()) return false;
    
    // 写入字段名长度和字段名
    int fieldNameLen = static_cast<int>(info.fieldName.length());
    file.write(reinterpret_cast<const char*>(&fieldNameLen), sizeof(int));
    if (!file.good()) return false;
    file.write(info.fieldName.c_str(), fieldNameLen);
    if (!file.good()) return false;
    
    // 写入索引类型长度和索引类型
    int indexTypeLen = static_cast<int>(info.indexType.length());
    file.write(reinterpret_cast<const char*>(&indexTypeLen), sizeof(int));
    if (!file.good()) return false;
    file.write(info.indexType.c_str(), indexTypeLen);
    if (!file.good()) return false;
    
    return true;
}

bool IndexStorageManager::readIndexInfo(std::ifstream& file, IndexInfo& info) {
    // 读取表名长度和表名
    int tableNameLen;
    file.read(reinterpret_cast<char*>(&tableNameLen), sizeof(int));
    if (file.gcount() != sizeof(int)) return false;
    
    info.tableName.resize(tableNameLen);
    file.read(&info.tableName[0], tableNameLen);
    if (file.gcount() != tableNameLen) return false;
    
    // 读取字段名长度和字段名
    int fieldNameLen;
    file.read(reinterpret_cast<char*>(&fieldNameLen), sizeof(int));
    if (file.gcount() != sizeof(int)) return false;
    
    info.fieldName.resize(fieldNameLen);
    file.read(&info.fieldName[0], fieldNameLen);
    if (file.gcount() != fieldNameLen) return false;
    
    // 读取索引类型长度和索引类型
    int indexTypeLen;
    file.read(reinterpret_cast<char*>(&indexTypeLen), sizeof(int));
    if (file.gcount() != sizeof(int)) return false;
    
    info.indexType.resize(indexTypeLen);
    file.read(&info.indexType[0], indexTypeLen);
    if (file.gcount() != indexTypeLen) return false;
    
    // 索引名称和字段索引会在重建索引时自动生成
    info.isActive = true;
    
    return true;
}

