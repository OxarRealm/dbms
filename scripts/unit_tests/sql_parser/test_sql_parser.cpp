/**
 * @file test_sql_parser.cpp
 * @brief SQL解析器测试程序
 * 
 * 用于手动测试SQL解析逻辑，验证是否符合预期
 */

#include "sql_parser/parser.h"
#include <iostream>
#include <string>

void testSQL(const std::string& sql, const std::string& description) {
    std::cout << "\n=== Testing: " << description << " ===" << std::endl;
    std::cout << "SQL: " << sql << std::endl;
    
    Parser parser(sql);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "✓ Parse SUCCESS" << std::endl;
    } else {
        std::cout << "✗ Parse FAILED: " << parser.getLastError() << std::endl;
    }
}

int main() {
    std::cout << "SQL Parser Test Suite" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // Test Case 1.1: CREATE TABLE
    testSQL(
        "CREATE TABLE Users ("
        "    UserID int KEY NO_NULL VALID,"
        "    UserName char[50] NOT_KEY NULL VALID,"
        "    Email char[100] NOT_KEY NULL VALID,"
        "    Age int NOT_KEY NULL VALID"
        ") INTO test_db;",
        "CREATE TABLE with char[50] syntax"
    );
    
    // Test Case 1.1 variant: char(50) syntax
    testSQL(
        "CREATE TABLE Products ("
        "    ProductID int KEY NO_NULL VALID,"
        "    ProductName char(100) NOT_KEY NULL VALID"
        ") INTO test_db;",
        "CREATE TABLE with char(100) syntax"
    );
    
    // Test Case 2.1: INSERT INTO
    testSQL(
        "INSERT INTO Users VALUES ('1', 'John Doe', 'john@example.com', '25') IN test_db;",
        "INSERT INTO"
    );
    
    // Test Case 2.3: UPDATE
    testSQL(
        "UPDATE Users ("
        "    SET Age='26'"
        "    WHERE UserID='1'"
        ") IN test_db;",
        "UPDATE"
    );
    
    // Test Case 2.4: DELETE
    testSQL(
        "DELETE FROM Users WHERE UserID='3' IN test_db;",
        "DELETE"
    );
    
    // Test Case 3.1: SELECT *
    testSQL(
        "SELECT * FROM Users;",
        "SELECT * FROM single table"
    );
    
    // Test Case 3.2: SELECT specific fields
    testSQL(
        "SELECT UserName, Email FROM Users;",
        "SELECT specific fields"
    );
    
    // Test Case 3.3: SELECT with WHERE
    testSQL(
        "SELECT * FROM Users WHERE UserID='1';",
        "SELECT with WHERE clause"
    );
    
    // Test Case 1.6: Missing semicolon (should fail)
    testSQL(
        "CREATE TABLE TestTable ("
        "    ID int KEY NO_NULL VALID"
        ") INTO test_db",
        "CREATE TABLE without semicolon (should fail)"
    );
    
    // Test Case 4.2: Invalid syntax (should fail)
    testSQL(
        "SELECT * FRM Users;",
        "Invalid SQL syntax (should fail)"
    );
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}

