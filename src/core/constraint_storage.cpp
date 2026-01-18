/**
 * @file constraint_storage.cpp
 * @brief 约束存储管理器实现
 */

#include "core/constraint_storage.h"
#include "core/constraint_registry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

// 文件格式：
// [MAGIC_HEADER: "DBMS_CST_V1"]
// [表数量: int]
// 对于每个表：
//   [表名长度: int] [表名: char[]]
//   [外键数量: int]
//     对于每个外键：[写入外键约束]
//   [唯一约束数量: int]
//     对于每个唯一约束：[写入唯一约束]
//   [检查约束数量: int]
//     对于每个检查约束：[写入检查约束]

const char* MAGIC_HEADER = "DBMS_CST_V1";

std::string ConstraintStorageManager::getConstraintFilePath(const std::string& dbName, const std::string& dbPath) {
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
    
    // 构建.cst文件路径：与.dbf文件在同一目录，文件名相同但扩展名为.cst
    std::string cstPath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        cstPath += "\\";
        #else
        cstPath += "/";
        #endif
    }
    cstPath += dbName + ".cst";
    return cstPath;
}

bool ConstraintStorageManager::saveConstraints(const std::string& dbName, const std::string& dbPath) {
    std::string filePath = getConstraintFilePath(dbName, dbPath);
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open constraint file for writing: " << filePath << std::endl;
        return false;
    }
    
    // 写入魔数头
    file.write(MAGIC_HEADER, strlen(MAGIC_HEADER));
    if (!file.good()) {
        std::cerr << "Error: Failed to write magic header" << std::endl;
        return false;
    }
    
    // 获取该数据库的所有约束
    ConstraintRegistry& registry = ConstraintRegistry::getInstance();
    std::vector<std::string> tableNames = registry.getTableNames(dbName);
    
    // 写入表数量
    int tableCount = static_cast<int>(tableNames.size());
    file.write(reinterpret_cast<const char*>(&tableCount), sizeof(int));
    if (!file.good()) {
        std::cerr << "Error: Failed to write table count" << std::endl;
        return false;
    }
    
    // 写入每个表的约束
    for (const std::string& tableName : tableNames) {
        TableConstraints constraints;
        if (!registry.getTableConstraints(dbName, tableName, constraints)) {
            // 如果获取失败，跳过该表
            continue;
        }
        
        // 写入表名长度和表名
        int tableNameLen = static_cast<int>(tableName.length());
        file.write(reinterpret_cast<const char*>(&tableNameLen), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write table name length" << std::endl;
            return false;
        }
        file.write(tableName.c_str(), tableNameLen);
        if (!file.good()) {
            std::cerr << "Error: Failed to write table name" << std::endl;
            return false;
        }
        
        // 写入外键约束
        int fkCount = static_cast<int>(constraints.foreignKeys.size());
        file.write(reinterpret_cast<const char*>(&fkCount), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write foreign key count" << std::endl;
            return false;
        }
        for (const auto& fk : constraints.foreignKeys) {
            if (!writeForeignKey(file, fk)) {
                std::cerr << "Error: Failed to write foreign key constraint" << std::endl;
                return false;
            }
        }
        
        // 写入唯一约束
        int uniqueCount = static_cast<int>(constraints.uniqueConstraints.size());
        file.write(reinterpret_cast<const char*>(&uniqueCount), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write unique constraint count" << std::endl;
            return false;
        }
        for (const auto& unique : constraints.uniqueConstraints) {
            if (!writeUniqueConstraint(file, unique)) {
                std::cerr << "Error: Failed to write unique constraint" << std::endl;
                return false;
            }
        }
        
        // 写入检查约束
        int checkCount = static_cast<int>(constraints.checkConstraints.size());
        file.write(reinterpret_cast<const char*>(&checkCount), sizeof(int));
        if (!file.good()) {
            std::cerr << "Error: Failed to write check constraint count" << std::endl;
            return false;
        }
        for (const auto& check : constraints.checkConstraints) {
            if (!writeCheckConstraint(file, check)) {
                std::cerr << "Error: Failed to write check constraint" << std::endl;
                return false;
            }
        }
    }
    
    file.close();
    return true;
}

bool ConstraintStorageManager::loadConstraints(const std::string& dbName, const std::string& dbPath) {
    std::string filePath = getConstraintFilePath(dbName, dbPath);
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // 文件不存在不算错误（可能是新数据库）
        return true;
    }
    
    // 读取魔数头
    char header[16];
    file.read(header, strlen(MAGIC_HEADER));
    if (file.gcount() != strlen(MAGIC_HEADER) || 
        strncmp(header, MAGIC_HEADER, strlen(MAGIC_HEADER)) != 0) {
        std::cerr << "Error: Invalid constraint file format" << std::endl;
        return false;
    }
    
    // 读取表数量
    int tableCount;
    file.read(reinterpret_cast<char*>(&tableCount), sizeof(int));
    if (file.gcount() != sizeof(int)) {
        std::cerr << "Error: Failed to read table count" << std::endl;
        return false;
    }
    
    ConstraintRegistry& registry = ConstraintRegistry::getInstance();
    
    // 读取每个表的约束
    for (int i = 0; i < tableCount; ++i) {
        // 读取表名
        int tableNameLen;
        file.read(reinterpret_cast<char*>(&tableNameLen), sizeof(int));
        if (file.gcount() != sizeof(int)) {
            std::cerr << "Error: Failed to read table name length" << std::endl;
            return false;
        }
        
        std::string tableName(tableNameLen, '\0');
        file.read(&tableName[0], tableNameLen);
        if (file.gcount() != tableNameLen) {
            std::cerr << "Error: Failed to read table name" << std::endl;
            return false;
        }
        
        TableConstraints constraints;
        
        // 读取外键约束
        int fkCount;
        file.read(reinterpret_cast<char*>(&fkCount), sizeof(int));
        if (file.gcount() != sizeof(int)) {
            std::cerr << "Error: Failed to read foreign key count" << std::endl;
            return false;
        }
        
        for (int j = 0; j < fkCount; ++j) {
            ForeignKeyConstraint fk;
            if (!readForeignKey(file, fk)) {
                std::cerr << "Error: Failed to read foreign key constraint" << std::endl;
                return false;
            }
            constraints.foreignKeys.push_back(fk);
        }
        
        // 读取唯一约束
        int uniqueCount;
        file.read(reinterpret_cast<char*>(&uniqueCount), sizeof(int));
        if (file.gcount() != sizeof(int)) {
            std::cerr << "Error: Failed to read unique constraint count" << std::endl;
            return false;
        }
        
        for (int j = 0; j < uniqueCount; ++j) {
            UniqueConstraint unique;
            if (!readUniqueConstraint(file, unique)) {
                std::cerr << "Error: Failed to read unique constraint" << std::endl;
                return false;
            }
            constraints.uniqueConstraints.push_back(unique);
        }
        
        // 读取检查约束
        int checkCount;
        file.read(reinterpret_cast<char*>(&checkCount), sizeof(int));
        if (file.gcount() != sizeof(int)) {
            std::cerr << "Error: Failed to read check constraint count" << std::endl;
            return false;
        }
        
        for (int j = 0; j < checkCount; ++j) {
            CheckConstraint check;
            if (!readCheckConstraint(file, check)) {
                std::cerr << "Error: Failed to read check constraint" << std::endl;
                return false;
            }
            constraints.checkConstraints.push_back(check);
        }
        
        // 注册约束
        registry.registerTableConstraints(dbName, tableName, constraints);
    }
    
    file.close();
    return true;
}

bool ConstraintStorageManager::deleteConstraints(const std::string& dbName, const std::string& dbPath) {
    std::string filePath = getConstraintFilePath(dbName, dbPath);
    // 检查文件是否存在
    std::ifstream testFile(filePath);
    if (testFile.good()) {
        testFile.close();
        // 文件存在，删除它
        return std::remove(filePath.c_str()) == 0;
    }
    return true;  // 文件不存在也算成功
}

bool ConstraintStorageManager::writeForeignKey(std::ofstream& file, const ForeignKeyConstraint& fk) {
    // 写入约束名
    file.write(fk.constraintName, CONSTRAINT_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入字段名
    file.write(fk.fieldName, FIELD_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入引用表名
    file.write(fk.referencedTable, TABLE_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入引用字段名
    file.write(fk.referencedField, FIELD_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入ON DELETE动作
    file.write(fk.onDeleteAction, 16);
    if (!file.good()) return false;
    
    // 写入ON UPDATE动作
    file.write(fk.onUpdateAction, 16);
    if (!file.good()) return false;
    
    return true;
}

bool ConstraintStorageManager::readForeignKey(std::ifstream& file, ForeignKeyConstraint& fk) {
    // 读取约束名
    file.read(fk.constraintName, CONSTRAINT_NAME_LENGTH);
    if (file.gcount() != CONSTRAINT_NAME_LENGTH) return false;
    fk.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
    
    // 读取字段名
    file.read(fk.fieldName, FIELD_NAME_LENGTH);
    if (file.gcount() != FIELD_NAME_LENGTH) return false;
    fk.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
    
    // 读取引用表名
    file.read(fk.referencedTable, TABLE_NAME_LENGTH);
    if (file.gcount() != TABLE_NAME_LENGTH) return false;
    fk.referencedTable[TABLE_NAME_LENGTH - 1] = '\0';
    
    // 读取引用字段名
    file.read(fk.referencedField, FIELD_NAME_LENGTH);
    if (file.gcount() != FIELD_NAME_LENGTH) return false;
    fk.referencedField[FIELD_NAME_LENGTH - 1] = '\0';
    
    // 读取ON DELETE动作
    file.read(fk.onDeleteAction, 16);
    if (file.gcount() != 16) return false;
    fk.onDeleteAction[15] = '\0';
    
    // 读取ON UPDATE动作
    file.read(fk.onUpdateAction, 16);
    if (file.gcount() != 16) return false;
    fk.onUpdateAction[15] = '\0';
    
    return true;
}

bool ConstraintStorageManager::writeUniqueConstraint(std::ofstream& file, const UniqueConstraint& unique) {
    // 写入约束名
    file.write(unique.constraintName, CONSTRAINT_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入字段数量
    int fieldCount = static_cast<int>(unique.fieldNames.size());
    file.write(reinterpret_cast<const char*>(&fieldCount), sizeof(int));
    if (!file.good()) return false;
    
    // 写入每个字段名
    for (const std::string& fieldName : unique.fieldNames) {
        int nameLen = static_cast<int>(fieldName.length());
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
        if (!file.good()) return false;
        file.write(fieldName.c_str(), nameLen);
        if (!file.good()) return false;
    }
    
    return true;
}

bool ConstraintStorageManager::readUniqueConstraint(std::ifstream& file, UniqueConstraint& unique) {
    // 读取约束名
    file.read(unique.constraintName, CONSTRAINT_NAME_LENGTH);
    if (file.gcount() != CONSTRAINT_NAME_LENGTH) return false;
    unique.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
    
    // 读取字段数量
    int fieldCount;
    file.read(reinterpret_cast<char*>(&fieldCount), sizeof(int));
    if (file.gcount() != sizeof(int)) return false;
    
    // 读取每个字段名
    unique.fieldNames.clear();
    for (int i = 0; i < fieldCount; ++i) {
        int nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
        if (file.gcount() != sizeof(int)) return false;
        
        std::string fieldName(nameLen, '\0');
        file.read(&fieldName[0], nameLen);
        if (file.gcount() != nameLen) return false;
        unique.fieldNames.push_back(fieldName);
    }
    
    return true;
}

bool ConstraintStorageManager::writeCheckConstraint(std::ofstream& file, const CheckConstraint& check) {
    // 写入约束名
    file.write(check.constraintName, CONSTRAINT_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入字段名
    file.write(check.fieldName, FIELD_NAME_LENGTH);
    if (!file.good()) return false;
    
    // 写入表达式长度和内容
    int exprLen = static_cast<int>(check.expression.length());
    file.write(reinterpret_cast<const char*>(&exprLen), sizeof(int));
    if (!file.good()) return false;
    file.write(check.expression.c_str(), exprLen);
    if (!file.good()) return false;
    
    return true;
}

bool ConstraintStorageManager::readCheckConstraint(std::ifstream& file, CheckConstraint& check) {
    // 读取约束名
    file.read(check.constraintName, CONSTRAINT_NAME_LENGTH);
    if (file.gcount() != CONSTRAINT_NAME_LENGTH) return false;
    check.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
    
    // 读取字段名
    file.read(check.fieldName, FIELD_NAME_LENGTH);
    if (file.gcount() != FIELD_NAME_LENGTH) return false;
    check.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
    
    // 读取表达式长度和内容
    int exprLen;
    file.read(reinterpret_cast<char*>(&exprLen), sizeof(int));
    if (file.gcount() != sizeof(int)) return false;
    
    check.expression.resize(exprLen);
    file.read(&check.expression[0], exprLen);
    if (file.gcount() != exprLen) return false;
    
    return true;
}
