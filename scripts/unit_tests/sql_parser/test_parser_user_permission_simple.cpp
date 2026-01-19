/**
 * @file test_parser_user_permission_simple.cpp
 * @brief SQL解析器权限相关语句简单测试程序
 * 
 * 用于快速测试解析器是否能正确解析权限相关SQL语句
 */

#include "../../include/sql_parser/parser.h"
#include "../../include/sql_parser/ast_node.h"
#include <iostream>
#include <string>

void testSQL(const std::string& sql, const std::string& description) {
    std::cout << "\n=== Testing: " << description << " ===" << std::endl;
    std::cout << "SQL: " << sql << std::endl;
    
    Parser parser(sql);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "✓ Parse SUCCESS" << std::endl;
        
        // 显示节点类型
        std::cout << "  Node Type: " << ast->getNodeType() << std::endl;
        
        // 根据节点类型显示详细信息
        if (CreateUserNode* node = dynamic_cast<CreateUserNode*>(ast.get())) {
            std::cout << "  User Name: " << node->userName << std::endl;
            std::cout << "  Password: " << (node->password.empty() ? "(empty)" : "***") << std::endl;
        } else if (AlterUserNode* node = dynamic_cast<AlterUserNode*>(ast.get())) {
            std::cout << "  User Name: " << node->userName << std::endl;
            std::cout << "  Is Password Change: " << (node->isPasswordChange ? "Yes" : "No") << std::endl;
            if (node->isPasswordChange) {
                std::cout << "  New Password: ***" << std::endl;
            } else {
                std::cout << "  Disable: " << (node->disable ? "Yes" : "No") << std::endl;
            }
        } else if (DropUserNode* node = dynamic_cast<DropUserNode*>(ast.get())) {
            std::cout << "  User Name: " << node->userName << std::endl;
        } else if (CreateRoleNode* node = dynamic_cast<CreateRoleNode*>(ast.get())) {
            std::cout << "  Role Name: " << node->roleName << std::endl;
        } else if (DropRoleNode* node = dynamic_cast<DropRoleNode*>(ast.get())) {
            std::cout << "  Role Name: " << node->roleName << std::endl;
        } else if (GrantNode* node = dynamic_cast<GrantNode*>(ast.get())) {
            std::cout << "  Is Role Grant: " << (node->isRoleGrant ? "Yes" : "No") << std::endl;
            if (node->isRoleGrant) {
                std::cout << "  Role Name: " << node->roleName << std::endl;
            } else {
                std::cout << "  Privilege Types: ";
                for (size_t i = 0; i < node->privilegeTypes.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << node->privilegeTypes[i];
                }
                std::cout << std::endl;
                std::cout << "  Object Type: " << node->objectType << std::endl;
                std::cout << "  Object Name: " << node->objectName << std::endl;
            }
            std::cout << "  Grantee: " << node->grantee << std::endl;
            std::cout << "  With Grant Option: " << (node->withGrantOption ? "Yes" : "No") << std::endl;
        } else if (RevokeNode* node = dynamic_cast<RevokeNode*>(ast.get())) {
            std::cout << "  Is Role Revoke: " << (node->isRoleRevoke ? "Yes" : "No") << std::endl;
            if (node->isRoleRevoke) {
                std::cout << "  Role Name: " << node->roleName << std::endl;
            } else {
                std::cout << "  Privilege Types: ";
                for (size_t i = 0; i < node->privilegeTypes.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << node->privilegeTypes[i];
                }
                std::cout << std::endl;
                std::cout << "  Object Type: " << node->objectType << std::endl;
                std::cout << "  Object Name: " << node->objectName << std::endl;
            }
            std::cout << "  Revokee: " << node->revokee << std::endl;
        }
    } else {
        std::cout << "✗ Parse FAILED: " << parser.getLastError() << std::endl;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SQL Parser User Permission Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 测试CREATE USER
    testSQL("CREATE USER admin IDENTIFIED BY 'admin';", "CREATE USER");
    
    // 测试ALTER USER（修改密码）
    testSQL("ALTER USER admin IDENTIFIED BY 'newpassword';", "ALTER USER (change password)");
    
    // 测试ALTER USER（启用）
    testSQL("ALTER USER admin ENABLE;", "ALTER USER (enable)");
    
    // 测试ALTER USER（禁用）
    testSQL("ALTER USER admin DISABLE;", "ALTER USER (disable)");
    
    // 测试DROP USER
    testSQL("DROP USER admin;", "DROP USER");
    
    // 测试CREATE ROLE
    testSQL("CREATE ROLE student_role;", "CREATE ROLE");
    
    // 测试DROP ROLE
    testSQL("DROP ROLE student_role;", "DROP ROLE");
    
    // 测试GRANT（单个权限）
    testSQL("GRANT SELECT ON TABLE Students TO student1;", "GRANT (single privilege)");
    
    // 测试GRANT（多个权限）
    testSQL("GRANT SELECT, INSERT, UPDATE ON TABLE Grades TO teacher1;", "GRANT (multiple privileges)");
    
    // 测试GRANT（ALL PRIVILEGES）
    testSQL("GRANT ALL PRIVILEGES ON TABLE Students TO admin;", "GRANT (ALL PRIVILEGES)");
    
    // 测试GRANT（WITH GRANT OPTION）
    testSQL("GRANT SELECT ON TABLE Students TO admin WITH GRANT OPTION;", "GRANT (WITH GRANT OPTION)");
    
    // 测试GRANT（DATABASE级别）
    testSQL("GRANT ALL PRIVILEGES ON DATABASE TO admin;", "GRANT (DATABASE level)");
    
    // 测试GRANT（角色授予）
    testSQL("GRANT student_role TO student1;", "GRANT (role)");
    
    // 测试REVOKE（权限撤销）
    testSQL("REVOKE SELECT, INSERT ON TABLE Students FROM student1;", "REVOKE (privileges)");
    
    // 测试REVOKE（角色撤销）
    testSQL("REVOKE student_role FROM student1;", "REVOKE (role)");
    
    // 测试错误情况：缺少分号
    testSQL("CREATE USER admin IDENTIFIED BY 'admin'", "CREATE USER (missing semicolon - should fail)");
    
    // 测试错误情况：语法错误
    testSQL("CREATE USER admin;", "CREATE USER (missing IDENTIFIED BY - should fail)");
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Complete" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}




