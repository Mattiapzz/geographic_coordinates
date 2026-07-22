//!
//! \file geodetic_transformations.cc
//! \brief Implementation of GeodeticTransformations.
//!
//! F. Biral & E. Bertolazzi & M. Piazza
//!

#include <geographic_coordinates/geodetic_transformations.hh>

namespace geographic_coordinates {

  namespace {
    constexpr real deg2rad( real deg ) { return deg*std::numbers::pi/180; }
    constexpr real rad2deg( real rad ) { return rad*180/std::numbers::pi; }
  }

  void
  GeodeticTransformations::set_origin( real phi, real lambda, real height ) {
    m_orig_latitude  = deg2rad( phi );
    m_orig_longitude = deg2rad( lambda );
    m_orig_height    = height;

    real const tmp = kWGS84Eccentricity*std::sin( m_orig_latitude );
    real const n   = kWGS84SemiMajorAxis/std::sqrt( 1-tmp*tmp );
    m_orig_altitude = height+n;

    compute_norm_to_ellipsoid( m_orig_latitude, m_orig_longitude, m_orig_norm_x, m_orig_norm_y, m_orig_norm_z );
    compute_unit_vector_east( m_orig_longitude, m_orig_uvec_east_x, m_orig_uvec_east_y, m_orig_uvec_east_z );
    compute_unit_vector_north( m_orig_latitude, m_orig_longitude, m_orig_uvec_north_x, m_orig_uvec_north_y, m_orig_uvec_north_z );
  }

  void
  GeodeticTransformations::info( std::ostream & stream ) const {
    stream
      << "\n\nWGS84 Ellipsoid data:\n"
      << "  semi major axis (a):    " << kWGS84SemiMajorAxis  << '\n'
      << "  semi minor axis (b):    " << kWGS84SemiMinorAxis  << '\n'
      << "  eccentricity (e):       " << kWGS84Eccentricity   << '\n'
      << "  reciprocal flat (1/f):  " << kWGS84ReciprocalFlat << '\n'
      << "\n\nOrigin coordinates\n"
      << "  latitude:               " << rad2deg( m_orig_latitude )  << "[deg]\n"
      << "  longitude:              " << rad2deg( m_orig_longitude ) << "[deg]\n"
      << "  altitude:               " << m_orig_height                << "[m]\n"
      << "\nUnit vector components East (X-axis)\n"
      << "   origin uve east x:     " << m_orig_uvec_east_x  << '\n'
      << "   origin uve east y:     " << m_orig_uvec_east_y  << '\n'
      << "   origin uve east z:     " << m_orig_uvec_east_z  << '\n'
      << "\nUnit vector components North (Y-axis)\n"
      << "   origin uvec north x:   " << m_orig_uvec_north_x << '\n'
      << "   origin uvec north y:   " << m_orig_uvec_north_y << '\n'
      << "   origin uvec north z:   " << m_orig_uvec_north_z << '\n'
      << "\nUnit vector components Up (Z-axis)\n"
      << "   origin normal x:       " << m_orig_norm_x       << '\n'
      << "   origin normal y:       " << m_orig_norm_y       << '\n'
      << "   origin normal z:       " << m_orig_norm_z       << '\n'
      << '\n';
  }

  void
  GeodeticTransformations::compute_norm_to_ellipsoid( real lat, real lon, real & norm_x, real & norm_y, real & norm_z ) const {
    norm_x = std::cos( lat )*std::cos( lon );
    norm_y = std::cos( lat )*std::sin( lon );
    norm_z = std::sin( lat );
  }

  void
  GeodeticTransformations::compute_unit_vector_east( real lon, real & uvec_east_x, real & uvec_east_y, real & uvec_east_z ) const {
    uvec_east_x = -std::sin( lon );
    uvec_east_y =  std::cos( lon );
    uvec_east_z =  0;
  }

  void
  GeodeticTransformations::compute_unit_vector_north( real lat, real lon, real & uvec_north_x, real & uvec_north_y, real & uvec_north_z ) const {
    uvec_north_x = -std::sin( lat )*std::cos( lon );
    uvec_north_y = -std::sin( lat )*std::sin( lon );
    uvec_north_z =  std::cos( lat );
  }

  /*   _____                  __                    _   _
   *  |_   _| _ __ _ _ _  ___/ _|___ _ _ _ __  __ _| |_(_)___ _ _  ___
   *    | || '_/ _` | ' \(_-<  _/ _ \ '_| '  \/ _` |  _| / _ \ ' \(_-<
   *    |_||_| \__,_|_||_/__/_| \___/_| |_|_|_\__,_|\__|_\___/_||_/__/
   */

  void
  GeodeticTransformations::convert_geodetic_coords_to_ECEF(
    real phi, real lambda, real h, real & x, real & y, real & z
  ) const {
    real const lambda_rad = deg2rad( lambda );
    real const phi_rad    = deg2rad( phi );

    // The prime vertical radius of curvature n(phi) is the distance from the surface
    // to the Z-axis along the ellipsoid normal.
    real const tmp = kWGS84Eccentricity*std::sin( phi_rad );
    real const n   = kWGS84SemiMajorAxis/std::sqrt( 1-tmp*tmp );

    real norm_x, norm_y, norm_z;
    compute_norm_to_ellipsoid( phi_rad, lambda_rad, norm_x, norm_y, norm_z );

    x = (h+n)*norm_x;
    y = (h+n)*norm_y;
    z = (h+n*(1-kWGS84Eccentricity*kWGS84Eccentricity))*norm_z;
  }

  void
  GeodeticTransformations::convert_ECEF_to_ENU(
    real x_ecef, real y_ecef, real z_ecef, real & x_east, real & y_north, real & z_up
  ) const {
    real const tmp = kWGS84Eccentricity*std::sin( m_orig_latitude );
    real const n   = kWGS84SemiMajorAxis/std::sqrt( 1-tmp*tmp );

    // geocentric cartesian coordinates of origin
    real const x0 = n*m_orig_norm_x;
    real const y0 = n*m_orig_norm_y;
    real const z0 = (n-kWGS84Eccentricity*kWGS84Eccentricity*n)*m_orig_norm_z;

    // Rotation matrix of ENU in ECEF: R_ECEF = [uv_east | uv_north | origin_norm]
    // Projection from ECEF to ENU: R_ECEF^T . (P-P0)
    real const dx = x_ecef-x0;
    real const dy = y_ecef-y0;
    real const dz = z_ecef-z0;

    x_east  = m_orig_uvec_east_x*dx  + m_orig_uvec_east_y*dy;
    y_north = m_orig_uvec_north_x*dx + m_orig_uvec_north_y*dy + m_orig_uvec_north_z*dz;
    z_up    = m_orig_norm_x*dx       + m_orig_norm_y*dy       + m_orig_norm_z*dz;
  }

  void
  GeodeticTransformations::convert_geodetic_coords_to_ENU(
    real phi, real lambda, real h, real & x_east, real & y_north, real & z_up
  ) const {
    real x_ecef, y_ecef, z_ecef;
    convert_geodetic_coords_to_ECEF( phi, lambda, h, x_ecef, y_ecef, z_ecef );
    convert_ECEF_to_ENU( x_ecef, y_ecef, z_ecef, x_east, y_north, z_up );
  }

  void
  GeodeticTransformations::convert_ENU_to_ECEF(
    real x_east, real y_north, real z_up, real & x_ecef, real & y_ecef, real & z_ecef
  ) const {
    real const tmp = kWGS84Eccentricity*std::sin( m_orig_latitude );
    real const n   = kWGS84SemiMajorAxis/std::sqrt( 1-tmp*tmp );

    // geocentric cartesian coordinates of origin
    real const x0 = n*m_orig_norm_x;
    real const y0 = n*m_orig_norm_y;
    real const z0 = (n-kWGS84Eccentricity*kWGS84Eccentricity*n)*m_orig_norm_z;

    // Rotation matrix of ENU in ECEF: R_ECEF = [uv_east | uv_north | origin_norm]
    // Projection from ENU to ECEF: P = P0 + R_ECEF . P_enu
    x_ecef = x0 + m_orig_uvec_east_x*x_east + m_orig_uvec_north_x*y_north + m_orig_norm_x*z_up;
    y_ecef = y0 + m_orig_uvec_east_y*x_east + m_orig_uvec_north_y*y_north + m_orig_norm_y*z_up;
    z_ecef = z0 + m_orig_uvec_east_z*x_east + m_orig_uvec_north_z*y_north + m_orig_norm_z*z_up;
  }

  void
  GeodeticTransformations::convert_ECEF_to_geodetic_coords(
    real x_ecef, real y_ecef, real z_ecef, real & phi, real & lambda, real & h
  ) const {
    // Vermeille, H., 2002. Direct transformation from geocentric coordinates to
    // geodetic coordinates. Journal of Geodesy, 76(8), pp.451-454.
    // https://doi.org/10.1007/s00190-002-0273-6
    real const a2 = kWGS84SemiMajorAxis*kWGS84SemiMajorAxis;
    real const e2 = kWGS84Eccentricity*kWGS84Eccentricity;
    real const e4 = e2*e2;
    real const p  = (x_ecef*x_ecef + y_ecef*y_ecef)/a2;
    real const q  = (1.0-e2)/a2*z_ecef*z_ecef;
    real const r  = (p+q-e4)/6.0;
    real const s  = e4*p*q/(4.0*r*r*r);
    real const t  = std::pow( 1.0+s+std::sqrt( s*(2.0+s) ), 1.0/3.0 );
    real const u  = r*(1.0+t+1.0/t);
    real const v  = std::sqrt( u*u+e4*q );
    real const w  = e2*(u+v-q)/(2.0*v);
    real const k  = std::sqrt( u+v+w*w )-w;
    real const D  = k*std::sqrt( x_ecef*x_ecef+y_ecef*y_ecef )/(k+e2);

    lambda = 2.0*std::atan2( y_ecef, x_ecef+std::sqrt( x_ecef*x_ecef+y_ecef*y_ecef ) );
    phi    = 2.0*std::atan2( z_ecef, D+std::sqrt( D*D+z_ecef*z_ecef ) );
    h      = (k+e2-1.0)/k*std::sqrt( D*D+z_ecef*z_ecef );

    lambda = rad2deg( lambda );
    phi    = rad2deg( phi );
  }

  void
  GeodeticTransformations::convert_ENU_to_geodetic_coords(
    real x_east, real y_north, real z_up, real & phi, real & lambda, real & h
  ) const {
    real x_ecef, y_ecef, z_ecef;
    convert_ENU_to_ECEF( x_east, y_north, z_up, x_ecef, y_ecef, z_ecef );
    convert_ECEF_to_geodetic_coords( x_ecef, y_ecef, z_ecef, phi, lambda, h );
  }

} // namespace geographic_coordinates
