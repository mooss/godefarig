#from https://eb2.co/blog/2015/06/driving-boost-dot-test-with-cmake/
#todo: find how to make functions with an optionnal number of arguments

function(add_boost_test SOURCE_FILE_NAME OTHER_SOURCES)
  get_filename_component(TEST_EXECUTABLE_NAME ${SOURCE_FILE_NAME} NAME_WE)
  add_executable(${TEST_EXECUTABLE_NAME} ${SOURCE_FILE_NAME} ${OTHER_SOURCES})
  target_link_libraries(${TEST_EXECUTABLE_NAME}
#    ${DEPENDENCY_LIB}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

#parse test file
  file(READ "${SOURCE_FILE_NAME}" SOURCE_FILE_CONTENTS)
  string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\( *([A-Za-z_0-9]+) *\\)" 
    FOUND_TESTS ${SOURCE_FILE_CONTENTS})

  
  foreach(HIT ${FOUND_TESTS})
    #remove useless characters surrounding the name
    string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+) *\\).*" "\\1" TEST_NAME ${HIT})
    #register the test
    add_test(NAME "${TEST_EXECUTABLE_NAME}.${TEST_NAME}" 
      COMMAND ${TEST_EXECUTABLE_NAME}
      --run_test=${TEST_NAME} --catch_system_error=yes)
  endforeach()
  
endfunction()
