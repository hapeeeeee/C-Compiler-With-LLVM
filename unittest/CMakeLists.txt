include(FetchContent)
FetchContent_Declare(
  googletest
  URL file://$ENV{CMAKE_GEOGLE_TEST_PATH}
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(codegen)