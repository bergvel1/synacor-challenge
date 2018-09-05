include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runBasicSynacorTests
        basic_check.cpp
        mem_check.cpp)

target_link_libraries(runBasicSynacorTests gtest gtest_main)
target_link_libraries(runBasicSynacorTests synacor)