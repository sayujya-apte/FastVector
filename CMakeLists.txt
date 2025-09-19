cmake_minimum_required(VERSION 3.5)
project(FastVector)

set(
    SOURCE_FILES

    src/Main.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCE_FILES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

set(
    INCLUDE_DIRS
    src/
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${INCLUDE_DIRS}
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)