enable_testing()

add_executable(
  codegen_test
  codegen_test.cpp

  ../../src/Lexer.cpp 
  ../../src/CType.cpp 
  ../../src/Diagnostics.cpp
  ../../src/Parser.cpp 
  ../../src/Sema.cpp 
  ../../src/Scope.cpp
  ../../src/CodeGen.cpp
)

llvm_map_components_to_libnames(llvm_all Support Core ExecutionEngine MC MCJIT OrcJit native)

#message(STATUS "iiicp: ${llvm_all}")

target_link_libraries(
  codegen_test
  GTest::gtest_main
  ${llvm_all}
)

include(GoogleTest)
gtest_discover_tests(codegen_test)