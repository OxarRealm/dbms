/**
 * @file test_user_permission_system.cpp
 * @brief 用户权限系统测试程序
 * 
 * 测试UserManager、RoleManager、PermissionManager的功能
 */

#include "../../include/core/user_manager.h"
#include "../../include/core/role_manager.h"
#include "../../include/core/permission_manager.h"
#include "../../include/core/session_manager.h"
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
const char* TEST_DB_FILE = "test_user_perm_db";

/**
 * @brief 清理测试文件
 */
void cleanupTestFiles() {
    std::string usrFile = std::string(TEST_DB_FILE) + ".usr";
    std::string roleFile = std::string(TEST_DB_FILE) + ".role";
    std::string permFile = std::string(TEST_DB_FILE) + ".perm";
    std::string usrrFile = std::string(TEST_DB_FILE) + ".usrr";
    std::remove(usrFile.c_str());
    std::remove(roleFile.c_str());
    std::remove(permFile.c_str());
    std::remove(usrrFile.c_str());
}

// ==================== UserManager测试 ====================

/**
 * @brief 测试创建用户
 */
void testCreateUser() {
    std::cout << "\n[测试1] 创建用户测试" << std::endl;
    
    cleanupTestFiles();
    
    UserManager userManager;
    userManager.setDatabasePath(TEST_DB_FILE);
    
    bool result = userManager.createUser("admin", "admin123");
    TEST_ASSERT(result == true, "创建用户成功");
    
    result = userManager.userExists("admin");
    TEST_ASSERT(result == true, "用户存在检查");
}

/**
 * @brief 测试用户认证
 */
void testUserAuthentication() {
    std::cout << "\n[测试2] 用户认证测试" << std::endl;
    
    UserManager userManager;
    userManager.setDatabasePath(TEST_DB_FILE);
    
    bool result = userManager.authenticate("admin", "admin123");
    TEST_ASSERT(result == true, "正确密码认证成功");
    
    result = userManager.authenticate("admin", "wrongpassword");
    TEST_ASSERT(result == false, "错误密码认证失败");
    
    result = userManager.authenticate("nonexist", "password");
    TEST_ASSERT(result == false, "不存在的用户认证失败");
}

/**
 * @brief 测试修改密码
 */
void testChangePassword() {
    std::cout << "\n[测试3] 修改密码测试" << std::endl;
    
    UserManager userManager;
    userManager.setDatabasePath(TEST_DB_FILE);
    
    bool result = userManager.changePassword("admin", "newpassword");
    TEST_ASSERT(result == true, "修改密码成功");
    
    result = userManager.authenticate("admin", "newpassword");
    TEST_ASSERT(result == true, "新密码认证成功");
    
    result = userManager.authenticate("admin", "admin123");
    TEST_ASSERT(result == false, "旧密码认证失败");
}

/**
 * @brief 测试启用/禁用用户
 */
void testEnableDisableUser() {
    std::cout << "\n[测试4] 启用/禁用用户测试" << std::endl;
    
    UserManager userManager;
    userManager.setDatabasePath(TEST_DB_FILE);
    
    // 创建新用户用于测试
    userManager.createUser("testuser", "test123");
    
    bool result = userManager.disableUser("testuser");
    TEST_ASSERT(result == true, "禁用用户成功");
    
    UserInfo userInfo;
    userManager.getUserInfo("testuser", userInfo);
    TEST_ASSERT(userInfo.status == USER_STATUS_DISABLED, "用户状态为禁用");
    
    result = userManager.enableUser("testuser");
    TEST_ASSERT(result == true, "启用用户成功");
    
    userManager.getUserInfo("testuser", userInfo);
    TEST_ASSERT(userInfo.status == USER_STATUS_ENABLED, "用户状态为启用");
}

// ==================== RoleManager测试 ====================

/**
 * @brief 测试创建角色
 */
void testCreateRole() {
    std::cout << "\n[测试5] 创建角色测试" << std::endl;
    
    RoleManager roleManager;
    roleManager.setDatabasePath(TEST_DB_FILE);
    
    bool result = roleManager.createRole("admin_role", "Administrator role");
    TEST_ASSERT(result == true, "创建角色成功");
    
    result = roleManager.roleExists("admin_role");
    TEST_ASSERT(result == true, "角色存在检查");
}

/**
 * @brief 测试授予角色给用户
 */
void testGrantRoleToUser() {
    std::cout << "\n[测试6] 授予角色给用户测试" << std::endl;
    
    RoleManager roleManager;
    roleManager.setDatabasePath(TEST_DB_FILE);
    
    bool result = roleManager.grantRoleToUser("admin_role", "admin");
    TEST_ASSERT(result == true, "授予角色成功");
    
    result = roleManager.userHasRole("admin", "admin_role");
    TEST_ASSERT(result == true, "用户拥有角色");
    
    std::vector<std::string> roles;
    roleManager.getUserRoles("admin", roles);
    TEST_ASSERT(roles.size() >= 1, "用户角色列表不为空");
    TEST_ASSERT(roles[0] == "admin_role", "角色名称正确");
}

/**
 * @brief 测试撤销角色
 */
void testRevokeRoleFromUser() {
    std::cout << "\n[测试7] 撤销角色测试" << std::endl;
    
    RoleManager roleManager;
    roleManager.setDatabasePath(TEST_DB_FILE);
    
    bool result = roleManager.revokeRoleFromUser("admin_role", "admin");
    TEST_ASSERT(result == true, "撤销角色成功");
    
    result = roleManager.userHasRole("admin", "admin_role");
    TEST_ASSERT(result == false, "用户不再拥有角色");
}

// ==================== PermissionManager测试 ====================

/**
 * @brief 测试授予权限
 */
void testGrantPermission() {
    std::cout << "\n[测试8] 授予权限测试" << std::endl;
    
    PermissionManager permManager;
    permManager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<PermissionType> permTypes;
    permTypes.push_back(PermissionType::SELECT);
    permTypes.push_back(PermissionType::INSERT);
    
    bool result = permManager.grantPermission("admin", OBJECT_TYPE_TABLE, "Students", 
                                              permTypes, false, "SYSTEM");
    TEST_ASSERT(result == true, "授予权限成功");
}

/**
 * @brief 测试权限检查
 */
void testHasPermission() {
    std::cout << "\n[测试9] 权限检查测试" << std::endl;
    
    PermissionManager permManager;
    permManager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<std::string> userRoles;
    userRoles.push_back("admin_role");
    
    bool result = permManager.hasPermission("admin", OBJECT_TYPE_TABLE, "Students", 
                                           PermissionType::SELECT, userRoles);
    TEST_ASSERT(result == true, "用户有SELECT权限");
    
    result = permManager.hasPermission("admin", OBJECT_TYPE_TABLE, "Students", 
                                      PermissionType::UPDATE, userRoles);
    TEST_ASSERT(result == false, "用户没有UPDATE权限");
}

/**
 * @brief 测试角色权限
 */
void testRolePermission() {
    std::cout << "\n[测试10] 角色权限测试" << std::endl;
    
    PermissionManager permManager;
    permManager.setDatabasePath(TEST_DB_FILE);
    
    // 授予权限给角色
    std::vector<PermissionType> permTypes;
    permTypes.push_back(PermissionType::UPDATE);
    
    bool result = permManager.grantPermissionToRole("admin_role", OBJECT_TYPE_TABLE, 
                                                    "Students", permTypes, false, "SYSTEM");
    TEST_ASSERT(result == true, "授予权限给角色成功");
    
    // 检查用户通过角色继承的权限
    std::vector<std::string> userRoles;
    userRoles.push_back("admin_role");
    
    result = permManager.hasPermission("admin", OBJECT_TYPE_TABLE, "Students", 
                                      PermissionType::UPDATE, userRoles);
    TEST_ASSERT(result == true, "用户通过角色拥有UPDATE权限");
}

/**
 * @brief 测试ALL PRIVILEGES
 */
void testAllPrivileges() {
    std::cout << "\n[测试11] ALL PRIVILEGES测试" << std::endl;
    
    PermissionManager permManager;
    permManager.setDatabasePath(TEST_DB_FILE);
    
    // 授予ALL PRIVILEGES
    std::vector<PermissionType> permTypes;
    permTypes.push_back(PermissionType::ALL_PRIVILEGES);
    
    bool result = permManager.grantPermission("admin", OBJECT_TYPE_TABLE, "Grades", 
                                              permTypes, false, "SYSTEM");
    TEST_ASSERT(result == true, "授予ALL PRIVILEGES成功");
    
    // 检查各种权限
    std::vector<std::string> userRoles;
    result = permManager.hasPermission("admin", OBJECT_TYPE_TABLE, "Grades", 
                                      PermissionType::SELECT, userRoles);
    TEST_ASSERT(result == true, "ALL PRIVILEGES包含SELECT");
    
    result = permManager.hasPermission("admin", OBJECT_TYPE_TABLE, "Grades", 
                                      PermissionType::DELETE, userRoles);
    TEST_ASSERT(result == true, "ALL PRIVILEGES包含DELETE");
}

/**
 * @brief 测试撤销权限
 */
void testRevokePermission() {
    std::cout << "\n[测试12] 撤销权限测试" << std::endl;
    
    PermissionManager permManager;
    permManager.setDatabasePath(TEST_DB_FILE);
    
    std::vector<PermissionType> permTypes;
    permTypes.push_back(PermissionType::INSERT);
    
    bool result = permManager.revokePermission("admin", OBJECT_TYPE_TABLE, "Students", permTypes);
    TEST_ASSERT(result == true, "撤销权限成功");
    
    std::vector<std::string> userRoles;
    result = permManager.hasPermission("admin", OBJECT_TYPE_TABLE, "Students", 
                                      PermissionType::INSERT, userRoles);
    TEST_ASSERT(result == false, "用户不再有INSERT权限");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  用户权限系统测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 运行所有测试
    testCreateUser();
    testUserAuthentication();
    testChangePassword();
    testEnableDisableUser();
    
    testCreateRole();
    testGrantRoleToUser();
    testRevokeRoleFromUser();
    
    testGrantPermission();
    testHasPermission();
    testRolePermission();
    testAllPrivileges();
    testRevokePermission();
    
    // 显示测试结果
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试结果统计" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  通过: " << testsPassed << std::endl;
    std::cout << "  失败: " << testsFailed << std::endl;
    std::cout << "  总计: " << (testsPassed + testsFailed) << std::endl;
    
    if (testsFailed == 0) {
        std::cout << "\n所有测试通过！" << std::endl;
        return 0;
    } else {
        std::cout << "\n部分测试失败！" << std::endl;
        return 1;
    }
}


