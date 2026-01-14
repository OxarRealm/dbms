#pragma once

#include <cstring>
#include <vector>
#include <string>

/**
 * @file table_mode.h
 * @brief 核心数据结构定义
 * 
 * 定义表结构、字段结构等核心数据结构
 */

// 常量定义
#define FIELD_NAME_LENGTH 32      // 字段名最大长度
#define TYPE_NAME_LENGTH 8        // 类型名最大长度
#define TABLE_NAME_LENGTH 64      // 表名最大长度
#define DB_NAME_LENGTH 64         // 数据库文件名最大长度
#define TABLE_SEPARATOR '~'       // 表分隔符

// 标志常量
#define FLAG_KEY 1                // KEY标志
#define FLAG_NOT_KEY 0            // NOT_KEY标志
#define FLAG_NULL 1               // NULL标志（允许为空）
#define FLAG_NO_NULL 0            // NO_NULL标志（不允许为空）
#define FLAG_VALID 1              // VALID标志（有效）
#define FLAG_INVALID 0            // INVALID标志（无效）

/**
 * @brief 字段结构定义
 * 
 * 对应课程要求的TableMode结构体
 * 用于定义表的字段结构
 */
typedef struct {
    char sFieldName[FIELD_NAME_LENGTH];  // 字段名
    char sType[TYPE_NAME_LENGTH];        // 字段类型（如"int", "char"等）
    int iSize;                           // 字长
    char bKey;                           // 是否为KEY键（1=KEY, 0=NOT_KEY）
    char bNullFlag;                      // 是否允许为空（1=NULL, 0=NO_NULL）
    char bValidFlag;                     // 是否有效（1=VALID, 0=INVALID）
} TableMode, *PTableMode;

/**
 * @brief 表结构信息
 * 
 * 包含表名和字段列表
 */
struct TableInfo {
    char tableName[TABLE_NAME_LENGTH];   // 表名
    std::vector<TableMode> fields;       // 字段列表
    
    TableInfo() {
        tableName[0] = '\0';
    }
    
    TableInfo(const char* name) {
        strncpy(tableName, name, TABLE_NAME_LENGTH - 1);
        tableName[TABLE_NAME_LENGTH - 1] = '\0';
    }
};

/**
 * @brief 记录数据结构
 * 
 * 用于存储一条记录的数据
 */
struct Record {
    std::vector<std::string> values;     // 字段值列表（按字段顺序）
    char validFlag;                      // 有效标识（1=有效, 0=无效/已删除）
    
    Record() : validFlag(FLAG_VALID) {
    }
    
    // 获取字段值
    std::string getValue(size_t index) const {
        if (index < values.size()) {
            return values[index];
        }
        return "";
    }
    
    // 设置字段值
    void setValue(size_t index, const std::string& value) {
        if (index >= values.size()) {
            values.resize(index + 1);
        }
        values[index] = value;
    }
    
    // 获取字段数量
    size_t getFieldCount() const {
        return values.size();
    }
    
    // 判断是否有效
    bool isValid() const {
        return validFlag == FLAG_VALID;
    }
    
    // 标记为无效
    void markInvalid() {
        validFlag = FLAG_INVALID;
    }
};

/**
 * @brief 表数据信息
 * 
 * 包含表的记录列表
 */
struct TableData {
    char tableName[TABLE_NAME_LENGTH];   // 表名
    std::vector<Record> records;         // 记录列表
    
    TableData() {
        tableName[0] = '\0';
    }
    
    TableData(const char* name) {
        strncpy(tableName, name, TABLE_NAME_LENGTH - 1);
        tableName[TABLE_NAME_LENGTH - 1] = '\0';
    }
    
    // 获取记录数量
    size_t getRecordCount() const {
        return records.size();
    }
    
    // 获取有效记录数量
    size_t getValidRecordCount() const {
        size_t count = 0;
        for (const auto& record : records) {
            if (record.isValid()) {
                count++;
            }
        }
        return count;
    }
};

// 辅助函数：初始化TableMode结构体
inline void initTableMode(TableMode& mode, const char* fieldName, const char* type, 
                          int size, char keyFlag, char nullFlag, char validFlag) {
    strncpy(mode.sFieldName, fieldName, FIELD_NAME_LENGTH - 1);
    mode.sFieldName[FIELD_NAME_LENGTH - 1] = '\0';
    strncpy(mode.sType, type, TYPE_NAME_LENGTH - 1);
    mode.sType[TYPE_NAME_LENGTH - 1] = '\0';
    mode.iSize = size;
    mode.bKey = keyFlag;
    mode.bNullFlag = nullFlag;
    mode.bValidFlag = validFlag;
}

// 辅助函数：比较两个TableMode是否相等
inline bool compareTableMode(const TableMode& mode1, const TableMode& mode2) {
    return strcmp(mode1.sFieldName, mode2.sFieldName) == 0 &&
           strcmp(mode1.sType, mode2.sType) == 0 &&
           mode1.iSize == mode2.iSize &&
           mode1.bKey == mode2.bKey &&
           mode1.bNullFlag == mode2.bNullFlag &&
           mode1.bValidFlag == mode2.bValidFlag;
}
