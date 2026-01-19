/**
 * @file test_parser_user_permission.cpp
 * @brief SQL解析器权限相关语句测试程序
 * 
 * 测试Parser类对权限相关语句的解析功能
 */

#include "../../include/sql_parser/parser.h"
#include "../../include/sql_parser/ast_node.h"
#include <iostream>
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
 * @brief 测试CREATE USER解析
 */
void testParseCreateUser() {
    std::cout << "\n[测试1] CREATE USER解析测试" << std::endl;
    
    std::string sql = "CREATE USER admin IDENTIFIED BY 'admin';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "CREATE USER解析成功");
    TEST_ASSERT(parser.getLastError().empty(), "无错误信息");
    
    if (node) {
        CreateUserNode* userNode = dynamic_cast<CreateUserNode*>(node.get());
        TEST_ASSERT(userNode != nullptr, "节点类型正确");
        if (userNode) {
            TEST_ASSERT(userNode->userName == "admin", "用户名正确");
            TEST_ASSERT(userNode->password == "admin", "密码正确");
        }
    }
}

/**
 * @brief 测试ALTER USER（修改密码）解析
 */
void testParseAlterUserPassword() {
    std::cout << "\n[测试2] ALTER USER（修改密码）解析测试" << std::endl;
    
    std::string sql = "ALTER USER admin IDENTIFIED BY 'newpassword';";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "ALTER USER解析成功");
    
    if (node) {
        AlterUserNode* userNode = dynamic_cast<AlterUserNode*>(node.get());
        TEST_ASSERT(userNode != nullptr, "节点类型正确");
        if (userNode) {
            TEST_ASSERT(userNode->userName == "admin", "用户名正确");
            TEST_ASSERT(userNode->isPasswordChange == true, "是密码修改");
            TEST_ASSERT(userNode->newPassword == "newpassword", "新密码正确");
        }
    }
}

/**
 * @brief 测试ALTER USER（启用/禁用）解析
 */
void testParseAlterUserEnable() {
    std::cout << "\n[测试3] ALTER USER（启用/禁用）解析测试" << std::endl;
    
    // 测试ENABLE
    std::string sql1 = "ALTER USER admin ENABLE;";
    Parser parser1(sql1);
    auto node1 = parser1.parse();
    
    TEST_ASSERT(node1 != nullptr, "ALTER USER ENABLE解析成功");
    
    if (node1) {
        AlterUserNode* userNode = dynamic_cast<AlterUserNode*>(node1.get());
        TEST_ASSERT(userNode != nullptr, "节点类型正确");
        if (userNode) {
            TEST_ASSERT(userNode->userName == "admin", "用户名正确");
            TEST_ASSERT(userNode->isPasswordChange == false, "不是密码修改");
            TEST_ASSERT(userNode->disable == false, "ENABLE标志正确");
        }
    }
    
    // 测试DISABLE
    std::string sql2 = "ALTER USER admin DISABLE;";
    Parser parser2(sql2);
    auto node2 = parser2.parse();
    
    TEST_ASSERT(node2 != nullptr, "ALTER USER DISABLE解析成功");
    
    if (node2) {
        AlterUserNode* userNode = dynamic_cast<AlterUserNode*>(node2.get());
        TEST_ASSERT(userNode != nullptr, "节点类型正确");
        if (userNode) {
            TEST_ASSERT(userNode->userName == "admin", "用户名正确");
            TEST_ASSERT(userNode->disable == true, "DISABLE标志正确");
        }
    }
}

/**
 * @brief 测试DROP USER解析
 */
void testParseDropUser() {
    std::cout << "\n[测试4] DROP USER解析测试" << std::endl;
    
    std::string sql = "DROP USER admin;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "DROP USER解析成功");
    
    if (node) {
        DropUserNode* userNode = dynamic_cast<DropUserNode*>(node.get());
        TEST_ASSERT(userNode != nullptr, "节点类型正确");
        if (userNode) {
            TEST_ASSERT(userNode->userName == "admin", "用户名正确");
        }
    }
}

/**
 * @brief 测试CREATE ROLE解析
 */
void testParseCreateRole() {
    std::cout << "\n[测试5] CREATE ROLE解析测试" << std::endl;
    
    std::string sql = "CREATE ROLE student_role;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "CREATE ROLE解析成功");
    
    if (node) {
        CreateRoleNode* roleNode = dynamic_cast<CreateRoleNode*>(node.get());
        TEST_ASSERT(roleNode != nullptr, "节点类型正确");
        if (roleNode) {
            TEST_ASSERT(roleNode->roleName == "student_role", "角色名正确");
        }
    }
}

/**
 * @brief 测试DROP ROLE解析
 */
void testParseDropRole() {
    std::cout << "\n[测试6] DROP ROLE解析测试" << std::endl;
    
    std::string sql = "DROP ROLE student_role;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "DROP ROLE解析成功");
    
    if (node) {
        DropRoleNode* roleNode = dynamic_cast<DropRoleNode*>(node.get());
        TEST_ASSERT(roleNode != nullptr, "节点类型正确");
        if (roleNode) {
            TEST_ASSERT(roleNode->roleName == "student_role", "角色名正确");
        }
    }
}

/**
 * @brief 测试GRANT（权限授予）解析
 */
void testParseGrantPrivileges() {
    std::cout << "\n[测试7] GRANT（权限授予）解析测试" << std::endl;
    
    // 测试单个权限
    std::string sql1 = "GRANT SELECT ON TABLE Students TO student1;";
    Parser parser1(sql1);
    auto node1 = parser1.parse();
    
    TEST_ASSERT(node1 != nullptr, "GRANT权限解析成功");
    
    if (node1) {
        GrantNode* grantNode = dynamic_cast<GrantNode*>(node1.get());
        TEST_ASSERT(grantNode != nullptr, "节点类型正确");
        if (grantNode) {
            TEST_ASSERT(grantNode->isRoleGrant == false, "不是角色授予");
            TEST_ASSERT(grantNode->privilegeTypes.size() == 1, "权限数量正确");
            TEST_ASSERT(grantNode->privilegeTypes[0] == "SELECT", "权限类型正确");
            TEST_ASSERT(grantNode->objectType == "TABLE", "对象类型正确");
            TEST_ASSERT(grantNode->objectName == "Students", "对象名正确");
            TEST_ASSERT(grantNode->grantee == "student1", "被授予者正确");
        }
    }
    
    // 测试多个权限
    std::string sql2 = "GRANT SELECT, INSERT, UPDATE ON TABLE Grades TO teacher1;";
    Parser parser2(sql2);
    auto node2 = parser2.parse();
    
    TEST_ASSERT(node2 != nullptr, "GRANT多个权限解析成功");
    
    if (node2) {
        GrantNode* grantNode = dynamic_cast<GrantNode*>(node2.get());
        TEST_ASSERT(grantNode != nullptr, "节点类型正确");
        if (grantNode) {
            TEST_ASSERT(grantNode->privilegeTypes.size() == 3, "权限数量正确");
            TEST_ASSERT(grantNode->privilegeTypes[0] == "SELECT", "第一个权限正确");
            TEST_ASSERT(grantNode->privilegeTypes[1] == "INSERT", "第二个权限正确");
            TEST_ASSERT(grantNode->privilegeTypes[2] == "UPDATE", "第三个权限正确");
        }
    }
    
    // 测试ALL PRIVILEGES
    std::string sql3 = "GRANT ALL PRIVILEGES ON TABLE Students TO admin;";
    Parser parser3(sql3);
    auto node3 = parser3.parse();
    
    TEST_ASSERT(node3 != nullptr, "GRANT ALL PRIVILEGES解析成功");
    
    if (node3) {
        GrantNode* grantNode = dynamic_cast<GrantNode*>(node3.get());
        TEST_ASSERT(grantNode != nullptr, "节点类型正确");
        if (grantNode) {
            TEST_ASSERT(grantNode->privilegeTypes.size() == 1, "权限数量正确");
            TEST_ASSERT(grantNode->privilegeTypes[0] == "ALL_PRIVILEGES", "ALL PRIVILEGES正确");
        }
    }
    
    // 测试WITH GRANT OPTION
    std::string sql4 = "GRANT SELECT ON TABLE Students TO admin WITH GRANT OPTION;";
    Parser parser4(sql4);
    auto node4 = parser4.parse();
    
    TEST_ASSERT(node4 != nullptr, "GRANT WITH GRANT OPTION解析成功");
    
    if (node4) {
        GrantNode* grantNode = dynamic_cast<GrantNode*>(node4.get());
        TEST_ASSERT(grantNode != nullptr, "节点类型正确");
        if (grantNode) {
            TEST_ASSERT(grantNode->withGrantOption == true, "GRANT OPTION标志正确");
        }
    }
    
    // 测试DATABASE级别权限
    std::string sql5 = "GRANT ALL PRIVILEGES ON DATABASE TO admin;";
    Parser parser5(sql5);
    auto node5 = parser5.parse();
    
    TEST_ASSERT(node5 != nullptr, "GRANT DATABASE权限解析成功");
    
    if (node5) {
        GrantNode* grantNode = dynamic_cast<GrantNode*>(node5.get());
        TEST_ASSERT(grantNode != nullptr, "节点类型正确");
        if (grantNode) {
            TEST_ASSERT(grantNode->objectType == "DATABASE", "对象类型正确");
        }
    }
}

/**
 * @brief 测试GRANT（角色授予）解析
 */
void testParseGrantRole() {
    std::cout << "\n[测试8] GRANT（角色授予）解析测试" << std::endl;
    
    std::string sql = "GRANT student_role TO student1;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "GRANT角色解析成功");
    
    if (node) {
        GrantNode* grantNode = dynamic_cast<GrantNode*>(node.get());
        TEST_ASSERT(grantNode != nullptr, "节点类型正确");
        if (grantNode) {
            TEST_ASSERT(grantNode->isRoleGrant == true, "是角色授予");
            TEST_ASSERT(grantNode->roleName == "student_role", "角色名正确");
            TEST_ASSERT(grantNode->grantee == "student1", "被授予者正确");
        }
    }
}

/**
 * @brief 测试REVOKE（权限撤销）解析
 */
void testParseRevokePrivileges() {
    std::cout << "\n[测试9] REVOKE（权限撤销）解析测试" << std::endl;
    
    std::string sql = "REVOKE SELECT, INSERT ON TABLE Students FROM student1;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "REVOKE权限解析成功");
    
    if (node) {
        RevokeNode* revokeNode = dynamic_cast<RevokeNode*>(node.get());
        TEST_ASSERT(revokeNode != nullptr, "节点类型正确");
        if (revokeNode) {
            TEST_ASSERT(revokeNode->isRoleRevoke == false, "不是角色撤销");
            TEST_ASSERT(revokeNode->privilegeTypes.size() == 2, "权限数量正确");
            TEST_ASSERT(revokeNode->objectType == "TABLE", "对象类型正确");
            TEST_ASSERT(revokeNode->objectName == "Students", "对象名正确");
            TEST_ASSERT(revokeNode->revokee == "student1", "被撤销者正确");
        }
    }
}

/**
 * @brief 测试REVOKE（角色撤销）解析
 */
void testParseRevokeRole() {
    std::cout << "\n[测试10] REVOKE（角色撤销）解析测试" << std::endl;
    
    std::string sql = "REVOKE student_role FROM student1;";
    Parser parser(sql);
    auto node = parser.parse();
    
    TEST_ASSERT(node != nullptr, "REVOKE角色解析成功");
    
    if (node) {
        RevokeNode* revokeNode = dynamic_cast<RevokeNode*>(node.get());
        TEST_ASSERT(revokeNode != nullptr, "节点类型正确");
        if (revokeNode) {
            TEST_ASSERT(revokeNode->isRoleRevoke == true, "是角色撤销");
            TEST_ASSERT(revokeNode->roleName == "student_role", "角色名正确");
            TEST_ASSERT(revokeNode->revokee == "student1", "被撤销者正确");
        }
    }
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试11] 错误处理测试" << std::endl;
    
    // 测试语法错误：缺少IDENTIFIED BY
    std::string sql1 = "CREATE USER admin;";
    Parser parser1(sql1);
    auto node1 = parser1.parse();
    TEST_ASSERT(node1 == nullptr, "语法错误返回nullptr");
    
    // 测试缺少分号
    std::string sql2 = "CREATE USER admin IDENTIFIED BY 'admin'";
    Parser parser2(sql2);
    auto node2 = parser2.parse();
    TEST_ASSERT(node2 == nullptr, "缺少分号返回nullptr");
    
    // 测试GRANT语法错误：缺少ON
    std::string sql3 = "GRANT SELECT TABLE Students TO admin;";
    Parser parser3(sql3);
    auto node3 = parser3.parse();
    TEST_ASSERT(node3 == nullptr, "GRANT语法错误返回nullptr");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SQL解析器权限相关语句测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testParseCreateUser();
        testParseAlterUserPassword();
        testParseAlterUserEnable();
        testParseDropUser();
        testParseCreateRole();
        testParseDropRole();
        testParseGrantPrivileges();
        testParseGrantRole();
        testParseRevokePrivileges();
        testParseRevokeRole();
        testErrorHandling();
        
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




