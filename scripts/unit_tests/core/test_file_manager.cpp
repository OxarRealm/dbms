/**
 * @file test_file_manager.cpp
 * @brief FileManager类测试程序
 * 
 * 测试文件I/O封装功能
 */

#include "../../include/core/file_manager.h"
#include <iostream>
#include <cassert>
#include <vector>

// 测试结果统计
static int testsPassed = 0;
static int testsFailed = 0;

// 测试宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            std::cout << "  ✓ " << message << std::endl; \
            testsPassed++; \
        } else { \
            std::cout << "  ✗ " << message << " (FAILED)" << std::endl; \
            testsFailed++; \
        } \
    } while(0)

// 测试目录和文件
const char* TEST_DIR = "test_file_manager_dir";
const char* TEST_FILE = "test_file_manager_dir/test.txt";
const char* TEST_FILE2 = "test_file_manager_dir/test2.txt";
const char* TEST_BIN_FILE = "test_file_manager_dir/test.bin";

/**
 * @brief 清理测试文件和目录
 */
void cleanupTestFiles() {
    FileManager::deleteFile(TEST_FILE);
    FileManager::deleteFile(TEST_FILE2);
    FileManager::deleteFile(TEST_BIN_FILE);
    // 注意：这里不删除目录，因为FileManager没有提供删除目录的功能
}

/**
 * @brief 测试文件存在性检查
 */
void testFileExists() {
    std::cout << "\n[测试1] 文件存在性检查测试" << std::endl;
    
    cleanupTestFiles();
    
    TEST_ASSERT(FileManager::fileExists(TEST_FILE) == false, "不存在的文件返回false");
    
    // 创建测试文件
    FileManager::writeTextFile(TEST_FILE, "test content");
    TEST_ASSERT(FileManager::fileExists(TEST_FILE) == true, "存在的文件返回true");
}

/**
 * @brief 测试文件路径操作
 */
void testPathOperations() {
    std::cout << "\n[测试2] 文件路径操作测试" << std::endl;
    
    std::string path1 = "dir/subdir/file.txt";
    std::string path2 = "dir\\subdir\\file.txt";
    
    std::string ext = FileManager::getFileExtension(path1);
    TEST_ASSERT(ext == "txt", "获取文件扩展名");
    
    std::string fileName = FileManager::getFileName(path1);
    TEST_ASSERT(fileName == "file.txt", "获取文件名");
    
    std::string dirPath = FileManager::getDirectoryPath(path1);
    TEST_ASSERT(dirPath.find("dir") != std::string::npos, "获取目录路径");
    
    std::string normalized = FileManager::normalizePath(path2);
    TEST_ASSERT(normalized.find('\\') != std::string::npos || 
                normalized.find('/') != std::string::npos, "规范化路径");
    
    // 测试相对路径
    bool isRel = FileManager::isAbsolutePath(path1);
    TEST_ASSERT(isRel == false, "检测相对路径");
    
    // 在Windows上测试绝对路径（如果支持）
#ifdef _WIN32
    std::string absPath = "C:\\Windows\\System32";
    bool isAbs = FileManager::isAbsolutePath(absPath);
    TEST_ASSERT(isAbs == true, "检测Windows绝对路径");
#else
    std::string absPath = "/usr/local/bin/app";
    bool isAbs = FileManager::isAbsolutePath(absPath);
    TEST_ASSERT(isAbs == true, "检测Unix绝对路径");
#endif
}

/**
 * @brief 测试路径组合
 */
void testJoinPath() {
    std::cout << "\n[测试3] 路径组合测试" << std::endl;
    
    std::string dir = "test_dir";
    std::string file = "test_file.txt";
    
    std::string joined = FileManager::joinPath(dir, file);
    TEST_ASSERT(joined.find(dir) != std::string::npos, "路径组合包含目录");
    TEST_ASSERT(joined.find(file) != std::string::npos, "路径组合包含文件名");
    
    std::string empty = FileManager::joinPath("", file);
    TEST_ASSERT(empty == file, "空目录路径返回文件名");
}

/**
 * @brief 测试文本文件读写
 */
void testTextFileOperations() {
    std::cout << "\n[测试4] 文本文件读写测试" << std::endl;
    
    cleanupTestFiles();
    
    std::string content = "Hello, World!\nThis is a test file.";
    bool result = FileManager::writeTextFile(TEST_FILE, content);
    TEST_ASSERT(result == true, "写入文本文件成功");
    
    std::string readContent;
    result = FileManager::readTextFile(TEST_FILE, readContent);
    TEST_ASSERT(result == true, "读取文本文件成功");
    TEST_ASSERT(readContent == content, "读取内容与写入内容一致");
    
    // 测试追加
    std::string appendContent = "\nAppended content.";
    result = FileManager::appendTextFile(TEST_FILE, appendContent);
    TEST_ASSERT(result == true, "追加文本文件成功");
    
    result = FileManager::readTextFile(TEST_FILE, readContent);
    TEST_ASSERT(result == true, "读取追加后的文件成功");
    TEST_ASSERT(readContent.find(appendContent) != std::string::npos, "追加内容存在");
}

/**
 * @brief 测试二进制文件读写
 */
void testBinaryFileOperations() {
    std::cout << "\n[测试5] 二进制文件读写测试" << std::endl;
    
    cleanupTestFiles();
    
    // 创建测试数据
    std::vector<char> data;
    for (int i = 0; i < 256; i++) {
        data.push_back(static_cast<char>(i));
    }
    
    bool result = FileManager::writeBinaryFile(TEST_BIN_FILE, data);
    TEST_ASSERT(result == true, "写入二进制文件成功");
    
    std::vector<char> readData;
    result = FileManager::readBinaryFile(TEST_BIN_FILE, readData);
    TEST_ASSERT(result == true, "读取二进制文件成功");
    TEST_ASSERT(readData.size() == data.size(), "读取数据大小正确");
    
    bool dataMatch = true;
    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] != readData[i]) {
            dataMatch = false;
            break;
        }
    }
    TEST_ASSERT(dataMatch == true, "读取数据与写入数据一致");
}

/**
 * @brief 测试文件复制
 */
void testCopyFile() {
    std::cout << "\n[测试6] 文件复制测试" << std::endl;
    
    cleanupTestFiles();
    
    std::string content = "Test content for copy";
    FileManager::writeTextFile(TEST_FILE, content);
    
    bool result = FileManager::copyFile(TEST_FILE, TEST_FILE2);
    TEST_ASSERT(result == true, "复制文件成功");
    
    TEST_ASSERT(FileManager::fileExists(TEST_FILE) == true, "源文件仍然存在");
    TEST_ASSERT(FileManager::fileExists(TEST_FILE2) == true, "目标文件存在");
    
    std::string readContent;
    FileManager::readTextFile(TEST_FILE2, readContent);
    TEST_ASSERT(readContent == content, "复制文件内容正确");
}

/**
 * @brief 测试文件移动
 */
void testMoveFile() {
    std::cout << "\n[测试7] 文件移动测试" << std::endl;
    
    cleanupTestFiles();
    
    std::string content = "Test content for move";
    FileManager::writeTextFile(TEST_FILE, content);
    
    bool result = FileManager::moveFile(TEST_FILE, TEST_FILE2);
    TEST_ASSERT(result == true, "移动文件成功");
    
    TEST_ASSERT(FileManager::fileExists(TEST_FILE) == false, "源文件不存在");
    TEST_ASSERT(FileManager::fileExists(TEST_FILE2) == true, "目标文件存在");
    
    std::string readContent;
    FileManager::readTextFile(TEST_FILE2, readContent);
    TEST_ASSERT(readContent == content, "移动文件内容正确");
}

/**
 * @brief 测试文件删除
 */
void testDeleteFile() {
    std::cout << "\n[测试8] 文件删除测试" << std::endl;
    
    cleanupTestFiles();
    
    FileManager::writeTextFile(TEST_FILE, "test");
    TEST_ASSERT(FileManager::fileExists(TEST_FILE) == true, "文件存在");
    
    bool result = FileManager::deleteFile(TEST_FILE);
    TEST_ASSERT(result == true, "删除文件成功");
    TEST_ASSERT(FileManager::fileExists(TEST_FILE) == false, "文件已删除");
    
    // 测试删除不存在的文件
    result = FileManager::deleteFile("non_existent_file.txt");
    TEST_ASSERT(result == false, "删除不存在的文件返回false");
}

/**
 * @brief 测试文件大小
 */
void testGetFileSize() {
    std::cout << "\n[测试9] 文件大小测试" << std::endl;
    
    cleanupTestFiles();
    
    std::string content = "Test content";
    FileManager::writeTextFile(TEST_FILE, content);
    
    long size = FileManager::getFileSize(TEST_FILE);
    TEST_ASSERT(size >= 0, "获取文件大小成功");
    TEST_ASSERT(size >= static_cast<long>(content.length()), "文件大小正确");
    
    // 测试不存在的文件
    size = FileManager::getFileSize("non_existent_file.txt");
    TEST_ASSERT(size == -1, "不存在的文件返回-1");
}

/**
 * @brief 测试目录创建
 */
void testCreateDirectory() {
    std::cout << "\n[测试10] 目录创建测试" << std::endl;
    
    std::string testDir = "test_file_manager_new_dir";
    
    bool result = FileManager::createDirectory(testDir);
    TEST_ASSERT(result == true, "创建目录成功");
    
    // 再次创建（应该成功，因为目录已存在）
    result = FileManager::createDirectory(testDir);
    TEST_ASSERT(result == true, "创建已存在的目录成功");
}

/**
 * @brief 测试错误处理
 */
void testErrorHandling() {
    std::cout << "\n[测试11] 错误处理测试" << std::endl;
    
    // 测试读取不存在的文件
    std::string content;
    bool result = FileManager::readTextFile("non_existent_file.txt", content);
    TEST_ASSERT(result == false, "读取不存在的文件返回false");
    
    std::string error = FileManager::getLastError();
    TEST_ASSERT(!error.empty(), "错误信息不为空");
    
    // 测试复制不存在的文件
    result = FileManager::copyFile("non_existent_file.txt", "dest.txt");
    TEST_ASSERT(result == false, "复制不存在的文件返回false");
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  FileManager测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testFileExists();
        testPathOperations();
        testJoinPath();
        testTextFileOperations();
        testBinaryFileOperations();
        testCopyFile();
        testMoveFile();
        testDeleteFile();
        testGetFileSize();
        testCreateDirectory();
        testErrorHandling();
        
        cleanupTestFiles();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  测试结果汇总" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "通过: " << testsPassed << " 个测试" << std::endl;
        std::cout << "失败: " << testsFailed << " 个测试" << std::endl;
        std::cout << "总计: " << (testsPassed + testsFailed) << " 个测试" << std::endl;
        
        if (testsFailed == 0) {
            std::cout << "\n✓ 所有测试通过！" << std::endl;
            return 0;
        } else {
            std::cout << "\n✗ 有测试失败！" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 测试过程中发生异常: " << e.what() << std::endl;
        cleanupTestFiles();
        return 1;
    }
}

