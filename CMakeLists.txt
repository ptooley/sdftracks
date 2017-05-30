cmake_minimum_required (VERSION 2.6)
project (sdftracks)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING
             "Choose the type of build, options are: None Debug Release"
        FORCE )
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2 -march=native")

include_directories("${PROJECT_SOURCE_DIR}/SDF/include")
include_directories("${PROJECT_SOURCE_DIR}/SDF")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS program_options filesystem) 
include_directories(${Boost_INCLUDE_DIRS})  
set(EXTRA_LIBS ${EXTRA_LIBS} ${Boost_LIBRARIES})

find_package(HDF5 REQUIRED COMPONENTS C HL)
include_directories(${HDF5_INCLUDE_DIRS})  
set(EXTRA_LIBS ${EXTRA_LIBS} ${HDF5_C_LIBRARIES} ${HDF5_HL_LIBRARIES})

add_subdirectory("SDF")
set(EXTRA_LIBS ${EXTRA_LIBS} sdfc)

set(EXTRA_LIBS ${EXTRA_LIBS} dl)

set (BLOCKLISTSOURCES
  blockmain
)

set (SDFTRACKSOURCES
  main
  filter
  particle
  configuration
)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#set(BUILD_SHARED_LIBRARIES OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(blocklist ${BLOCKLISTSOURCES})
set_target_properties(blocklist PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(blocklist ${EXTRA_LIBS})

add_executable(sdftracks ${SDFTRACKSOURCES})
set_target_properties(sdftracks PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(sdftracks ${EXTRA_LIBS})
