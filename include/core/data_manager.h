#pragma once

#include "core/table_mode.h"
#include <string>
#include <vector>
#include <fstream>
#include <map>

/**
 * @file data_manager.h
 * @brief 数据记录管理器
 *
 * 负责管理.dat文件（数据文件）的读写操作
 * 支持多表数据存储，使用'~'分隔符分隔不同表的数据
 */

/**
 * @brief 数据记录管理器类
 *
 * 提供记录的插入、读取、更新、删除等功能
 * 管理.dat文件的读写操作
 */
class DataManager {
public:
    DataManager();
    ~DataManager();

    /**
     * @brief 设置数据库文件路径
     * @param dbFilePath 数据库文件路径（不含扩展名）
     */
    void setDatabasePath(const std::string& dbFilePath);

    /**
     * @brief 插入记录
     * @param tableName 表名
     * @param record 记录数据
     * @return 成功返回true，失败返回false
     */
    bool insertRecord(const std::string& tableName, const Record& record);

    /**
     * @brief 插入多条记录
     * @param tableName 表名
     * @param records 记录列表
     * @return 成功返回true，失败返回false
     */
    bool insertRecords(const std::string& tableName, const std::vector<Record>& records);

    /**
     * @brief 读取所有记录
     * @param tableName 表名
     * @param records 输出参数，记录列表
     * @return 成功返回true，失败返回false
     */
    bool readAllRecords(const std::string& tableName, std::vector<Record>& records);

    /**
     * @brief 读取有效记录（过滤掉已删除的记录）
     * @param tableName 表名
     * @param records 输出参数，有效记录列表
     * @return 成功返回true，失败返回false
     */
    bool readValidRecords(const std::string& tableName, std::vector<Record>& records);

    /**
     * @brief 更新记录
     * @param tableName 表名
     * @param recordIndex 记录索引（从0开始）
     * @param record 新的记录数据
     * @return 成功返回true，失败返回false
     */
    bool updateRecord(const std::string& tableName, size_t recordIndex, const Record& record);

    /**
     * @brief 删除记录（标记为无效）
     * @param tableName 表名
     * @param recordIndex 记录索引（从0开始）
     * @return 成功返回true，失败返回false
     */
    bool deleteRecord(const std::string& tableName, size_t recordIndex);

    /**
     * @brief 获取记录数量
     * @param tableName 表名
     * @param count 输出参数，记录数量
     * @return 成功返回true，失败返回false
     */
    bool getRecordCount(const std::string& tableName, size_t& count);

    /**
     * @brief 获取有效记录数量
     * @param tableName 表名
     * @param count 输出参数，有效记录数量
     * @return 成功返回true，失败返回false
     */
    bool getValidRecordCount(const std::string& tableName, size_t& count);

    /**
     * @brief 清空表的所有记录
     * @param tableName 表名
     * @return 成功返回true，失败返回false
     */
    bool clearTable(const std::string& tableName);

private:
    std::string m_dbFilePath;  // 数据库文件路径（不含扩展名）

    /**
     * @brief 获取.dat文件路径
     * @return .dat文件完整路径
     */
    std::string getDatFilePath() const;

    /**
     * @brief 读取单个表的数据（从文件流的当前位置）
     * @param file 文件流（已打开）
     * @param tableName 输出参数，表名
     * @param records 输出参数，记录列表
     * @return 成功返回true，失败返回false
     */
    bool readTableDataFromStream(std::ifstream& file, std::string& tableName, 
                                 std::vector<Record>& records);

    /**
     * @brief 写入单个表的数据到文件流
     * @param file 文件流（已打开）
     * @param tableName 表名
     * @param records 记录列表
     * @return 成功返回true，失败返回false
     */
    bool writeTableDataToStream(std::ofstream& file, const std::string& tableName,
                                const std::vector<Record>& records);

    /**
     * @brief 重新写入所有表数据（用于更新和删除操作）
     * @param allTableData 所有表的数据（表名->记录列表的映射）
     * @return 成功返回true，失败返回false
     */
    bool rewriteAllTableData(const std::map<std::string, std::vector<Record>>& allTableData);

    /**
     * @brief 写入字符串字段值（变长存储）
     * @param file 文件流
     * @param value 字段值
     * @return 成功返回true，失败返回false
     */
    bool writeStringValue(std::ofstream& file, const std::string& value);

    /**
     * @brief 读取字符串字段值（变长存储）
     * @param file 文件流
     * @param value 输出参数，字段值
     * @return 成功返回true，失败返回false
     */
    bool readStringValue(std::ifstream& file, std::string& value);
};

