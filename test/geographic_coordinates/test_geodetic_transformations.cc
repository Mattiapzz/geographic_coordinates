#include <geographic_coordinates/geodetic_transformations.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using geographic_coordinates::GeodeticTransformations;
using geographic_coordinates::real;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

namespace {
  constexpr real kLat0 = 44.53449107748528;
  constexpr real kLon0 = 10.85770480278604;
  constexpr real kH0   = 121.0;
}

TEST_CASE( "origin accessors reflect the constructor arguments", "[geographic_coordinates][origin]" ) {
  GeodeticTransformations const gt( kLat0, kLon0, kH0 );
  CHECK_THAT( gt.origin_latitude_deg(), WithinAbs( kLat0, 1e-9 ) );
  CHECK_THAT( gt.origin_longitude_deg(), WithinAbs( kLon0, 1e-9 ) );
  CHECK_THAT( gt.origin_height(), WithinAbs( kH0, 1e-9 ) );
}

TEST_CASE( "geodetic to ECEF and back is the identity", "[geographic_coordinates][ecef]" ) {
  GeodeticTransformations const gt( kLat0, kLon0, kH0 );

  real x, y, z;
  gt.convert_geodetic_coords_to_ECEF( kLat0, kLon0, kH0, x, y, z );

  real phi, lambda, h;
  gt.convert_ECEF_to_geodetic_coords( x, y, z, phi, lambda, h );

  CHECK_THAT( phi, WithinRel( kLat0, 1e-9 ) );
  CHECK_THAT( lambda, WithinRel( kLon0, 1e-9 ) );
  CHECK_THAT( h, WithinAbs( kH0, 1e-6 ) );
}

TEST_CASE( "ENU coordinates above the origin have zero east/north and z_up == height", "[geographic_coordinates][enu]" ) {
  // The ENU frame is anchored on the ellipsoid surface at (lat0, lon0, h=0),
  // so a point at (lat0, lon0, h0) sits directly above it at height h0.
  GeodeticTransformations const gt( kLat0, kLon0, kH0 );

  real x_east, y_north, z_up;
  gt.convert_geodetic_coords_to_ENU( kLat0, kLon0, kH0, x_east, y_north, z_up );

  CHECK_THAT( x_east, WithinAbs( 0.0, 1e-6 ) );
  CHECK_THAT( y_north, WithinAbs( 0.0, 1e-6 ) );
  CHECK_THAT( z_up, WithinAbs( kH0, 1e-6 ) );
}

TEST_CASE( "ENU to ECEF and back is the identity", "[geographic_coordinates][enu]" ) {
  GeodeticTransformations const gt( kLat0, kLon0, kH0 );

  real x_ecef, y_ecef, z_ecef;
  gt.convert_geodetic_coords_to_ECEF( kLat0+0.05, kLon0+0.05, kH0+30.0, x_ecef, y_ecef, z_ecef );

  real x_east, y_north, z_up;
  gt.convert_ECEF_to_ENU( x_ecef, y_ecef, z_ecef, x_east, y_north, z_up );

  real x2, y2, z2;
  gt.convert_ENU_to_ECEF( x_east, y_north, z_up, x2, y2, z2 );

  CHECK_THAT( x2, WithinRel( x_ecef, 1e-9 ) );
  CHECK_THAT( y2, WithinRel( y_ecef, 1e-9 ) );
  CHECK_THAT( z2, WithinRel( z_ecef, 1e-9 ) );
}

TEST_CASE( "set_origin updates subsequent conversions", "[geographic_coordinates][origin]" ) {
  GeodeticTransformations gt( kLat0, kLon0, kH0 );
  gt.set_origin( kLat0+1.0, kLon0+1.0, kH0 );

  CHECK_THAT( gt.origin_latitude_deg(), WithinAbs( kLat0+1.0, 1e-9 ) );
  CHECK_THAT( gt.origin_longitude_deg(), WithinAbs( kLon0+1.0, 1e-9 ) );

  real x_east, y_north, z_up;
  gt.convert_geodetic_coords_to_ENU( kLat0+1.0, kLon0+1.0, kH0, x_east, y_north, z_up );
  CHECK_THAT( x_east, WithinAbs( 0.0, 1e-6 ) );
  CHECK_THAT( y_north, WithinAbs( 0.0, 1e-6 ) );
  CHECK_THAT( z_up, WithinAbs( kH0, 1e-6 ) );
}
