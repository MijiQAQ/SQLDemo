cmake_minimum_required(VERSION 3.19)
project(SQLDemo LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)              # 查找Qt6的Core、Widgets和Sql组件

qt_standard_project_setup()

add_subdirectory(SqliteKit)                                              # 添加子目录，包含SqliteKit

qt_add_executable(SQLDemo
    WIN32 MACOSX_BUNDLE
    main.cpp
    SQLDemo.cpp
    SQLDemo.h
    SQLDemo.ui
)

target_link_libraries(SQLDemo
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql 
        SqliteKit                                                                   # 链接SqliteKit组件库

)

include(GNUInstallDirs)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE                        # 指定头文件搜索路径
    ${PROJECT_SOURCE_DIR}/SqliteKit
)
install(TARGETS SQLDemo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SQLDemo
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# 自定义的终止目标方法(通过自定义的kill目标)
add_custom_target(kill COMMAND tasklist /FI "IMAGENAME eq ${CMAKE_PROJECT_NAME}.exe" |
    findstr /I "${CMAKE_PROJECT_NAME}.exe" && taskkill /IM ${CMAKE_PROJECT_NAME}.exe /F || exit 0)
add_dependencies(${CMAKE_PROJECT_NAME} kill)
