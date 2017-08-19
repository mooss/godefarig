cmake_minimum_required(VERSION 2.8)

project(godefarig)

#include(ExternalProject)
include(build_external_project.cmake)

# set(EXTERNAL_DIR "${CMAKE_BINARY_DIR}/third_party")
# SET_DIRECTORY_PROPERTIES(PROPERTIES EP_BASE ${EXTERNAL_DIR})

#set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++11 -march=native -ldl")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Og ")

include_directories(include)

add_executable(
  gltest
  src/gltest.cpp
  src/display.cpp
  src/camera.cpp

  src/fractahedron.cpp
  src/gfg_utils.cpp
  src/glad.c
  src/slice.cpp
  src/node.cpp
  src/mesh_generation.cpp
  src/container_overload.cpp

  src/gl_toggle.cpp

  src/input/glfw_interaction.cpp
  src/input/input_manager.cpp
  src/input/controllers.cpp
  src/input/comparison_component.cpp

  src/graphics/buffer.cpp
  src/graphics/shader.cpp
  src/graphics/transformations.cpp
  src/graphics/drawable.cpp
#  src/graphics/shading_unit.cpp
  src/graphics/uniform.cpp
  )

add_executable(
  terrain_test
  src/terrain_test.cpp
  src/terrain/hexagonal_iteration.cpp
  src/slice.cpp
  src/node.cpp
  src/mesh_generation.cpp
  src/fractahedron.cpp
  src/gfg_utils.cpp
  )

#glfw
find_package(glfw3 3.2)
if(glfw3_FOUND)
  message("glfw was found")
else(glfw3_FOUND)
  message("glfw was not found and will be downloaded")

  download_external_project( glfw https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.zip)
  set(glfw_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/glfw_download_dir)

  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory( ${glfw_DOWNLOAD_DIR})
  
endif(glfw3_FOUND)

#glm
find_path(glm_FOUND glm)
if(glm_FOUND)
  message("glm was found here :" ${glm_FOUND})
  include_directories(${glm_FOUND})
else(glm_FOUND)
  message("glm was not found and will be downloaded")

  download_external_project(glm https://github.com/g-truc/glm/releases/download/0.9.8.4/glm-0.9.8.4.zip)
  set(glm_INCLUDE_DIR ${CMAKE_BINARY_DIR}/glm_download_dir)
  message("glm include dir: " ${glm_INCLUDE_DIR})
  include_directories(${glm_INCLUDE_DIR})
endif(glm_FOUND)

target_link_libraries(gltest glfw)

#boost
set(NEEDED_BOOST_LIBS program_options unit_test_framework)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS ${NEEDED_BOOST_LIBS})

if(Boost_FOUND)
  message("boost was found")
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(gltest ${Boost_LIBRARIES})
  target_link_libraries(terrain_test ${Boost_LIBRARIES})
  #target_link_libraries(slicetest ${Boost_LIBRARIES})
  message("boost location " ${Boost_LIBRARIES})
else(Boost_FOUND)
  # message(FATAL_ERROR "boost was not found")
  message("boost was not found. downloading needed libraries")
  download_external_project(boost_needed   https://www.dropbox.com/s/j9thr57njgzpiez/boost_needed.zip)
  set(boost_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/boost_needed_download_dir)
  message( "about to execute " ${boost_DOWNLOAD_DIR}/build_needed_libraries.sh " " ${CMAKE_BINARY_DIR} )
  execute_process(COMMAND ${boost_DOWNLOAD_DIR}/build_needed_libraries.sh ${CMAKE_BINARY_DIR}
                  WORKING_DIRECTORY ${boost_DOWNLOAD_DIR})
#                  OUTPUT_VARIABLE output)
  message( "script said ${output}")
  include_directories( ${CMAKE_BINARY_DIR}/include )
  #link_directories(${CMAKE_SOURCE_DIR}/lib)
  set(Boost_UNIT_TEST_FRAMEWORK_LIBRARY ${CMAKE_BINARY_DIR}/lib/libboost_unit_test_framework.so)
  set(LOCAL_PO_LIB ${CMAKE_BINARY_DIR}/lib/libboost_program_options.a)
  target_link_libraries(gltest ${LOCAL_PO_LIB})
  target_link_libraries(terrain_test ${LOCAL_PO_LIB})
  #target_link_libraries(slicetest ${LOCAL_PO_LIB})
endif(Boost_FOUND)

enable_testing()
add_subdirectory(tests)

file(INSTALL res/ DESTINATION res)
file(INSTALL full_valgrind DESTINATION .)
install(TARGETS gltest DESTINATION .)
install(DIRECTORY res/ DESTINATION res)
