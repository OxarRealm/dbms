#pragma once

#include "core/table_mode.h"
#include <string>
#include <vector>
#include <fstream>

/**
 * @file table_manager.h
 * @brief 表结构管理器
 *
 * 负责管理.dbf文件（表结构文件）的读写操作
 * 支持多表存储，使用'~'分隔符分隔不同表
 */

/**
 * @brief 表结构管理器类
 *
 * 提供表的创建、读取、更新、删除等功能
 * 管理.dbf文件的读写操作
 */
class TableManager {
public:
    TableManager();
    ~TableManager();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 创建表
     * @param tableInfo 表结构信息
     * @return 成功返回true，失败返回false
     */
    bool createTable(const TableInfo& tableInfo);

    /**
     * @brief 读取表结构
     * @param tableName 表名
     * @param tableInfo 输出参数，表结构信息
     * @return 成功返回true，失败返回false
     */
    bool readTable(const std::string& tableName, TableInfo& tableInfo);

    /**
     * @brief 读取所有表结构
     * @param tableList 输出参数，所有表结构列表
     * @return 成功返回true，失败返回false
     */
    bool readAllTables(std::vector<TableInfo>& tableList);

    /**
     * @brief 更新表结构
     * @param tableName 表名
     * @param tableInfo 新的表结构信息
     * @return 成功返回true，失败返回false
     */
    bool updateTable(const std::string& tableName, const TableInfo& tableInfo);

    /**
     * @brief 删除表
     * @param tableName 表名
     * @return 成功返回true，失败返回false
     */
    bool deleteTable(const std::string& tableName);

    /**
     * @brief 重命名表
     * @param oldTableName 旧表名
     * @param newTableName 新表名
     * @return 成功返回true，失败返回false
     */
    bool renameTable(const std::string& oldTableName, const std::string& newTableName);

    /**
     * @brief 检查表是否存在
     * @param tableName 表名
     * @return 存在返回true，不存在返回false
     */
    bool tableExists(const std::string& tableName);

    /**
     * @brief 获取所有表名
     * @param tableNames 输出参数，所有表名列表
     * @return 成功返回true，失败返回false
     */
    bool getAllTableNames(std::vector<std::string>& tableNames);

private:
    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）

    /**
     * @brief 获取.dbf文件路径
     * @return .dbf文件完整路径
     */
    std::string getDbfFilePath() const;

    /**
     * @brief 读取单个表结构（从文件流的当前位置）
     * @param file 文件流（已打开）
     * @param tableInfo 输出参数，表结构信息
     * @return 成功返回true，失败返回false
     */
    bool readTableFromStream(std::ifstream& file, TableInfo& tableInfo);

    /**
     * @brief 写入单个表结构到文件流
     * @param file 文件流（已打开）
     * @param tableInfo 表结构信息
     * @return 成功返回true，失败返回false
     */
    bool writeTableToStream(std::ofstream& file, const TableInfo& tableInfo);

    /**
     * @brief 重新写入所有表结构（用于更新和删除操作）
     * @param tableList 所有表结构列表
     * @return 成功返回true，失败返回false
     */
    bool rewriteAllTables(const std::vector<TableInfo>& tableList);
};

