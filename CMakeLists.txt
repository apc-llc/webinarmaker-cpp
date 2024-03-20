cmake_minimum_required(VERSION 3.1)

project(webinarmaker-cpp)

add_subdirectory(ThirdParty/moviemaker-cpp)
add_subdirectory(ThirdParty/json)
add_subdirectory(ThirdParty/task_manager_py)

add_executable(mkwebinar "src/webinarmaker.cpp")
target_link_libraries(mkwebinar moviemaker-cpp task_manager nlohmann_json)

