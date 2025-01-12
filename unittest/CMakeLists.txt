include(FetchContent)
FetchContent_Declare(
  googletest
  URL file:///home/zax/project/CC_LLVM/unittest/googletest-03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(lexer)