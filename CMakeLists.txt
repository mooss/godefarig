cmake_minimum_required(VERSION 2.8)

project(godefarig)


#set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Ofast -std=c++11 -ldl")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Og ")

include_directories(include)

add_executable(
  gltest
  src/gltest.cpp
  src/display.cpp
  src/shader.cpp
  src/transformatrix.cpp
  src/camera.cpp

  src/fractahedron.cpp
  src/gfg_utils.cpp
  src/glad.c
  src/slice.cpp
  src/node.cpp
  src/mesh_generation.cpp
  src/drawable.cpp

  src/glfw_interaction.cpp
  src/input_manager.cpp
  src/controllers.cpp
  src/gl_toggle.cpp
  src/comparison_component.cpp

  src/graphics/buffer.cpp
  )

# add_executable(
#   slicetest
#   src/slicetest.cpp
#   src/slice.cpp
#   src/gfg_utils.cpp
#   src/fractahedron.cpp
#   src/node.cpp
#   src/mesh_generation.cpp )

#add_executable(cubetest cubetest.cpp display.cpp shader.cpp glObjects.cpp transformatrix.cpp camera.cpp interaction.cpp fractahedron.cpp)


#glfw
find_package(glfw3 3.2)
if(glfw3_FOUND)
  message("glfw was found")
else()
  message("glfw was not found and will be compiled from source")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory(glfw-3.2.1)
endif()

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
  #target_link_libraries(slicetest ${Boost_LIBRARIES})
  message("boost location " ${Boost_LIBRARIES})
endif()

enable_testing()
add_subdirectory(tests)

file(INSTALL res/ DESTINATION res/)
