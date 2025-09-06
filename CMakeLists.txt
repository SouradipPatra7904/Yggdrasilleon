cmake_minimum_required(VERSION 3.16)

# Project name and version
project(Yggdrasilleon VERSION 1.0 LANGUAGES CXX)

# Require modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 (Widgets & Core are enough)
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Graph.cpp
    src/GraphWidget.cpp
    src/Algorithms.cpp
    src/ThemeManager.cpp
)

# Header files (for IDEs)
set(HEADERS
    src/MainWindow.hpp
    src/Graph.hpp
    src/GraphWidget.hpp
    src/Algorithms.hpp
    src/ThemeManager.hpp
)

# Executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link against Qt
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# Enable automoc/uic/rcc (Qt meta object compiler & UI support)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Organize in folders for IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${SOURCES} ${HEADERS})
