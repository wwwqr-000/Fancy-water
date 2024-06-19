cmake_minimum_required(VERSION 3.27)
project(fancy-water)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent) # Include the FetchContent module

# Add Bardrix as a subdirectory
FetchContent_Declare(
        Bardrix
        GIT_REPOSITORY https://github.com/bardobard/Bardrix.git
        GIT_TAG v0.3.3
)

FetchContent_MakeAvailable(Bardrix)

# Add your project files
add_executable(${PROJECT_NAME} fancy-water/main.cpp fancy-water/sphere.cpp fancy-water/window.cpp fancy-water/cube.cpp)
add_library(${PROJECT_NAME}_lib STATIC fancy-water/sphere.cpp fancy-water/window.cpp fancy-water/cube.cpp)

# Add bardrix library
target_link_libraries(${PROJECT_NAME} PUBLIC Bardrix)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC Bardrix)

target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/fancy-water)

# Add the tests
add_subdirectory(fancy-water-tests) 