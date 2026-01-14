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

