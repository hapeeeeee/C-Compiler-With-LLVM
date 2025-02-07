enable_testing()

add_executable(
  parser_test
  parser_test.cpp

  ../../src/Lexer.cpp 
  ../../src/CType.cpp 
  ../../src/Diagnostics.cpp
  ../../src/Parser.cpp 
  ../../src/PrintVisitor.cpp
  ../../src/Sema.cpp 
  ../../src/Scope.cpp
)

llvm_map_components_to_libnames(llvm_all Support Core)

#message(STATUS "iiicp: ${llvm_all}")

target_link_libraries(
  parser_test
  GTest::gtest_main
  ${llvm_all}
)

include(GoogleTest)
gtest_discover_tests(parser_test)