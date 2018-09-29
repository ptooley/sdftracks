
set (BLOCKLISTSOURCES
  blockmain
)

set (SDFTRACKSOURCES
  main
  filter
  particle
  configuration
)

add_executable(blocklist ${BLOCKLISTSOURCES})
set_target_properties(blocklist PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(blocklist ${EXTRA_LIBS})

add_executable(sdftracks ${SDFTRACKSOURCES})
set_target_properties(sdftracks PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(sdftracks ${EXTRA_LIBS})
