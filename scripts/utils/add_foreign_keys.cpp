/**
 * @file add_foreign_keys.cpp
 * @brief 为学生成绩数据库添加外键约束
 * 
 * 用途：为student_grade_db数据库的StudentGrades表添加外键约束
 * - StudentID -> Students(StudentID) ON DELETE CASCADE
 * - GradeID -> Grades(GradeID) ON DELETE CASCADE
 */

#include "../include/core/constraint_registry.h"
#include "../include/core/constraint_storage.h"
#include "../include/core/constraint.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║      为学生成绩数据库添加外键约束                        ║" << std::endl;
    std::cout << "║      Add Foreign Key Constraints                         ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    // 数据库名称（注意：使用base name，不是完整路径）
    std::string dbName = "student_grade_db";
    std::string tableName = "StudentGrades";
    
    // 获取现有的约束（如果有）
    TableConstraints constraints;
    ConstraintRegistry::getInstance().getTableConstraints(dbName, tableName, constraints);
    
    std::cout << "[1] 检查现有约束..." << std::endl;
    std::cout << "  当前外键约束数量: " << constraints.foreignKeys.size() << std::endl;
    
    // 创建第一个外键约束：StudentID -> Students(StudentID)
    bool hasFK1 = false;
    for (const auto& fk : constraints.foreignKeys) {
        if (std::string(fk.fieldName) == "StudentID" && 
            std::string(fk.referencedTable) == "Students" &&
            std::string(fk.referencedField) == "StudentID") {
            hasFK1 = true;
            break;
        }
    }
    
    if (!hasFK1) {
        std::cout << "[2] 添加外键约束: StudentID -> Students(StudentID)..." << std::endl;
        ForeignKeyConstraint fk1;
        strncpy(fk1.constraintName, "FK_StudentGrades_StudentID", CONSTRAINT_NAME_LENGTH - 1);
        fk1.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
        strncpy(fk1.fieldName, "StudentID", FIELD_NAME_LENGTH - 1);
        fk1.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
        strncpy(fk1.referencedTable, "Students", TABLE_NAME_LENGTH - 1);
        fk1.referencedTable[TABLE_NAME_LENGTH - 1] = '\0';
        strncpy(fk1.referencedField, "StudentID", FIELD_NAME_LENGTH - 1);
        fk1.referencedField[FIELD_NAME_LENGTH - 1] = '\0';
        strncpy(fk1.onDeleteAction, "CASCADE", 15);
        fk1.onDeleteAction[15] = '\0';
        strncpy(fk1.onUpdateAction, "RESTRICT", 15);
        fk1.onUpdateAction[15] = '\0';
        
        constraints.foreignKeys.push_back(fk1);
        std::cout << "  ✓ 外键约束1已添加" << std::endl;
    } else {
        std::cout << "[2] 外键约束 StudentID -> Students(StudentID) 已存在" << std::endl;
    }
    
    // 创建第二个外键约束：GradeID -> Grades(GradeID)
    bool hasFK2 = false;
    for (const auto& fk : constraints.foreignKeys) {
        if (std::string(fk.fieldName) == "GradeID" && 
            std::string(fk.referencedTable) == "Grades" &&
            std::string(fk.referencedField) == "GradeID") {
            hasFK2 = true;
            break;
        }
    }
    
    if (!hasFK2) {
        std::cout << "[3] 添加外键约束: GradeID -> Grades(GradeID)..." << std::endl;
        ForeignKeyConstraint fk2;
        strncpy(fk2.constraintName, "FK_StudentGrades_GradeID", CONSTRAINT_NAME_LENGTH - 1);
        fk2.constraintName[CONSTRAINT_NAME_LENGTH - 1] = '\0';
        strncpy(fk2.fieldName, "GradeID", FIELD_NAME_LENGTH - 1);
        fk2.fieldName[FIELD_NAME_LENGTH - 1] = '\0';
        strncpy(fk2.referencedTable, "Grades", TABLE_NAME_LENGTH - 1);
        fk2.referencedTable[TABLE_NAME_LENGTH - 1] = '\0';
        strncpy(fk2.referencedField, "GradeID", FIELD_NAME_LENGTH - 1);
        fk2.referencedField[FIELD_NAME_LENGTH - 1] = '\0';
        strncpy(fk2.onDeleteAction, "CASCADE", 15);
        fk2.onDeleteAction[15] = '\0';
        strncpy(fk2.onUpdateAction, "RESTRICT", 15);
        fk2.onUpdateAction[15] = '\0';
        
        constraints.foreignKeys.push_back(fk2);
        std::cout << "  ✓ 外键约束2已添加" << std::endl;
    } else {
        std::cout << "[3] 外键约束 GradeID -> Grades(GradeID) 已存在" << std::endl;
    }
    
    // 注册约束到内存
    std::cout << "[4] 注册约束到内存..." << std::endl;
    ConstraintRegistry::getInstance().registerTableConstraints(dbName, tableName, constraints);
    std::cout << "  ✓ 约束已注册到内存" << std::endl;
    
    // 先加载现有约束（以确保其他表的约束也被保存）
    std::cout << "[5] 加载现有约束..." << std::endl;
    std::string dbPath = dbName;  // 使用base name
    ConstraintStorageManager::loadConstraints(dbName, dbPath);
    std::cout << "  ✓ 现有约束已加载" << std::endl;
    
    // 再次注册StudentGrades表的约束（确保最新）
    ConstraintRegistry::getInstance().registerTableConstraints(dbName, tableName, constraints);
    
    // 保存整个数据库的所有约束到文件
    std::cout << "[6] 保存约束到文件..." << std::endl;
    if (!ConstraintStorageManager::saveConstraints(dbName, dbPath)) {
        std::cerr << "  ✗ 错误: 保存约束失败" << std::endl;
        return 1;
    }
    std::cout << "  ✓ 约束已保存到 " << dbName << ".cst" << std::endl;
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  外键约束添加完成！                       ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    std::cout << "已添加的外键约束:" << std::endl;
    std::cout << "  1. StudentGrades.StudentID -> Students.StudentID (ON DELETE CASCADE)" << std::endl;
    std::cout << "  2. StudentGrades.GradeID -> Grades.GradeID (ON DELETE CASCADE)" << std::endl;
    std::cout << "\n";
    std::cout << "约束文件位置: " << dbPath << ".cst" << std::endl;
    std::cout << "\n";
    std::cout << "现在你可以在GUI中查看这些外键约束：" << std::endl;
    std::cout << "  1. 打开 student_grade_db 数据库" << std::endl;
    std::cout << "  2. 选择 StudentGrades 表" << std::endl;
    std::cout << "  3. 点击 \"View Constraints\" 按钮" << std::endl;
    std::cout << "\n";
    
    return 0;
}

