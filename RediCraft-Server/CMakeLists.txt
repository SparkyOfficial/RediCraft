cmake_minimum_required(VERSION 3.10)
project(RediCraftServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Check if ASIO is installed system-wide
find_path(ASIO_INCLUDE_DIR asio.hpp)
if(ASIO_INCLUDE_DIR)
    include_directories(${ASIO_INCLUDE_DIR})
else()
    # If not found, we'll use the standalone version
    # Download and extract standalone ASIO
    include(FetchContent)
    FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-28-0
    )
    FetchContent_MakeAvailable(asio)
    include_directories(${asio_SOURCE_DIR}/asio/include)
endif()

# Source files for main server
set(SERVER_SOURCES 
    src/main.cpp
    src/server.cpp
    src/storage.cpp
    src/parser.cpp
    src/session.cpp
    src/config.cpp
    src/persistence.cpp
    src/replication.cpp
    src/cluster.cpp
)

# Source files for benchmark
set(BENCHMARK_SOURCES
    src/benchmark.cpp
    src/storage.cpp
    src/parser.cpp
)

# Create executable for main server
add_executable(${PROJECT_NAME} ${SERVER_SOURCES})

# Create executable for benchmark
add_executable(benchmark ${BENCHMARK_SOURCES})

# Link libraries for main server
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Link libraries for benchmark
target_link_libraries(benchmark PRIVATE Threads::Threads)

# Compiler options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_options(benchmark PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(benchmark PRIVATE -Wall -Wextra -pedantic)
endif()

# Define ASIO_STANDALONE to use standalone ASIO
target_compile_definitions(${PROJECT_NAME} PRIVATE ASIO_STANDALONE)
target_compile_definitions(benchmark PRIVATE ASIO_STANDALONE)