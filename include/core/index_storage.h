#pragma once

#include "core/index_manager.h"
#include <string>
#include <fstream>

/**
 * @file index_storage.h
 * @brief 索引存储管理器
 * 
 * 负责将索引信息保存到.idx文件，并在数据库打开时加载索引
 */

/**
 * @brief 索引存储管理器类
 */
class IndexStorageManager {
public:
    /**
     * @brief 保存数据库的所有索引到.idx文件
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径（用于确定.idx文件位置）
     * @param indexManager 索引管理器（包含所有索引信息）
     * @return 成功返回true，失败返回false
     */
    static bool saveIndices(const std::string& dbName, const std::string& dbPath, 
                           IndexManager& indexManager);
    
    /**
     * @brief 从.idx文件加载数据库的所有索引
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径（用于确定.idx文件位置）
     * @param indexManager 索引管理器（用于重建索引）
     * @return 成功返回true，失败返回false（文件不存在不算错误）
     */
    static bool loadIndices(const std::string& dbName, const std::string& dbPath, 
                           IndexManager& indexManager);
    
    /**
     * @brief 删除数据库的索引文件（当数据库被删除时）
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径（用于确定.idx文件位置）
     * @return 成功返回true，失败返回false
     */
    static bool deleteIndices(const std::string& dbName, const std::string& dbPath);
    
    /**
     * @brief 获取索引文件路径
     * @param dbName 数据库名（不含扩展名）
     * @param dbPath 数据库路径
     * @return 索引文件完整路径
     */
    static std::string getIndexFilePath(const std::string& dbName, const std::string& dbPath);
    
private:
    /**
     * @brief 写入索引信息到文件流
     */
    static bool writeIndexInfo(std::ofstream& file, const IndexInfo& info);
    
    /**
     * @brief 从文件流读取索引信息
     */
    static bool readIndexInfo(std::ifstream& file, IndexInfo& info);
};




