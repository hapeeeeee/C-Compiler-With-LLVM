enable_testing()

add_executable(
    lexer_test
    lexer_test.cpp

    ../../src/Lexer.cpp
    ../../src/CType.cpp
    ../../src/Diagnostics.cpp
)

llvm_map_components_to_libnames(llvm_all support core)

target_link_libraries(
    lexer_test
    GTest::gtest_main
    ${llvm_all}
)

include(GoogleTest)
gtest_discover_tests(lexer_test)