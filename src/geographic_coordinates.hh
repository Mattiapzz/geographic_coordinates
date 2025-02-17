#ifndef __GEOGRAPHIC_CLASS_HH
#define __GEOGRAPHIC_CLASS_HH

#include <cmath>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>


namespace geographic_coordinates {

  using namespace std;

  //!
  //! \brief Conversion between geographic coordinates
  //!
  //! This class stores a geographic position as origin of the local reference frame which is built runtime
  //! The class implements a number of transformation from Earth Centric coordinates and East-North-Up Coordinates
  //!
  //! The state consists of the latitude and longitude and altitude.
  //!
  //! The datum used for GPS positioning is called WGS84 (World Geodetic System
  //! 1984). It consists of a three-dimensional Cartesian coordinate system and
  //! an associated ellipsoid so that WGS84 positions can be described as either
  //! XYZ Cartesian coordinates or latitude, longitude and ellipsoid height
  //! coordinates. The origin of the datum is the Geocentre (the centre of mass
  //! of the Earth) and it is designed for positioning anywhere on Earth.
  //! The shape and size of the WGS84 biaxial ellipsoid is defined by the semi-major axis length
  //! a = 6378137.0 metres, and the reciprocal of flattening 1/ f = 298.257223563.
  //! This ellipsoid is the same shape and size as the GRS80 ellipsoid.
  //!
  //! All the internal coordinates follow the International Unit System and
  //! referes to earth center (or WGS84 ellipsoid center) except where explicitly  written.
  //!
  //! Geodetic Coordinates
  //! A geodetic system uses the coordinates (lat,lon,h) to represent position relative to a reference ellipsoid.
  //! lat: the latitude, originates at the equator. More specifically, the latitude of a point is the angle a normal to the
  //!      ellipsoid at that point makes with the equatorial plane, which contains the center and equator of the ellipsoid.
  //!      An angle of latitude is within the range [-90°, 90°].
  //!      Positive latitudes correspond to north and negative latitudes correspond to south.
  //! lon: the longitude, originates at the prime meridian.
  //!      More specifically, the longitude of a point is the angle that a plane containing the ellipsoid center and the meridian *      containing that point makes with the plane containing the ellipsoid center and prime meridian.
  //!      Positive longitudes are measured in a counterclockwise direction from a vantage point above the North Pole.
  //!      Typically, longitude is within the range [-180°, 180°] or [0°, 360°].
  //! h:   the ellipsoidal height, is measured along a normal of the reference spheroid.
  //!      Coordinate transformation functions such as geodetic2ecef require you to specify h in the same units
  //!      as the reference ellipsoid.
  //! altitude: height is measured along a normal of the reference spheroid starting from the intersection with equatiorial plane.
  //!
  //!
  //! East-North-Up Coordinates
  //! An east-north-up (ENU) system uses the Cartesian coordinates (xEast,yNorth,zUp) to represent position relative
  //! to a local origin.
  //! The local origin is described by the geodetic coordinates (lat0,lon0,h0).
  //! Note that the origin does not necessarily lie on the surface of the ellipsoid.
  //! The positive xEast-axis points east along the parallel of latitude containing lat0.
  //! The positive yNorth-axis points north along the meridian of longitude containing lon0.
  //! The positive zUp-axis points upward along the ellipsoid normal.
  //!*********************************************************************/

  typedef double real ;

  class geodetic_transformations {
  private:

    /*WGS84 ellipsoid*/
    real const WGS84_semi_major_axis{6378137.0};        /* a  */
    real const WGS84_semi_minor_axis{6356752.31424518}; /* b  */
    real const WGS84_eccentricity{0.0818191908426215};  /* e  */
    real const WGS84_reciprocal_flat{298.257223563};    /*reciprocal of flattening 1/ f*/

    /*GPS coordinates of origin*/
    real m_orig_latitude;  /* latitude  of orgin in radiants [-Pi/2, Pi/2]*/
    real m_orig_longitude; /* longitude of orgin in radiants  [-Pi, Pi]*/
    real m_orig_height;    /* height mesured orhtogonal from elipsoid  surface */
    real m_orig_altitude;  /* height mesured orhtogonal to elipsoid surface from equatorial plane */

    /*Unit vectors of local reference frame ENU (East North Up)*/

    // Unit vector to east (x-axis)
    real m_orig_uvec_east_x;    /* component x of unit vector poiting to east @origin */
    real m_orig_uvec_east_y;    /* component y of unit vector poiting to east @origin */
    real m_orig_uvec_east_z;    /* component z of unit vector poiting to east @origin */

    // Unit vector to north (y-axis)
    real m_orig_uvec_north_x;    /* component x of unit vector poiting to north @origin */
    real m_orig_uvec_north_y;    /* component y of unit vector poiting to north @origin */
    real m_orig_uvec_north_z;    /* component z of unit vector poiting to north @origin */

    // Unit vector Up (Normal to ellipsoid) (z-axis)
    real m_orig_norm_x ;    /* component x of normal to ellipsoid @origin */
    real m_orig_norm_y ;    /* component y of normal to ellipsoid @origin */
    real m_orig_norm_z ;    /* component z of normal to ellipsoid @origin  */

  public:

    //!
    //! The default constructor sets the coordinate of the origin for ENU perojection.
    //!
    explicit
    geodetic_transformations( real lat0, real lon0, real h0)
    : m_orig_latitude(lat0*M_PI/180)
    , m_orig_longitude(lon0*M_PI/180)
    , m_orig_height(h0)
    {

      real tmp = WGS84_eccentricity*sin(lat0*M_PI/180);
      real n   = WGS84_semi_major_axis/sqrt(1-tmp*tmp);
      m_orig_altitude = h0+n;

      //cout << lat0*M_PI/180 << " , " << m_orig_latitude << '\n';
      //cout << lon0*M_PI/180 << " , " << m_orig_longitude << '\n';

      // update normal to ellipsoid at origin location
      compute_norm_to_elipsoid(
        m_orig_latitude, m_orig_longitude,
        m_orig_norm_x,
        m_orig_norm_y,
        m_orig_norm_z
      );

      // update unit vector pointing to east
      compute_unit_vector_east(
        m_orig_longitude,
        m_orig_uvec_east_x,
        m_orig_uvec_east_y,
        m_orig_uvec_east_z
      );

      //cout << "m_orig_norm_x" <<  m_orig_norm_x <<"\n";

      // update unit vector pointing to north
      compute_unit_vector_north(
        m_orig_latitude,
        m_orig_longitude,
        m_orig_uvec_north_x,
        m_orig_uvec_north_y,
        m_orig_uvec_north_z
      );
    }

    ~geodetic_transformations() {}
    //!
    //! @brief
    //! set geodetic coordinates for origin of projection in ENU (East North Up)
    //!
    void set_origin( real phi, real lambda, real height );

    //!
    //! @return latitude (rad)
    //!
    real origin_latitude() const { return m_orig_latitude; }

    //!
    //! @return latitude (degrees)
    //!
    real origin_latitude_deg() const { return m_orig_latitude*180/M_PI; }

    //!
    //! @return longitude (rad)
    //!
    real origin_longitude() const { return m_orig_longitude; }

    //!
    //! @return longitude (degrees)
    //!
    real origin_longitude_deg() const { return m_orig_longitude*180/M_PI; }

    //!
    //! @return longitude (degrees)
    //!
    real origin_heigth() const { return m_orig_height; }

    //!
    //! @return longitude (degrees)
    //!
    real origin_altitude() const { return m_orig_altitude; }

    //!
    //! @return longitude (degrees)
    //!
    real origin_normal_x() const { return m_orig_norm_x; }
    real origin_normal_y() const { return m_orig_norm_y; }
    real origin_normal_z() const { return m_orig_norm_z; }

    real origin_uvec_east_x() const { return m_orig_uvec_east_x; }
    real origin_uvec_east_y() const { return m_orig_uvec_east_y; }
    real origin_uvec_east_z() const { return m_orig_uvec_east_z; }

    real origin_uvec_north_x() const { return m_orig_uvec_north_x; }
    real origin_uvec_north_y() const { return m_orig_uvec_north_y; }
    real origin_uvec_north_z() const { return m_orig_uvec_north_z; }

    void
    info( ostream & stream ) const {
      stream
        << "\n\nWGS84 Ellipsoid data:\n"
        << "  semi major axis (a):    " << WGS84_semi_major_axis <<'\n'
        << "  semi minor axis (b):    " << WGS84_semi_minor_axis <<'\n'
        << "  eccentricity (e):       " << WGS84_eccentricity    <<'\n'
        << "  reciprocal flat (1/f):  " << WGS84_reciprocal_flat <<'\n'
        << "\n\nOrigin coordinates\n"
        << "  latitude:               "  << m_orig_latitude*180/M_PI  << "[deg]\n"
        << "  longitude:              "  << m_orig_longitude*180/M_PI << "[deg]\n"
        << "  altitude:               "  << m_orig_height             << "[m]\n"
        << "\nUnit vector components East (X-axis)\n"
        << "   origin uve east x:     " << m_orig_uvec_east_x    << '\n'
        << "   origin uve east y:     " << m_orig_uvec_east_y    << '\n'
        << "   origin uve east z:     " << m_orig_uvec_east_z    << '\n'
        << "\nUnit vector components North (Y-axis)\n"
        << "   origin uvec north x:    " << m_orig_uvec_north_x   << '\n'
        << "   origin uvec north y:    " << m_orig_uvec_north_y   << '\n'
        << "   origin uvec north z:    " << m_orig_uvec_north_z   << '\n'
        << "\nUnit vector components Up (Z-axis)\n"
        << "   origin normal x:        " << m_orig_norm_x         << '\n'
        << "   origin normal y:        " << m_orig_norm_y         << '\n'
        << "   origin normal z:        " << m_orig_norm_z         << '\n'
        << '\n';
    }

    //!
    //! compute the normal to elipsoid surface at a given location ()
    //!
    void
    compute_norm_to_elipsoid(
      real  lat,
      real  lon,
      real & norm_x,
      real & norm_y,
      real & norm_z
    ) const;

    //!
    //! compute the  unit vector poiting to east at a given location ()
    //!
    void
    compute_unit_vector_east(
      real   lon,
      real & uvec_east_x,
      real & uvec_east_y,
      real & uvec_east_z
    ) const;

    //!
    //! compute the  unit vector poiting to north at a given location ()
    //!
    void
    compute_unit_vector_north(
      real   lat,
      real   lon,
      real & uvec_north_x,
      real & uvec_north_y,
      real & uvec_north_z
    ) const;

    /*   _____                  __                    _   _
     *  |_   _| _ __ _ _ _  ___/ _|___ _ _ _ __  __ _| |_(_)___ _ _  ___
     *    | || '_/ _` | ' \(_-<  _/ _ \ '_| '  \/ _` |  _| / _ \ ' \(_-<
     *    |_||_| \__,_|_||_/__/_| \___/_| |_|_|_\__,_|\__|_\___/_||_/__/
     */

    //!
    //! @brief
    //!   transforms the geodetic coordinates specified by lattitude (phi),
    //!   longitude (lambda), and height (h) to the geocentric Earth-Centered Earth-Fixed (ECEF)
    //!   Cartesian coordinates specified by X, Y, and Z.
    //!   lambda: geodetic longitude (degrees)
    //!   phi:    geodetic lattitude (degrees)
    //!   h:      geodetic height    (m)
    //!
    //! @return geocentric Earth-Centered Earth-Fixed (ECEF) Cartesian coordinates
    //!
    void
    convert_geodetic_coords_to_ECEF(
      real   phi,
      real   lambda,
      real   h,
      real & x,
      real & y,
      real & z
    ) const;

    //!
    //! @brief
    //!   transforms the geocentric Earth-centered Earth-fixed (ECEF) Cartesian coordinates specified by X, Y, and Z to the local *   east-north-up (ENU) Cartesian coordinates specified by xEast, yNorth, and zUp.
    //!   Specify the origin of the local ENU system with the geodetic coordinates m_orig_latitude, m_orig_longitude, and
    //!    m_orig_heigth.
    //!   x_ecef: geocentric cartesian coordinate x Earth-centered Earth-fixed (m)
    //!   y_ecef: geocentric cartesian coordinate y Earth-centered Earth-fixed (m)
    //!   z_ecef: geocentric cartesian coordinate zEarth-centered Earth-fixed  (m)
    //!
    //! @return geocentric East North Up (ENU) Cartesian coordinates
    //!
    void
    convert_ECEF_to_ENU(
      real   x_ecef,
      real   y_ecef,
      real   z_ecef,
      real & x_east,
      real & y_north,
      real & z_up
    ) const;

    //!
    //! @brief
    //!   transforms the geodetic coordinates specified by lattitude (phi),
    //!   longitude (lambda), and height (h) to  to the local *   east-north-up (ENU) Cartesian coordinates specified by xEast, yNorth, and zUp.
    //!   Specify the origin of the local ENU system with the geodetic coordinates m_orig_latitude, m_orig_longitude, and
    //!    m_orig_heigth.
    //!   lambda: geodetic longitude (degrees)
    //!   phi:    geodetic lattitude (degrees)
    //!   h:      geodetic height    (m)
    //!
    //! @return geocentric East North Up (ENU) Cartesian coordinates
    //!
    void
    convert_geodetic_coords_to_ENU(
      real   phi,
      real   lambda,
      real   h,
      real & x_east,
      real & y_north,
      real & z_up
    ) const;

    //!
    //! @brief
    //!   transforms the geocentric Earth-centered Earth-fixed (ECEF) Cartesian coordinates specified by X, Y, and Z to the local *   east-north-up (ENU) Cartesian coordinates specified by xEast, yNorth, and zUp.
    //!   Specify the origin of the local ENU system with the geodetic coordinates m_orig_latitude, m_orig_longitude, and
    //!    m_orig_heigth.
    //!   x_ecef: geocentric cartesian coordinate x Earth-centered Earth-fixed (m)
    //!   y_ecef: geocentric cartesian coordinate y Earth-centered Earth-fixed (m)
    //!   z_ecef: geocentric cartesian coordinate zEarth-centered Earth-fixed  (m)
    //!
    //! @return geocentric Earth-Centered Earth-Fixed (ECEF) Cartesian coordinates
    //!
    void
    convert_ENU_to_ECEF(
      real   x_east,
      real   y_north,
      real   z_up,
      real & x_ecef,
      real & y_ecef,
      real & z_ecef
    ) const;

    //!
    //! @brief
    //!   transforms the geocentric Earth-Centered Earth-Fixed (ECEF) Cartesian coordinates specified by X, Y, and Z
    //!   to the geodetic coordinates specified by m_orig_latitude, m_orig_longitude, and
    //!    m_orig_heigth..
    //!   x_ecef: geocentric cartesian coordinate x Earth-centered Earth-fixed (m)
    //!   y_ecef: geocentric cartesian coordinate y Earth-centered Earth-fixed (m)
    //!   z_ecef: geocentric cartesian coordinate zEarth-centered Earth-fixed  (m)
    //!
    //! @return geodetic m_orig_latitude, m_orig_longitude, and m_orig_heigth..
    //!
    void
    convert_ECEF_to_geodetic_coords(
      real   x_ecef,
      real   y_ecef,
      real   z_ecef ,
      real & phi,
      real & lambda,
      real & h
    ) const;

    //!
    //! @brief
    //!  transforms the local east-north-up (ENU) Cartesian coordinates specified by xEast, yNorth, and zUp
    //!  to the geodetic coordinates specified by  m_orig_latitude, m_orig_longitude, and  m_orig_heigth..
    //!   x_east: geocentric cartesian coordinate x Earth-centered Earth-fixed (m)
    //!   y_north: geocentric cartesian coordinate y Earth-centered Earth-fixed (m)
    //!   z_ecef: geocentric cartesian coordinate zEarth-centered Earth-fixed  (m)
    //!
    //! @return geodetic m_orig_latitude, m_orig_longitude, and m_orig_heigth..
    //!
    void
    convert_ENU_to_geodetic_coords(
      real   x_east,
      real   y_north,
      real   z_up,
      real & phi,
      real & lambda,
      real & h
    ) const;
  };
}

#endif
