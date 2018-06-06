# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.
#if( NOT LLVM_REQUIRES_RTTI )
#  if( NOT LLVM_REQUIRES_EH )
#    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Hello.exports)
#  endif()
#endif()

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_loadable_module( llvm-vsa
  src/vsa.cpp
  src/BoundedSet.cpp
  test/BoundedSetTest.cpp
  test/TestRunner.cpp
  src/vsa_visitor.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
