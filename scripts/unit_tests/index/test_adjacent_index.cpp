/**
 * @file test_adjacent_index.cpp
 * @brief 相邻索引测试程序
 * 
 * 测试AdjacentIndex类的功能
 */

#include "../../../include/core/adjacent_index.h"
#include "../../../include/core/table_manager.h"
#include "../../../include/core/data_manager.h"
#include <iostream>
#include <vector>
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

// 测试数据库文件名
const char* TEST_DB_FILE = "test_db_adjacent_index";

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::string dbfFile = std::string(TEST_DB_FILE) + ".dbf";
    std::string datFile = std::string(TEST_DB_FILE) + ".dat";
    std::remove(dbfFile.c_str());
    std::remove(datFile.c_str());
}

/**
 * @brief 创建测试表
 */
bool createTestTable() {
    TableManager tableManager;
    tableManager.setDatabasePath(TEST_DB_FILE);

    TableInfo tableInfo("Songs");
    TableMode field1, field2, field3;
    initTableMode(field1, "SongID", "int", sizeof(int), FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(field2, "SongName", "string", 100, FLAG_NOT_KEY, FLAG_NO_NULL, FLAG_VALID);
    initTableMode(field3, "Year", "int", sizeof(int), FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    tableInfo.fields.push_back(field1);
    tableInfo.fields.push_back(field2);
    tableInfo.fields.push_back(field3);

    return tableManager.createTable(tableInfo);
}

/**
 * @brief 插入测试数据
 */
bool insertTestData() {
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);

    // 插入5条记录（SongID: 5, 2, 8, 1, 9 - 无序）
    Record r1, r2, r3, r4, r5;
    r1.setValue(0, "5");
    r1.setValue(1, "Song5");
    r1.setValue(2, "2020");
    
    r2.setValue(0, "2");
    r2.setValue(1, "Song2");
    r2.setValue(2, "2018");
    
    r3.setValue(0, "8");
    r3.setValue(1, "Song8");
    r3.setValue(2, "2022");
    
    r4.setValue(0, "1");
    r4.setValue(1, "Song1");
    r4.setValue(2, "2015");
    
    r5.setValue(0, "9");
    r5.setValue(1, "Song9");
    r5.setValue(2, "2023");

    if (!dataManager.insertRecord("Songs", r1)) return false;
    if (!dataManager.insertRecord("Songs", r2)) return false;
    if (!dataManager.insertRecord("Songs", r3)) return false;
    if (!dataManager.insertRecord("Songs", r4)) return false;
    if (!dataManager.insertRecord("Songs", r5)) return false;

    return true;
}

/**
 * @brief 测试索引构建
 */
void testBuildIndex() {
    std::cout << "\n[测试1] 索引构建测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);

    // 为SongID字段构建索引
    TEST_ASSERT(index.buildIndex("Songs", "SongID", 0), "为SongID字段构建索引");

    // 检查索引是否存在
    TEST_ASSERT(index.hasIndex("Songs", "SongID"), "索引存在性检查");

    // 为Year字段构建索引
    TEST_ASSERT(index.buildIndex("Songs", "Year", 2), "为Year字段构建索引");
    TEST_ASSERT(index.hasIndex("Songs", "Year"), "Year索引存在性检查");

    cleanupTestFiles();
}

/**
 * @brief 测试相邻关系
 */
void testAdjacentRelations() {
    std::cout << "\n[测试2] 相邻关系测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    // 读取记录以确定索引
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    dataManager.readAllRecords("Songs", records);

    // 找到SongID=1的记录索引（应该是第一个插入的，索引为3）
    size_t record1Index = SIZE_MAX;
    for (size_t i = 0; i < records.size(); i++) {
        if (records[i].getValue(0) == "1") {
            record1Index = i;
            break;
        }
    }

    TEST_ASSERT(record1Index != SIZE_MAX, "找到SongID=1的记录");

    // 获取下一个相邻记录（应该是SongID=2）
    size_t nextIndex;
    if (index.getNextIndex("Songs", "SongID", record1Index, nextIndex)) {
        TEST_ASSERT(nextIndex < records.size(), "下一个记录索引有效");
        TEST_ASSERT(records[nextIndex].getValue(0) == "2", "下一个记录是SongID=2");
    } else {
        TEST_ASSERT(false, "获取下一个相邻记录");
    }

    // 获取上一个相邻记录（应该是第一个，没有上一个）
    size_t prevIndex;
    bool hasPrev = index.getPrevIndex("Songs", "SongID", record1Index, prevIndex);
    TEST_ASSERT(!hasPrev || prevIndex == SIZE_MAX, "第一个记录没有上一个记录");

    cleanupTestFiles();
}

/**
 * @brief 测试顺序扫描
 */
void testSequentialScan() {
    std::cout << "\n[测试3] 顺序扫描测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    std::vector<size_t> resultIndices;
    TEST_ASSERT(index.sequentialScan("Songs", "SongID", resultIndices), "执行顺序扫描");

    // 应该返回5个记录的索引
    TEST_ASSERT(resultIndices.size() == 5, "顺序扫描返回5个记录");

    // 读取记录验证顺序
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    dataManager.readAllRecords("Songs", records);

    // 验证顺序：1, 2, 5, 8, 9
    TEST_ASSERT(records[resultIndices[0]].getValue(0) == "1", "第一个记录是SongID=1");
    TEST_ASSERT(records[resultIndices[1]].getValue(0) == "2", "第二个记录是SongID=2");
    TEST_ASSERT(records[resultIndices[2]].getValue(0) == "5", "第三个记录是SongID=5");
    TEST_ASSERT(records[resultIndices[3]].getValue(0) == "8", "第四个记录是SongID=8");
    TEST_ASSERT(records[resultIndices[4]].getValue(0) == "9", "第五个记录是SongID=9");

    cleanupTestFiles();
}

/**
 * @brief 测试范围查询
 */
void testRangeQuery() {
    std::cout << "\n[测试4] 范围查询测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    // 查询SongID在[2, 8]范围内的记录
    std::vector<size_t> resultIndices;
    TEST_ASSERT(index.rangeQuery("Songs", "SongID", "2", "8", resultIndices), "执行范围查询");

    // 应该返回3个记录：2, 5, 8
    TEST_ASSERT(resultIndices.size() == 3, "范围查询返回3个记录");

    // 读取记录验证
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    dataManager.readAllRecords("Songs", records);

    // 验证结果
    std::vector<std::string> resultIds;
    for (size_t idx : resultIndices) {
        resultIds.push_back(records[idx].getValue(0));
    }

    TEST_ASSERT(resultIds[0] == "2", "第一个结果是SongID=2");
    TEST_ASSERT(resultIds[1] == "5", "第二个结果是SongID=5");
    TEST_ASSERT(resultIds[2] == "8", "第三个结果是SongID=8");

    cleanupTestFiles();
}

/**
 * @brief 测试索引更新
 */
void testUpdateIndex() {
    std::cout << "\n[测试5] 索引更新测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    // 插入新记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    Record newRecord;
    newRecord.setValue(0, "3");
    newRecord.setValue(1, "Song3");
    newRecord.setValue(2, "2019");
    dataManager.insertRecord("Songs", newRecord);

    // 更新索引
    TEST_ASSERT(index.updateIndex("Songs", "SongID", 0), "更新索引");

    // 验证顺序扫描包含新记录
    std::vector<size_t> resultIndices;
    index.sequentialScan("Songs", "SongID", resultIndices);
    TEST_ASSERT(resultIndices.size() == 6, "更新后顺序扫描返回6个记录");

    cleanupTestFiles();
}

/**
 * @brief 测试索引删除
 */
void testRemoveIndex() {
    std::cout << "\n[测试6] 索引删除测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    TEST_ASSERT(index.hasIndex("Songs", "SongID"), "索引存在");

    TEST_ASSERT(index.removeIndex("Songs", "SongID"), "删除索引");
    TEST_ASSERT(!index.hasIndex("Songs", "SongID"), "索引已删除");

    cleanupTestFiles();
}

/**
 * @brief 测试空表索引
 */
void testEmptyTableIndex() {
    std::cout << "\n[测试7] 空表索引测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();

    AdjacentIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(index.buildIndex("Songs", "SongID", 0), "为空表构建索引");

    std::vector<size_t> resultIndices;
    TEST_ASSERT(index.sequentialScan("Songs", "SongID", resultIndices), "空表顺序扫描");
    TEST_ASSERT(resultIndices.empty(), "空表顺序扫描返回空结果");

    cleanupTestFiles();
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "相邻索引功能测试" << std::endl;
    std::cout << "========================================" << std::endl;

    testBuildIndex();
    testAdjacentRelations();
    testSequentialScan();
    testRangeQuery();
    testUpdateIndex();
    testRemoveIndex();
    testEmptyTableIndex();

    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;

    return (testsFailed == 0) ? 0 : 1;
}

