/**
 * @file ast_node.cpp
 * @brief AST节点实现
 */

#include "sql_parser/ast_node.h"

void CreateTableNode::accept(ASTVisitor* visitor) {
    visitor->visitCreateTable(this);
}

void EditTableNode::accept(ASTVisitor* visitor) {
    visitor->visitEditTable(this);
}

void RenameTableNode::accept(ASTVisitor* visitor) {
    visitor->visitRenameTable(this);
}

void DropTableNode::accept(ASTVisitor* visitor) {
    visitor->visitDropTable(this);
}

void CreateIndexNode::accept(ASTVisitor* visitor) {
    // 索引语句不使用Visitor模式，直接使用Handler模式
    // 这个方法不会被调用，因为索引语句通过Handler直接处理
    // 为了满足抽象基类要求，提供一个空实现
    (void)visitor;  // 避免未使用参数警告
}

void DropIndexNode::accept(ASTVisitor* visitor) {
    // 索引语句不使用Visitor模式，直接使用Handler模式
    // 这个方法不会被调用，因为索引语句通过Handler直接处理
    // 为了满足抽象基类要求，提供一个空实现
    (void)visitor;  // 避免未使用参数警告
}

void InsertNode::accept(ASTVisitor* visitor) {
    visitor->visitInsert(this);
}

void DeleteNode::accept(ASTVisitor* visitor) {
    visitor->visitDelete(this);
}

void UpdateNode::accept(ASTVisitor* visitor) {
    visitor->visitUpdate(this);
}

void SelectNode::accept(ASTVisitor* visitor) {
    visitor->visitSelect(this);
}

void CreateUserNode::accept(ASTVisitor* visitor) {
    visitor->visitCreateUser(this);
}

void AlterUserNode::accept(ASTVisitor* visitor) {
    visitor->visitAlterUser(this);
}

void DropUserNode::accept(ASTVisitor* visitor) {
    visitor->visitDropUser(this);
}

void CreateRoleNode::accept(ASTVisitor* visitor) {
    visitor->visitCreateRole(this);
}

void DropRoleNode::accept(ASTVisitor* visitor) {
    visitor->visitDropRole(this);
}

void GrantNode::accept(ASTVisitor* visitor) {
    visitor->visitGrant(this);
}

void RevokeNode::accept(ASTVisitor* visitor) {
    visitor->visitRevoke(this);
}

