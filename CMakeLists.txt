set(LLVM_LINK_COMPONENTS support)

add_clang_executable(cast-matcher
  CastMatcher.cpp
  )
target_link_libraries(cast-matcher
  clangTooling
  clangBasic
  clangASTMatchers
  )
