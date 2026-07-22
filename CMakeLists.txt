############################################################################
#                                                                          #
#  file: CMakeLists.txt                                                   #
#      Enrico Bertolazzi & Francesco Biral & Mattia Piazza                #
#      Dipartimento di Ingegneria Industriale                             #
#      Universita` degli Studi di Trento                                  #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.21)

project(
  geographic_coordinates
  VERSION 1.0.0
  LANGUAGES CXX
)

include(ProjectOptions.cmake)
include(Dependencies.cmake)

install(
  TARGETS geographic_coordinates_project_warnings geographic_coordinates_sanitizers
  EXPORT geographic_coordinatesTargets
)

add_subdirectory(src)

if(GEOGRAPHIC_COORDINATES_BUILD_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()

if(GEOGRAPHIC_COORDINATES_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if(GEOGRAPHIC_COORDINATES_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

install(
  EXPORT geographic_coordinatesTargets
  FILE geographic_coordinatesTargets.cmake
  NAMESPACE geographic_coordinates::
  DESTINATION lib/cmake/geographic_coordinates
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/geographic_coordinatesConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/geographic_coordinatesConfig.cmake
  INSTALL_DESTINATION lib/cmake/geographic_coordinates
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/geographic_coordinatesConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/geographic_coordinatesConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/geographic_coordinatesConfigVersion.cmake
  DESTINATION lib/cmake/geographic_coordinates
)
