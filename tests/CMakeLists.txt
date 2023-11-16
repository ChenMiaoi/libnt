# Allow test use
enable_testing()

# Retrieve all cc files in the tests folder
file(GLOB TEST_SOURCES "*.cc")

# Generate a test set for each cc file. 
# The test name is test_* and the executable file is *_test
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(EXE_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${EXE_NAME} ${TEST_SOURCE})
    target_link_libraries(${EXE_NAME} fd gtest)
    # let *_test to test_*
    string(REPLACE "_test" "" TEST_NAME ${EXE_NAME})
    add_test(NAME test_${TEST_NAME} COMMAND ${EXE_NAME})
    # add_test(NAME ${EXE_NAME} COMMAND ${EXE_NAME})
    message("Added test: test_${TEST_NAME}, Executable: ${EXE_NAME}")
endforeach()
