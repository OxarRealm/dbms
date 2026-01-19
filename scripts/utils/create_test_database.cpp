/**
 * @file create_test_database.cpp
 * @brief 创建测试数据库工具
 * 
 * 用途：生成测试用的数据库文件（.dbf和.dat），包含示例表和数据
 * 使用方法：编译后运行，会在当前目录生成demo_db.dbf和demo_db.dat
 */

#include "../include/core/table_manager.h"
#include "../include/core/data_manager.h"
#include "../include/core/table_mode.h"
#include <iostream>
#include <vector>

// 测试数据库名称
const char* TEST_DB_NAME = "demo_db";

/**
 * @brief 创建Users表
 */
bool createUsersTable(TableManager& tableManager) {
    TableInfo tableInfo("Users");
    
    // 定义字段
    TableMode field1, field2, field3, field4, field5;
    
    // UserID int KEY NO_NULL VALID
    initTableMode(field1, "UserID", "int", sizeof(int), FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    
    // UserName char[50] NOT_KEY NULL VALID
    initTableMode(field2, "UserName", "char", 50, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    // Email char[100] NOT_KEY NULL VALID
    initTableMode(field3, "Email", "char", 100, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    // Age int NOT_KEY NULL VALID
    initTableMode(field4, "Age", "int", sizeof(int), FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    // Phone char[20] NOT_KEY NULL VALID
    initTableMode(field5, "Phone", "char", 20, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    tableInfo.fields.push_back(field1);
    tableInfo.fields.push_back(field2);
    tableInfo.fields.push_back(field3);
    tableInfo.fields.push_back(field4);
    tableInfo.fields.push_back(field5);
    
    if (!tableManager.createTable(tableInfo)) {
        std::cerr << "Error: Failed to create Users table" << std::endl;
        return false;
    }
    
    std::cout << "✓ Users table created successfully" << std::endl;
    return true;
}

/**
 * @brief 创建Products表
 */
bool createProductsTable(TableManager& tableManager) {
    TableInfo tableInfo("Products");
    
    // 定义字段
    TableMode field1, field2, field3, field4;
    
    // ProductID int KEY NO_NULL VALID
    initTableMode(field1, "ProductID", "int", sizeof(int), FLAG_KEY, FLAG_NO_NULL, FLAG_VALID);
    
    // ProductName char[100] NOT_KEY NULL VALID
    initTableMode(field2, "ProductName", "char", 100, FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    // Price float NOT_KEY NULL VALID
    initTableMode(field3, "Price", "float", sizeof(float), FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    // UserID int NOT_KEY NULL VALID
    initTableMode(field4, "UserID", "int", sizeof(int), FLAG_NOT_KEY, FLAG_NULL, FLAG_VALID);
    
    tableInfo.fields.push_back(field1);
    tableInfo.fields.push_back(field2);
    tableInfo.fields.push_back(field3);
    tableInfo.fields.push_back(field4);
    
    if (!tableManager.createTable(tableInfo)) {
        std::cerr << "Error: Failed to create Products table" << std::endl;
        return false;
    }
    
    std::cout << "✓ Products table created successfully" << std::endl;
    return true;
}

/**
 * @brief 插入Users表数据
 */
bool insertUsersData(DataManager& dataManager) {
    // 插入5条用户记录
    std::vector<Record> records;
    
    // UserID=1: John Doe
    Record r1;
    r1.setValue(0, "1");
    r1.setValue(1, "John Doe");
    r1.setValue(2, "john@example.com");
    r1.setValue(3, "26");
    r1.setValue(4, "13800138001");
    records.push_back(r1);
    
    // UserID=2: Jane Smith
    Record r2;
    r2.setValue(0, "2");
    r2.setValue(1, "Jane Smith");
    r2.setValue(2, "jane@example.com");
    r2.setValue(3, "30");
    r2.setValue(4, "13800138002");
    records.push_back(r2);
    
    // UserID=3: Bob Johnson
    Record r3;
    r3.setValue(0, "3");
    r3.setValue(1, "Bob Johnson");
    r3.setValue(2, "bob@example.com");
    r3.setValue(3, "28");
    r3.setValue(4, "13800138003");
    records.push_back(r3);
    
    // UserID=4: Alice Brown
    Record r4;
    r4.setValue(0, "4");
    r4.setValue(1, "Alice Brown");
    r4.setValue(2, "alice@example.com");
    r4.setValue(3, "24");
    r4.setValue(4, "13800138004");
    records.push_back(r4);
    
    // UserID=5: Charlie Wilson
    Record r5;
    r5.setValue(0, "5");
    r5.setValue(1, "Charlie Wilson");
    r5.setValue(2, "charlie@example.com");
    r5.setValue(3, "27");
    r5.setValue(4, "13800138005");
    records.push_back(r5);
    
    if (!dataManager.insertRecords("Users", records)) {
        std::cerr << "Error: Failed to insert Users data" << std::endl;
        return false;
    }
    
    std::cout << "✓ Inserted 5 Users records" << std::endl;
    return true;
}

/**
 * @brief 插入Products表数据
 */
bool insertProductsData(DataManager& dataManager) {
    // 插入5条产品记录
    std::vector<Record> records;
    
    // ProductID=1: Laptop (UserID=1)
    Record r1;
    r1.setValue(0, "1");
    r1.setValue(1, "Laptop");
    r1.setValue(2, "1000.50");
    r1.setValue(3, "1");
    records.push_back(r1);
    
    // ProductID=2: Mouse (UserID=1)
    Record r2;
    r2.setValue(0, "2");
    r2.setValue(1, "Mouse");
    r2.setValue(2, "25.99");
    r2.setValue(3, "1");
    records.push_back(r2);
    
    // ProductID=3: Keyboard (UserID=2)
    Record r3;
    r3.setValue(0, "3");
    r3.setValue(1, "Keyboard");
    r3.setValue(2, "89.99");
    r3.setValue(3, "2");
    records.push_back(r3);
    
    // ProductID=4: Monitor (UserID=2)
    Record r4;
    r4.setValue(0, "4");
    r4.setValue(1, "Monitor");
    r4.setValue(2, "299.99");
    r4.setValue(3, "2");
    records.push_back(r4);
    
    // ProductID=5: Headphones (UserID=3)
    Record r5;
    r5.setValue(0, "5");
    r5.setValue(1, "Headphones");
    r5.setValue(2, "79.99");
    r5.setValue(3, "3");
    records.push_back(r5);
    
    if (!dataManager.insertRecords("Products", records)) {
        std::cerr << "Error: Failed to insert Products data" << std::endl;
        return false;
    }
    
    std::cout << "✓ Inserted 5 Products records" << std::endl;
    return true;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  测试数据库生成工具" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "数据库名称: " << TEST_DB_NAME << std::endl;
    std::cout << "输出文件: " << TEST_DB_NAME << ".dbf, " << TEST_DB_NAME << ".dat" << std::endl;
    std::cout << std::endl;
    
    // 设置数据库路径
    TableManager tableManager;
    tableManager.setDatabasePath(TEST_DB_NAME);
    
    DataManager dataManager;
    dataManager.setDatabasePath(TEST_DB_NAME);
    
    // 创建表
    std::cout << "[1/4] 创建表结构..." << std::endl;
    if (!createUsersTable(tableManager)) {
        return 1;
    }
    if (!createProductsTable(tableManager)) {
        return 1;
    }
    std::cout << std::endl;
    
    // 插入数据
    std::cout << "[2/4] 插入Users表数据..." << std::endl;
    if (!insertUsersData(dataManager)) {
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "[3/4] 插入Products表数据..." << std::endl;
    if (!insertProductsData(dataManager)) {
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "[4/4] 完成！" << std::endl;
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "测试数据库已创建：" << std::endl;
    std::cout << "  - " << TEST_DB_NAME << ".dbf (表结构文件)" << std::endl;
    std::cout << "  - " << TEST_DB_NAME << ".dat (数据文件)" << std::endl;
    std::cout << std::endl;
    std::cout << "包含的表和数据：" << std::endl;
    std::cout << "  - Users表：5条记录（UserID, UserName, Email, Age, Phone）" << std::endl;
    std::cout << "  - Products表：5条记录（ProductID, ProductName, Price, UserID）" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}












