/**
 * @file test_ast_node.cpp
 * @brief AST Node模块测试程序
 * 
 * 测试AST节点的基础功能：
 * - AST节点创建和访问
 * - Visitor模式正确性
 * - 节点类型转换（dynamic_cast）
 * - AST节点结构完整性
 */

#include "sql_parser/ast_node.h"
#include "core/table_mode.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

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
 * @brief 简单的Visitor实现用于测试
 */
class TestVisitor : public ASTVisitor {
public:
    int createTableCount = 0;
    int editTableCount = 0;
    int renameTableCount = 0;
    int dropTableCount = 0;
    int insertCount = 0;
    int deleteCount = 0;
    int updateCount = 0;
    int selectCount = 0;
    
    void visitCreateTable(CreateTableNode* node) override {
        createTableCount++;
    }
    
    void visitEditTable(EditTableNode* node) override {
        editTableCount++;
    }
    
    void visitRenameTable(RenameTableNode* node) override {
        renameTableCount++;
    }
    
    void visitDropTable(DropTableNode* node) override {
        dropTableCount++;
    }
    
    void visitInsert(InsertNode* node) override {
        insertCount++;
    }
    
    void visitDelete(DeleteNode* node) override {
        deleteCount++;
    }
    
    void visitUpdate(UpdateNode* node) override {
        updateCount++;
    }
    
    void visitSelect(SelectNode* node) override {
        selectCount++;
    }
};

/**
 * @brief 测试CreateTableNode
 */
void testCreateTableNode() {
    std::cout << "\n[测试1] CreateTableNode测试" << std::endl;
    
    CreateTableNode node;
    node.tableName = "Users";
    node.databaseFileName = "test_db";
    
    TableMode field1;
    strncpy(field1.sFieldName, "UserID", FIELD_NAME_LENGTH - 1);
    field1.sFieldName[FIELD_NAME_LENGTH - 1] = '\0';
    strncpy(field1.sType, "int", TYPE_NAME_LENGTH - 1);
    field1.sType[TYPE_NAME_LENGTH - 1] = '\0';
    field1.iSize = 4;
    field1.bKey = true;
    field1.bNullFlag = false;
    field1.bValidFlag = true;
    
    node.fields.push_back(field1);
    
    TEST_ASSERT(node.tableName == "Users", "表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(node.fields.size() == 1, "字段列表大小正确");
    TEST_ASSERT(node.getNodeType() == "CreateTableNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.createTableCount == 1, "Visitor访问CreateTableNode");
}

/**
 * @brief 测试EditTableNode
 */
void testEditTableNode() {
    std::cout << "\n[测试2] EditTableNode测试" << std::endl;
    
    EditTableNode node;
    node.tableName = "Users";
    node.databaseFileName = "test_db";
    
    strncpy(node.field.sFieldName, "Phone", FIELD_NAME_LENGTH - 1);
    node.field.sFieldName[FIELD_NAME_LENGTH - 1] = '\0';
    strncpy(node.field.sType, "char", TYPE_NAME_LENGTH - 1);
    node.field.sType[TYPE_NAME_LENGTH - 1] = '\0';
    node.field.iSize = 20;
    node.field.bKey = false;
    node.field.bNullFlag = true;
    node.field.bValidFlag = true;
    
    TEST_ASSERT(node.tableName == "Users", "表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(std::string(node.field.sFieldName) == "Phone", "字段名设置正确");
    TEST_ASSERT(node.getNodeType() == "EditTableNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.editTableCount == 1, "Visitor访问EditTableNode");
}

/**
 * @brief 测试RenameTableNode
 */
void testRenameTableNode() {
    std::cout << "\n[测试3] RenameTableNode测试" << std::endl;
    
    RenameTableNode node;
    node.oldTableName = "Users";
    node.newTableName = "Customers";
    node.databaseFileName = "test_db";
    
    TEST_ASSERT(node.oldTableName == "Users", "旧表名设置正确");
    TEST_ASSERT(node.newTableName == "Customers", "新表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(node.getNodeType() == "RenameTableNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.renameTableCount == 1, "Visitor访问RenameTableNode");
}

/**
 * @brief 测试DropTableNode
 */
void testDropTableNode() {
    std::cout << "\n[测试4] DropTableNode测试" << std::endl;
    
    DropTableNode node;
    node.tableName = "Users";
    node.databaseFileName = "test_db";
    
    TEST_ASSERT(node.tableName == "Users", "表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(node.getNodeType() == "DropTableNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.dropTableCount == 1, "Visitor访问DropTableNode");
}

/**
 * @brief 测试InsertNode
 */
void testInsertNode() {
    std::cout << "\n[测试5] InsertNode测试" << std::endl;
    
    InsertNode node;
    node.tableName = "Users";
    node.databaseFileName = "test_db";
    node.values.push_back("1");
    node.values.push_back("John Doe");
    node.values.push_back("john@example.com");
    
    TEST_ASSERT(node.tableName == "Users", "表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(node.values.size() == 3, "值列表大小正确");
    TEST_ASSERT(node.values[0] == "1", "第一个值正确");
    TEST_ASSERT(node.getNodeType() == "InsertNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.insertCount == 1, "Visitor访问InsertNode");
}

/**
 * @brief 测试DeleteNode
 */
void testDeleteNode() {
    std::cout << "\n[测试6] DeleteNode测试" << std::endl;
    
    DeleteNode node;
    node.tableName = "Users";
    node.databaseFileName = "test_db";
    node.conditionField = "UserID";
    node.conditionValue = "1";
    
    TEST_ASSERT(node.tableName == "Users", "表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(node.conditionField == "UserID", "WHERE字段设置正确");
    TEST_ASSERT(node.conditionValue == "1", "WHERE值设置正确");
    TEST_ASSERT(node.getNodeType() == "DeleteNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.deleteCount == 1, "Visitor访问DeleteNode");
}

/**
 * @brief 测试UpdateNode
 */
void testUpdateNode() {
    std::cout << "\n[测试7] UpdateNode测试" << std::endl;
    
    UpdateNode node;
    node.tableName = "Users";
    node.databaseFileName = "test_db";
    node.setField = "Age";
    node.setValue = "26";
    node.whereField = "UserID";
    node.whereValue = "1";
    
    TEST_ASSERT(node.tableName == "Users", "表名设置正确");
    TEST_ASSERT(node.databaseFileName == "test_db", "数据库文件名设置正确");
    TEST_ASSERT(node.setField == "Age", "SET字段设置正确");
    TEST_ASSERT(node.setValue == "26", "SET值设置正确");
    TEST_ASSERT(node.whereField == "UserID", "WHERE字段设置正确");
    TEST_ASSERT(node.whereValue == "1", "WHERE值设置正确");
    TEST_ASSERT(node.getNodeType() == "UpdateNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.updateCount == 1, "Visitor访问UpdateNode");
}

/**
 * @brief 测试SelectNode（基础结构）
 */
void testSelectNode() {
    std::cout << "\n[测试8] SelectNode基础结构测试" << std::endl;
    
    SelectNode node;
    node.selectFields.push_back("*");
    node.fromTables.push_back("Users");
    
    TEST_ASSERT(node.selectFields.size() == 1, "SELECT字段列表大小正确");
    TEST_ASSERT(node.selectFields[0] == "*", "SELECT字段值正确");
    TEST_ASSERT(node.fromTables.size() == 1, "FROM表列表大小正确");
    TEST_ASSERT(node.fromTables[0] == "Users", "FROM表名正确");
    TEST_ASSERT(node.getNodeType() == "SelectNode", "节点类型名称正确");
    
    // 测试Visitor模式
    TestVisitor visitor;
    node.accept(&visitor);
    TEST_ASSERT(visitor.selectCount == 1, "Visitor访问SelectNode");
}

/**
 * @brief 测试节点类型转换（dynamic_cast）
 */
void testNodeTypeConversion() {
    std::cout << "\n[测试9] 节点类型转换测试" << std::endl;
    
    // 测试CreateTableNode转换
    std::unique_ptr<ASTNode> node1 = std::make_unique<CreateTableNode>();
    CreateTableNode* createNode = dynamic_cast<CreateTableNode*>(node1.get());
    TEST_ASSERT(createNode != nullptr, "CreateTableNode类型转换成功");
    
    // 测试SelectNode转换
    std::unique_ptr<ASTNode> node2 = std::make_unique<SelectNode>();
    SelectNode* selectNode = dynamic_cast<SelectNode*>(node2.get());
    TEST_ASSERT(selectNode != nullptr, "SelectNode类型转换成功");
    
    // 测试错误的类型转换
    std::unique_ptr<ASTNode> node3 = std::make_unique<CreateTableNode>();
    SelectNode* wrongNode = dynamic_cast<SelectNode*>(node3.get());
    TEST_ASSERT(wrongNode == nullptr, "错误的类型转换返回nullptr");
}

/**
 * @brief 测试多态性
 */
void testPolymorphism() {
    std::cout << "\n[测试10] 多态性测试" << std::endl;
    
    TestVisitor visitor;
    
    // 创建不同类型的节点
    std::vector<std::unique_ptr<ASTNode>> nodes;
    nodes.push_back(std::make_unique<CreateTableNode>());
    nodes.push_back(std::make_unique<EditTableNode>());
    nodes.push_back(std::make_unique<RenameTableNode>());
    nodes.push_back(std::make_unique<DropTableNode>());
    nodes.push_back(std::make_unique<InsertNode>());
    nodes.push_back(std::make_unique<DeleteNode>());
    nodes.push_back(std::make_unique<UpdateNode>());
    nodes.push_back(std::make_unique<SelectNode>());
    
    // 通过基类指针调用accept方法
    for (auto& node : nodes) {
        node->accept(&visitor);
    }
    
    TEST_ASSERT(visitor.createTableCount == 1, "CreateTableNode多态调用");
    TEST_ASSERT(visitor.editTableCount == 1, "EditTableNode多态调用");
    TEST_ASSERT(visitor.renameTableCount == 1, "RenameTableNode多态调用");
    TEST_ASSERT(visitor.dropTableCount == 1, "DropTableNode多态调用");
    TEST_ASSERT(visitor.insertCount == 1, "InsertNode多态调用");
    TEST_ASSERT(visitor.deleteCount == 1, "DeleteNode多态调用");
    TEST_ASSERT(visitor.updateCount == 1, "UpdateNode多态调用");
    TEST_ASSERT(visitor.selectCount == 1, "SelectNode多态调用");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  AST Node Module Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 运行所有测试
    testCreateTableNode();
    testEditTableNode();
    testRenameTableNode();
    testDropTableNode();
    testInsertNode();
    testDeleteNode();
    testUpdateNode();
    testSelectNode();
    testNodeTypeConversion();
    testPolymorphism();
    
    // 输出测试结果
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total tests: " << (testsPassed + testsFailed) << std::endl;
    std::cout << "Passed: " << testsPassed << std::endl;
    std::cout << "Failed: " << testsFailed << std::endl;
    
    if (testsFailed == 0) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Some tests failed!" << std::endl;
        return 1;
    }
}

