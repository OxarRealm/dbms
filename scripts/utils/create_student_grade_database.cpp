/**
 * @file create_student_grade_database.cpp
 * @brief 创建学生成绩数据库工具
 * 
 * 用途：生成学生成绩管理系统的测试数据库
 * 包含：学生表、成绩表、学生-成绩中间表
 * 使用方法：编译后运行，会在当前目录生成student_grade_db.dbf和student_grade_db.dat
 */

#include "../include/core/table_manager.h"
#include "../include/core/data_manager.h"
#include "../include/core/table_mode.h"
#include "../include/ddl/create_table_handler.h"
#include "../include/dml/insert_handler.h"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>

// 测试数据库名称
const char* DB_NAME = "student_grade_db";

// 数据量配置
const int STUDENT_COUNT = 100;           // 学生数量
const int GRADES_PER_STUDENT_MIN = 3;    // 每个学生最少课程数
const int GRADES_PER_STUDENT_MAX = 8;    // 每个学生最多课程数

// 课程名称列表
const char* COURSE_NAMES[] = {
    "数学", "英语", "物理", "化学", "生物", "历史", "地理", "政治",
    "计算机科学", "数据结构", "算法设计", "数据库原理", "操作系统",
    "计算机网络", "软件工程", "人工智能", "机器学习", "Web开发",
    "移动开发", "信息安全", "编译原理", "离散数学", "线性代数",
    "概率论", "统计学"
};
const int COURSE_COUNT = 25;

// 姓名前缀和后缀（用于生成中文姓名）
const char* SURNAMES[] = {
    "李", "王", "张", "刘", "陈", "杨", "赵", "黄", "周", "吴",
    "徐", "孙", "胡", "朱", "高", "林", "何", "郭", "马", "罗"
};
const int SURNAME_COUNT = 20;

const char* GIVEN_NAMES[] = {
    "明", "强", "伟", "磊", "军", "勇", "敏", "静", "丽", "红",
    "华", "芳", "娜", "秀", "英", "杰", "涛", "超", "鹏", "飞",
    "龙", "健", "文", "武", "斌", "辉", "欣", "悦", "雨", "雪"
};
const int GIVEN_NAME_COUNT = 30;

/**
 * @brief 生成随机中文姓名
 */
std::string generateChineseName(std::mt19937& gen) {
    std::uniform_int_distribution<> surnameDist(0, SURNAME_COUNT - 1);
    std::uniform_int_distribution<> givenDist(0, GIVEN_NAME_COUNT - 1);
    
    std::string name;
    name += SURNAMES[surnameDist(gen)];  // 姓氏
    name += GIVEN_NAMES[givenDist(gen)];  // 名字
    
    // 50%概率有第二个字的名字
    std::uniform_int_distribution<> hasSecondName(0, 1);
    if (hasSecondName(gen) == 1) {
        name += GIVEN_NAMES[givenDist(gen)];
    }
    
    return name;
}

/**
 * @brief 生成随机分数（分数波动大）
 * 分布：高分（80-100）30%，中等（60-79）40%，低分（0-59）30%
 */
int generateScore(std::mt19937& gen) {
    std::uniform_int_distribution<> dist(0, 99);
    int rand = dist(gen);
    
    if (rand < 30) {
        // 30%概率生成高分（80-100）
        std::uniform_int_distribution<> highScore(80, 100);
        return highScore(gen);
    } else if (rand < 70) {
        // 40%概率生成中等分数（60-79）
        std::uniform_int_distribution<> midScore(60, 79);
        return midScore(gen);
    } else {
        // 30%概率生成低分（0-59）
        std::uniform_int_distribution<> lowScore(0, 59);
        return lowScore(gen);
    }
}

/**
 * @brief 生成随机年级（1-4，表示大一到大四）
 */
int generateGrade(std::mt19937& gen) {
    std::uniform_int_distribution<> gradeDist(1, 4);
    return gradeDist(gen);
}

/**
 * @brief 生成随机年龄（18-23）
 */
int generateAge(std::mt19937& gen) {
    std::uniform_int_distribution<> ageDist(18, 23);
    return ageDist(gen);
}

/**
 * @brief 创建学生表
 */
bool createStudentsTable(CreateTableHandler& handler) {
    std::cout << "[1] 创建Students表..." << std::endl;
    
    std::string sql = "CREATE TABLE Students ("
                     "StudentID int KEY NO_NULL VALID,"
                     "StudentName char[50] NOT_KEY NULL VALID,"
                     "StudentAge int NOT_KEY NULL VALID,"
                     "StudentGrade int NOT_KEY NULL VALID"
                     ") INTO " + std::string(DB_NAME) + ";";
    
    bool result = handler.execute(sql);
    if (result) {
        std::cout << "  ✓ Students表创建成功" << std::endl;
    } else {
        std::cerr << "  ✗ Students表创建失败: " << handler.getLastError() << std::endl;
    }
    return result;
}

/**
 * @brief 创建成绩表
 */
bool createGradesTable(CreateTableHandler& handler) {
    std::cout << "[2] 创建Grades表..." << std::endl;
    
    std::string sql = "CREATE TABLE Grades ("
                     "GradeID int KEY NO_NULL VALID,"
                     "Subject char[50] NOT_KEY NULL VALID,"
                     "Score float NOT_KEY NULL VALID"
                     ") INTO " + std::string(DB_NAME) + ";";
    
    bool result = handler.execute(sql);
    if (result) {
        std::cout << "  ✓ Grades表创建成功" << std::endl;
    } else {
        std::cerr << "  ✗ Grades表创建失败: " << handler.getLastError() << std::endl;
    }
    return result;
}

/**
 * @brief 创建学生-成绩中间表
 */
bool createStudentGradesTable(CreateTableHandler& handler) {
    std::cout << "[3] 创建StudentGrades表..." << std::endl;
    
    std::string sql = "CREATE TABLE StudentGrades ("
                     "ID int KEY NO_NULL VALID,"
                     "StudentID int NOT_KEY NULL VALID,"
                     "GradeID int NOT_KEY NULL VALID"
                     ") INTO " + std::string(DB_NAME) + ";";
    
    bool result = handler.execute(sql);
    if (result) {
        std::cout << "  ✓ StudentGrades表创建成功" << std::endl;
    } else {
        std::cerr << "  ✗ StudentGrades表创建失败: " << handler.getLastError() << std::endl;
    }
    return result;
}

/**
 * @brief 插入学生数据
 */
bool insertStudentsData(InsertHandler& handler, std::mt19937& gen) {
    std::cout << "[4] 插入学生数据（" << STUDENT_COUNT << "条记录）..." << std::endl;
    
    int successCount = 0;
    for (int i = 1; i <= STUDENT_COUNT; ++i) {
        std::string name = generateChineseName(gen);
        int age = generateAge(gen);
        int grade = generateGrade(gen);
        
        std::ostringstream sql;
        sql << "INSERT INTO Students VALUES ("
            << i << ", '"
            << name << "', "
            << age << ", "
            << grade
            << ") INTO " << DB_NAME << ";";
        
        if (handler.execute(sql.str())) {
            successCount++;
        } else {
            std::cerr << "  ✗ 插入学生" << i << "失败: " << handler.getLastError() << std::endl;
        }
        
        // 每插入10条记录显示一次进度
        if (i % 10 == 0) {
            std::cout << "  进度: " << i << "/" << STUDENT_COUNT << std::endl;
        }
    }
    
    std::cout << "  ✓ 成功插入 " << successCount << " 条学生记录" << std::endl;
    return successCount == STUDENT_COUNT;
}

/**
 * @brief 插入成绩数据和学生-成绩关联数据
 */
bool insertGradesData(InsertHandler& handler, std::mt19937& gen) {
    std::cout << "[5] 插入成绩数据和关联数据..." << std::endl;
    
    std::uniform_int_distribution<> coursesPerStudent(GRADES_PER_STUDENT_MIN, GRADES_PER_STUDENT_MAX);
    std::uniform_int_distribution<> courseDist(0, COURSE_COUNT - 1);
    
    int gradeID = 1;  // 成绩ID从1开始
    int relationID = 1;  // 关联表ID从1开始
    int totalGrades = 0;
    int successGrades = 0;
    int successRelations = 0;
    
    for (int studentID = 1; studentID <= STUDENT_COUNT; ++studentID) {
        // 每个学生的课程数
        int numCourses = coursesPerStudent(gen);
        
        for (int j = 0; j < numCourses; ++j) {
            // 随机选择课程
            const char* subject = COURSE_NAMES[courseDist(gen)];
            int score = generateScore(gen);
            
            // 插入成绩记录
            std::ostringstream gradeSQL;
            gradeSQL << "INSERT INTO Grades VALUES ("
                     << gradeID << ", '"
                     << subject << "', "
                     << score
                     << ") INTO " << DB_NAME << ";";
            
            if (handler.execute(gradeSQL.str())) {
                successGrades++;
                
                // 插入学生-成绩关联记录
                std::ostringstream relationSQL;
                relationSQL << "INSERT INTO StudentGrades VALUES ("
                           << relationID << ", "
                           << studentID << ", "
                           << gradeID
                           << ") INTO " << DB_NAME << ";";
                
                if (handler.execute(relationSQL.str())) {
                    successRelations++;
                } else {
                    std::cerr << "  ✗ 插入关联记录" << relationID << "失败: " << handler.getLastError() << std::endl;
                }
                
                gradeID++;
                relationID++;
                totalGrades++;
            } else {
                std::cerr << "  ✗ 插入成绩" << gradeID << "失败: " << handler.getLastError() << std::endl;
            }
        }
        
        // 每处理10个学生显示一次进度
        if (studentID % 10 == 0) {
            std::cout << "  进度: 学生 " << studentID << "/" << STUDENT_COUNT 
                      << ", 成绩记录 " << totalGrades << std::endl;
        }
    }
    
    std::cout << "  ✓ 成功插入 " << successGrades << " 条成绩记录" << std::endl;
    std::cout << "  ✓ 成功插入 " << successRelations << " 条关联记录" << std::endl;
    
    return successGrades > 0 && successRelations > 0;
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║      学生成绩数据库生成工具                              ║" << std::endl;
    std::cout << "║      Student Grade Database Generator                    ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    
    // 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // 清理之前的数据库文件
    std::cout << "[0] 清理之前的数据库文件..." << std::endl;
    std::remove((std::string(DB_NAME) + ".dbf").c_str());
    std::remove((std::string(DB_NAME) + ".dat").c_str());
    std::remove((std::string(DB_NAME) + ".cst").c_str());
    std::remove((std::string(DB_NAME) + ".idx").c_str());
    std::cout << "  ✓ 清理完成\n" << std::endl;
    
    CreateTableHandler createHandler;
    InsertHandler insertHandler;
    
    // 创建表
    if (!createStudentsTable(createHandler)) {
        std::cerr << "\n错误: 无法创建Students表，程序退出" << std::endl;
        return 1;
    }
    
    if (!createGradesTable(createHandler)) {
        std::cerr << "\n错误: 无法创建Grades表，程序退出" << std::endl;
        return 1;
    }
    
    if (!createStudentGradesTable(createHandler)) {
        std::cerr << "\n错误: 无法创建StudentGrades表，程序退出" << std::endl;
        return 1;
    }
    
    std::cout << "\n所有表创建完成！\n" << std::endl;
    
    // 插入数据
    if (!insertStudentsData(insertHandler, gen)) {
        std::cerr << "\n警告: 学生数据插入不完整" << std::endl;
    }
    
    std::cout << "\n" << std::endl;
    
    if (!insertGradesData(insertHandler, gen)) {
        std::cerr << "\n警告: 成绩数据插入不完整" << std::endl;
    }
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  数据库生成完成！                         ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
    std::cout << "数据库文件位置:" << std::endl;
    std::cout << "  " << DB_NAME << ".dbf (表结构文件)" << std::endl;
    std::cout << "  " << DB_NAME << ".dat (数据文件)" << std::endl;
    std::cout << "\n";
    std::cout << "数据统计:" << std::endl;
    std::cout << "  - Students表: " << STUDENT_COUNT << " 条记录" << std::endl;
    std::cout << "  - Grades表: 约 " << (STUDENT_COUNT * (GRADES_PER_STUDENT_MIN + GRADES_PER_STUDENT_MAX) / 2) << " 条记录" << std::endl;
    std::cout << "  - StudentGrades表: 约 " << (STUDENT_COUNT * (GRADES_PER_STUDENT_MIN + GRADES_PER_STUDENT_MAX) / 2) << " 条记录" << std::endl;
    std::cout << "\n";
    
    return 0;
}








