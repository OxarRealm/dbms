#pragma once

#include "core/table_mode.h"
#include "sql_parser/token.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @file ast_node.h
 * @brief SQL抽象语法树（AST）节点定义
 *
 * 定义SQL语句的AST节点结构
 */

// 前向声明
class ASTVisitor;
class SelectNode;

/**
 * @brief AST节点基类
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    /**
     * @brief 接受访问者（Visitor模式）
     */
    virtual void accept(ASTVisitor* visitor) = 0;
    
    /**
     * @brief 获取节点类型名称（用于调试）
     */
    virtual std::string getNodeType() const = 0;
};

/**
 * @brief CREATE TABLE语句AST节点
 */
class CreateTableNode : public ASTNode {
public:
    std::string tableName;             // 表名
    std::vector<TableMode> fields;     // 字段列表
    std::string databaseFileName;      // 数据库文件名
    
    CreateTableNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "CreateTableNode"; }
};

/**
 * @brief EDIT TABLE语句AST节点
 */
class EditTableNode : public ASTNode {
public:
    std::string tableName;             // 表名
    TableMode field;                   // 要修改的字段
    std::string databaseFileName;      // 数据库文件名
    
    EditTableNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "EditTableNode"; }
};

/**
 * @brief RENAME TABLE语句AST节点
 */
class RenameTableNode : public ASTNode {
public:
    std::string oldTableName;          // 旧表名
    std::string newTableName;          // 新表名
    std::string databaseFileName;      // 数据库文件名
    
    RenameTableNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "RenameTableNode"; }
};

/**
 * @brief DROP TABLE语句AST节点
 */
class DropTableNode : public ASTNode {
public:
    std::string tableName;             // 表名
    std::string databaseFileName;      // 数据库文件名
    
    DropTableNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "DropTableNode"; }
};

/**
 * @brief INSERT语句AST节点
 */
class InsertNode : public ASTNode {
public:
    std::string tableName;             // 表名
    std::vector<std::string> values;   // 值列表
    std::string databaseFileName;      // 数据库文件名
    
    InsertNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "InsertNode"; }
};

/**
 * @brief DELETE语句AST节点
 */
class DeleteNode : public ASTNode {
public:
    std::string tableName;             // 表名
    std::string conditionField;        // 条件字段名
    std::string conditionValue;        // 条件值
    std::string databaseFileName;      // 数据库文件名
    
    DeleteNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "DeleteNode"; }
};

/**
 * @brief UPDATE语句AST节点
 */
class UpdateNode : public ASTNode {
public:
    std::string tableName;             // 表名
    std::string setField;              // SET字段名
    std::string setValue;               // SET值
    std::string whereField;             // WHERE字段名
    std::string whereValue;             // WHERE值
    std::string databaseFileName;      // 数据库文件名
    
    UpdateNode() {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "UpdateNode"; }
};

/**
 * @brief JOIN连接信息结构
 */
struct JoinInfo {
    std::string joinType;          // 连接类型：INNER, LEFT, RIGHT（默认为INNER）
    std::string rightTable;        // 右表名
    std::string leftField;         // 左表字段（格式：TableName.FieldName 或 FieldName）
    std::string rightField;        // 右表字段（格式：TableName.FieldName 或 FieldName）
    std::string operator_;         // 连接运算符（=, >, <等，当前只支持=）
    
    JoinInfo() : joinType("INNER"), operator_("=") {}
};

/**
 * @brief ORDER BY排序信息结构
 */
struct OrderByInfo {
    std::string fieldName;    // 排序字段名
    std::string direction;    // 排序方向：ASC 或 DESC（默认为ASC）
    
    OrderByInfo() : direction("ASC") {}
};

/**
 * @brief WHERE条件节点（支持复杂条件树）
 */
struct WhereCondition {
    std::string logicalOp;        // 逻辑运算符：AND, OR, NOT（空字符串表示简单条件）
    std::string fieldName;        // 字段名（简单条件）
    std::string operator_;        // 比较运算符：=, !=, >, <, >=, <=
    std::string value;            // 值（简单条件）
    std::unique_ptr<WhereCondition> left;   // 左子树（复杂条件）
    std::unique_ptr<WhereCondition> right;  // 右子树（复杂条件）
    
    WhereCondition() : logicalOp("") {}
    
    // 判断是否为简单条件
    bool isSimple() const {
        return logicalOp.empty() && !fieldName.empty();
    }
};

/**
 * @brief SELECT语句AST节点
 */
class SelectNode : public ASTNode {
public:
    std::vector<std::string> selectFields;    // 选择的字段列表（*表示所有字段）
    std::vector<std::string> fromTables;      // FROM表列表
    std::vector<JoinInfo> joins;              // JOIN连接列表（可选）
    std::unique_ptr<WhereCondition> whereClause;  // WHERE条件（树形结构，可选）
    // 保留旧字段以保持向后兼容（如果whereClause为空，使用这些字段）
    std::string whereField;                   // WHERE条件字段名（可选，向后兼容）
    std::string whereValue;                   // WHERE条件值（可选，向后兼容）
    std::string whereOperator;                // WHERE条件运算符（可选，向后兼容）
    bool distinct;                            // DISTINCT标志
    std::vector<OrderByInfo> orderBy;         // ORDER BY子句（可选）
    int limitCount;                           // LIMIT子句（可选，-1表示无限制）
    
    SelectNode() : distinct(false), limitCount(-1) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string getNodeType() const override { return "SelectNode"; }
};

/**
 * @brief AST访问者接口（Visitor模式）
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visitCreateTable(CreateTableNode* node) = 0;
    virtual void visitEditTable(EditTableNode* node) = 0;
    virtual void visitRenameTable(RenameTableNode* node) = 0;
    virtual void visitDropTable(DropTableNode* node) = 0;
    virtual void visitInsert(InsertNode* node) = 0;
    virtual void visitDelete(DeleteNode* node) = 0;
    virtual void visitUpdate(UpdateNode* node) = 0;
    virtual void visitSelect(SelectNode* node) = 0;
};

