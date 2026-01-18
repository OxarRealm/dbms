/**
 * @file test_hash_index.cpp
 * @brief 哈希索引测试程序
 * 
 * 测试HashIndex类的功能
 */

#include "../../../include/core/hash_index.h"
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
const char* TEST_DB_FILE = "test_db_hash_index";

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
    initTableMode(field3, "Artist", "string", 50, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
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

    // 插入10条记录（SongID: 1-10）
    for (int i = 1; i <= 10; i++) {
        Record r;
        r.setValue(0, std::to_string(i));
        r.setValue(1, "Song" + std::to_string(i));
        r.setValue(2, "Artist" + std::to_string((i % 3) + 1));
        
        if (!dataManager.insertRecord("Songs", r)) {
            return false;
        }
    }

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

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);

    // 为SongID字段构建索引
    TEST_ASSERT(index.buildIndex("Songs", "SongID", 0), "为SongID字段构建索引");

    // 检查索引是否存在
    TEST_ASSERT(index.hasIndex("Songs", "SongID"), "索引存在性检查");

    // 为Artist字段构建索引
    TEST_ASSERT(index.buildIndex("Songs", "Artist", 2), "为Artist字段构建索引");
    TEST_ASSERT(index.hasIndex("Songs", "Artist"), "Artist索引存在性检查");

    cleanupTestFiles();
}

/**
 * @brief 测试点查询
 */
void testPointQuery() {
    std::cout << "\n[测试2] 点查询测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    // 查询SongID=5的记录
    std::vector<size_t> resultIndices;
    TEST_ASSERT(index.pointQuery("Songs", "SongID", "5", resultIndices), "执行点查询");

    // 应该返回1个记录索引
    TEST_ASSERT(resultIndices.size() == 1, "点查询返回1个记录");

    // 验证记录内容
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    std::vector<Record> records;
    dataManager.readAllRecords("Songs", records);

    TEST_ASSERT(resultIndices[0] < records.size(), "记录索引有效");
    TEST_ASSERT(records[resultIndices[0]].getValue(0) == "5", "查询结果正确（SongID=5）");
    TEST_ASSERT(records[resultIndices[0]].getValue(1) == "Song5", "查询结果正确（SongName=Song5）");

    // 查询不存在的记录
    resultIndices.clear();
    TEST_ASSERT(index.pointQuery("Songs", "SongID", "99", resultIndices), "查询不存在的记录");
    TEST_ASSERT(resultIndices.empty(), "不存在的记录返回空结果");

    cleanupTestFiles();
}

/**
 * @brief 测试哈希冲突处理
 */
void testHashCollision() {
    std::cout << "\n[测试3] 哈希冲突处理测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();

    // 插入具有相同Artist值的多条记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);

    for (int i = 1; i <= 5; i++) {
        Record r;
        r.setValue(0, std::to_string(i));
        r.setValue(1, "Song" + std::to_string(i));
        r.setValue(2, "SameArtist");  // 所有记录的Artist都相同
        
        dataManager.insertRecord("Songs", r);
    }

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "Artist", 2);

    // 查询Artist="SameArtist"的记录
    std::vector<size_t> resultIndices;
    TEST_ASSERT(index.pointQuery("Songs", "Artist", "SameArtist", resultIndices), "查询相同Artist值的记录");

    // 应该返回5个记录
    TEST_ASSERT(resultIndices.size() == 5, "哈希冲突处理正确，返回5个记录");

    cleanupTestFiles();
}

/**
 * @brief 测试索引更新
 */
void testUpdateIndex() {
    std::cout << "\n[测试4] 索引更新测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    // 插入新记录
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_FILE);
    Record newRecord;
    newRecord.setValue(0, "11");
    newRecord.setValue(1, "Song11");
    newRecord.setValue(2, "Artist1");
    dataManager.insertRecord("Songs", newRecord);

    // 更新索引
    TEST_ASSERT(index.updateIndex("Songs", "SongID", 0), "更新索引");

    // 验证新记录可以被查询到
    std::vector<size_t> resultIndices;
    index.pointQuery("Songs", "SongID", "11", resultIndices);
    TEST_ASSERT(resultIndices.size() == 1, "更新后可以查询到新记录");

    cleanupTestFiles();
}

/**
 * @brief 测试索引删除
 */
void testRemoveIndex() {
    std::cout << "\n[测试5] 索引删除测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    TEST_ASSERT(index.hasIndex("Songs", "SongID"), "索引存在");

    TEST_ASSERT(index.removeIndex("Songs", "SongID"), "删除索引");
    TEST_ASSERT(!index.hasIndex("Songs", "SongID"), "索引已删除");

    cleanupTestFiles();
}

/**
 * @brief 测试索引统计信息
 */
void testIndexStats() {
    std::cout << "\n[测试6] 索引统计信息测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);

    size_t bucketCount, totalItems, maxChainLength;
    TEST_ASSERT(index.getIndexStats("Songs", "SongID", bucketCount, totalItems, maxChainLength), "获取索引统计信息");

    TEST_ASSERT(bucketCount > 0, "哈希桶数量大于0");
    TEST_ASSERT(totalItems == 10, "索引项总数等于10（10条记录）");
    TEST_ASSERT(maxChainLength >= 1, "最长冲突链长度至少为1");

    cleanupTestFiles();
}

/**
 * @brief 测试空表索引
 */
void testEmptyTableIndex() {
    std::cout << "\n[测试7] 空表索引测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    TEST_ASSERT(index.buildIndex("Songs", "SongID", 0), "为空表构建索引");

    std::vector<size_t> resultIndices;
    TEST_ASSERT(index.pointQuery("Songs", "SongID", "1", resultIndices), "空表点查询");
    TEST_ASSERT(resultIndices.empty(), "空表点查询返回空结果");

    cleanupTestFiles();
}

/**
 * @brief 测试清除索引
 */
void testClearIndex() {
    std::cout << "\n[测试8] 清除索引测试" << std::endl;
    
    cleanupTestFiles();
    createTestTable();
    insertTestData();

    HashIndex index;
    index.setDatabasePath(TEST_DB_FILE);
    index.buildIndex("Songs", "SongID", 0);
    index.buildIndex("Songs", "Artist", 2);

    TEST_ASSERT(index.hasIndex("Songs", "SongID"), "SongID索引存在");
    TEST_ASSERT(index.hasIndex("Songs", "Artist"), "Artist索引存在");

    index.clearTable("Songs");
    TEST_ASSERT(!index.hasIndex("Songs", "SongID"), "清除表索引后SongID索引不存在");
    TEST_ASSERT(!index.hasIndex("Songs", "Artist"), "清除表索引后Artist索引不存在");

    cleanupTestFiles();
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "哈希索引功能测试" << std::endl;
    std::cout << "========================================" << std::endl;

    testBuildIndex();
    testPointQuery();
    testHashCollision();
    testUpdateIndex();
    testRemoveIndex();
    testIndexStats();
    testEmptyTableIndex();
    testClearIndex();

    std::cout << "\n========================================" << std::endl;
    std::cout << "测试完成" << std::endl;
    std::cout << "通过: " << testsPassed << std::endl;
    std::cout << "失败: " << testsFailed << std::endl;
    std::cout << "总计: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "========================================" << std::endl;

    return (testsFailed == 0) ? 0 : 1;
}

