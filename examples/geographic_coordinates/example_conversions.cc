#include <geographic_coordinates/geodetic_transformations.hh>

#include <iomanip>
#include <iostream>

using geographic_coordinates::GeodeticTransformations;
using geographic_coordinates::real;

int main() {
  real const lat0 = 44.53449107748528;
  real const lon0 = 10.85770480278604;
  real const h0   = 121.0;

  real const lat = lat0;
  real const lon = lon0;
  real const h   = h0;

  GeodeticTransformations const gt( lat0, lon0, h0 );
  gt.info( std::cout );

  real x_ecef, y_ecef, z_ecef;
  gt.convert_geodetic_coords_to_ECEF( lat, lon, h, x_ecef, y_ecef, z_ecef );

  std::cout << std::setprecision( 18 );
  std::cout << "x_ecef = " << x_ecef << "[m]\n";
  std::cout << "y_ecef = " << y_ecef << "[m]\n";
  std::cout << "z_ecef = " << z_ecef << "[m]\n";

  real lat2, lon2, alt2;
  gt.convert_ECEF_to_geodetic_coords( x_ecef, y_ecef, z_ecef, lat2, lon2, alt2 );

  std::cout << "lat2, " << lat2 << "  -- " << lat << '\n';
  std::cout << "lon2, " << lon2 << "  -- " << lon << '\n';
  std::cout << "alt2, " << alt2 << "  -- " << h << '\n';

  real x_enu, y_enu, z_enu;
  gt.convert_geodetic_coords_to_ENU( lat, lon, h, x_enu, y_enu, z_enu );

  std::cout << "x_enu = " << x_enu << "[m]\n";
  std::cout << "y_enu = " << y_enu << "[m]\n";
  std::cout << "z_enu = " << z_enu << "[m]\n";

  std::cout << "All Done Folks\n";
  return 0;
}
