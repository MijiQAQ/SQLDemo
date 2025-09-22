cmake_minimum_required(VERSION 3.18)
project(SqliteKit VERSION 1.0 LANGUAGES CXX)

# 选项：是否构建为共享库
option(SQLITEKIT_BUILD_SHARED "Build SqliteKit as shared library" OFF)
option(SQLITEKIT_BUILD_TESTS "Build SqliteKit tests" OFF)
option(SQLITEKIT_BUILD_DOCS "Build SqliteKit documentation" OFF)

# 设置为静态或动态库
if (SQLITEKIT_BUILD_SHARED)
    set(SQLITEKIT_LIB_TYPE SHARED)
else()
    set(SQLITEKIT_LIB_TYPE STATIC)
endif()

# 设置 Qt 自动处理（UIC/MOC/资源）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui)  # 设置UI文件搜索路径
# 查找 Qt6 Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# 添加库目标
add_library(SqliteKit ${SQLITEKIT_LIB_TYPE}
    resources/ui/SqliteKit.ui)

# 设置包含目录
target_include_directories(SqliteKit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接库
target_link_libraries(SqliteKit PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

# 添加核心模块源文件
target_sources(SqliteKit PRIVATE
    src/SqliteKit.h src/SqliteKit.cpp
    # include/tablemodel.h src/tablemodel.cpp
    # include/treemodel.h src/treemodel.cpp
    # include/tableview.h src/tableview.cpp
    # include/treeview.h src/treeview.cpp
    resources/ui/SqliteKit.ui
)

# 添加资源文件
target_sources(SqliteKit PRIVATE
    resources/SqliteKit.qrc
)


# 安装规则
install(TARGETS SqliteKit
    EXPORT SqliteKitTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT SqliteKitTargets
    FILE SqliteKitTargets.cmake
    NAMESPACE SqliteKit::
    DESTINATION lib/cmake/SqliteKit
)

