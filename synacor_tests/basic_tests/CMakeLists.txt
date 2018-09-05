include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runAllTests
        all_tests.cpp
        basic_check.cpp
        mem_check.cpp
        stack_check.cpp)

target_link_libraries(runAllTests gtest gtest_main)
target_link_libraries(runAllTests synacor)