cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

set(PROJECT "project")

project (${PROJECT})

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

find_package (Threads)

# 指定生成目标
add_executable(${PROJECT} ${DIR_SRCS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_link_libraries (${PROJECT} pthread)
