#pragma once

#include "core/table_mode.h"
#include "core/constraint.h"
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
    // 扩展：表级约束
    std::vector<ForeignKeyConstraint> foreignKeys;    // 外键约束列表
    std::vector<UniqueConstraint> uniqueConstraints;  // 唯一约束列表（多字段）
    std::vector<CheckConstraint> checkConstraints;    // 检查约束列表
    
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
 * @brief 聚合函数信息结构
 */
struct AggregateFunction {
    std::string funcName;     // 聚合函数名：COUNT, SUM, AVG, MAX, MIN
    std::string fieldName;    // 聚合字段名（COUNT(*)时为空字符串）
    bool isStar;              // 是否为COUNT(*)
    
    AggregateFunction() : isStar(false) {}
};

/**
 * @brief SELECT字段信息（可以是普通字段或聚合函数）
 */
struct SelectField {
    bool isAggregate;                    // 是否为聚合函数
    std::string fieldName;               // 普通字段名（isAggregate=false时使用）
    AggregateFunction aggregateFunc;     // 聚合函数信息（isAggregate=true时使用）
    std::string alias;                  // 字段别名（可选）
    
    SelectField() : isAggregate(false) {}
};

/**
 * @brief WHERE条件节点（支持复杂条件树）
 */
struct WhereCondition {
    std::string logicalOp;        // 逻辑运算符：AND, OR, NOT（空字符串表示简单条件）
    std::string fieldName;        // 字段名（简单条件）
    std::string operator_;        // 比较运算符：=, !=, >, <, >=, <=, LIKE, IN, BETWEEN, EXISTS
    std::string value;            // 值（简单条件，用于=, !=, >, <, >=, <=, LIKE）
    std::vector<std::string> inValues;  // IN子句的值列表
    std::string betweenStart;     // BETWEEN起始值
    std::string betweenEnd;       // BETWEEN结束值
    std::unique_ptr<SelectNode> subquery;  // 子查询（用于=, !=, >, <, >=, <=, IN, EXISTS等）
    std::unique_ptr<WhereCondition> left;   // 左子树（复杂条件）
    std::unique_ptr<WhereCondition> right;  // 右子树（复杂条件）
    
    WhereCondition() : logicalOp("") {}
    
    // 判断是否为简单条件
    bool isSimple() const {
        return logicalOp.empty() && !fieldName.empty();
    }
    
    // 判断是否包含子查询
    bool hasSubquery() const {
        return subquery != nullptr;
    }
};

/**
 * @brief SELECT语句AST节点
 */
class SelectNode : public ASTNode {
public:
    // 新字段：支持聚合函数的字段列表
    std::vector<SelectField> selectFieldsNew;  // 新字段列表（支持聚合函数和别名）
    // 保留旧字段以保持向后兼容
    std::vector<std::string> selectFields;     // 选择的字段列表（*表示所有字段，向后兼容）
    
    std::vector<std::string> fromTables;       // FROM表列表
    std::vector<JoinInfo> joins;               // JOIN连接列表（可选）
    std::unique_ptr<WhereCondition> whereClause;  // WHERE条件（树形结构，可选）
    // 保留旧字段以保持向后兼容（如果whereClause为空，使用这些字段）
    std::string whereField;                    // WHERE条件字段名（可选，向后兼容）
    std::string whereValue;                     // WHERE条件值（可选，向后兼容）
    std::string whereOperator;                 // WHERE条件运算符（可选，向后兼容）
    bool distinct;                             // DISTINCT标志
    std::vector<OrderByInfo> orderBy;          // ORDER BY子句（可选）
    std::vector<std::string> groupBy;          // GROUP BY字段列表（可选）
    std::unique_ptr<WhereCondition> havingClause;  // HAVING条件（可选，与WHERE条件结构相同）
    int limitCount;                            // LIMIT子句（可选，-1表示无限制）
    
    // UNION相关字段
    std::vector<std::unique_ptr<SelectNode>> unionQueries;  // UNION的查询列表（可选）
    bool unionAll;                             // 是否为UNION ALL（保留重复，默认为false，即去重）
    
    SelectNode() : distinct(false), limitCount(-1), unionAll(false) {}
    
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

