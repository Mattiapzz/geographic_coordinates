add_executable(test_geographic_coordinates test_geodetic_transformations.cc)

target_link_libraries(
  test_geographic_coordinates
  PRIVATE
    geographic_coordinates::geographic_coordinates
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(test_geographic_coordinates)
