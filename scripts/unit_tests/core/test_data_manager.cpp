/**
 * @file test_data_manager.cpp
 * @brief DataManager类测试程序
 * 
 * 测试.dat文件读写功能
 */

#include "../../include/core/data_manager.h"
#include "../../include/core/table_mode.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>

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

// 测试数据库文件名
const char* TEST_DB_FILE = "test_db_data";

/**
 * @brief 创建测试记录
 */
Record createTestRecord(const std::vector<std::string>& values) {
    Record record;
    for (size_t i = 0; i < values.size(); i++) {
        record.setValue(i, values[i]);
    }
    return record;
}

/**
 * @brief 测试插入记录
 */
void testInsertRecord() {
    std::cout << "\n[测试1] 插入记录测试" << std::endl;
    
    // 删除测试文件（如果存在）
    std::remove((std::string(TEST_DB_FILE) + ".dat").c_str());
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    Record record = createTestRecord({"1", "Alice", "25"});
    bool result = manager.insertRecord("Users", record);
    
    TEST_ASSERT(result == true, "插入记录成功");
    
    size_t count = 0;
    manager.getRecordCount("Users", count);
    TEST_ASSERT(count == 1, "记录数量正确");
}

/**
 * @brief 测试读取记录
 */
void testReadRecords() {
    std::cout << "\n[测试2] 读取记录测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<Record> records;
    bool result = manager.readAllRecords("Users", records);
    
    TEST_ASSERT(result == true, "读取记录成功");
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].getValue(0) == "1", "第一个字段值正确");
    TEST_ASSERT(records[0].getValue(1) == "Alice", "第二个字段值正确");
    TEST_ASSERT(records[0].getValue(2) == "25", "第三个字段值正确");
    TEST_ASSERT(records[0].isValid() == true, "记录有效");
}

/**
 * @brief 测试插入多条记录
 */
void testInsertMultipleRecords() {
    std::cout << "\n[测试3] 插入多条记录测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<Record> records;
    records.push_back(createTestRecord({"2", "Bob", "30"}));
    records.push_back(createTestRecord({"3", "Charlie", "35"}));
    
    bool result = manager.insertRecords("Users", records);
    TEST_ASSERT(result == true, "插入多条记录成功");
    
    size_t count = 0;
    manager.getRecordCount("Users", count);
    TEST_ASSERT(count == 3, "记录数量正确");
}

/**
 * @brief 测试多表数据存储
 */
void testMultipleTables() {
    std::cout << "\n[测试4] 多表数据存储测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    Record songRecord = createTestRecord({"1", "Song1", "Artist1", "2020"});
    bool result = manager.insertRecord("Songs", songRecord);
    TEST_ASSERT(result == true, "插入Songs表记录成功");
    
    // 检查两个表的记录
    size_t userCount = 0, songCount = 0;
    manager.getRecordCount("Users", userCount);
    manager.getRecordCount("Songs", songCount);
    
    TEST_ASSERT(userCount == 3, "Users表记录数量正确");
    TEST_ASSERT(songCount == 1, "Songs表记录数量正确");
}

/**
 * @brief 测试更新记录
 */
void testUpdateRecord() {
    std::cout << "\n[测试5] 更新记录测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    Record newRecord = createTestRecord({"1", "Alice Updated", "26"});
    bool result = manager.updateRecord("Users", 0, newRecord);
    TEST_ASSERT(result == true, "更新记录成功");
    
    std::vector<Record> records;
    manager.readAllRecords("Users", records);
    TEST_ASSERT(records[0].getValue(1) == "Alice Updated", "记录更新正确");
    TEST_ASSERT(records[0].getValue(2) == "26", "记录更新正确");
}

/**
 * @brief 测试删除记录（标记为无效）
 */
void testDeleteRecord() {
    std::cout << "\n[测试6] 删除记录测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    bool result = manager.deleteRecord("Users", 1);
    TEST_ASSERT(result == true, "删除记录成功");
    
    // 检查所有记录（包括已删除的）
    size_t totalCount = 0;
    manager.getRecordCount("Users", totalCount);
    TEST_ASSERT(totalCount == 3, "总记录数量不变");
    
    // 检查有效记录
    size_t validCount = 0;
    manager.getValidRecordCount("Users", validCount);
    TEST_ASSERT(validCount == 2, "有效记录数量正确");
    
    // 验证记录被标记为无效
    std::vector<Record> allRecords;
    manager.readAllRecords("Users", allRecords);
    TEST_ASSERT(allRecords[0].isValid() == true, "第一条记录有效");
    TEST_ASSERT(allRecords[1].isValid() == false, "第二条记录无效");
    TEST_ASSERT(allRecords[2].isValid() == true, "第三条记录有效");
}

/**
 * @brief 测试读取有效记录
 */
void testReadValidRecords() {
    std::cout << "\n[测试7] 读取有效记录测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<Record> validRecords;
    bool result = manager.readValidRecords("Users", validRecords);
    
    TEST_ASSERT(result == true, "读取有效记录成功");
    TEST_ASSERT(validRecords.size() == 2, "有效记录数量正确");
    TEST_ASSERT(validRecords[0].getValue(0) == "1", "第一条记录正确");
    TEST_ASSERT(validRecords[1].getValue(0) == "3", "第二条记录正确");
}

/**
 * @brief 测试清空表
 */
void testClearTable() {
    std::cout << "\n[测试8] 清空表测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    bool result = manager.clearTable("Songs");
    TEST_ASSERT(result == true, "清空表成功");
    
    size_t count = 0;
    manager.getRecordCount("Songs", count);
    TEST_ASSERT(count == 0, "表记录数量为0");
    
    // 验证其他表不受影响
    size_t userCount = 0;
    manager.getRecordCount("Users", userCount);
    TEST_ASSERT(userCount == 3, "其他表记录不受影响");
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试9] 错误处理测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 测试更新不存在的记录索引
    Record record = createTestRecord({"999", "Test", "99"});
    bool result = manager.updateRecord("Users", 100, record);
    TEST_ASSERT(result == false, "更新不存在的记录索引返回false");
    
    // 测试删除不存在的记录索引
    result = manager.deleteRecord("Users", 100);
    TEST_ASSERT(result == false, "删除不存在的记录索引返回false");
    
    // 测试读取不存在的表（应该返回空列表，不算错误）
    std::vector<Record> records;
    result = manager.readAllRecords("NonExistentTable", records);
    TEST_ASSERT(result == true, "读取不存在的表返回true");
    TEST_ASSERT(records.size() == 0, "不存在的表返回空列表");
}

/**
 * @brief 测试空记录和特殊值
 */
void testSpecialValues() {
    std::cout << "\n[测试10] 特殊值测试" << std::endl;
    
    DataManager manager;
    manager.setDatabasePath(TEST_DB_FILE);
    
    // 测试空字符串
    Record emptyRecord = createTestRecord({"", "", ""});
    bool result = manager.insertRecord("TestTable", emptyRecord);
    TEST_ASSERT(result == true, "插入空字符串记录成功");
    
    std::vector<Record> records;
    manager.readAllRecords("TestTable", records);
    TEST_ASSERT(records.size() == 1, "记录数量正确");
    TEST_ASSERT(records[0].getValue(0) == "", "空字符串正确保存");
    
    // 测试长字符串
    std::string longString(200, 'A');
    Record longRecord = createTestRecord({longString, "Test", "Value"});
    result = manager.insertRecord("TestTable", longRecord);
    TEST_ASSERT(result == true, "插入长字符串记录成功");
    
    manager.readAllRecords("TestTable", records);
    TEST_ASSERT(records[1].getValue(0) == longString, "长字符串正确保存");
}

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::remove((std::string(TEST_DB_FILE) + ".dat").c_str());
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  DataManager测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testInsertRecord();
        testReadRecords();
        testInsertMultipleRecords();
        testMultipleTables();
        testUpdateRecord();
        testDeleteRecord();
        testReadValidRecords();
        testClearTable();
        testErrorHandling();
        testSpecialValues();
        
        cleanupTestFiles();
        
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
        cleanupTestFiles();
        return 1;
    }
}

