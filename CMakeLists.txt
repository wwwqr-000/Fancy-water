cmake_minimum_required(VERSION 3.27)
project(fancy-water)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent) # Include the FetchContent module

# Add Bardrix as a subdirectory
FetchContent_Declare(
        Bardrix
        GIT_REPOSITORY https://github.com/bardobard/Bardrix.git
        GIT_TAG v0.2.3
)

FetchContent_MakeAvailable(Bardrix)

# Add your project files
add_executable(${PROJECT_NAME} fancy-water/main.cpp fancy-water/sphere.cpp fancy-water/window.cpp)

# Add bardrix library
target_link_libraries(${PROJECT_NAME} Bardrix)