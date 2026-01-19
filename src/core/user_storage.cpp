/**
 * @file user_storage.cpp
 * @brief 用户权限存储管理器实现
 */

#include "core/user_storage.h"
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

std::string UserStorageManager::getUserFilePath(const std::string& dbName, const std::string& dbPath) {
    // 从dbPath提取目录和文件名
    std::string dirStr;
    std::string fileName;
    
    size_t lastSlash = dbPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirStr = dbPath.substr(0, lastSlash);
        fileName = dbPath.substr(lastSlash + 1);
    } else {
        dirStr = ".";
        fileName = dbPath;
    }
    
    // 移除文件扩展名（如果有）
    size_t lastDot = fileName.find_last_of('.');
    if (lastDot != std::string::npos) {
        fileName = fileName.substr(0, lastDot);
    }
    
    // 如果dbName不为空且与fileName不同，使用dbName（向后兼容）
    if (!dbName.empty() && dbName != dbPath) {
        // 从dbName提取文件名（不含路径和扩展名）
        std::string dbNameFile = dbName;
        size_t dbNameLastSlash = dbNameFile.find_last_of("/\\");
        if (dbNameLastSlash != std::string::npos) {
            dbNameFile = dbNameFile.substr(dbNameLastSlash + 1);
        }
        size_t dbNameLastDot = dbNameFile.find_last_of('.');
        if (dbNameLastDot != std::string::npos) {
            dbNameFile = dbNameFile.substr(0, dbNameLastDot);
        }
        if (!dbNameFile.empty()) {
            fileName = dbNameFile;
        }
    }
    
    // 构建.usr文件路径：与.dbf文件在同一目录，文件名相同但扩展名为.usr
    std::string usrPath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        usrPath += "\\";
        #else
        usrPath += "/";
        #endif
    }
    usrPath += fileName + ".usr";
    return usrPath;
}

std::string UserStorageManager::getRoleFilePath(const std::string& dbName, const std::string& dbPath) {
    // 从dbPath提取目录和文件名
    std::string dirStr;
    std::string fileName;
    
    size_t lastSlash = dbPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirStr = dbPath.substr(0, lastSlash);
        fileName = dbPath.substr(lastSlash + 1);
    } else {
        dirStr = ".";
        fileName = dbPath;
    }
    
    // 移除文件扩展名（如果有）
    size_t lastDot = fileName.find_last_of('.');
    if (lastDot != std::string::npos) {
        fileName = fileName.substr(0, lastDot);
    }
    
    // 如果dbName不为空且与fileName不同，使用dbName（向后兼容）
    if (!dbName.empty() && dbName != dbPath) {
        std::string dbNameFile = dbName;
        size_t dbNameLastSlash = dbNameFile.find_last_of("/\\");
        if (dbNameLastSlash != std::string::npos) {
            dbNameFile = dbNameFile.substr(dbNameLastSlash + 1);
        }
        size_t dbNameLastDot = dbNameFile.find_last_of('.');
        if (dbNameLastDot != std::string::npos) {
            dbNameFile = dbNameFile.substr(0, dbNameLastDot);
        }
        if (!dbNameFile.empty()) {
            fileName = dbNameFile;
        }
    }
    
    std::string rolePath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        rolePath += "\\";
        #else
        rolePath += "/";
        #endif
    }
    rolePath += fileName + ".role";
    return rolePath;
}

std::string UserStorageManager::getPermissionFilePath(const std::string& dbName, const std::string& dbPath) {
    // 从dbPath提取目录和文件名
    std::string dirStr;
    std::string fileName;
    
    size_t lastSlash = dbPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirStr = dbPath.substr(0, lastSlash);
        fileName = dbPath.substr(lastSlash + 1);
    } else {
        dirStr = ".";
        fileName = dbPath;
    }
    
    // 移除文件扩展名（如果有）
    size_t lastDot = fileName.find_last_of('.');
    if (lastDot != std::string::npos) {
        fileName = fileName.substr(0, lastDot);
    }
    
    // 如果dbName不为空且与fileName不同，使用dbName（向后兼容）
    if (!dbName.empty() && dbName != dbPath) {
        std::string dbNameFile = dbName;
        size_t dbNameLastSlash = dbNameFile.find_last_of("/\\");
        if (dbNameLastSlash != std::string::npos) {
            dbNameFile = dbNameFile.substr(dbNameLastSlash + 1);
        }
        size_t dbNameLastDot = dbNameFile.find_last_of('.');
        if (dbNameLastDot != std::string::npos) {
            dbNameFile = dbNameFile.substr(0, dbNameLastDot);
        }
        if (!dbNameFile.empty()) {
            fileName = dbNameFile;
        }
    }
    
    std::string permPath = dirStr;
    if (!dirStr.empty() && dirStr.back() != '/' && dirStr.back() != '\\') {
        #ifdef _WIN32
        permPath += "\\";
        #else
        permPath += "/";
        #endif
    }
    permPath += fileName + ".perm";
    return permPath;
}




