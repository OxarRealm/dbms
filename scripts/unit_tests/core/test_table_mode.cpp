/**
 * @file test_table_mode.cpp
 * @brief TableMode数据结构测试程序
 * 
 * 测试TableMode结构体及相关数据结构的编译和基本功能
 */

#include "../../include/core/table_mode.h"
#include <iostream>
#include <cassert>
#include <cstring>

// 测试结果统计
static int testsPassed = 0;
static int testsFailed = 0;

// 测试宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            std::cout << "  ✓ " << message << std::endl; \
            testsPassed++; \
        } else { \
            std::cout << "  ✗ " << message << " (FAILED)" << std::endl; \
            testsFailed++; \
        } \
    } while(0)

/**
 * @brief 测试常量定义
 */
void testConstants() {
    std::cout << "\n[测试1] 常量定义测试" << std::endl;
    
    TEST_ASSERT(FIELD_NAME_LENGTH == 32, "FIELD_NAME_LENGTH常量");
    TEST_ASSERT(TYPE_NAME_LENGTH == 8, "TYPE_NAME_LENGTH常量");
    TEST_ASSERT(TABLE_NAME_LENGTH == 64, "TABLE_NAME_LENGTH常量");
    TEST_ASSERT(TABLE_SEPARATOR == '~', "TABLE_SEPARATOR常量");
    TEST_ASSERT(FLAG_KEY == 1, "FLAG_KEY常量");
    TEST_ASSERT(FLAG_NOT_KEY == 0, "FLAG_NOT_KEY常量");
    TEST_ASSERT(FLAG_NULL == 1, "FLAG_NULL常量");
    TEST_ASSERT(FLAG_NO_NULL == 0, "FLAG_NO_NULL常量");
    TEST_ASSERT(FLAG_VALID == 1, "FLAG_VALID常量");
    TEST_ASSERT(FLAG_INVALID == 0, "FLAG_INVALID常量");
}

/**
 * @brief 测试TableMode结构体定义
 */
void testTableModeStruct() {
    std::cout << "\n[测试2] TableMode结构体定义测试" << std::endl;
    
    TableMode mode;
    initTableMode(mode, "UserID", "int", 4, FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    
    TEST_ASSERT(strcmp(mode.sFieldName, "UserID") == 0, "字段名初始化");
    TEST_ASSERT(strcmp(mode.sType, "int") == 0, "字段类型初始化");
    TEST_ASSERT(mode.iSize == 4, "字段大小初始化");
    TEST_ASSERT(mode.bKey == FLAG_KEY, "KEY标志初始化");
    TEST_ASSERT(mode.bNullFlag == FLAG_NO_NULL, "NULL标志初始化");
    TEST_ASSERT(mode.bValidFlag == FLAG_VALID, "VALID标志初始化");
    
    // 测试结构体大小
    std::cout << "  TableMode结构体大小: " << sizeof(TableMode) << " 字节" << std::endl;
}

/**
 * @brief 测试TableInfo结构
 */
void testTableInfo() {
    std::cout << "\n[测试3] TableInfo结构测试" << std::endl;
    
    TableInfo tableInfo("Users");
    TEST_ASSERT(strcmp(tableInfo.tableName, "Users") == 0, "表名初始化");
    TEST_ASSERT(tableInfo.fields.size() == 0, "初始字段列表为空");
    
    // 添加字段
    TableMode field1, field2;
    initTableMode(field1, "UserID", "int", 4, FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(field2, "UserName", "char", 50, FLAG_NOT_KEY, FLAG_NO_NULL, FLAG_VALID);
    
    tableInfo.fields.push_back(field1);
    tableInfo.fields.push_back(field2);
    
    TEST_ASSERT(tableInfo.fields.size() == 2, "字段列表添加");
    TEST_ASSERT(strcmp(tableInfo.fields[0].sFieldName, "UserID") == 0, "第一个字段名");
    TEST_ASSERT(strcmp(tableInfo.fields[1].sFieldName, "UserName") == 0, "第二个字段名");
}

/**
 * @brief 测试Record结构
 */
void testRecord() {
    std::cout << "\n[测试4] Record结构测试" << std::endl;
    
    Record record;
    TEST_ASSERT(record.isValid() == true, "记录初始状态为有效");
    TEST_ASSERT(record.getFieldCount() == 0, "初始字段数量为0");
    
    // 设置字段值
    record.setValue(0, "1");
    record.setValue(1, "Alice");
    record.setValue(2, "2025-01-01");
    
    TEST_ASSERT(record.getFieldCount() == 3, "字段数量");
    TEST_ASSERT(record.getValue(0) == "1", "第一个字段值");
    TEST_ASSERT(record.getValue(1) == "Alice", "第二个字段值");
    TEST_ASSERT(record.getValue(2) == "2025-01-01", "第三个字段值");
    
    // 测试无效标记
    record.markInvalid();
    TEST_ASSERT(record.isValid() == false, "记录标记为无效");
}

/**
 * @brief 测试TableData结构
 */
void testTableData() {
    std::cout << "\n[测试5] TableData结构测试" << std::endl;
    
    TableData tableData("Users");
    TEST_ASSERT(strcmp(tableData.tableName, "Users") == 0, "表名初始化");
    TEST_ASSERT(tableData.getRecordCount() == 0, "初始记录数量为0");
    TEST_ASSERT(tableData.getValidRecordCount() == 0, "初始有效记录数量为0");
    
    // 添加记录
    Record record1, record2;
    record1.setValue(0, "1");
    record1.setValue(1, "Alice");
    record2.setValue(0, "2");
    record2.setValue(1, "Bob");
    
    tableData.records.push_back(record1);
    tableData.records.push_back(record2);
    
    TEST_ASSERT(tableData.getRecordCount() == 2, "记录数量");
    TEST_ASSERT(tableData.getValidRecordCount() == 2, "有效记录数量");
    
    // 标记一条记录为无效
    tableData.records[0].markInvalid();
    TEST_ASSERT(tableData.getValidRecordCount() == 1, "标记无效后的有效记录数量");
}

/**
 * @brief 测试辅助函数
 */
void testHelperFunctions() {
    std::cout << "\n[测试6] 辅助函数测试" << std::endl;
    
    TableMode mode1, mode2, mode3;
    initTableMode(mode1, "UserID", "int", 4, FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(mode2, "UserID", "int", 4, FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(mode3, "UserName", "char", 50, FLAG_NOT_KEY, FLAG_NO_NULL, FLAG_VALID);
    
    TEST_ASSERT(compareTableMode(mode1, mode2) == true, "相同TableMode比较");
    TEST_ASSERT(compareTableMode(mode1, mode3) == false, "不同TableMode比较");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  TableMode数据结构测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testConstants();
        testTableModeStruct();
        testTableInfo();
        testRecord();
        testTableData();
        testHelperFunctions();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  测试结果汇总" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "通过: " << testsPassed << " 个测试" << std::endl;
        std::cout << "失败: " << testsFailed << " 个测试" << std::endl;
        std::cout << "总计: " << (testsPassed + testsFailed) << " 个测试" << std::endl;
        
        if (testsFailed == 0) {
            std::cout << "\n✓ 所有测试通过！" << std::endl;
            return 0;
        } else {
            std::cout << "\n✗ 有测试失败！" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
}