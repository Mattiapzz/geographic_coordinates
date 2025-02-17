#include "geographic_coordinates.hh"
#include <iostream>
#include <iomanip>
#include <stdlib.h>     /* atof */

using namespace std;
using namespace geographic_coordinates ;

#define MSG_ERROR \
"Usage:\n"

int
main( int argc, char const * argv[] ) {

   // Coordinates of origin of local frame
   //const real lon0 =  11.717902;
   //const real lat0 = 44.344089 ;
   //const real h0   = 42.0;

   
   // Coordinates to test the library
   //const real lon =  11.710209;
   //const real lat = 44.340360 ;
   //const real h   = 49.0;

   const real lat0 =  44.53449107748528 ;
   const real lon0 =  10.85770480278604 ;
  const real h0  =  121.0  ;
   const real lat =  44.53449107748528 ;
   const real lon =  10.85770480278604 ;
    const real h  =  121.0  ;

   // Instantiate a geodetic transformation object with its origin
   geodetic_transformations GT( lat0, lon0, h0);
   // print out the main parameters
   GT.info(cout);

   // convert from geodesic to cartesia ECEF coordinates
   real x_ecef, y_ecef, z_ecef;
   real lat2,lon2,alt2;
   GT.convert_geodetic_coords_to_ECEF(lat,lon,h,x_ecef, y_ecef, z_ecef );
   cout << setprecision(18);
   cout << "x_ecef = " << x_ecef <<  "[m]\n";
   cout << "y_ecef = " << y_ecef <<  "[m]\n";
   cout << "z_ecef = " << z_ecef <<  "[m]\n";
 
   GT.convert_ECEF_to_geodetic_coords(x_ecef, y_ecef, z_ecef,lat2,lon2,alt2 );
   
   cout << "lat2," << lat2 << "  -- " << lat <<'\n';
   cout << "lon2," << lon2 << "  -- " << lon <<'\n';
   cout << "alt2," << alt2 << "  -- " << h <<'\n';

   real x_enu, y_enu, z_enu;
   GT.convert_geodetic_coords_to_ENU(lat,lon,h,x_enu, y_enu, z_enu );

   cout << "x_enu = " << x_enu <<  "[m]\n";
   cout << "y_enu = " << y_enu <<  "[m]\n";
   cout << "z_enu = " << z_enu <<  "[m]\n";
  
 

  
  cout << "All Done Folks\n";
  return 0;
}
