# geographic_coordinates

Conversion between geographic coordinates: geodetic (latitude/longitude/height),
Earth-Centered-Earth-Fixed (ECEF), and local East-North-Up (ENU) frames, based on
the WGS84 ellipsoid.

## Background

This library stores a geographic position as the origin of a local reference
frame, built at runtime, and implements transformations between
Earth-Centered-Earth-Fixed (ECEF) coordinates and East-North-Up (ENU)
coordinates. The state consists of the latitude, longitude and altitude of
the local frame origin.

### WGS84

The datum used for GPS positioning is called WGS84 (World Geodetic System
1984). It consists of a three-dimensional Cartesian coordinate system and an
associated ellipsoid so that WGS84 positions can be described as either XYZ
Cartesian coordinates or latitude, longitude and ellipsoid height
coordinates. The origin of the datum is the Geocentre (the centre of mass of
the Earth) and it is designed for positioning anywhere on Earth. The shape
and size of the WGS84 biaxial ellipsoid is defined by the semi-major axis
length a = 6378137.0 metres, and the reciprocal of flattening 1/f =
298.257223563. This ellipsoid is the same shape and size as the GRS80
ellipsoid.

All internal coordinates follow the International System of Units and refer
to the Earth center (WGS84 ellipsoid center) except where explicitly noted.

### Geodetic coordinates

A geodetic system uses the coordinates `(lat, lon, h)` to represent position
relative to a reference ellipsoid.

- `lat`: the latitude, originates at the equator. The latitude of a point is
  the angle a normal to the ellipsoid at that point makes with the
  equatorial plane, which contains the center and equator of the ellipsoid.
  An angle of latitude is within the range `[-90 deg, 90 deg]`. Positive
  latitudes correspond to north and negative latitudes correspond to south.
- `lon`: the longitude, originates at the prime meridian. The longitude of a
  point is the angle that a plane containing the ellipsoid center and the
  meridian containing that point makes with the plane containing the
  ellipsoid center and the prime meridian. Positive longitudes are measured
  counterclockwise from a vantage point above the North Pole. Typically,
  longitude is within the range `[-180 deg, 180 deg]` or `[0 deg, 360 deg]`.
- `h`: the ellipsoidal height, measured along a normal of the reference
  ellipsoid.

`altitude` is height measured along a normal of the reference ellipsoid,
starting from the intersection with the equatorial plane.

### East-North-Up (ENU) coordinates

An East-North-Up (ENU) system uses the Cartesian coordinates
`(x_east, y_north, z_up)` to represent position relative to a local origin.
The local origin is described by the geodetic coordinates `(lat0, lon0, h0)`.
Note that the origin does not necessarily lie on the surface of the
ellipsoid: internally, the ENU frame is anchored on the ellipsoid surface at
`(lat0, lon0, h=0)`, so a point converted at `(lat0, lon0, h0)` reports
`z_up == h0` rather than `0`.

- the positive `x_east` axis points east along the parallel of latitude
  containing `lat0`.
- the positive `y_north` axis points north along the meridian of longitude
  containing `lon0`.
- the positive `z_up` axis points upward along the ellipsoid normal.

## Repository layout

```text
.
├── CMakeLists.txt              # top-level orchestration
├── CMakePresets.json           # standard debug/release presets
├── Dependencies.cmake          # FetchContent declarations (Catch2)
├── ProjectOptions.cmake        # warnings, sanitizers, feature toggles
├── src/geographic_coordinates/
│   ├── include/geographic_coordinates/geodetic_transformations.hh
│   └── geodetic_transformations.cc
├── python/                      # C API + ctypes Python bindings
├── test/geographic_coordinates/ # Catch2 test suite
└── examples/geographic_coordinates/ # example program
```

## Building (C++)

Requires CMake 3.21+ and a C++20 compiler (GCC, Clang/AppleClang, or MSVC).

```sh
cmake --preset debug        # or: cmake --preset release
cmake --build --preset debug
ctest --preset debug
```

This builds the `geographic_coordinates` library, the Catch2 test suite, the
`geographic_coordinates_c_api` shared library used by the Python bindings,
and the example program under `examples/geographic_coordinates/`.

Useful `cmake --preset debug` cache options (pass as `-D<option>=<ON|OFF>`):

| Option | Default | Description |
|---|---|---|
| `GEOGRAPHIC_COORDINATES_BUILD_TESTS` | `ON` | build the Catch2 test suite |
| `GEOGRAPHIC_COORDINATES_BUILD_EXAMPLES` | `ON` | build the example program |
| `GEOGRAPHIC_COORDINATES_BUILD_PYTHON_BINDINGS` | `ON` | build the C API shared library used by Python |
| `GEOGRAPHIC_COORDINATES_WARNINGS_AS_ERRORS` | `OFF` | treat compiler warnings as errors |
| `GEOGRAPHIC_COORDINATES_ENABLE_SANITIZERS` | `OFF` | enable ASan/UBSan (Debug builds, non-MSVC) |

### Installing

```sh
cmake --build --preset release --target install
```

installs the library, public headers, and CMake package config, so
downstream projects can `find_package(geographic_coordinates)` and link
against the `geographic_coordinates::geographic_coordinates` target.

## Python bindings

The Python bindings are a thin `ctypes` wrapper around a hand-written C API
(`geographic_coordinates_c_api`), built as a shared library by the CMake
build above (`GEOGRAPHIC_COORDINATES_BUILD_PYTHON_BINDINGS=ON`, the
default).

```sh
cmake --build --preset debug --target geographic_coordinates_c_api
cp build/debug/python/libgeographic_coordinates_c_api.* \
   python/
```

Then, from Python:

```python
from geographic_coordinates import GeodeticTransformations

gt = GeodeticTransformations(44.53449107748528, 10.85770480278604, 121.0)
x, y, z = gt.geodetic_to_ecef(44.53449107748528, 10.85770480278604, 121.0)
phi, lam, h = gt.ecef_to_geodetic(x, y, z)
```

Run the Python tests with:

```sh
python3 -m pytest python/tests
```

## License

See [LICENSE](LICENSE).
