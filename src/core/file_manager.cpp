/**
 * @file file_manager.cpp
 * @brief 文件管理器实现
 */

#include "core/file_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cerrno>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STR "\\"
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STR "/"
#endif

// 静态成员变量定义
std::string FileManager::m_lastError = "";

bool FileManager::fileExists(const std::string& filePath) {
    clearError();
    
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(filePath.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        setError("文件不存在或无法访问");
        return false;
    }
    return !(attributes & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0) {
        setError("文件不存在或无法访问");
        return false;
    }
    return S_ISREG(buffer.st_mode);
#endif
}

bool FileManager::deleteFile(const std::string& filePath) {
    clearError();
    
    if (!fileExists(filePath)) {
        setError("文件不存在");
        return false;
    }
    
    if (std::remove(filePath.c_str()) != 0) {
        setError("删除文件失败");
        return false;
    }
    
    return true;
}

long FileManager::getFileSize(const std::string& filePath) {
    clearError();
    
    if (!fileExists(filePath)) {
        setError("文件不存在");
        return -1;
    }
    
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        setError("无法打开文件");
        return -1;
    }
    
    long size = static_cast<long>(file.tellg());
    file.close();
    return size;
}

bool FileManager::createDirectory(const std::string& dirPath) {
    clearError();
    
    if (dirPath.empty()) {
        setError("目录路径为空");
        return false;
    }
    
#ifdef _WIN32
    if (_mkdir(dirPath.c_str()) != 0) {
        // 检查是否因为目录已存在而失败
        if (errno != EEXIST) {
            setError("创建目录失败");
            return false;
        }
    }
#else
    if (mkdir(dirPath.c_str(), 0755) != 0) {
        // 检查是否因为目录已存在而失败
        if (errno != EEXIST) {
            setError("创建目录失败");
            return false;
        }
    }
#endif
    
    return true;
}

std::string FileManager::getFileExtension(const std::string& filePath) {
    clearError();
    
    size_t pos = filePath.find_last_of('.');
    if (pos == std::string::npos || pos == filePath.length() - 1) {
        return "";
    }
    
    return filePath.substr(pos + 1);
}

std::string FileManager::getFileName(const std::string& filePath) {
    clearError();
    
    std::string normalized = normalizePath(filePath);
    size_t pos = normalized.find_last_of(PATH_SEPARATOR);
    
    if (pos == std::string::npos) {
        return normalized;
    }
    
    return normalized.substr(pos + 1);
}

std::string FileManager::getDirectoryPath(const std::string& filePath) {
    clearError();
    
    std::string normalized = normalizePath(filePath);
    size_t pos = normalized.find_last_of(PATH_SEPARATOR);
    
    if (pos == std::string::npos) {
        return ".";
    }
    
    return normalized.substr(0, pos);
}

std::string FileManager::joinPath(const std::string& dirPath, const std::string& fileName) {
    clearError();
    
    if (dirPath.empty()) {
        return fileName;
    }
    
    if (fileName.empty()) {
        return dirPath;
    }
    
    std::string normalizedDir = normalizePath(dirPath);
    std::string normalizedFile = normalizePath(fileName);
    
    // 如果目录路径以分隔符结尾，不需要再添加
    if (normalizedDir.back() == PATH_SEPARATOR) {
        return normalizedDir + normalizedFile;
    }
    
    return normalizedDir + PATH_SEPARATOR_STR + normalizedFile;
}

std::string FileManager::normalizePath(const std::string& filePath) {
    clearError();
    
    std::string result = filePath;
    
    // 统一路径分隔符（Windows上可能使用/或\）
#ifdef _WIN32
    std::replace(result.begin(), result.end(), '/', PATH_SEPARATOR);
#else
    std::replace(result.begin(), result.end(), '\\', PATH_SEPARATOR);
#endif
    
    return result;
}

bool FileManager::isAbsolutePath(const std::string& filePath) {
    clearError();
    
    if (filePath.empty()) {
        return false;
    }
    
#ifdef _WIN32
    // Windows: 检查是否以盘符开头 (C:\) 或 UNC路径 (\\)
    return (filePath.length() >= 2 && filePath[1] == ':') ||
           (filePath.length() >= 2 && filePath[0] == '\\' && filePath[1] == '\\');
#else
    // Unix/Linux: 检查是否以/开头
    return filePath[0] == '/';
#endif
}

bool FileManager::readTextFile(const std::string& filePath, std::string& content) {
    clearError();
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        setError("无法打开文件: " + filePath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    
    file.close();
    return true;
}

bool FileManager::writeTextFile(const std::string& filePath, const std::string& content) {
    clearError();
    
    // 确保目录存在
    std::string dirPath = getDirectoryPath(filePath);
    if (!dirPath.empty() && dirPath != ".") {
        createDirectory(dirPath);
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        setError("无法创建文件: " + filePath);
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

bool FileManager::appendTextFile(const std::string& filePath, const std::string& content) {
    clearError();
    
    std::ofstream file(filePath, std::ios::app);
    if (!file.is_open()) {
        setError("无法打开文件: " + filePath);
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

bool FileManager::readBinaryFile(const std::string& filePath, std::vector<char>& data) {
    clearError();
    
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        setError("无法打开文件: " + filePath);
        return false;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    data.resize(static_cast<size_t>(size));
    if (!file.read(data.data(), size)) {
        setError("读取文件失败: " + filePath);
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

bool FileManager::writeBinaryFile(const std::string& filePath, const std::vector<char>& data) {
    clearError();
    
    // 确保目录存在
    std::string dirPath = getDirectoryPath(filePath);
    if (!dirPath.empty() && dirPath != ".") {
        createDirectory(dirPath);
    }
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        setError("无法创建文件: " + filePath);
        return false;
    }
    
    if (!data.empty()) {
        file.write(data.data(), data.size());
        if (!file.good()) {
            setError("写入文件失败: " + filePath);
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

bool FileManager::copyFile(const std::string& srcPath, const std::string& dstPath) {
    clearError();
    
    if (!fileExists(srcPath)) {
        setError("源文件不存在: " + srcPath);
        return false;
    }
    
    std::vector<char> data;
    if (!readBinaryFile(srcPath, data)) {
        return false;  // 错误信息已设置
    }
    
    if (!writeBinaryFile(dstPath, data)) {
        return false;  // 错误信息已设置
    }
    
    return true;
}

bool FileManager::moveFile(const std::string& srcPath, const std::string& dstPath) {
    clearError();
    
    if (!fileExists(srcPath)) {
        setError("源文件不存在: " + srcPath);
        return false;
    }
    
    // 先尝试重命名（同文件系统内移动，更快）
#ifdef _WIN32
    if (MoveFileA(srcPath.c_str(), dstPath.c_str()) != 0) {
        return true;
    }
#else
    if (rename(srcPath.c_str(), dstPath.c_str()) == 0) {
        return true;
    }
#endif
    
    // 如果重命名失败，尝试复制+删除
    if (!copyFile(srcPath, dstPath)) {
        return false;
    }
    
    if (!deleteFile(srcPath)) {
        setError("移动文件失败: 无法删除源文件");
        return false;
    }
    
    return true;
}

std::string FileManager::getLastError() {
    return m_lastError;
}

void FileManager::setError(const std::string& error) {
    m_lastError = error;
}

void FileManager::clearError() {
    m_lastError = "";
}

