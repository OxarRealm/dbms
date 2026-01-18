/**
 * @file test_constraints.cpp
 * @brief 约束功能测试脚本
 * 
 * 测试外键约束、多字段唯一约束、检查约束等功能
 */

#include <iostream>
#include <string>
#include <vector>
#include "ddl/create_table_handler.h"
#include "dml/insert_handler.h"
#include "dml/update_handler.h"
#include "dml/delete_handler.h"
#include "core/constraint_registry.h"

void printTestResult(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << std::endl;
}

int main() {
    std::cout << "=== 约束功能测试 ===" << std::endl;
    std::cout << std::endl;
    
    int passedTests = 0;
    int totalTests = 0;
    
    // 清理之前的测试数据
    std::string dbName = "test_constraints_db";
    ConstraintRegistry::getInstance().clearDatabase(dbName);
    
    // ========== 测试1: 创建带外键约束的表 ==========
    {
        totalTests++;
        std::cout << "测试1: 创建带外键约束的表" << std::endl;
        
        CreateTableHandler handler;
        
        // 先创建被引用的表（Departments）
        std::string sql1 = "CREATE TABLE Departments ("
                          "DeptID int KEY NO NULL VALID,"
                          "DeptName string[50] NOT KEY NO NULL VALID"
                          ") INTO " + dbName + ";";
        
        if (!handler.execute(sql1)) {
            printTestResult("创建Departments表", false);
            std::cout << "  错误: " << handler.getLastError() << std::endl;
            goto next_test1;
        }
        
        // 创建引用表（Employees），带外键约束
        std::string sql2 = "CREATE TABLE Employees ("
                          "EmpID int KEY NO NULL VALID,"
                          "EmpName string[50] NOT KEY NO NULL VALID,"
                          "DeptID int NOT KEY NULL VALID,"
                          "FOREIGN KEY (DeptID) REFERENCES Departments(DeptID) ON DELETE RESTRICT"
                          ") INTO " + dbName + ";";
        
        if (handler.execute(sql2)) {
            printTestResult("创建带外键约束的Employees表", true);
            passedTests++;
        } else {
            printTestResult("创建带外键约束的Employees表", false);
            std::cout << "  错误: " << handler.getLastError() << std::endl;
        }
    next_test1:;
    }
    
    std::cout << std::endl;
    
    // ========== 测试2: 外键约束验证（引用不存在的值） ==========
    {
        totalTests++;
        std::cout << "测试2: 外键约束验证（引用不存在的值）" << std::endl;
        
        InsertHandler handler;
        
        // 尝试插入引用不存在DeptID的记录
        std::string sql = "INSERT INTO Employees VALUES (1, 'John', 999) IN " + dbName + ";";
        
        if (!handler.execute(sql)) {
            printTestResult("外键约束阻止插入无效引用", true);
            std::cout << "  预期错误: " << handler.getLastError() << std::endl;
            passedTests++;
        } else {
            printTestResult("外键约束阻止插入无效引用", false);
            std::cout << "  错误: 应该阻止插入，但没有阻止" << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // ========== 测试3: 外键约束验证（引用存在的值） ==========
    {
        totalTests++;
        std::cout << "测试3: 外键约束验证（引用存在的值）" << std::endl;
        
        InsertHandler handler;
        
        // 先插入Department
        std::string sql1 = "INSERT INTO Departments VALUES (1, 'IT') IN " + dbName + ";";
        if (!handler.execute(sql1)) {
            printTestResult("插入Department记录", false);
            std::cout << "  错误: " << handler.getLastError() << std::endl;
            goto next_test3;
        }
        
        // 插入引用有效DeptID的记录
        std::string sql2 = "INSERT INTO Employees VALUES (1, 'John', 1) IN " + dbName + ";";
        
        if (handler.execute(sql2)) {
            printTestResult("外键约束允许插入有效引用", true);
            passedTests++;
        } else {
            printTestResult("外键约束允许插入有效引用", false);
            std::cout << "  错误: " << handler.getLastError() << std::endl;
        }
    next_test3:;
    }
    
    std::cout << std::endl;
    
    // ========== 测试4: 外键约束DELETE RESTRICT ==========
    {
        totalTests++;
        std::cout << "测试4: 外键约束DELETE RESTRICT" << std::endl;
        
        DeleteHandler handler;
        
        // 尝试删除被引用的Department
        std::string sql = "DELETE FROM Departments WHERE DeptID=1 IN " + dbName + ";";
        
        if (!handler.execute(sql)) {
            printTestResult("RESTRICT模式阻止删除被引用的记录", true);
            std::cout << "  预期错误: " << handler.getLastError() << std::endl;
            passedTests++;
        } else {
            printTestResult("RESTRICT模式阻止删除被引用的记录", false);
            std::cout << "  错误: 应该阻止删除，但没有阻止" << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // ========== 测试5: 多字段唯一约束 ==========
    {
        totalTests++;
        std::cout << "测试5: 多字段唯一约束" << std::endl;
        
        CreateTableHandler createHandler;
        InsertHandler insertHandler;
        
        // 创建带多字段唯一约束的表
        std::string sql1 = "CREATE TABLE Orders ("
                          "OrderID int KEY NO NULL VALID,"
                          "CustomerID int NOT KEY NO NULL VALID,"
                          "ProductID int NOT KEY NO NULL VALID,"
                          "UNIQUE (CustomerID, ProductID)"
                          ") INTO " + dbName + ";";
        
        if (!createHandler.execute(sql1)) {
            printTestResult("创建带多字段唯一约束的表", false);
            std::cout << "  错误: " << createHandler.getLastError() << std::endl;
            goto next_test5;
        }
        
        // 插入第一条记录
        std::string sql2 = "INSERT INTO Orders VALUES (1, 100, 200) IN " + dbName + ";";
        if (!insertHandler.execute(sql2)) {
            printTestResult("插入第一条Orders记录", false);
            std::cout << "  错误: " << insertHandler.getLastError() << std::endl;
            goto next_test5;
        }
        
        // 尝试插入违反唯一约束的记录（相同的CustomerID和ProductID组合）
        std::string sql3 = "INSERT INTO Orders VALUES (2, 100, 200) IN " + dbName + ";";
        
        if (!insertHandler.execute(sql3)) {
            printTestResult("多字段唯一约束阻止重复组合", true);
            std::cout << "  预期错误: " << insertHandler.getLastError() << std::endl;
            passedTests++;
        } else {
            printTestResult("多字段唯一约束阻止重复组合", false);
            std::cout << "  错误: 应该阻止插入，但没有阻止" << std::endl;
        }
    next_test5:;
    }
    
    std::cout << std::endl;
    
    // ========== 测试6: 检查约束 ==========
    {
        totalTests++;
        std::cout << "测试6: 检查约束" << std::endl;
        
        CreateTableHandler createHandler;
        InsertHandler insertHandler;
        
        // 创建带检查约束的表
        std::string sql1 = "CREATE TABLE Products ("
                          "ProductID int KEY NO NULL VALID,"
                          "ProductName string[50] NOT KEY NO NULL VALID,"
                          "Price float NOT KEY NULL VALID,"
                          "CHECK (Price > 0)"
                          ") INTO " + dbName + ";";
        
        if (!createHandler.execute(sql1)) {
            printTestResult("创建带检查约束的表", false);
            std::cout << "  错误: " << createHandler.getLastError() << std::endl;
            goto next_test6;
        }
        
        // 尝试插入违反检查约束的记录（Price <= 0）
        std::string sql2 = "INSERT INTO Products VALUES (1, 'Product1', -10) IN " + dbName + ";";
        
        if (!insertHandler.execute(sql2)) {
            printTestResult("检查约束阻止无效值", true);
            std::cout << "  预期错误: " << insertHandler.getLastError() << std::endl;
            passedTests++;
        } else {
            printTestResult("检查约束阻止无效值", false);
            std::cout << "  错误: 应该阻止插入，但没有阻止" << std::endl;
        }
        
        // 插入符合检查约束的记录
        std::string sql3 = "INSERT INTO Products VALUES (1, 'Product1', 10.5) IN " + dbName + ";";
        
        if (insertHandler.execute(sql3)) {
            printTestResult("检查约束允许有效值", true);
            passedTests++;
        } else {
            printTestResult("检查约束允许有效值", false);
            std::cout << "  错误: " << insertHandler.getLastError() << std::endl;
        }
    next_test6:;
    }
    
    std::cout << std::endl;
    
    // ========== 测试总结 ==========
    std::cout << "=== 测试总结 ===" << std::endl;
    std::cout << "通过: " << passedTests << " / " << totalTests << std::endl;
    
    if (passedTests == totalTests) {
        std::cout << "所有测试通过！" << std::endl;
        return 0;
    } else {
        std::cout << "部分测试失败！" << std::endl;
        return 1;
    }
}

