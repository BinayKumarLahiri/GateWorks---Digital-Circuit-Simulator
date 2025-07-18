# cmake_minimum_required(VERSION 3.15)
# project(MyRaylibGame)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Collect all .cpp and .hpp files in src directory recursively
# file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
# file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")

# # Add executable
# add_executable(MyRaylibGame ${SOURCES} ${HEADERS})

# # Find Raylib installed via MSYS2 or vcpkg
# find_package(raylib REQUIRED)

# # Include both your src and third-party lib headers
# target_include_directories(MyRaylibGame
#     PRIVATE
#     ${PROJECT_SOURCE_DIR}/src
#     ${PROJECT_SOURCE_DIR}/lib
# )

# # Link raylib (automatically links raymath too)
# target_link_libraries(MyRaylibGame raylib)

# # Optional: Group sources logically in IDE
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${SOURCES} ${HEADERS})



cmake_minimum_required(VERSION 3.15)
project(GateSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Recursively collect all .cpp and .c files from src/
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")

# Add executable
add_executable(GateSimulator ${SOURCES} ${HEADERS})

# Find raylib (installed via vcpkg, system, etc.)
find_package(raylib REQUIRED)

# Include directories for Raylib, Raygui, Tinyfiledialogs
target_include_directories(GateSimulator
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/lib
)

# Link against raylib (includes raymath and other dependencies)
target_link_libraries(GateSimulator raylib)

# Optional: Group sources in IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${SOURCES} ${HEADERS})

