cmake_minimum_required(VERSION 3.10)
project(referee_interface)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-std=c++20)

set(PACKAGES ament_cmake rclcpp serial)

aux_source_directory(src/package PROJECT_SOURCE)

# find package
foreach(package ${PACKAGES})
  find_package(${package} REQUIRED)
  if(${package}_FOUND)
    message("found package: ${package}")
  endif()
endforeach()

include_directories(${PACKAGES}_INCLUDE_DIR src)

# executable for test
add_executable(${PROJECT_NAME}_test ${PROJECT_SOURCE} src/main.cpp)
ament_target_dependencies(${PROJECT_NAME}_test ${PACKAGES})
target_link_libraries(${PROJECT_NAME}_test ${CUSTOM_MESSAGE})
install(TARGETS ${PROJECT_NAME}_test DESTINATION lib/${PROJECT_NAME})

# generate ament index for other package
ament_package()
