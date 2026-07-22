# Always built as a shared library: ctypes loads it at runtime from Python.
add_library(geographic_coordinates_c_api SHARED geographic_coordinates_c_api.cc)
add_library(geographic_coordinates::geographic_coordinates_c_api ALIAS geographic_coordinates_c_api)

target_include_directories(
  geographic_coordinates_c_api
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_features(geographic_coordinates_c_api PUBLIC cxx_std_20)
target_compile_definitions(geographic_coordinates_c_api PRIVATE GEOGRAPHIC_COORDINATES_EXPORTS)
target_link_libraries(geographic_coordinates_c_api PUBLIC geographic_coordinates::geographic_coordinates)
set_target_properties(geographic_coordinates_c_api PROPERTIES CXX_VISIBILITY_PRESET hidden)

install(
  TARGETS geographic_coordinates_c_api
  EXPORT geographic_coordinatesTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(FILES geographic_coordinates.py DESTINATION python/geographic_coordinates)
