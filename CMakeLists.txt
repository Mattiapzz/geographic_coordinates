############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                    #
#  Copyright (C) 2019                                                      #
#      Enrico Bertolazzi  & Francesco Biral & Mattia Piazza                #
#      Dipartimento di Ingegneria Industriale                              #
#      Universita` degli Studi di Trento                                   #
#      <mailto:enrico.bertolazzi@unitn.it>                                 #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.5)

set( TARGET GeoConversion )
project( ${TARGET} C CXX )

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
set( CMAKE_VERBOSE_MAKEfile            FALSE )
set( CMAKE_BUILD_TYPE                  Release )

set( CMAKE_CXX_STANDARD 11 )

set( CXXFLAGS "-O3 -DNDEBUG" )
set( LFLAGS "" )
set( LIBS "" )
set( INCLUDE "" )



set( CMAKE_FLAGS            "${CXXFLAGS} ${INCLUDE}" )
set( CMAKE_LINKER           ${LFLAGS} ${LIBS} )
set( CMAKE_CXX_FLAGS_DEBUG  ${CMAKE_FLAGS}  )
set( CMAKE_CXX_FLAGS        ${CMAKE_FLAGS}  )

include_directories(
  src
)

#
#   _____  _____ ___ _   _ _____ _   ___ _    ___
#  | __\ \/ / __/ __| | | |_   _/_\ | _ ) |  | __|
#  | _| >  <| _| (__| |_| | | |/ _ \| _ \ |__| _|
#  |___/_/\_\___\___|\___/  |_/_/ \_\___/____|___|
#

set( SOURCES )
file( GLOB S src/*.c*  )
foreach(F ${S})
  file( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  list( APPEND SOURCES ${RF} )
endforeach()

set( HEADERS )
file( GLOB S src/*.h*  )
foreach(F ${S})
  file( RELATIVE_PATH RF ${CMAKE_CURRENT_SOURCE_DIR} "${F}" )
  list( APPEND HEADERS ${RF} )
endforeach()


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#SET( EXECUTABLE VirtualCoach VirtualCoachReader Receive )
set( EXECUTABLE test_1  )
foreach( EXE ${EXECUTABLE} )
  add_executable( ${EXE}  test/${EXE}.cc ${SOURCES} ) # non uso tutti sources
endforeach()

message( STATUS "Using ${SSE_FLAGS} extensions")
message( STATUS "C compiler                  = ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER}" )
message( STATUS "C++ compiler                = ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER}" )
message( STATUS "CMAKE_CXX_FLAGS_RELEASE     = ${CMAKE_CXX_FLAGS_RELEASE}" )
message( STATUS "CMAKE_CXX_FLAGS             = ${CMAKE_CXX_FLAGS}" )
message( STATUS "CMAKE_C_FLAGS_RELEASE       = ${CMAKE_C_FLAGS_RELEASE}" )
message( STATUS "CMAKE_C_FLAGS               = ${CMAKE_C_FLAGS}" )
message( STATUS "CMAKE_SYSTEM_NAME           = ${CMAKE_SYSTEM_NAME}" )
message( STATUS "CMAKE_SYSTEM_PROCESSOR      = ${CMAKE_SYSTEM_PROCESSOR}" )
if(APPLE)
  message( STATUS "CMAKE_OSX_SYSROOT           = ${CMAKE_OSX_SYSROOT}" )
  message( STATUS "CMAKE_OSX_ARCHITECTURES     = ${CMAKE_OSX_ARCHITECTURES}" )
  message( STATUS "CMAKE_OSX_DEPLOYMENT_TARGET = ${CMAKE_OSX_DEPLOYMENT_TARGET}" )
endif()

# Define the GeoConversion library
add_library(GeoConversion ${SOURCES})

# set the public headers
set_target_properties(GeoConversion PROPERTIES PUBLIC "${HEADERS}")

# install GeoConversion library in lib directory
install(
  TARGETS GeoConversion
  ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib
  LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib
  RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# install the public headers
install(
  FILES ${HEADERS}
  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)