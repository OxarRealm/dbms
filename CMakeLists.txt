cmake_minimum_required(VERSION 3.10)
project(DBMS VERSION 0.6.1 LANGUAGES CXX)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows特定：强制使用UTF-8编码
if(WIN32 AND MSVC)
    add_compile_options(/utf-8)
endif()

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Qt配置
# 使用D盘的Qt 6.10.1安装路径
set(CMAKE_PREFIX_PATH "D:/Qt/6.10.1/msvc2022_64" CACHE PATH "Qt installation path")
# 如果使用其他路径，请修改为实际路径，例如：
# set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64" CACHE PATH "Qt installation path")
# 或者使用环境变量: set(CMAKE_PREFIX_PATH $ENV{QT_DIR})

# 查找Qt包
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
# 如果使用Qt5，改为: find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# 启用Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ddl
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dml
    ${CMAKE_CURRENT_SOURCE_DIR}/include/query
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sql_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/index
)

# 源文件（后续根据实际文件添加）
file(GLOB_RECURSE CORE_SOURCES 
    "src/core/*.cpp"
    "src/index/*.cpp"
)

# 显式添加B+树索引和索引管理器（确保被包含）
list(APPEND CORE_SOURCES "src/core/btree_index.cpp")
list(APPEND CORE_SOURCES "src/core/index_manager.cpp")
list(APPEND CORE_SOURCES "src/core/index_storage.cpp")

# 显式添加约束管理器（确保被包含）
list(APPEND CORE_SOURCES "src/core/constraint_manager.cpp")
list(APPEND CORE_SOURCES "src/core/constraint_registry.cpp")
list(APPEND CORE_SOURCES "src/core/constraint_storage.cpp")

file(GLOB_RECURSE DDL_SOURCES 
    "src/ddl/*.cpp"
)

file(GLOB_RECURSE DML_SOURCES 
    "src/dml/*.cpp"
)

# 显式添加INSERT和UPDATE约束处理（确保被包含）
list(APPEND DML_SOURCES "src/dml/insert_handler_constraints.cpp")
list(APPEND DML_SOURCES "src/dml/update_handler_constraints.cpp")

file(GLOB_RECURSE QUERY_SOURCES 
    "src/query/*.cpp"
)

file(GLOB_RECURSE SQL_PARSER_SOURCES 
    "src/sql_parser/*.cpp"
)

# 显式添加约束解析器（确保被包含）
list(APPEND SQL_PARSER_SOURCES "src/sql_parser/parser_constraints.cpp")
# 显式添加权限相关语句解析器（确保被包含）
list(APPEND SQL_PARSER_SOURCES "src/sql_parser/parser_user_permission.cpp")

file(GLOB_RECURSE AI_SOURCES 
    "src/ai/*.cpp"
)

file(GLOB_RECURSE GUI_SOURCES 
    "src/gui/*.cpp"
    "src/gui/*.ui"
)

# 显式添加需要 MOC 处理的头文件
set(GUI_HEADERS
    "include/gui/main_window.h"
    "include/gui/table_management_widget.h"
    "include/gui/index_management_widget.h"
    "include/gui/data_operation_widget.h"
    "include/gui/sql_query_widget.h"
    "include/gui/user_management_widget.h"
    "include/gui/login_dialog.h"
    "include/gui/user_selection_dialog.h"
    "include/gui/role_management_dialog.h"
    "include/gui/permission_management_dialog.h"
)

# 所有源文件
set(SOURCES
    ${CORE_SOURCES}
    ${DDL_SOURCES}
    ${DML_SOURCES}
    ${QUERY_SOURCES}
    ${SQL_PARSER_SOURCES}
    ${AI_SOURCES}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)

# 测试可执行文件（不需要Qt）
# 已注释：临时测试文件，不需要在主程序编译时构建
# add_executable(test_index_integration
#     scripts/module_tests/test_index_integration.cpp
#     ${CORE_SOURCES}
#     ${DDL_SOURCES}
#     ${DML_SOURCES}
#     ${QUERY_SOURCES}
#     ${SQL_PARSER_SOURCES}
#     ${INDEX_SOURCES}
# )

# 测试程序需要链接Qt Core（因为包含了user_manager等使用Qt的源文件）
# target_link_libraries(test_index_integration
#     Qt6::Core
# )
# target_compile_definitions(test_index_integration PRIVATE
#     -D_CRT_SECURE_NO_WARNINGS
# )

# 数据库生成工具（不需要Qt）
# 已注释：临时工具文件，不需要在主程序编译时构建
# add_executable(create_student_grade_database
#     scripts/utils/create_student_grade_database.cpp
#     ${CORE_SOURCES}
#     ${DDL_SOURCES}
#     ${DML_SOURCES}
#     ${QUERY_SOURCES}
#     ${SQL_PARSER_SOURCES}
# )

# 数据库生成工具需要链接Qt Core（因为包含了user_manager等使用Qt的源文件）
# target_link_libraries(create_student_grade_database
#     Qt6::Core
# )
# target_compile_definitions(create_student_grade_database PRIVATE
#     -D_CRT_SECURE_NO_WARNINGS
# )

# 外键约束添加工具（不需要Qt）
# 已注释：临时工具文件，不需要在主程序编译时构建
# add_executable(add_foreign_keys
#     scripts/utils/add_foreign_keys.cpp
#     ${CORE_SOURCES}
#     ${DDL_SOURCES}
#     ${SQL_PARSER_SOURCES}
# )

# 外键约束添加工具需要链接Qt Core（因为包含了user_manager等使用Qt的源文件）
# target_link_libraries(add_foreign_keys
#     Qt6::Core
# )
# target_compile_definitions(add_foreign_keys PRIVATE
#     -D_CRT_SECURE_NO_WARNINGS
# )

# 如果是Qt5，改为:
# target_link_libraries(${PROJECT_NAME}
#     Qt5::Core
#     Qt5::Widgets
# )

# Windows特定设置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # 自动部署Qt DLL（使用windeployqt）
    # 查找windeployqt工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS
        "${Qt6_DIR}/../../../bin"
        "${CMAKE_PREFIX_PATH}/bin"
    )
    
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Deploying Qt libraries..."
        )
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
    else()
        message(WARNING "windeployqt not found. Qt DLLs need to be manually copied to the executable directory.")
        message(WARNING "Qt bin directory: D:/Qt/6.10.1/msvc2022_64/bin")
    endif()
endif()

# 调试信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt version: ${Qt6_VERSION}")
