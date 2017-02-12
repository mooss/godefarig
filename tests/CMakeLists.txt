#find_package(Boost COMPONENTS unit_test_framework)

include_directories(${godefarig_SOURCE_DIR}/include)
 # ${Boost_INCLUDE_DIR})

include(BoostTestHelpers.cmake)

# add_executable(pression_mask_test pression_mask.cpp)
# target_link_libraries(
#   pression_mask_test
#   ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
#   )
# add_test(NAME pression_mask COMMAND pression_mask_test)
add_boost_test(
  input_test.cpp
  "${godefarig_SOURCE_DIR}/src/input_manager.cpp;${godefarig_SOURCE_DIR}/src/gfg_utils.cpp" )
