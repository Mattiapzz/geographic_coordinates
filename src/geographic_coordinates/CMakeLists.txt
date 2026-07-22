add_library(geographic_coordinates geodetic_transformations.cc)
add_library(geographic_coordinates::geographic_coordinates ALIAS geographic_coordinates)

target_include_directories(
  geographic_coordinates
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(geographic_coordinates PUBLIC cxx_std_20)
target_link_libraries(geographic_coordinates PRIVATE geographic_coordinates_project_warnings geographic_coordinates_sanitizers)

install(
  TARGETS geographic_coordinates
  EXPORT geographic_coordinatesTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/geographic_coordinates
  DESTINATION include
)
