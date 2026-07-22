include(CMakeDependentOption)

option(GEOGRAPHIC_COORDINATES_BUILD_TESTS "Build the Catch2 test suite" ON)
option(GEOGRAPHIC_COORDINATES_BUILD_EXAMPLES "Build the examples" ON)
option(GEOGRAPHIC_COORDINATES_BUILD_PYTHON_BINDINGS "Build the C API + ctypes Python bindings" ON)
option(GEOGRAPHIC_COORDINATES_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(GEOGRAPHIC_COORDINATES_ENABLE_SANITIZERS "Enable address/undefined-behavior sanitizers (Debug only)" OFF)

add_library(geographic_coordinates_project_warnings INTERFACE)

if(MSVC)
  target_compile_options(geographic_coordinates_project_warnings INTERFACE /W4)
  if(GEOGRAPHIC_COORDINATES_WARNINGS_AS_ERRORS)
    target_compile_options(geographic_coordinates_project_warnings INTERFACE /WX)
  endif()
else()
  target_compile_options(geographic_coordinates_project_warnings INTERFACE -Wall -Wextra)
  if(GEOGRAPHIC_COORDINATES_WARNINGS_AS_ERRORS)
    target_compile_options(geographic_coordinates_project_warnings INTERFACE -Werror)
  endif()
endif()

add_library(geographic_coordinates_sanitizers INTERFACE)

if(GEOGRAPHIC_COORDINATES_ENABLE_SANITIZERS AND NOT MSVC)
  target_compile_options(geographic_coordinates_sanitizers INTERFACE -fsanitize=address,undefined)
  target_link_options(geographic_coordinates_sanitizers INTERFACE -fsanitize=address,undefined)
endif()
