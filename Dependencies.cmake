include(FetchContent)

# Catch2 v3.5.4 - test framework used by test/geographic_coordinates.
if(GEOGRAPHIC_COORDINATES_BUILD_TESTS)
  find_package(Catch2 3 QUIET)
  if(NOT Catch2_FOUND)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
  endif()
endif()
