#pragma once

#include <string>
#include <fstream>
#include <vector>

/**
 * @file file_manager.h
 * @brief 文件管理器
 *
 * 提供基础的文件I/O封装，包括文件路径管理、错误处理等
 */

/**
 * @brief 文件管理器类
 *
 * 封装基础的文件操作，提供统一的接口
 */
class FileManager {
public:
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 存在返回true，不存在返回false
     */
    static bool fileExists(const std::string& filePath);

    /**
     * @brief 删除文件
     * @param filePath 文件路径
     * @return 成功返回true，失败返回false
     */
    static bool deleteFile(const std::string& filePath);

    /**
     * @brief 获取文件大小
     * @param filePath 文件路径
     * @return 文件大小（字节），失败返回-1
     */
    static long getFileSize(const std::string& filePath);

    /**
     * @brief 创建目录（如果不存在）
     * @param dirPath 目录路径
     * @return 成功返回true，失败返回false
     */
    static bool createDirectory(const std::string& dirPath);

    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 扩展名（不含点号），无扩展名返回空字符串
     */
    static std::string getFileExtension(const std::string& filePath);

    /**
     * @brief 获取文件名（不含路径）
     * @param filePath 文件路径
     * @return 文件名
     */
    static std::string getFileName(const std::string& filePath);

    /**
     * @brief 获取文件目录路径
     * @param filePath 文件路径
     * @return 目录路径
     */
    static std::string getDirectoryPath(const std::string& filePath);

    /**
     * @brief 组合路径
     * @param dirPath 目录路径
     * @param fileName 文件名
     * @return 组合后的完整路径
     */
    static std::string joinPath(const std::string& dirPath, const std::string& fileName);

    /**
     * @brief 规范化路径（统一路径分隔符）
     * @param filePath 文件路径
     * @return 规范化后的路径
     */
    static std::string normalizePath(const std::string& filePath);

    /**
     * @brief 检查路径是否为绝对路径
     * @param filePath 文件路径
     * @return 是绝对路径返回true，否则返回false
     */
    static bool isAbsolutePath(const std::string& filePath);

    /**
     * @brief 读取文件的全部内容（文本模式）
     * @param filePath 文件路径
     * @param content 输出参数，文件内容
     * @return 成功返回true，失败返回false
     */
    static bool readTextFile(const std::string& filePath, std::string& content);

    /**
     * @brief 写入文件（文本模式，覆盖）
     * @param filePath 文件路径
     * @param content 文件内容
     * @return 成功返回true，失败返回false
     */
    static bool writeTextFile(const std::string& filePath, const std::string& content);

    /**
     * @brief 追加内容到文件（文本模式）
     * @param filePath 文件路径
     * @param content 要追加的内容
     * @return 成功返回true，失败返回false
     */
    static bool appendTextFile(const std::string& filePath, const std::string& content);

    /**
     * @brief 读取二进制文件的全部内容
     * @param filePath 文件路径
     * @param data 输出参数，文件数据
     * @return 成功返回true，失败返回false
     */
    static bool readBinaryFile(const std::string& filePath, std::vector<char>& data);

    /**
     * @brief 写入二进制文件
     * @param filePath 文件路径
     * @param data 文件数据
     * @return 成功返回true，失败返回false
     */
    static bool writeBinaryFile(const std::string& filePath, const std::vector<char>& data);

    /**
     * @brief 复制文件
     * @param srcPath 源文件路径
     * @param dstPath 目标文件路径
     * @return 成功返回true，失败返回false
     */
    static bool copyFile(const std::string& srcPath, const std::string& dstPath);

    /**
     * @brief 移动/重命名文件
     * @param srcPath 源文件路径
     * @param dstPath 目标文件路径
     * @return 成功返回true，失败返回false
     */
    static bool moveFile(const std::string& srcPath, const std::string& dstPath);

    /**
     * @brief 获取错误信息
     * @return 最后一次操作的错误信息
     */
    static std::string getLastError();

private:
    static std::string m_lastError;  // 最后一次操作的错误信息

    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    static void setError(const std::string& error);

    /**
     * @brief 清除错误信息
     */
    static void clearError();
};

