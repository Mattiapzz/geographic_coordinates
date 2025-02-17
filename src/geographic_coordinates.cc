//!
//! \file geographic_coordinates.cc
//! \brief Header for geographic_transformation class
//!
//! F. Biral & E. Bertolazzi & Mattia Piazza
//!

#include "geographic_coordinates.hh"


namespace geographic_coordinates {
  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::set_origin( real  phi,real lambda, real  height){

    m_orig_latitude  = phi*M_PI/180;
    m_orig_longitude = lambda*M_PI/180;
    m_orig_height    = height;

    real tmp = WGS84_eccentricity*sin(m_orig_latitude);
    real n   = WGS84_semi_major_axis/sqrt(1-tmp*tmp);
    m_orig_altitude  = height+n;

    // update normal to ellipsoid at origin location
    compute_norm_to_elipsoid(m_orig_latitude, m_orig_longitude,
                             m_orig_norm_x,
                             m_orig_norm_y,
                             m_orig_norm_z);

    // update unit vector pointing to east
    compute_unit_vector_east(m_orig_longitude,
                             m_orig_uvec_east_x,
                             m_orig_uvec_east_y,
                             m_orig_uvec_east_z);

    // update unit vector pointing to north
    compute_unit_vector_north(m_orig_latitude,  m_orig_longitude,
                               m_orig_uvec_north_x,
                               m_orig_uvec_north_y,
                               m_orig_uvec_north_z);

  }

  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::compute_norm_to_elipsoid(
    real   lat,
    real   lon,
    real & norm_x,
    real & norm_y,
    real & norm_z
  ) const {

    // Normal vector to ellipsoid surface
    norm_x = cos(lat)*cos(lon);
    norm_y = cos(lat)*sin(lon);
    norm_z = sin(lat);

    //cout << "lat "  << lat  << '\n';
    //cout << "lon "  << lon  << '\n';
    //cout << "norm_x" <<  norm_x <<"\n";
    //cout << "cos(lat)*cos(lon)" << cos(lat)*cos(lon) << "\n";

  }

  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::compute_unit_vector_east(
    real   lon,
    real & uvec_east_x,
    real & uvec_east_y,
    real & uvec_east_z
  ) const {
    uvec_east_x = -sin(lon) ;
    uvec_east_y =  cos(lon);
    uvec_east_z =  0 ;
  }

  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::compute_unit_vector_north(
    real   lat,
    real   lon,
    real & uvec_north_x,
    real & uvec_north_y,
    real & uvec_north_z
  ) const {
    uvec_north_x = -sin(lat)*cos(lon) ;
    uvec_north_y = -sin(lat)*sin(lon);
    uvec_north_z =  cos(lat) ;
  }

  /*   _____                  __                    _   _
   *  |_   _| _ __ _ _ _  ___/ _|___ _ _ _ __  __ _| |_(_)___ _ _  ___
   *    | || '_/ _` | ' \(_-<  _/ _ \ '_| '  \/ _` |  _| / _ \ ' \(_-<
   *    |_||_| \__,_|_||_/__/_| \___/_| |_|_|_\__,_|\__|_\___/_||_/__/
   */

  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::convert_geodetic_coords_to_ECEF(
    real    phi,
    real    lambda,
    real    h,
    real  & x,
    real  & y,
    real  & z
  ) const {

    real lambda_rad = lambda/180.0*M_PI;
    real phi_rad    = phi/180.0*M_PI;

    // The prime vertical radius of curvature n(phi) is the distance from the surface
    // to the Z-axis along the ellipsoid normal.
    real tmp = WGS84_eccentricity*sin(phi_rad);
    real n   = WGS84_semi_major_axis/sqrt(1-tmp*tmp);

    // compute local normal to ellipsoid
    real norm_x, norm_y, norm_z ;
    compute_norm_to_elipsoid( phi_rad, lambda_rad, norm_x, norm_y, norm_z );

    x = (h+n)*norm_x;
    y = (h+n)*norm_y;
    z = (h+n*(1-WGS84_eccentricity*WGS84_eccentricity))*norm_z;

  }


  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::convert_ECEF_to_ENU(
    real   x_ecef,
    real   y_ecef,
    real   z_ecef,
    real & x_east,
    real & y_north,
    real & z_up
  ) const {

    real tmp = WGS84_eccentricity*sin(m_orig_latitude);
    real n = WGS84_semi_major_axis/sqrt(1-tmp*tmp);

    // geocentric cartesian coordinates of origin
    real x0 =  n*m_orig_norm_x;
    real y0 = n*m_orig_norm_y;
    real z0 = (n-WGS84_eccentricity*WGS84_eccentricity*n)*m_orig_norm_z;

    // Rotation matrix of ENU in ECEF: R_ECEF = [uv_east | uv_north | origin_norm];
    // Projection from ecef to enu: R_ECEF^T.(P-P0)
    // Normal vector
    real dx = x_ecef-x0;
    real dy = y_ecef-y0;
    real dz = z_ecef-z0;

    // dx*
    x_east  = m_orig_uvec_east_x*dx  + m_orig_uvec_east_y*dy; // + uv_east_z*dz
    y_north = m_orig_uvec_north_x*dx + m_orig_uvec_north_y*dy+ m_orig_uvec_north_z*dz ;
    z_up    = m_orig_norm_x*dx + m_orig_norm_y*dy+ m_orig_norm_z*dz ;

  }

  void
  geodetic_transformations::convert_geodetic_coords_to_ENU(
    real   phi,
    real   lambda,
    real   h,
    real & x_east,
    real & y_north,
    real & z_up
  ) const {
    real x_ecef, y_ecef, z_ecef;
    convert_geodetic_coords_to_ECEF( phi, lambda, h, x_ecef, y_ecef, z_ecef ) ;
    convert_ECEF_to_ENU( x_ecef, y_ecef, z_ecef, x_east, y_north, z_up ) ;
  }


  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::convert_ENU_to_ECEF(
    real   x_east,
    real   y_north,
    real   z_up,
    real & x_ecef,
    real & y_ecef,
    real & z_ecef
  ) const {

    real tmp = WGS84_eccentricity*sin(m_orig_latitude);
    real n = WGS84_semi_major_axis/sqrt(1-tmp*tmp);

    // geocentric cartesian coordinates of origin
    real x0 =  n*m_orig_norm_x;
    real y0 = n*m_orig_norm_y;
    real z0 = (n-WGS84_eccentricity*WGS84_eccentricity*n)*m_orig_norm_z;

    // Rotation matrix of ENU in ECEF: R_ECEF = [uv_east | uv_north | origin_norm];
    // Projection from enu to ecef: P = P0+R_ECEF.P_enu
    x_ecef = x0+m_orig_uvec_east_x*x_east +m_orig_uvec_north_x*y_north +m_orig_norm_x*z_up;
    y_ecef = y0+m_orig_uvec_east_y*x_east +m_orig_uvec_north_y*y_north +m_orig_norm_y*z_up;
    z_ecef = z0+m_orig_uvec_east_z*x_east +m_orig_uvec_north_z*y_north +m_orig_norm_z*z_up;

  }


  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::convert_ECEF_to_geodetic_coords(
    real   x_ecef,
    real   y_ecef,
    real   z_ecef ,
    real & phi,
    real & lambda,
    real & h
  ) const {

    lambda = atan2(y_ecef, x_ecef) ;

    // implementation from article:
    // Vermeille, H., 2002. Direct transformation from geocentric coordinates to geodetic coordinates. Journal of Geodesy, 76(8), pp.451-454.
    // https://doi.org/10.1007/s00190-002-0273-6

    real a2 = WGS84_semi_major_axis*WGS84_semi_major_axis;
    real e2 = WGS84_eccentricity*WGS84_eccentricity;
    real e4 = e2*e2;
    real p  = (x_ecef*x_ecef + y_ecef*y_ecef) / a2;
    real q  = (1.0 - e2) / a2 * z_ecef*z_ecef;
    real r  = (p + q - e4) / 6.0;
    real s  = e4 * p * q / (4.0 * r*r*r);
    real t = pow(1.0 + s + sqrt(s * (2.0 + s)), 1.0/3.0);
    real u = r * (1.0 + t + 1.0/t);
    real v = sqrt(u*u + e4 * q);
    real w = e2 * (u + v - q) / (2.0 * v);
    real k = sqrt(u + v + w*w) - w;
    real D = k * sqrt(x_ecef*x_ecef + y_ecef*y_ecef) / (k + e2);

    lambda = 2.0 * atan2(y_ecef, x_ecef + sqrt(x_ecef*x_ecef + y_ecef*y_ecef)) ;
    phi    = 2.0 * atan2(z_ecef, D + sqrt(D*D + z_ecef*z_ecef)) ;
    h      = (k + e2 - 1.0)/k*sqrt(D*D + z_ecef*z_ecef) ;

    // conversion in degrees for latitude and longitude
    lambda = lambda*180/M_PI;
    phi  = phi*180/M_PI;

  }

  /*  ------------------------------------------------------- */
  void
  geodetic_transformations::convert_ENU_to_geodetic_coords(
    real   x_east,
    real   y_north,
    real   z_up,
    real & phi,
    real & lambda,
    real & h
  ) const {
    real x_ecef, y_ecef, z_ecef;
    convert_ENU_to_ECEF( x_east, y_north, z_up, x_ecef, y_ecef, z_ecef );
    convert_ECEF_to_geodetic_coords(x_ecef, y_ecef, z_ecef, phi, lambda, h );
  } 

} /*close namespace*/
